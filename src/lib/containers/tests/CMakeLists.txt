cmake_minimum_required(VERSION 3.10)
project(FML_Containers_Tests)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# ============ 测试可执行文件 ============

# TimeRing 测试
add_executable(time_ring_test time_ring_test.cpp)

# Heap 测试
add_executable(heap_test heap_test.cpp)

# ============ 测试命令 ============

enable_testing()

add_test(NAME TimeRingTest COMMAND time_ring_test)
add_test(NAME HeapTest COMMAND heap_test)

# ============ 便捷目标 ============

# 运行所有测试
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS time_ring_test heap_test
    COMMENT "Running all container tests..."
)
