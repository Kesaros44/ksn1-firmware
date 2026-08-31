# ksn1-firmware

ZMK firmware configuration for the KSN-1 split keyboard — the original, already-shipping design that KSN-2 and the Nexplit Keypad are both derived from. This has the longest debugging history in the lineup; several patterns proven here (custom peripheral-LED drivers, the BLE conn-status relay, `wakeup-source` handling) have been reused on the newer boards.

* Keyboard Maintainer: [AJG](https://github.com/Kesaros44)
* Hardware Supported: KSN-1 split keyboard, nice!nano v2 (nRF52840), BLE

## Hardware

- **MCU:** nice!nano v2 (nRF52840) per half, wireless (BLE)
- **Central:** the **right** half (not the left — see below)
- **Matrix:** 6 rows x 22 columns combined (11 columns per half), `col2row`
- **Encoder:** one EC11 encoder, on the right half (central) only
- **LEDs (left/peripheral only):** a Caps Lock LED and a BLE connection-status LED, each driven by a custom driver (see below) rather than ZMK's built-in indicator node
- **Backlight:** a PWM backlight node exists in the devicetree, but it's currently disabled in firmware (`CONFIG_ZMK_BACKLIGHT=n` on both halves) by request — the two LEDs above are unaffected by this
- **RGB underglow:** not present on this board (`CONFIG_ZMK_RGB_UNDERGLOW=n`)
- **Battery:** reporting enabled on both halves; central also periodically fetches the peripheral's battery level and reports it to the host alongside its own (`CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING`/`_PROXY`)

### Why central is the right half, not the left

An earlier revision had central on the left. Row 6's last four columns (right ctrl / left / down / right arrow) land at matrix positions 128–131, which exceeds ZMK split BLE's hardcoded `POS_STATE_LEN` (16 bytes = 128 bits) for peripheral-to-central position reporting — those keys silently stopped registering. A central half's own local key input never has to cross that BLE transport, so moving central to the right side (making the previously-overflowing columns local to central instead of relayed) avoided the limit entirely. If a future matrix expansion runs into the same limit, the fix is the same: put the overflowing positions on whichever half is central.

### Custom LED drivers (why they exist)

ZMK's built-in `zmk,indicator-leds` devicetree node calls central-only functions (`zmk_hid_indicators_get_current_profile()`, `zmk_endpoint_is_connected()`), which aren't linkable on a peripheral build — but both of this board's status LEDs are physically wired to the peripheral (left) half. Two custom drivers work around this:

- **`ksn1_peripheral_indicators.c`** — drives the Caps Lock LED by subscribing to the `zmk_hid_indicators_changed` event instead, which *is* safe on a peripheral (central pushes the host's raw HID indicator bitmask over BLE when `CONFIG_ZMK_SPLIT_PERIPHERAL_HID_INDICATORS=y` is set on both sides).
- **`ksn1_conn_status_relay_central.c`** / **`ksn1_conn_status_relay_peripheral.c`** — there's no built-in ZMK channel for "does central have an active host connection" on a peripheral LED, so these add a private BLE GATT service on top of the existing split link: central polls `zmk_endpoint_is_connected()` every 250ms and writes a byte to the peripheral over that private characteristic whenever it changes; the peripheral drives the LED solid when connected, blinking (300ms) when not. GATT discovery for this side-channel is deliberately delayed 3s after a split reconnect so it doesn't compete with ZMK's own critical-path discovery (position state, indicators, battery) for the connection's single-outstanding-ATT-request slot.

All three files are wired into the build via `config/zephyr/module.yml` (registers `config/` as a Zephyr module) + `config/CMakeLists.txt` (`target_sources(app PRIVATE src/...)`, paths relative to `config/`) + the source files under `config/src/`. If you ever add another custom `.c` file here, all three have to agree — a missing or misplaced piece silently compiles the file out with no build error.

### Keymap (`config/ksn_1.keymap`)

Three layers:

- **`default_layer`** — Windows base layer with an integrated numpad on the left columns. Encoder = volume up/down. The numpad's top-left key runs a macro (Win+R → `calc` → Enter) to launch the Windows calculator.
- **`func_layer`** (momentary, `&mo 1`) — Bluetooth profile select (0–4) and clear, output toggle (`OUT_TOG`), backlight inc/dec on the encoder, and a toggle (`&tog 2`) into `mac_layer`.
- **`mac_layer`** — same base layout with Mac modifier ordering and Mac media/brightness keys in place of the F-row; its numpad corner key instead runs a macro (Cmd+Space → `calculator` → Enter) for Spotlight, with longer wait times to account for Spotlight's indexing/animation delay.

## Building

CI (`.github/workflows/`) uses ZMK's standard reusable workflow (`zmkfirmware/zmk/.github/workflows/build-user-config.yml@main`) — no custom build logic. `build.yaml` defines three targets:

```yaml
include:
  - board: nice_nano//zmk
    shield: ksn_1_left
    snippet: zmk-usb-logging
  - board: nice_nano//zmk
    shield: ksn_1_right
    snippet: zmk-usb-logging
  - board: nice_nano//zmk
    shield: settings_reset
    artifact-name: settings_reset
```

Pushing to this repo builds `ksn_1_left`, `ksn_1_right`, and a `settings_reset` firmware as GitHub Actions artifacts — grab the `.uf2` files from the workflow run.

To build locally with `west`:

```sh
west init -l config
west update
west build -p -b nice_nano_v2 -- -DSHIELD=ksn_1_left -DZMK_EXTRA_MODULES=$(pwd)/config
west build -p -b nice_nano_v2 -- -DSHIELD=ksn_1_right -DZMK_EXTRA_MODULES=$(pwd)/config
```

## Flashing

Double-tap reset on the nice!nano to enter its UF2 bootloader, then drag the matching `.uf2` (left firmware to the left half, right to the right half) onto the mounted `NICENANO` drive. Flash both halves — they run different firmware images (only the right/central image has `CONFIG_ZMK_SPLIT_ROLE_CENTRAL=y`, the encoder, and the conn-status relay's central half).

## Re-pairing / clearing Bluetooth bonds

Flash the `settings_reset` artifact (built alongside the two main targets) to a half to wipe its stored BLE bonds, then reflash that half's normal firmware and re-pair.

## Notable fixed issues (for future reference)

These were real bugs found and fixed during this board's development — kept here so the same symptom is recognized quickly if it ever reappears (including on KSN-2 or the Nexplit Keypad, which share these patterns):

- **"Paired but not connected"**: the peripheral's `kscan0` and encoder GPIOs weren't registered as wakeup sources, so they couldn't raise an interrupt to wake the MCU out of low-power mode. Fixed by adding `wakeup-source;` to `kscan0` (in the shared `ksn_1.dtsi`) and `CONFIG_PM_DEVICE=y` on the right/central `.conf`. (The EC11 encoder's `alps,ec11` binding does **not** support `wakeup-source` — don't try to add it there.) Disabling power management entirely (`CONFIG_PM=n`) also "fixed" it during diagnosis but was rejected — it stops the board from sleeping at all, which is a battery-life regression, not a real fix.
- **Bluetooth profile-slot exhaustion made worse by raising `CONFIG_BT_MAX_CONN`**: bumping it from the default (2) to 3 to add BLE profile slots instead caused a repeating `Security failed ... err 4` (`AUTH_REQUIREMENT`) loop on every connection attempt — strictly worse than the original slot-exhaustion symptom (`err 9`) it was meant to fix. Reverted to the default.
- **Status LED polarity ("blinks then goes dark on connect")**: the left half's `status_led` devicetree node was `GPIO_ACTIVE_HIGH`; flipping it to `GPIO_ACTIVE_LOW` fixed the inverted-looking behavior. A cheap thing to check first whenever an LED's on/off state looks backwards from what the driving code intends.
- **Inaccurate battery percentage**: an earlier custom `zmk,battery-voltage-divider` node (external resistor divider) produced bogus readings (readings like 528mV/0%). Removed in favor of the nice!nano v2 board default (`zmk,battery-nrf-vddh`, internal VDDH sensing), which reports correctly.
- **Debugging tip**: don't set `CONFIG_LOG_DEFAULT_LEVEL` by hand to chase a specific symptom — it's a *global* Zephyr log level, so it also turns on debug logging for unrelated low-level modules (e.g. the USB driver), which can flood the log buffer and drop the very messages you're trying to capture. Use the project's `zmk-usb-logging` build snippet (already in `build.yaml`) instead.

## Leftover dependency (harmless)

`config/west.yml` still pulls in the `zmk-poor-mans-led-indicator` module from an earlier iteration of the status LED, but its Kconfig options are all disabled (`CONFIG_INDICATOR_LED_WIDGET=n`) now that `ksn1_conn_status_relay_*.c` handles that LED directly — two drivers must never touch the same LED at once. The module is unused dead weight in the manifest, not a functional dependency; safe to leave as-is or remove later.
