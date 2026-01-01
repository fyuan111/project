cmake_minimum_required(VERSION 3.20.0)

# 查找 Zephyr 包（必须在 project() 之前）
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# 项目定义
project(workqueue_latency_test)

# 定义平台宏
add_definitions(-DFML_ZEPHYR)

# 添加 FML 库的包含路径
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platforms/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platforms/zephyr/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../..  # 为了找到 fml_config
)

# 添加测试源文件
target_sources(app PRIVATE
    src/test_latency.cpp
)

# 添加 FML 库源文件（需要被测试的代码）
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platforms/common/shared_thread/workqueue_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platforms/common/shared_thread/workqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platforms/zephyr/src/hrt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platforms/zephyr/src/event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platforms/zephyr/src/thread.cpp
    # 如果有其他依赖，在这里添加
)
