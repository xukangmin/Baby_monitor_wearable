# list(APPEND ZEPHYR_EXTRA_MODULES
#  ${CMAKE_CURRENT_SOURCE_DIR}/max30102
# )

list(APPEND ZEPHYR_EXTRA_MODULES
  ${CMAKE_CURRENT_SOURCE_DIR}/max32664
  ${CMAKE_CURRENT_SOURCE_DIR}/max30205
)


cmake_minimum_required(VERSION 3.8.2)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NONE)

target_sources(app PRIVATE 
  src/main.c
  src/sphr.c
)