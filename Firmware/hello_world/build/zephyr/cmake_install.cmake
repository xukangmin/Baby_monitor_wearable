# Install script for directory: C:/Users/kangmin/ncs/v1.5.0/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Zephyr-Kernel")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/arch/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/lib/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/soc/arm/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/boards/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/subsys/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/drivers/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/nrf/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/mcuboot/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/nrfxlib/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/tfm/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/cddl-gen/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/cmsis/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/canopennode/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/civetweb/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/fatfs/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/hal_nordic/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/st/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/libmetal/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/lvgl/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/mbedtls/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/mcumgr/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/open-amp/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/loramac-node/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/openthread/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/segger/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/tinycbor/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/tinycrypt/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/littlefs/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/mipi-sys-t/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/nrf_hw_models/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/modules/connectedhomeip/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/kernel/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/cmake/flash/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/cmake/usage/cmake_install.cmake")
  include("C:/Users/kangmin/Documents/GitHub/Baby_monitor_wearable/Firmware/hello_world/build/zephyr/cmake/reports/cmake_install.cmake")

endif()

