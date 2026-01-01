# Zephyr 平台特定的源文件
add_library(fml_zephyr_platform INTERFACE)

# 添加平台实现源文件
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/time_for.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hrt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/queue.c
    ${CMAKE_SOURCE_DIR}/src/drivers/hrtimer/hrtcall.cpp
    ${CMAKE_SOURCE_DIR}/platforms/common/event.c
)

# 添加平台头文件目录
target_include_directories(app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}  # 允许 #include <fml_config/hrt.h>
    ${CMAKE_SOURCE_DIR}/platforms/common/include
    ${CMAKE_SOURCE_DIR}/platforms/common  # 允许 shared_thread 等
    ${CMAKE_SOURCE_DIR}/src
)

message(STATUS "Zephyr platform configured")
