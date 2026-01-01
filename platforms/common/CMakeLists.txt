# FML Common Platform Layer
add_library(fml_common INTERFACE)

# 添加 common 平台的头文件目录
target_include_directories(fml_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 工作队列源文件
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared_thread/workitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared_thread/workqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared_thread/workqueue_manager.cpp
)

# 添加 IDC 库
add_subdirectory(idc)

message(STATUS "Common platform layer configured")
