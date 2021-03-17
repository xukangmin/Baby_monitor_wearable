@echo off
cd /D C:\Users\kangmin\Documents\GitHub\Baby_monitor_wearable\Firmware\hello_world\nrf53dk\zephyr\kconfig || (set FAIL_LINE=2& goto :ABORT)
"C:\Program Files\CMake\bin\cmake.exe" -E env ZEPHYR_BASE=C:/Users/kangmin/zephyrproject/zephyr ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb PYTHON_EXECUTABLE=C:/Python39/python.exe srctree=C:/Users/kangmin/zephyrproject/zephyr KERNELVERSION=0x2056300 KCONFIG_CONFIG=C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/nrf53dk/zephyr/.config ARCH=arm ARCH_DIR=C:/Users/kangmin/zephyrproject/zephyr/arch BOARD_DIR=C:/Users/kangmin/zephyrproject/zephyr/boards/arm/nrf5340dk_nrf5340 KCONFIG_BINARY_DIR=C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/nrf53dk/Kconfig TOOLCHAIN_KCONFIG_DIR=C:/Users/kangmin/zephyrproject/zephyr/cmake/toolchain/gnuarmemb EDT_PICKLE=C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/nrf53dk/zephyr/edt.pickle ZEPHYR_CMSIS_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/cmsis ZEPHYR_ATMEL_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/atmel ZEPHYR_ALTERA_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/altera ZEPHYR_CANOPENNODE_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/lib/canopennode ZEPHYR_CIVETWEB_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/lib/civetweb ZEPHYR_ESPRESSIF_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/espressif ZEPHYR_FATFS_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/fs/fatfs ZEPHYR_CYPRESS_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/cypress ZEPHYR_INFINEON_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/infineon ZEPHYR_HAL_NORDIC_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/nordic ZEPHYR_HAL_NORDIC_KCONFIG=C:/Users/kangmin/zephyrproject/zephyr/modules/hal_nordic/Kconfig ZEPHYR_OPENISA_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/openisa ZEPHYR_NUVOTON_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/nuvoton ZEPHYR_MICROCHIP_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/microchip ZEPHYR_SILABS_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/silabs ZEPHYR_ST_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/st ZEPHYR_STM32_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/stm32 ZEPHYR_TI_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/ti ZEPHYR_LIBMETAL_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/libmetal ZEPHYR_QUICKLOGIC_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/quicklogic ZEPHYR_LVGL_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/lib/gui/lvgl ZEPHYR_MBEDTLS_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/crypto/mbedtls ZEPHYR_MCUBOOT_MODULE_DIR=C:/Users/kangmin/zephyrproject/bootloader/mcuboot ZEPHYR_MCUMGR_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/lib/mcumgr ZEPHYR_NXP_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/nxp ZEPHYR_OPEN_AMP_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/lib/open-amp ZEPHYR_LORAMAC_NODE_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/lib/loramac-node ZEPHYR_OPENTHREAD_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/lib/openthread ZEPHYR_SEGGER_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/debug/segger ZEPHYR_SOF_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/audio/sof ZEPHYR_TINYCBOR_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/lib/tinycbor ZEPHYR_TINYCRYPT_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/crypto/tinycrypt ZEPHYR_LITTLEFS_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/fs/littlefs ZEPHYR_MIPI_SYS_T_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/debug/mipi-sys-t ZEPHYR_NRF_HW_MODELS_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/bsim_hw_models/nrf_hw_models ZEPHYR_XTENSA_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/hal/xtensa ZEPHYR_TRUSTED_FIRMWARE_M_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/tee/tfm ZEPHYR_TRUSTED_FIRMWARE_M_KCONFIG=C:/Users/kangmin/zephyrproject/zephyr/modules/trusted-firmware-m/Kconfig ZEPHYR_TFM_MCUBOOT_MODULE_DIR=C:/Users/kangmin/zephyrproject/modules/tee/tfm-mcuboot SHIELD_AS_LIST= EXTRA_DTC_FLAGS=-Wno-unique_unit_address_if_enabled DTS_POST_CPP=C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/nrf53dk/zephyr/nrf5340dk_nrf5340_cpuapp.dts.pre.tmp DTS_ROOT_BINDINGS=C:/Users/kangmin/zephyrproject/zephyr/dts/bindings C:/Python39/python.exe C:/Users/kangmin/zephyrproject/zephyr/scripts/kconfig/guiconfig.py C:/Users/kangmin/zephyrproject/zephyr/Kconfig || (set FAIL_LINE=3& goto :ABORT)
goto :EOF

:ABORT
set ERROR_CODE=%ERRORLEVEL%
echo Batch file failed at line %FAIL_LINE% with errorcode %ERRORLEVEL%
exit /b %ERROR_CODE%