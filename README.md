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

---

# ksn1-firmware (한국어)

KSN-1 스플릿 키보드용 ZMK 펌웨어 설정입니다 — KSN-2와 Nexplit Keypad가 파생된 원조 설계이자, 이미 실제로 사용 중인 보드입니다. 이 라인업에서 가장 오랜 디버깅 이력을 가지고 있으며, 여기서 검증된 여러 패턴(커스텀 peripheral LED 드라이버, BLE 연결상태 중계, `wakeup-source` 처리)이 이후 보드들에 재사용되었습니다.

* 키보드 관리자: [AJG](https://github.com/Kesaros44)
* 지원 하드웨어: KSN-1 스플릿 키보드, nice!nano v2 (nRF52840), BLE

## 하드웨어

- **MCU:** half마다 nice!nano v2 (nRF52840), 무선(BLE)
- **Central:** **오른쪽** half (왼쪽이 아님 — 아래 설명 참고)
- **매트릭스:** 6행 x 22열 (half당 11열), `col2row`
- **인코더:** EC11 인코더 1개, 오른쪽(central) half에만 있음
- **LED (왼쪽/peripheral 전용):** Caps Lock LED와 BLE 연결상태 LED, 둘 다 ZMK 내장 indicator 노드가 아니라 커스텀 드라이버(아래 참고)로 구동
- **백라이트:** 디바이스트리에 PWM 백라이트 노드는 존재하지만, 요청에 따라 현재 펌웨어에서는 비활성화(`CONFIG_ZMK_BACKLIGHT=n`, 양쪽 half 모두) — 위 두 LED는 이 설정과 무관하게 동작함
- **RGB 언더글로우:** 이 보드에는 없음 (`CONFIG_ZMK_RGB_UNDERGLOW=n`)
- **배터리:** 양쪽 half 모두 배터리 보고 활성화; central이 peripheral의 배터리 잔량도 주기적으로 가져와 자신의 배터리와 함께 호스트에 보고 (`CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING`/`_PROXY`)

### central이 왼쪽이 아니라 오른쪽 half인 이유

이전 리비전에서는 central이 왼쪽이었습니다. 6번 행의 마지막 4개 열(오른쪽 ctrl / 왼쪽 / 아래 / 오른쪽 화살표)이 매트릭스 포지션 128–131에 위치하는데, 이는 ZMK split BLE가 peripheral→central 포지션 보고에 사용하는 하드코딩된 `POS_STATE_LEN`(16바이트 = 128비트)를 초과합니다 — 그 결과 해당 키들이 조용히 입력되지 않게 되었습니다. central half 자신의 로컬 키 입력은 이 BLE 전송 구간을 거칠 필요가 없기 때문에, central을 오른쪽으로 옮겨서(이전에 오버플로우하던 열들을 relay가 아니라 central의 로컬 입력으로 만들어서) 이 한계를 완전히 피했습니다. 향후 매트릭스가 확장되어 같은 한계에 부딪히면, 해법은 동일합니다: 오버플로우하는 포지션들을 central인 쪽 half에 배치하면 됩니다.

### 커스텀 LED 드라이버 (존재하는 이유)

ZMK 내장 `zmk,indicator-leds` 디바이스트리 노드는 central 전용 함수(`zmk_hid_indicators_get_current_profile()`, `zmk_endpoint_is_connected()`)를 호출하는데, 이 함수들은 peripheral 빌드에서는 링크가 안 됩니다 — 그런데 이 보드의 두 상태 LED는 모두 물리적으로 peripheral(왼쪽) half에 배선되어 있습니다. 두 개의 커스텀 드라이버로 이 문제를 우회합니다:

- **`ksn1_peripheral_indicators.c`** — 대신 `zmk_hid_indicators_changed` 이벤트를 구독해서 Caps Lock LED를 구동합니다. 이 이벤트는 peripheral에서도 안전합니다(양쪽에 `CONFIG_ZMK_SPLIT_PERIPHERAL_HID_INDICATORS=y`가 설정되어 있으면 central이 호스트의 원시 HID indicator 비트마스크를 BLE로 밀어줌).
- **`ksn1_conn_status_relay_central.c`** / **`ksn1_conn_status_relay_peripheral.c`** — "central이 호스트와 연결되어 있는가"를 peripheral의 LED에 전달할 ZMK 내장 채널이 없기 때문에, 기존 split 링크 위에 전용 BLE GATT 서비스를 추가했습니다: central이 250ms마다 `zmk_endpoint_is_connected()`를 폴링하고, 상태가 바뀔 때마다 전용 characteristic으로 peripheral에 1바이트를 씁니다. peripheral은 연결됨이면 LED를 상시 점등, 아니면 300ms 간격으로 점멸시킵니다. 이 서브채널의 GATT discovery는 split 재연결 후 의도적으로 3초 지연시켜서, ZMK 자체의 critical-path discovery(포지션 상태, indicator, 배터리)와 연결당 하나뿐인 outstanding ATT 요청 슬롯을 두고 경쟁하지 않도록 했습니다.

이 세 파일은 `config/zephyr/module.yml`(`config/`를 Zephyr 모듈로 등록) + `config/CMakeLists.txt`(`target_sources(app PRIVATE src/...)`, `config/` 기준 상대 경로) + `config/src/` 아래의 소스 파일, 이렇게 세 곳이 맞물려 빌드에 연결됩니다. 커스텀 `.c` 파일을 추가할 때는 이 세 곳이 모두 일치해야 하며, 하나라도 빠지거나 잘못되면 빌드 에러 없이 조용히 그 파일이 빌드에서 빠집니다.

### 키맵 (`config/ksn_1.keymap`)

3개 레이어:

- **`default_layer`** — 왼쪽 열에 넘버패드가 통합된 Windows 기본 레이어. 인코더 = 볼륨 업/다운. 넘버패드 좌상단 키가 매크로(Win+R → `calc` → Enter)로 Windows 계산기를 실행.
- **`func_layer`** (모멘터리, `&mo 1`) — 블루투스 프로필 선택(0–4) 및 clear, 출력 토글(`OUT_TOG`), 인코더로 백라이트 증감, `mac_layer`로의 토글(`&tog 2`).
- **`mac_layer`** — 동일한 기본 배열에 Mac 방식 모디파이어 순서와 F행 대신 Mac 미디어/밝기 키. 넘버패드 코너 키는 대신 매크로(Cmd+Space → `calculator` → Enter)로 Spotlight를 실행하며, Spotlight의 인덱싱/애니메이션 지연을 고려해 대기시간을 더 길게 둠.

## 빌드

CI(`.github/workflows/`)는 ZMK 표준 재사용 워크플로우(`zmkfirmware/zmk/.github/workflows/build-user-config.yml@main`)를 사용합니다 — 별도 커스텀 빌드 로직 없음. `build.yaml`이 세 개의 타겟을 정의합니다:

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

이 저장소에 push하면 `ksn_1_left`, `ksn_1_right`, `settings_reset` 펌웨어가 GitHub Actions artifact로 빌드됩니다 — 워크플로우 실행 결과에서 `.uf2` 파일을 받으면 됩니다.

`west`로 로컬 빌드하려면:

```sh
west init -l config
west update
west build -p -b nice_nano_v2 -- -DSHIELD=ksn_1_left -DZMK_EXTRA_MODULES=$(pwd)/config
west build -p -b nice_nano_v2 -- -DSHIELD=ksn_1_right -DZMK_EXTRA_MODULES=$(pwd)/config
```

## 플래싱

nice!nano의 리셋 버튼을 더블탭해서 UF2 부트로더로 진입한 뒤, 마운트된 `NICENANO` 드라이브에 해당하는 `.uf2`(왼쪽 펌웨어는 왼쪽 half에, 오른쪽은 오른쪽 half에)를 드래그하면 됩니다. 양쪽 half 모두 플래시해야 합니다 — 서로 다른 펌웨어 이미지를 사용합니다(오른쪽/central 이미지만 `CONFIG_ZMK_SPLIT_ROLE_CENTRAL=y`, 인코더, conn-status relay의 central 쪽을 가짐).

## 재페어링 / 블루투스 본딩 초기화

두 메인 타겟과 함께 빌드되는 `settings_reset` artifact를 해당 half에 플래시하면 저장된 BLE 본딩이 초기화됩니다. 그 다음 해당 half에 정상 펌웨어를 다시 플래시하고 재페어링하세요.

## 알려진 해결 이슈 (참고용)

이 보드 개발 중 실제로 발견되어 고친 버그들입니다 — 같은 증상이 다시 나타났을 때(KSN-2나 Nexplit Keypad에서도 같은 패턴을 공유하므로) 빠르게 알아볼 수 있도록 남겨둡니다:

- **"페어링됨이지만 연결 안 됨"**: peripheral의 `kscan0`과 인코더 GPIO가 wakeup source로 등록되지 않아서, 저전력 모드에서 MCU를 깨울 인터럽트를 발생시키지 못했습니다. `kscan0`(공용 `ksn_1.dtsi`)에 `wakeup-source;`를 추가하고, 오른쪽/central `.conf`에 `CONFIG_PM_DEVICE=y`를 추가해서 해결했습니다. (EC11 인코더의 `alps,ec11` 바인딩은 `wakeup-source`를 **지원하지 않으므로** 거기에는 추가하지 마세요.) 전원관리를 아예 끄는 것(`CONFIG_PM=n`)도 진단 중에는 "해결"되는 것처럼 보였지만 채택하지 않았습니다 — 보드가 아예 슬립하지 않게 되어 배터리 수명이 나빠지는 것이지 진짜 해결책이 아닙니다.
- **`CONFIG_BT_MAX_CONN`을 올려서 블루투스 프로필 슬롯 부족 문제를 더 악화시킴**: 기본값(2)에서 3으로 올려서 BLE 프로필 슬롯을 늘리려 했으나, 오히려 연결 시도마다 `Security failed ... err 4`(`AUTH_REQUIREMENT`) 루프가 반복되었습니다 — 원래 해결하려던 슬롯 부족 증상(`err 9`)보다 명백히 더 나쁜 상태였습니다. 기본값으로 되돌렸습니다.
- **상태 LED 극성 문제("점멸하다가 연결되면 오히려 꺼짐")**: 왼쪽 half의 `status_led` 디바이스트리 노드가 `GPIO_ACTIVE_HIGH`였는데, `GPIO_ACTIVE_LOW`로 바꾸니 반전되어 보이던 동작이 고쳐졌습니다. LED의 켜짐/꺼짐 상태가 구동 코드의 의도와 반대로 보일 때 가장 먼저 확인해볼 만한 저렴한 체크포인트입니다.
- **부정확한 배터리 퍼센트**: 이전의 커스텀 `zmk,battery-voltage-divider` 노드(외부 저항 분배기)가 엉터리 값(528mV/0% 같은)을 냈습니다. nice!nano v2 보드 기본값(`zmk,battery-nrf-vddh`, 내장 VDDH 센싱)으로 대체해서 정확하게 보고되도록 했습니다.
- **디버깅 팁**: 특정 증상을 쫓는다고 `CONFIG_LOG_DEFAULT_LEVEL`을 손으로 올리지 마세요 — 이건 *전역* Zephyr 로그 레벨이라, 관련 없는 저수준 모듈(예: USB 드라이버)까지 디버그 로깅이 켜져서 로그 버퍼가 넘치고 정작 보고 싶은 메시지가 밀려날 수 있습니다. 대신 프로젝트의 `zmk-usb-logging` 빌드 스니펫(이미 `build.yaml`에 있음)을 사용하세요.

## 남아있는 의존성 (무해함)

`config/west.yml`은 이전 상태 LED 구현에서 쓰던 `zmk-poor-mans-led-indicator` 모듈을 여전히 가져오지만, 이제 `ksn1_conn_status_relay_*.c`가 해당 LED를 직접 구동하므로 그 모듈의 Kconfig 옵션은 전부 비활성화되어 있습니다(`CONFIG_INDICATOR_LED_WIDGET=n`) — 두 드라이버가 동시에 같은 LED를 건드리면 안 되기 때문입니다. 이 모듈은 매니페스트에 남아있는 사용하지 않는 짐일 뿐 기능적인 의존성은 아닙니다; 그대로 둬도 되고 나중에 제거해도 됩니다.
