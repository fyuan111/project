 
function(fml_register)
    set(oneValueArgs MAIN)
    set(multiValueArgs SRCS DEPENDS)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_MAIN)
        message(FATAL_ERROR "fml_register: MAIN is required")
    endif()

    if(NOT ARG_SRCS)
        message(FATAL_ERROR "fml_register: SRCS is required")
    endif()

    # Get module name from current directory
    get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    set(MODULE_FULL_NAME "app_${MODULE_NAME}")

    # Create static library
    add_library(${MODULE_FULL_NAME} STATIC ${ARG_SRCS})

    # Add include directories
    target_include_directories(${MODULE_FULL_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/platforms/common/include
        ${CMAKE_SOURCE_DIR}/platforms/common
        ${CMAKE_SOURCE_DIR}/src
    )

    # Platform-specific include directories
    if(DEFINED ENV{ZEPHYR_BASE})
        target_include_directories(${MODULE_FULL_NAME} PUBLIC
            ${CMAKE_SOURCE_DIR}/platforms/zephyr/include
            ${CMAKE_SOURCE_DIR}/src/lib
        )
        # Link to Zephyr app to inherit Zephyr includes
        target_link_libraries(${MODULE_FULL_NAME} PUBLIC zephyr_interface)
        # Link to fml_msg for generated message headers
        target_link_libraries(${MODULE_FULL_NAME} PUBLIC fml_msg)
    else()
        target_include_directories(${MODULE_FULL_NAME} PUBLIC
            ${CMAKE_SOURCE_DIR}/platforms/linux/include
            ${CMAKE_SOURCE_DIR}/src/lib
        )
    endif()

    # Link dependencies
    if(ARG_DEPENDS)
        target_link_libraries(${MODULE_FULL_NAME} PUBLIC ${ARG_DEPENDS})
    endif()

    # Collect to global list for registry generation
    set_property(GLOBAL APPEND PROPERTY FML_MODULE_LIST ${ARG_MAIN})
    set_property(GLOBAL APPEND PROPERTY FML_MODULE_LIBS ${MODULE_FULL_NAME})

    message(STATUS "    Registered: ${MODULE_FULL_NAME} -> ${ARG_MAIN}_main()")
endfunction()
