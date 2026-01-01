# SPDX-License-Identifier: Apache-2.0
#
# FML (Formula) - Main CMake Entry Point
#
# This file detects the platform and delegates to platform-specific build files:
#   - Zephyr: cmake/zephyr.cmake
#   - Linux:  cmake/linux.cmake
#

cmake_minimum_required(VERSION 3.21.0)

# ==================== 平台检测 ====================
if(DEFINED ENV{ZEPHYR_BASE})
    # Zephyr 平台
    message(STATUS "Platform detected: Zephyr")
    add_definitions(-DFML_ZEPHYR)

    # 禁用静态局部变量的线程安全初始化（避免 __cxa_guard_* 问题）
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>)

    # Zephyr 需要在 project() 之前 find_package
    find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
    project(fml_zephyr)

    # 加载 Zephyr 构建配置
    include(cmake/zephyr.cmake)

else()
    # Linux 平台
    message(STATUS "Platform detected: Linux")
    add_definitions(-DFML_LINUX)

    project(fml_linux CXX)

    # 加载 Linux 构建配置
    include(cmake/linux.cmake)

endif()

# 构建完成
message(STATUS "FML build configuration complete")
