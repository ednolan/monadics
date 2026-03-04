# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(test_dir_target "beman.monadics.tests.detail")
add_custom_target(${test_dir_target})
add_dependencies(${test_global_target} ${test_dir_target})

set(tests
    # get_value_query_fn
    get_value_fn
    # get_error_fn
    # get_make_fn
    # get_make_error_fn
    # get_value_type
    # get_error_type
    # get_rebind
    # get_rebind_error
)

foreach(test_name ${tests})
    set(test_target "${test_dir_target}.${test_name}")

    add_beman_test(${test_target})
    add_dependencies(${test_dir_target} ${test_target})

    target_sources(${test_target} PRIVATE "${test_name}.test.cpp")
endforeach()
