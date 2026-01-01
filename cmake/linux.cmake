# Linux 平台构建配置

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
add_compile_options(-Wall -Wextra)

# 包含 CMake 函数
include(cmake/register.cmake)
include(cmake/generate_registry.cmake)

# 添加平台相关代码
add_subdirectory(platforms/linux)
add_subdirectory(platforms/common)

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

# 创建可执行文件
add_executable(fml_main
    src/main.cpp
    ${REGISTRY_FILE}
)

# 链接所有模块库
get_property(module_libs GLOBAL PROPERTY FML_MODULE_LIBS)
target_link_libraries(fml_main
    ${module_libs}
    pthread  # Linux 需要 pthread
)

# 设置输出目录
set_target_properties(fml_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Linux build configured")
message(STATUS "  Executable: ${CMAKE_BINARY_DIR}/bin/fml_main")
