Run west build -s zmk/app -d "/tmp/tmp.ZpoDrjH2jE" -b "nice_nano//zmk"  -- -DZMK_CONFIG=/__w/ksn1-firmware/ksn1-firmware/config -DSHIELD="ksn_1_left" 
+ west build -s zmk/app -d /tmp/tmp.ZpoDrjH2jE -b nice_nano//zmk -- -DZMK_CONFIG=/__w/ksn1-firmware/ksn1-firmware/config -DSHIELD=ksn_1_left
-- west build: generating a build system
Loading Zephyr default modules (Zephyr base).
-- Application: /__w/ksn1-firmware/ksn1-firmware/zmk/app
-- CMake version: 3.31.6
-- Found Python3: /usr/bin/python3 (found suitable version "3.12.3", minimum required is "3.10") found components: Interpreter
-- Cache files will be written to: /__w/ksn1-firmware/ksn1-firmware/zephyr/.cache
-- Zephyr version: 4.1.0 (/__w/ksn1-firmware/ksn1-firmware/zephyr)
-- Found west (found suitable version "1.5.0", minimum required is "0.14.0")
CMake Deprecation Warning at keymap-module/modules/modules.cmake:53 (message):
-- Adding ZMK config directory as board root: /__w/ksn1-firmware/ksn1-firmware/config
  The `config/boards` folder is deprecated.  Please use a module instead.
  See https://zmk.dev/docs/hardware-integration/new-shield and
  https://zmk.dev/docs/development/module-creation for more information.
Call Stack (most recent call first):
  /__w/ksn1-firmware/ksn1-firmware/zephyr/cmake/modules/zephyr_module.cmake:126 (include)
  /__w/ksn1-firmware/ksn1-firmware/zephyr/cmake/modules/zephyr_default.cmake:134 (include)
  /__w/ksn1-firmware/ksn1-firmware/zephyr/share/zephyr-package/cmake/ZephyrConfig.cmake:66 (include)
  /__w/ksn1-firmware/ksn1-firmware/zephyr/share/zephyr-package/cmake/ZephyrConfig.cmake:92 (include_boilerplate)
  CMakeLists.txt:9 (find_package)


CMake Warning at /__w/ksn1-firmware/ksn1-firmware/zephyr/cmake/modules/boards.cmake:136 (message):
  BOARD_ROOT element without a 'boards' subdirectory:

  /__w/ksn1-firmware/ksn1-firmware/zmk-poor-mans-led-indicator

  Hints:

    - if your board directory is '/foo/bar/boards/my_board' then add '/foo/bar' to BOARD_ROOT, not the entire board directory
    - if in doubt, use absolute paths
Call Stack (most recent call first):
  /__w/ksn1-firmware/ksn1-firmware/zephyr/cmake/modules/zephyr_default.cmake:134 (include)
  /__w/ksn1-firmware/ksn1-firmware/zephyr/share/zephyr-package/cmake/ZephyrConfig.cmake:66 (include)
  /__w/ksn1-firmware/ksn1-firmware/zephyr/share/zephyr-package/cmake/ZephyrConfig.cmake:92 (include_boilerplate)
  CMakeLists.txt:9 (find_package)


-- Board: nice_nano, Revision: 2.0.0, qualifiers: nrf52840/zmk
-- Shield(s): ksn_1_left
Checking /__w/ksn1-firmware/ksn1-firmware/zmk-poor-mans-led-indicator
Checking /__w/ksn1-firmware/ksn1-firmware/zmk/app/module
Checking /__w/ksn1-firmware/ksn1-firmware/zmk/app
found /__w/ksn1-firmware/ksn1-firmware/zmk/app
nice_nano_nrf52840_zmk for _nrf52840_zmk for nice_nano with version 2.0.0
Checking /__w/ksn1-firmware/ksn1-firmware/config
Checking /__w/ksn1-firmware/ksn1-firmware/zephyr
-- ZMK Config directory: /__w/ksn1-firmware/ksn1-firmware/config
-- ZMK Config Kconfig: /__w/ksn1-firmware/ksn1-firmware/config/ksn_1.conf
-- ZMK Config Kconfig: /__w/ksn1-firmware/ksn1-firmware/config/ksn_1.conf
-- Using keymap file: /__w/ksn1-firmware/ksn1-firmware/config/ksn_1.keymap
-- ZEPHYR_TOOLCHAIN_VARIANT not set, trying to locate Zephyr SDK
-- Found host-tools: zephyr 0.16.9 (/opt/zephyr-sdk-0.16.9)
-- Found toolchain: zephyr 0.16.9 (/opt/zephyr-sdk-0.16.9)
-- Found Dtc: /opt/zephyr-sdk-0.16.9/sysroots/x86_64-pokysdk-linux/usr/bin/dtc (found suitable version "1.6.0", minimum required is "1.4.6")
-- Found BOARD.dts: /__w/ksn1-firmware/ksn1-firmware/zmk/app/boards/nicekeyboards/nice_nano/nice_nano_nrf52840_zmk.dts
-- Found devicetree overlay: /__w/ksn1-firmware/ksn1-firmware/zmk/app/boards/nicekeyboards/nice_nano/nice_nano_nrf52840_zmk_2_0_0.overlay
-- Found devicetree overlay: /__w/ksn1-firmware/ksn1-firmware/config/boards/shields/ksn_1/ksn_1_left.overlay
-- Found devicetree overlay: /__w/ksn1-firmware/ksn1-firmware/config/boards/shields/ksn_1/boards/nice_nano_nrf52840_zmk.overlay
-- Found devicetree overlay: /__w/ksn1-firmware/ksn1-firmware/config/ksn_1.keymap
-- Generated zephyr.dts: /tmp/tmp.ZpoDrjH2jE/zephyr/zephyr.dts
-- Generated pickled edt: /tmp/tmp.ZpoDrjH2jE/zephyr/edt.pickle
-- Generated devicetree_generated.h: /tmp/tmp.ZpoDrjH2jE/zephyr/include/generated/zephyr/devicetree_generated.h
-- Including generated dts.cmake file: /tmp/tmp.ZpoDrjH2jE/zephyr/dts.cmake

warning: ZMK_USB (defined at /__w/ksn1-firmware/ksn1-firmware/zmk/app/Kconfig:134) was assigned the
value 'y' but got the value 'n'. Check these unsatisfied dependencies: (!ZMK_SPLIT || (ZMK_SPLIT &&
ZMK_SPLIT_ROLE_CENTRAL)) (=n). See http://docs.zephyrproject.org/latest/kconfig.html#CONFIG_ZMK_USB
and/or look up ZMK_USB in the menuconfig/guiconfig interface. The Application Development Primer,
Setting Configuration Values, and Kconfig - Tips and Best Practices sections of the manual might be
helpful too.


warning: Deprecated symbol NFCT_PINS_AS_GPIOS is enabled.


warning: Deprecated symbol KSCAN is enabled.

Parsing /__w/ksn1-firmware/ksn1-firmware/zmk/app/Kconfig
Loaded configuration '/__w/ksn1-firmware/ksn1-firmware/zmk/app/boards/nicekeyboards/nice_nano/nice_nano_nrf52840_zmk_2_0_0_defconfig'
Merged configuration '/__w/ksn1-firmware/ksn1-firmware/zmk/app/prj.conf'
Merged configuration '/__w/ksn1-firmware/ksn1-firmware/config/boards/shields/ksn_1/ksn_1_left.conf'
Merged configuration '/__w/ksn1-firmware/ksn1-firmware/config/ksn_1.conf'
Merged configuration '/__w/ksn1-firmware/ksn1-firmware/config/ksn_1.conf'
Configuration saved to '/tmp/tmp.ZpoDrjH2jE/zephyr/.config'
Kconfig header saved to '/tmp/tmp.ZpoDrjH2jE/zephyr/include/generated/zephyr/autoconf.h'
-- Found GnuLd: /opt/zephyr-sdk-0.16.9/arm-zephyr-eabi/arm-zephyr-eabi/bin/ld.bfd (found version "2.38")
-- The C compiler identification is GNU 12.2.0
-- The CXX compiler identification is GNU 12.2.0
-- The ASM compiler identification is GNU
-- Found assembler: /opt/zephyr-sdk-0.16.9/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc
-- Using ccache: /usr/bin/ccache
-- Configuring done (4.3s)
-- Generating done (0.1s)
-- Build files have been written to: /tmp/tmp.ZpoDrjH2jE
-- west build: building application
[1/412] Preparing syscall dependency handling

[2/412] Generating include/generated/zephyr/app_version.h
[3/412] Generating include/generated/zephyr/version.h
-- Zephyr version: 4.1.0 (/__w/ksn1-firmware/ksn1-firmware/zephyr), build: 9df4b12b5af3
[4/412] Generating misc/generated/syscalls_subdirs.trigger
[5/412] Generating misc/generated/syscalls.json, misc/generated/struct_tags.json
[6/412] Generating include/generated/device-api-sections.ld, include/generated/device-api-sections.cmake
[7/412] Generating include/generated/zephyr/syscall_dispatch.c, include/generated/zephyr/syscall_exports_llext.c, syscall_weakdefs_llext.c, include/generated/zephyr/syscall_list.h
[8/412] Generating include/generated/zephyr/kobj-types-enum.h, include/generated/zephyr/otype-to-str.h, include/generated/zephyr/otype-to-size.h
[9/412] Generating include/generated/zephyr/driver-validation.h
[10/412] Building C object zephyr/CMakeFiles/offsets.dir/arch/arm/core/offsets/offsets.c.obj
[11/412] Generating include/generated/zephyr/offsets.h
[12/412] Building C object CMakeFiles/app.dir/src/stdlib.c.obj
[13/412] Building C object CMakeFiles/app.dir/__w/ksn1-firmware/ksn1-firmware/config/src/ksn1_conn_status_relay_central.c.obj
[14/412] Building C object CMakeFiles/app.dir/__w/ksn1-firmware/ksn1-firmware/config/src/ksn1_peripheral_indicators.c.obj
[15/412] Building C object CMakeFiles/app.dir/src/boot/bootmode_to_magic_mapper.c.obj
[16/412] Building C object CMakeFiles/app.dir/__w/ksn1-firmware/ksn1-firmware/config/src/ksn1_conn_status_relay_peripheral.c.obj
FAILED: CMakeFiles/app.dir/__w/ksn1-firmware/ksn1-firmware/config/src/ksn1_conn_status_relay_peripheral.c.obj 
ccache /opt/zephyr-sdk-0.16.9/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc -DKERNEL -DK_HEAP_MEM_POOL_SIZE=0 -DMBEDTLS_CONFIG_FILE=\"config-tls-generic.h\" -DNRF52840_XXAA -DPICOLIBC_DOUBLE_PRINTF_SCANF -D__LINUX_ERRNO_EXTENSIONS__ -D__PROGRAM_START -D__ZEPHYR__=1 -I/__w/ksn1-firmware/ksn1-firmware/zmk/app/include -I/tmp/tmp.ZpoDrjH2jE/zephyr/include/generated/zephyr -I/__w/ksn1-firmware/ksn1-firmware/zephyr/include -I/tmp/tmp.ZpoDrjH2jE/zephyr/include/generated -I/__w/ksn1-firmware/ksn1-firmware/zephyr/soc/nordic -I/__w/ksn1-firmware/ksn1-firmware/zephyr/soc/nordic/nrf52/. -I/__w/ksn1-firmware/ksn1-firmware/zephyr/soc/nordic/common/. -I/__w/ksn1-firmware/ksn1-firmware/zephyr/subsys/usb/device -I/__w/ksn1-firmware/ksn1-firmware/zephyr/subsys/bluetooth/controller/ll_sw/nordic/hal/nrf5/nrfx_glue -I/__w/ksn1-firmware/ksn1-firmware/zephyr/subsys/bluetooth -I/__w/ksn1-firmware/ksn1-firmware/zephyr/subsys/settings/include -I/__w/ksn1-firmware/ksn1-firmware/zephyr/drivers/usb/common/nrf_usbd_common/. -I/__w/ksn1-firmware/ksn1-firmware/modules/hal/cmsis/CMSIS/Core/Include -I/__w/ksn1-firmware/ksn1-firmware/zephyr/modules/cmsis/. -I/__w/ksn1-firmware/ksn1-firmware/modules/hal/nordic/nrfx -I/__w/ksn1-firmware/ksn1-firmware/modules/hal/nordic/nrfx/drivers/include -I/__w/ksn1-firmware/ksn1-firmware/modules/hal/nordic/nrfx/mdk -I/__w/ksn1-firmware/ksn1-firmware/zephyr/modules/hal_nordic/nrfx/. -I/__w/ksn1-firmware/ksn1-firmware/zmk/app/module/include -I/__w/ksn1-firmware/ksn1-firmware/zmk/app/module/drivers/sensor/battery/. -I/__w/ksn1-firmware/ksn1-firmware/modules/crypto/mbedtls/include -I/__w/ksn1-firmware/ksn1-firmware/zephyr/modules/mbedtls/configs -I/__w/ksn1-firmware/ksn1-firmware/zephyr/modules/mbedtls/include -isystem /__w/ksn1-firmware/ksn1-firmware/zephyr/lib/libc/common/include -fno-strict-aliasing -Os -imacros /tmp/tmp.ZpoDrjH2jE/zephyr/include/generated/zephyr/autoconf.h -fno-common -g -gdwarf-4 -fdiagnostics-color=always -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mfp16-format=ieee -mtp=soft --sysroot=/opt/zephyr-sdk-0.16.9/arm-zephyr-eabi/arm-zephyr-eabi -imacros /__w/ksn1-firmware/ksn1-firmware/zephyr/include/zephyr/toolchain/zephyr_stdint.h -Wall -Wformat -Wformat-security -Wno-format-zero-length -Wdouble-promotion -Wno-pointer-sign -Wpointer-arith -Wexpansion-to-defined -Wno-unused-but-set-variable -Werror=implicit-int -fno-pic -fno-pie -fno-asynchronous-unwind-tables -ftls-model=local-exec -fno-reorder-functions --param=min-pagesize=0 -fno-defer-pop -fmacro-prefix-map=/__w/ksn1-firmware/ksn1-firmware/zmk/app=CMAKE_SOURCE_DIR -fmacro-prefix-map=/__w/ksn1-firmware/ksn1-firmware/zephyr=ZEPHYR_BASE -fmacro-prefix-map=/__w/ksn1-firmware/ksn1-firmware=WEST_TOPDIR -ffunction-sections -fdata-sections -specs=picolibc.specs -std=c99 -Wfatal-errors -MD -MT CMakeFiles/app.dir/__w/ksn1-firmware/ksn1-firmware/config/src/ksn1_conn_status_relay_peripheral.c.obj -MF CMakeFiles/app.dir/__w/ksn1-firmware/ksn1-firmware/config/src/ksn1_conn_status_relay_peripheral.c.obj.d -o CMakeFiles/app.dir/__w/ksn1-firmware/ksn1-firmware/config/src/ksn1_conn_status_relay_peripheral.c.obj -c /__w/ksn1-firmware/ksn1-firmware/config/src/ksn1_conn_status_relay_peripheral.c
/__w/ksn1-firmware/ksn1-firmware/config/src/ksn1_conn_status_relay_peripheral.c:123:98: error: macro "BT_GATT_ATTRIBUTE" passed 22 arguments, but takes just 5
  123 |                                                 BT_GATT_PERM_WRITE_ENCRYPT, NULL, on_write, NULL));
      |                                                                                                  ^
compilation terminated due to -Wfatal-errors.
[17/412] Building C object CMakeFiles/app.dir/src/activity.c.obj
[18/412] Building C object CMakeFiles/app.dir/src/matrix_transform.c.obj
[19/412] Building C object CMakeFiles/app.dir/src/behavior.c.obj
[20/412] Building C object CMakeFiles/app.dir/src/sensors.c.obj
[21/412] Building C object CMakeFiles/app.dir/src/physical_layouts.c.obj
ninja: build stopped: subcommand failed.
FATAL ERROR: command exited with status 1: /usr/local/bin/cmake --build /tmp/tmp.ZpoDrjH2jE
Error: Process completed with exit code 1.
