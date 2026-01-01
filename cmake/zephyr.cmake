# Zephyr 平台构建配置

# 检测并验证平台
if(NOT DEFINED ENV{ZEPHYR_BASE})
    message(FATAL_ERROR "zephyr.cmake should only be included when ZEPHYR_BASE environment variable is defined")
endif()

message(STATUS "Configuring for Zephyr platform")

# 添加 Zephyr 平台宏定义
add_compile_definitions(FML_ZEPHYR)

# 包含 CMake 函数
include(cmake/register.cmake)
include(cmake/generate_registry.cmake)
include(cmake/msg_gen.cmake)

# 添加平台相关代码
add_subdirectory(platforms/zephyr)
add_subdirectory(platforms/common)

# 添加库
add_subdirectory(src/lib/perf)

# 自动发现并添加所有应用模块
message(STATUS "Scanning for application modules...")
file(GLOB app_dirs LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/src/Applications/*")

foreach(app_dir ${app_dirs})
    if(IS_DIRECTORY ${app_dir})
        # 检查是否有 CMakeLists.txt
        if(EXISTS "${app_dir}/CMakeLists.txt")
            get_filename_component(app_name ${app_dir} NAME)
            message(STATUS "  Found: ${app_name}")
            add_subdirectory(${app_dir})
        endif()
    endif()
endforeach()

# 生成注册表
set(REGISTRY_FILE ${CMAKE_BINARY_DIR}/module_registry.gen.cpp)
fml_generate_registry(${REGISTRY_FILE})

# 添加注册表到 Zephyr app（main.cpp 已在 platforms/zephyr/CMakeLists.txt 中添加）
target_sources(app PRIVATE
    ${REGISTRY_FILE}
)

# 链接所有模块库
get_property(module_libs GLOBAL PROPERTY FML_MODULE_LIBS)
target_link_libraries(app PRIVATE ${module_libs} fml_perf fml_idc fml_msg)

message(STATUS "Zephyr build configured")
