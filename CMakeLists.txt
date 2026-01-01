cmake_minimum_required(VERSION 3.10)
project(acpi-table-generator C)

# Option to export compile commands for editor tooling (clangd, etc.).
option(EXPORT_COMPILE_COMMANDS "Generate compile_commands.json for editors (clangd)" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ${EXPORT_COMPILE_COMMANDS})

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

# Find Python for running tests
find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
    message(STATUS "Found Python3: ${Python3_EXECUTABLE}")
    set(PYTHON_AVAILABLE TRUE)
else()
    message(WARNING "Python3 not found, tests will be disabled")
    set(PYTHON_AVAILABLE FALSE)
endif()

# Find iasl tool
find_program(IASL_EXECUTABLE iasl)
if(IASL_EXECUTABLE)
    message(STATUS "Found iasl: ${IASL_EXECUTABLE}")
    set(IASL_AVAILABLE TRUE)
else()
    message(WARNING "iasl tool not found, DSL decompilation step will be skipped")
    set(IASL_AVAILABLE FALSE)
endif()

# Build acpi_extractor tool
add_executable(acpi_extractor src/acpi_extractor.c lib/utils.c)
target_include_directories(acpi_extractor PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Build iort_reader tool
add_executable(iort_reader src/iort_reader.c lib/utils.c)
target_include_directories(iort_reader PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# Automatically scan src/dummy/ for ACPI table source files
# Each .c file corresponds to a table type (e.g., csrt.c -> csrt table type)
file(GLOB DUMMY_SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/dummy/*.c")
set(ACPI_TABLE_TYPES "")

foreach(SOURCE_FILE ${DUMMY_SOURCE_FILES})
    get_filename_component(TABLE_TYPE ${SOURCE_FILE} NAME_WE)
    list(APPEND ACPI_TABLE_TYPES ${TABLE_TYPE})
    
    # Set header file name (e.g., csrt.c -> csrt.h)
    set(ACPI_TABLE_${TABLE_TYPE}_HEADER "${TABLE_TYPE}.h")
    set(ACPI_TABLE_${TABLE_TYPE}_SOURCE "${SOURCE_FILE}")
    
    message(STATUS "Detected ACPI table type: ${TABLE_TYPE}")
endforeach()

# Sort table types for consistent ordering
list(SORT ACPI_TABLE_TYPES)

# Scan include/vendor directory for all vendors and targets
file(GLOB VENDOR_DIRS "${CMAKE_SOURCE_DIR}/include/vendor/*")

# Collect all targets to build
set(ALL_DEVICE_TARGETS "")
# Store table types per device for later processing
# Format: DEVICE_<target>_TABLES = "pptt;madt;..."

foreach(VENDOR_DIR ${VENDOR_DIRS})
    if(IS_DIRECTORY ${VENDOR_DIR})
        get_filename_component(VENDOR_NAME ${VENDOR_DIR} NAME)
        
        # Scan all subdirectories under this vendor (e.g., qcom/sm8750, qcom/sm8850)
        file(GLOB TARGET_DIRS "${VENDOR_DIR}/*")
        foreach(TARGET_DIR ${TARGET_DIRS})
            if(IS_DIRECTORY ${TARGET_DIR})
                get_filename_component(TARGET_NAME_RAW ${TARGET_DIR} NAME)
                
                set(DEVICE_NAME "${VENDOR_NAME}_${TARGET_NAME_RAW}")
                
                # Detect which ACPI tables this device supports
                set(DEVICE_TABLES "")
                
                foreach(TABLE_TYPE ${ACPI_TABLE_TYPES})
                    set(HEADER_FILE "${TARGET_DIR}/${ACPI_TABLE_${TABLE_TYPE}_HEADER}")
                    set(SOURCE_FILE "${ACPI_TABLE_${TABLE_TYPE}_SOURCE}")
                    
                    if(EXISTS ${HEADER_FILE} AND EXISTS ${SOURCE_FILE})
                        # Check if header file is not empty (empty file means placeholder)
                        file(READ ${HEADER_FILE} HEADER_CONTENT)
                        string(LENGTH "${HEADER_CONTENT}" HEADER_SIZE)
                        
                        if(HEADER_SIZE EQUAL 0)
                            message(STATUS "Skipping ${DEVICE_NAME}_${TABLE_TYPE}: header file is empty (placeholder)")
                            continue()
                        endif()
                        
                        # Create a separate library for each table type
                        set(TARGET_NAME "${DEVICE_NAME}_${TABLE_TYPE}")
                        
                        message(STATUS "Configuring target: ${TARGET_NAME} (vendor: ${VENDOR_NAME}, device: ${TARGET_NAME_RAW}, table: ${TABLE_TYPE})")
                        
                        # Create library with the table-specific source file
                        add_library(${TARGET_NAME} ${SOURCE_FILE})
                        
                        # Suppress specific warnings
                        target_compile_options(${TARGET_NAME} PRIVATE
                            -Wno-missing-braces
                        )

                        # Set include directories
                        target_include_directories(${TARGET_NAME} PRIVATE 
                            ${CMAKE_SOURCE_DIR}/include
                            ${VENDOR_DIR}
                            ${TARGET_DIR}
                        )

                        list(APPEND ALL_DEVICE_TARGETS ${TARGET_NAME})
                        list(APPEND DEVICE_TABLES ${TABLE_TYPE})
                        
                        # Store device info for this target
                        set(TARGET_${TARGET_NAME}_DEVICE ${DEVICE_NAME})
                        set(TARGET_${TARGET_NAME}_TABLE ${TABLE_TYPE})
                        set(TARGET_${TARGET_NAME}_DIR ${TARGET_DIR})
                    endif()
                endforeach()
                
                if(NOT DEVICE_TABLES)
                    message(WARNING "Skipping ${DEVICE_NAME}: No supported ACPI tables found")
                endif()
            endif()
        endforeach()
    endif()
endforeach()

# Create a target to build all device targets
add_custom_target(build_all_devices ALL)
foreach(DEVICE_TARGET ${ALL_DEVICE_TARGETS})
    add_dependencies(build_all_devices ${DEVICE_TARGET})
endforeach()

# Ensure acpi_extractor is built first
add_dependencies(build_all_devices acpi_extractor iort_reader)

# Collect all DSL files for testing
set(ALL_DSL_FILES "")
set(ALL_AML_FILES "")

# For each device target, add custom commands: extract ACPI tables, decompile, validate
foreach(DEVICE_TARGET ${ALL_DEVICE_TARGETS})
    # Get the table type and device name for this target
    set(TABLE_TYPE ${TARGET_${DEVICE_TARGET}_TABLE})
    set(DEVICE_NAME ${TARGET_${DEVICE_TARGET}_DEVICE})
    string(TOUPPER ${TABLE_TYPE} TABLE_NAME_UPPER)
    
    # Set output directory (use device name, not target name, so all tables for a device are together)
    set(TARGET_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${DEVICE_NAME}")
    file(MAKE_DIRECTORY ${TARGET_OUTPUT_DIR})
    
    # Library file path (now specific to table type)
    set(LIB_FILE "${CMAKE_BINARY_DIR}/lib${DEVICE_TARGET}.a")
    
    # ACPI table output paths
    set(AML_FILE "${TARGET_OUTPUT_DIR}/${TABLE_NAME_UPPER}.aml")
    set(DSL_FILE "${TARGET_OUTPUT_DIR}/${TABLE_NAME_UPPER}.dsl")
    set(DEVICE_IASL_LOG_FILE "${TARGET_OUTPUT_DIR}/iasl.log")
    set(HEX_OUTPUT_DIR "${CMAKE_BINARY_DIR}/test")
    file(MAKE_DIRECTORY ${HEX_OUTPUT_DIR})
    
    # Add custom command: extract ACPI table
    add_custom_command(
        OUTPUT ${AML_FILE}
        COMMAND ${CMAKE_BINARY_DIR}/acpi_extractor ${LIB_FILE} ${AML_FILE}
        DEPENDS ${DEVICE_TARGET} acpi_extractor
        COMMENT "Extracting ${TABLE_NAME_UPPER}.aml from ${DEVICE_TARGET}..."
        VERBATIM
    )
    
    list(APPEND ALL_AML_FILES ${AML_FILE})
    
    # If iasl is available, add decompilation step
    if(IASL_AVAILABLE)
        add_custom_command(
            OUTPUT ${DSL_FILE}
            COMMAND ${IASL_EXECUTABLE} -d ${AML_FILE} >> ${DEVICE_IASL_LOG_FILE} 2>&1 || true
            COMMAND /bin/bash -c "[ -f '${TARGET_OUTPUT_DIR}/${TABLE_NAME_UPPER}.hex' ] && mv '${TARGET_OUTPUT_DIR}/${TABLE_NAME_UPPER}.hex' '${HEX_OUTPUT_DIR}/${DEVICE_NAME}_${TABLE_NAME_UPPER}.hex' || true"
            COMMAND ${CMAKE_COMMAND} -E rename ${TARGET_OUTPUT_DIR}/${TABLE_NAME_UPPER}.dsl ${DSL_FILE} || true
            DEPENDS ${AML_FILE}
            WORKING_DIRECTORY ${TARGET_OUTPUT_DIR}
            COMMENT "Decompiling ${DEVICE_NAME}/${TABLE_NAME_UPPER}.aml -> ${TABLE_NAME_UPPER}.dsl (log: iasl.log)..."
            VERBATIM
        )
        
        list(APPEND ALL_DSL_FILES ${DSL_FILE})
        
        # Add validation step (check for errors in DSL)
        set(VALIDATED_FILE "${TARGET_OUTPUT_DIR}/.${TABLE_TYPE}_validated")
        add_custom_command(
            OUTPUT ${VALIDATED_FILE}
            COMMAND ${CMAKE_COMMAND} -E echo "Validating ${DSL_FILE}..."
            COMMAND sh -c "if grep -qi 'error' ${DSL_FILE} 2>/dev/null; then echo 'Error found!'; exit 1; else echo 'Validation passed: no errors'; fi"
            COMMAND ${CMAKE_COMMAND} -E touch ${VALIDATED_FILE}
            DEPENDS ${DSL_FILE}
            COMMENT "Validating ${DEVICE_NAME}/${TABLE_NAME_UPPER}.dsl..."
            VERBATIM
        )
        
        # Create target to trigger the entire process
        add_custom_target(${DEVICE_TARGET}_process ALL
            DEPENDS ${VALIDATED_FILE}
        )
    else()
        # If iasl not available, just extract AML files
        add_custom_target(${DEVICE_TARGET}_process ALL
            DEPENDS ${AML_FILE}
        )
    endif()
    
    # Add dependency
    add_dependencies(${DEVICE_TARGET}_process ${DEVICE_TARGET})
endforeach()

# Add a master target to process all device targets
add_custom_target(process_all_tables)
foreach(DEVICE_TARGET ${ALL_DEVICE_TARGETS})
    add_dependencies(process_all_tables ${DEVICE_TARGET}_process)
endforeach()

# ============================================================================
# Test targets
# ============================================================================

if(PYTHON_AVAILABLE AND IASL_AVAILABLE)
    # Add test target that runs all validation tests
    add_custom_target(test
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/run_all_tests.py ${CMAKE_BINARY_DIR}
        DEPENDS process_all_tables
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running all ACPI table validation tests..."
        VERBATIM
    )
    
    # Add individual test targets for each test script
    add_custom_target(test_aml_validator
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/aml_validator.py ${CMAKE_BINARY_DIR}
        DEPENDS process_all_tables
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running AML validator tests..."
        VERBATIM
    )
    
    add_custom_target(test_pptt_validate
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/pptt_validate.py ${CMAKE_BINARY_DIR}
        DEPENDS process_all_tables
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running PPTT validation tests..."
        VERBATIM
    )
    
    add_custom_target(test_verify_topology
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/verify_topology.py ${CMAKE_BINARY_DIR}
        DEPENDS process_all_tables
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running topology verification tests..."
        VERBATIM
    )
    
    add_custom_target(test_verify_node_references
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/verify_node_references.py ${CMAKE_BINARY_DIR}
        DEPENDS process_all_tables
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running node reference verification tests..."
        VERBATIM
    )
    
    add_custom_target(test_verify_per_core_l2
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/verify_per_core_l2.py ${CMAKE_BINARY_DIR}
        DEPENDS process_all_tables
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running per-core L2 verification tests..."
        VERBATIM
    )
    
    message(STATUS "Test targets enabled: test, test_aml_validator, test_pptt_validate, test_verify_topology, test_verify_node_references, test_verify_per_core_l2")
else()
    message(STATUS "Test targets disabled (requires Python3 and iasl)")
endif()

# Print configuration information
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
list(LENGTH ALL_DEVICE_TARGETS NUM_TARGETS)
message(STATUS "Found ${NUM_TARGETS} device target(s)")
if(IASL_AVAILABLE)
    message(STATUS "IASL available: decompilation and validation enabled")
else()
    message(STATUS "IASL not available: only AML extraction will be performed")
endif()
