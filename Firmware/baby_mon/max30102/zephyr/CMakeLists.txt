# Makefile - MAX30101 heart rate sensor
#
# Copyright (c) 2017, NXP
#
# SPDX-License-Identifier: Apache-2.0
#


if(CONFIG_MAX30102)
  # Add hello_world_driver.h to the set of global include paths.
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    max30102.c
    )
endif()