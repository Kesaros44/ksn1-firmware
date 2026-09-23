# ksn1-firmware

ZMK firmware for the KSN-1 split keyboard — the original design that KSN-2 and the Nexplit Keypad are both derived from.

* Keyboard Maintainer: [AJG](https://github.com/Kesaros44)
* Hardware Supported: KSN-1 split keyboard, nice!nano v2 (nRF52840), BLE

![KSN-1 완성 사진](images/ksn1_board.webp)

## Hardware

- **MCU:** nice!nano v2 (nRF52840) per half, wireless (BLE)
- **Central:** right half
- **Matrix:** 6 rows × 22 columns combined (11 per half), `col2row`
- **Encoder:** one EC11 encoder, right half only
- **LEDs (left half only):** Caps Lock LED, BLE connection-status LED
- **Backlight:** designed into the schematic, but parts aren't populated and the feature is disabled in firmware (`CONFIG_ZMK_BACKLIGHT=n`) — no backlight on the physical board
- **RGB underglow:** none
- **Battery:** reported from both halves

## Keymap (`config/ksn_1.keymap`)

Three layers:

- **`default_layer`** — Windows base layer with an integrated numpad on the left. Encoder = volume. Numpad corner key runs a macro (Win+R → `calc` → Enter) to launch the calculator.
- **`mac_layer`** — same layout with Mac modifier order and Mac media/brightness keys; numpad corner key instead runs Cmd+Space → Spotlight calculator.
- **`func_layer`** (hold `&mo 2`; top layer, so FN always takes priority in both Windows and Mac modes) — Bluetooth profile select (0–4) and clear, output toggle, backlight inc/dec on the encoder (no-op — no backlight hardware), toggle (`&tog 1`) into `mac_layer`.

## Building

GitHub Actions builds on every push — grab the `.uf2` files (`ksn_1_left`, `ksn_1_right`, `settings_reset`) from the workflow run's artifacts.

Local build with `west`:

```sh
west init -l config
west update
west build -p -b nice_nano_v2 -- -DSHIELD=ksn_1_left -DZMK_EXTRA_MODULES=$(pwd)/config
west build -p -b nice_nano_v2 -- -DSHIELD=ksn_1_right -DZMK_EXTRA_MODULES=$(pwd)/config
```

## Flashing

Double-tap reset on the nice!nano to enter the UF2 bootloader, then drag the matching `.uf2` onto the `NICENANO` drive (left firmware → left half, right → right half). Flash both halves — they run different images.

## Re-pairing / clearing Bluetooth bonds

Flash the `settings_reset` artifact to a half to wipe its BLE bonds, then reflash normal firmware and re-pair.

## Recent Changes

- Added dedicated Hangul (`LANG1`) / Hanja (`LANG2`) keys to the right-half thumb cluster; on macOS the language toggle uses Caps Lock instead (`LANG1` is read there as a JIS kana key, not Hangul/English).
- Fixed `word_flip` (the Hangul/English auto-correct trigger) doing nothing at all (usage-page mismatch in the letter check, wrong `&kp` behavior device name).
- Fixed the status LED staying stuck blinking over a USB connection.
- Added USB HID settings for compatibility with corporate security software over a wired connection (HKRO fixed at 6-key, boot protocol, BASIC consumer usages, dedicated VID/PID).
- Raised BLE TX power by +8dBm for more reliable connections.

## Known Issues / TODO

- **USB PID not registered:** `CONFIG_USB_DEVICE_PID=0x4B53` is a temporary placeholder. Needs a real PID from [pid.codes](https://pid.codes) before any commercial sale.
- **No right RCTRL/RGUI:** the Hangul/Hanja keys took over what used to be RCTRL and RGUI on the right half — those modifiers now live only on the left half.
- **mac_layer Hanja key may need Option+Return:** [KSN-2](https://github.com/Kesaros44/ksn2-firmware)'s `mac_layer` sends `LA(RET)` (Option+Return, macOS's actual Hanja shortcut) for its Hanja key because `LANG2` does nothing there; this board's `mac_layer` still sends plain `LANG2`. Worth porting the same fix.
- **word_flip's macOS delete may share a word-boundary bug found on KSN-3:** on 2026-09-16, [KSN-3](https://github.com/Kesaros44/ksn3-firmware) found that Option+Backspace doesn't respect word boundaries inside Hangul IME composition and switched to resending plain Backspace instead. This board still uses Option+Backspace for that step — check whether it needs the same fix.
- **Modifier keys may still reset the word_flip buffer:** pressing Shift (or any other non-letter key) clears the tracked word, which could truncate a capital letter typed mid-word. Not yet confirmed fixed.

---

# ksn1-firmware (한국어)

KSN-1 스플릿 키보드용 ZMK 펌웨어 설정입니다 — KSN-2와 Nexplit Keypad가 파생된 원조 설계입니다.

* 키보드 관리자: [AJG](https://github.com/Kesaros44)
* 지원 하드웨어: KSN-1 스플릿 키보드, nice!nano v2 (nRF52840), BLE

## 하드웨어

- **MCU:** half마다 nice!nano v2 (nRF52840), 무선(BLE)
- **Central:** 오른쪽 half
- **매트릭스:** 6행 × 22열 (half당 11열), `col2row`
- **인코더:** EC11 인코더 1개, 오른쪽 half에만 있음
- **LED (왼쪽 half 전용):** Caps Lock LED, BLE 연결상태 LED
- **백라이트:** 회로도에는 설계되어 있지만 부품이 실장되지 않았고 펌웨어 기능도 꺼져 있음(`CONFIG_ZMK_BACKLIGHT=n`) — 실제 보드에는 백라이트 없음
- **RGB 언더글로우:** 없음
- **배터리:** 양쪽 half 모두 보고

## 키맵 (`config/ksn_1.keymap`)

3개 레이어:

- **`default_layer`** — 왼쪽에 넘버패드가 통합된 Windows 기본 레이어. 인코더 = 볼륨. 넘버패드 코너 키가 매크로(Win+R → `calc` → Enter)로 계산기 실행.
- **`mac_layer`** — 동일한 배열에 Mac 모디파이어 순서와 Mac 미디어/밝기 키; 넘버패드 코너 키는 대신 Cmd+Space → Spotlight 계산기 실행.
- **`func_layer`** (홀드 `&mo 2`, 맨 위 레이어 — Windows/Mac 모드와 관계없이 FN을 누르면 항상 우선) — 블루투스 프로필 선택(0–4) 및 clear, 출력 토글, 인코더로 백라이트 증감(백라이트 하드웨어 자체가 없어서 실제 동작은 없음), `mac_layer`로의 토글(`&tog 1`).

## 빌드

GitHub Actions가 push마다 자동으로 빌드합니다 — 워크플로우 실행의 아티팩트에서 `.uf2` 파일(`ksn_1_left`, `ksn_1_right`, `settings_reset`)을 받으면 됩니다.

`west`로 로컬 빌드:

```sh
west init -l config
west update
west build -p -b nice_nano_v2 -- -DSHIELD=ksn_1_left -DZMK_EXTRA_MODULES=$(pwd)/config
west build -p -b nice_nano_v2 -- -DSHIELD=ksn_1_right -DZMK_EXTRA_MODULES=$(pwd)/config
```

## 플래싱

nice!nano의 리셋 버튼을 더블탭해서 UF2 부트로더로 진입한 뒤, 마운트된 `NICENANO` 드라이브에 해당하는 `.uf2`(왼쪽 → 왼쪽 half, 오른쪽 → 오른쪽 half)를 드래그하면 됩니다. 양쪽 half 모두 플래시해야 합니다 — 서로 다른 이미지를 사용합니다.

## 재페어링 / 블루투스 본딩 초기화

`settings_reset` artifact를 해당 half에 플래시하면 BLE 본딩이 초기화됩니다. 그 다음 정상 펌웨어를 다시 플래시하고 재페어링하세요.

## 최근 변경 사항

- 오른쪽 half 엄지 클러스터에 전용 한/영(`LANG1`)·한자(`LANG2`) 키 추가. macOS에서는 `LANG1`이 한/영 전환이 아니라 JIS 가나 키로 인식되기 때문에, 언어 전환을 Caps Lock 방식으로 대체함.
- 한/영 오타 자동 되돌리기(`word_flip`)가 아무 동작도 하지 않던 버그 수정 (글자 인식 시 usage page 불일치, `&kp` behavior 디바이스명 오류).
- 유선(USB) 연결 시 상태 LED가 계속 점멸하던 버그 수정.
- 유선 연결 시 사내 보안 프로그램 호환을 위한 USB HID 설정 추가 (HKRO 6키 고정, 부트 프로토콜, 컨슈머 리포트 BASIC 제한, 전용 VID/PID).
- BLE 연결 안정성을 위해 TX 파워 +8dBm 상향.

## 알려진 이슈 / TODO

- **USB PID 미등록:** `CONFIG_USB_DEVICE_PID=0x4B53`은 임시로 지정한 값입니다. 정식 판매 전 [pid.codes](https://pid.codes)에서 정식 PID를 할당받아야 합니다.
- **오른쪽 RCTRL/RGUI 부재:** 한/영·한자 키가 그 자리를 대체하면서, 해당 모디파이어는 왼쪽 half에만 남았습니다.
- **mac_layer 한자 키는 Option+Return이 필요할 수 있음:** [KSN-2](https://github.com/Kesaros44/ksn2-firmware)는 `mac_layer`의 한자 키를 `LA(RET)`(Option+Return, macOS의 실제 한자 변환 단축키)로 이미 바꿨습니다 — `LANG2`가 macOS에서 아무 동작도 안 하기 때문입니다. 이 저장소의 `mac_layer`는 아직 `LANG2` 그대로라 동일하게 수정하는 게 좋습니다.
- **word_flip의 macOS 삭제 방식이 KSN-3에서 발견된 단어 경계 버그를 공유할 수 있음:** 2026-09-16, [KSN-3](https://github.com/Kesaros44/ksn3-firmware)에서 Option+Backspace가 한글 IME 조합 중 단어 경계를 지키지 않는 문제가 발견되어 일반 Backspace 반복 전송 방식으로 교체했습니다. 이 저장소는 아직 Option+Backspace를 쓰고 있어 같은 문제가 있을 수 있으니 확인이 필요합니다.
- **수식키가 word_flip 버퍼를 여전히 리셋시킬 수 있음:** Shift 등 알파벳이 아닌 키를 누르면 인식 중이던 단어가 초기화되어, 단어 중간의 대문자가 잘릴 수 있습니다. 아직 수정 여부가 확인되지 않았습니다.
