file(GLOB LIBSMARTSCAN_TESTS_SOURCES "${PROJECT_SOURCE_DIR}/tests/*/test_*.cpp")

######################################################################################################################
# DEPENDENCIES
######################################################################################################################

# valgrind
find_program(VALGRIND_BIN valgrind)
if ("${VALGRIND_BIN}" STREQUAL "VALGRIND_BIN-NOTFOUND")
    message(WARNING "BusTub/test couldn't find valgrind.")
else()
    message(STATUS "BusTub/test found valgrind at ${VALGRIND_BIN}")
endif()

set(VALGRIND_OPTIONS
        --error-exitcode=1                      # if leaks are detected, return nonzero value
        # --gen-suppressions=all  # uncomment for leak suppression syntax
        --leak-check=full                       # detailed leak information
        --soname-synonyms=somalloc=*jemalloc*   # also intercept jemalloc
        --trace-children=yes                    # trace child processes
        --track-origins=yes                     # track origin of uninitialized values
        )
set(VALGRIND_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/build_support/valgrind.supp")

######################################################################################################################
# MAKE TARGETS
######################################################################################################################

##########################################
# "make Module_test"
##########################################
message(STATUS, ${LIBSMARTSCAN_TESTS_SOURCES})
foreach (libsmartscan_test_src ${LIBSMARTSCAN_TESTS_SOURCES})
    # Format test module source name
    get_filename_component(libsmartscan_filename ${libsmartscan_test_src} NAME)
    string(REPLACE ".cpp" "" libsmartscan_test_module ${libsmartscan_test_filename})

    # Add the test module to target separately
    add_executable(${libsmartscan_test_module} EXCLUDE_FROM_ALL ${libsmartscan_test_src})

    target_link_libraries(${libsmartscan_test_module} bustub_shared gtest gmock_main)

    # Set test target properties and dependencies.
    set_target_properties(${libsmartscan_test_module}
        PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/tests"
        COMMAND ${libsmartscan_test_module}
    )

    # Add the test under CTest.
    add_test(${libsmartscan_test_module} ${VALGRIND_BIN} ${VALGRIND_OPTIONS}
    --suppressions=${VALGRIND_SUPPRESSIONS_FILE} ${CMAKE_BINARY_DIR}/test/${libsmartscan_test_module}
    --gtest_color=yes --gtest_output=xml:${CMAKE_BINARY_DIR}/test/unit_${test_name}.xml)
    add_test(${libsmartscan_test_module} ${CMAKE_BINARY_DIR}/test/${libsmartscan_test_module} --gtest_color=yes
            --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${libsmartscan_test_module}.xml)
endforeach(libsmartscan_test_src ${LIBSMARTSCAN_TESTS_SOURCES})
