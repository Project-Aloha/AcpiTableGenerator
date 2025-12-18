cmake_minimum_required(VERSION 3.10)
project(acpi-table-generator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

# 查找 iasl 工具
find_program(IASL_EXECUTABLE iasl)
if(IASL_EXECUTABLE)
    message(STATUS "找到 iasl: ${IASL_EXECUTABLE}")
    set(IASL_AVAILABLE TRUE)
else()
    message(WARNING "未找到 iasl 工具，将跳过 DSL 反编译步骤")
    set(IASL_AVAILABLE FALSE)
endif()

# 扫描 include 目录下的所有子目录
file(GLOB PLATFORM_DIRS "${CMAKE_SOURCE_DIR}/include/*")

# 为每个平台创建独立的构建目标
foreach(PLATFORM_DIR ${PLATFORM_DIRS})
    if(IS_DIRECTORY ${PLATFORM_DIR})
        # 提取平台名称（如 sm8550, sm8850）
        get_filename_component(PLATFORM_NAME ${PLATFORM_DIR} NAME)
        
        # 跳过 common 目录（它包含通用定义，不是平台）
        if(PLATFORM_NAME STREQUAL "common")
            continue()
        endif()
        
        message(STATUS "配置平台: ${PLATFORM_NAME}")
        
        # 创建该平台的可执行文件目标
        set(TARGET_NAME pptt_generator_${PLATFORM_NAME})
        add_executable(${TARGET_NAME} src/pptt.c)
        
        # 设置该目标的包含目录
        target_include_directories(${TARGET_NAME} PRIVATE 
            ${CMAKE_SOURCE_DIR}/include
            ${PLATFORM_DIR}
        )
        
        # 设置平台输出目录（在构建目录下）
        set(PLATFORM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${PLATFORM_NAME}")
        set(PLATFORM_BUILTIN_DIR "${PLATFORM_OUTPUT_DIR}/builtin")
        set(PLATFORM_SRC_DIR "${PLATFORM_OUTPUT_DIR}/src")
        
        # 添加自定义命令：运行生成器并输出到指定目录
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            # 创建目录结构
            COMMAND ${CMAKE_COMMAND} -E make_directory ${PLATFORM_BUILTIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${PLATFORM_SRC_DIR}
            # 运行生成器
            COMMAND ${TARGET_NAME}
            # 复制 AML 到 builtin 目录
            COMMAND ${CMAKE_COMMAND} -E copy PPTT.aml ${PLATFORM_BUILTIN_DIR}/PPTT.aml
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "生成 ${PLATFORM_NAME} 的 PPTT.aml 到 ${PLATFORM_BUILTIN_DIR}"
        )
        
        # 如果 iasl 可用，添加反编译步骤
        if(IASL_AVAILABLE)
            add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${IASL_EXECUTABLE} -d ${PLATFORM_BUILTIN_DIR}/PPTT.aml > /dev/null 2>&1 || true
                COMMAND ${CMAKE_COMMAND} -E rename ${PLATFORM_BUILTIN_DIR}/PPTT.dsl ${PLATFORM_SRC_DIR}/PPTT.dsl
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "反编译 ${PLATFORM_NAME} 的 PPTT.aml 到 DSL"
            )
        endif()
        
        # 添加安装规则（可选）
        install(FILES ${PLATFORM_BUILTIN_DIR}/PPTT.aml 
                DESTINATION ${PLATFORM_NAME}/builtin)
        if(IASL_AVAILABLE AND EXISTS ${PLATFORM_SRC_DIR}/PPTT.dsl)
            install(FILES ${PLATFORM_SRC_DIR}/PPTT.dsl 
                    DESTINATION ${PLATFORM_NAME}/src)
        endif()
    endif()
endforeach()

# 创建一个 all 目标来构建所有平台
add_custom_target(build_all_platforms ALL)
foreach(PLATFORM_DIR ${PLATFORM_DIRS})
    if(IS_DIRECTORY ${PLATFORM_DIR})
        get_filename_component(PLATFORM_NAME ${PLATFORM_DIR} NAME)
        # 跳过 common 目录
        if(NOT PLATFORM_NAME STREQUAL "common")
            add_dependencies(build_all_platforms pptt_generator_${PLATFORM_NAME})
        endif()
    endif()
endforeach()

# 添加清理目标
add_custom_target(clean_output
    COMMAND ${CMAKE_COMMAND} -E echo "清理所有输出文件"
    COMMENT "清理所有输出文件"
)

# 打印配置信息
message(STATUS "源代码目录: ${CMAKE_SOURCE_DIR}")
message(STATUS "构建目录: ${CMAKE_BINARY_DIR}")
message(STATUS "AML 文件输出到: build/<平台名>/builtin/PPTT.aml")
if(IASL_AVAILABLE)
    message(STATUS "DSL 文件输出到: build/<平台名>/src/PPTT.dsl")
endif()
