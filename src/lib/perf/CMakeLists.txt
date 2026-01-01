# FML Performance Monitoring Library
add_library(fml_perf INTERFACE)

# 添加头文件目录
target_include_directories(fml_perf INTERFACE
    ${CMAKE_SOURCE_DIR}/src/lib
)

# perf.hpp 是 header-only，不需要添加源文件
# 如果将来 perf.cpp 有实现，可以这样添加：
# target_sources(fml_perf INTERFACE
#     ${CMAKE_CURRENT_SOURCE_DIR}/perf.cpp
# )

message(STATUS "Performance library configured")
