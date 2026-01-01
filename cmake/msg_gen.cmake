# FML 消息生成模块
# 从 msg/*.msg 文件生成 C 头文件

set(MSG_DIR ${CMAKE_SOURCE_DIR}/msg)
set(MSG_OUTPUT_DIR ${CMAKE_BINARY_DIR}/msg)
set(MSG_GEN_SCRIPT ${CMAKE_SOURCE_DIR}/tool/msg_gen.py)

# 查找所有 .msg 文件
file(GLOB MSG_FILES "${MSG_DIR}/*.msg")

# 为每个 .msg 文件生成对应的 .h 文件名
set(MSG_HEADERS "")
foreach(msg_file ${MSG_FILES})
    get_filename_component(msg_name ${msg_file} NAME_WE)
    list(APPEND MSG_HEADERS "${MSG_OUTPUT_DIR}/${msg_name}.h")
endforeach()

# 添加总头文件
list(APPEND MSG_HEADERS "${MSG_OUTPUT_DIR}/messages.h")
list(APPEND MSG_HEADERS "${MSG_OUTPUT_DIR}/topic_registry.h")

# 自定义命令: 生成消息头文件
add_custom_command(
    OUTPUT ${MSG_HEADERS}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MSG_OUTPUT_DIR}
    COMMAND python3 ${MSG_GEN_SCRIPT} ${MSG_DIR} ${MSG_OUTPUT_DIR}
    DEPENDS ${MSG_FILES} ${MSG_GEN_SCRIPT}
    COMMENT "Generating message headers from .msg files"
    VERBATIM
)

# 自定义目标: fml_messages
add_custom_target(fml_messages ALL
    DEPENDS ${MSG_HEADERS}
)

# 创建接口库,用于包含头文件路径
add_library(fml_msg INTERFACE)

target_include_directories(fml_msg INTERFACE ${CMAKE_BINARY_DIR})
add_dependencies(fml_msg fml_messages)

# 打印生成的消息
list(LENGTH MSG_FILES msg_count)
message(STATUS "FML Messages: Found ${msg_count} message file(s)")
foreach(msg_file ${MSG_FILES})
    get_filename_component(msg_name ${msg_file} NAME)
    message(STATUS "  - ${msg_name}")
endforeach()
