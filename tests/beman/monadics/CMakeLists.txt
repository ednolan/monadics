# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(Catch2 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

set(test_global_target "beman.monadics.tests")
add_custom_target(${test_global_target})

function(add_beman_test target)
    add_executable(${test_target})
    target_link_libraries(
        ${test_target}
        PRIVATE ${beman_monadics_target} Catch2::Catch2WithMain
    )

    set_target_properties(
        ${test_target}
        PROPERTIES
            CXX_EXTENSIONS OFF
            CXX_STANDARD_REQUIRED ON
            CXX_SCAN_FOR_MODULES ${BEMAN_USE_MODULES}
    )

    catch_discover_tests(${test_target}
      TEST_PREFIX "${test_target}."
    )
endfunction()

add_subdirectory(detail)

set(tests
    optional
    raw_ptr
    shared_ptr
    enum
    expected
)

foreach(test_dir ${tests})
    set(test_set
        and_then
        or_else
        trait
        transform
        transform_error
        compose
    )

    set(test_dir_target "${test_global_target}.${test_dir}")
    add_custom_target(${test_dir_target})
    add_dependencies(${test_global_target} ${test_dir_target})

    foreach(test_name ${test_set})
        set(test_target "${test_dir_target}.${test_name}")

        add_beman_test(${test_target})
        add_dependencies(${test_dir_target} ${test_target})

        target_sources(
            ${test_target}
            PRIVATE "${test_dir}/${test_name}.test.cpp"
        )
        target_include_directories(
            ${test_target}
            PRIVATE ${CMAKE_CURRENT_LIST_DIR}/${test_name}
        )
    endforeach()
endforeach()
