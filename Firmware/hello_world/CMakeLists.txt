cmake_minimum_required(VERSION 3.8.2)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NONE)
zephyr_include_directories(inc)        #Add this line
include_directories(inc)

target_sources(app PRIVATE src/multiply.c)  #Add this line
target_sources(app PRIVATE src/main.c)