# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
project(project)

if(DEFINED ZEPHYR_BASE)
    add_definitions(-DFML_ZEPHYR)
else()
    add_definitions(-DFML_LINUX)
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

target_sources(app PRIVATE src/main.c)
