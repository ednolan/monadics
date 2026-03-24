# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

function(beman_add_fetchcontent_test suffix)
    # Additional cmake options can be passed as extra arguments
    add_test(
        NAME "beman.monadics.integration.fetchcontent.${suffix}"
        COMMAND
            ${CMAKE_CTEST_COMMAND} --build-and-test
            "${CMAKE_CURRENT_SOURCE_DIR}/fetchcontent"
            "${CMAKE_CURRENT_BINARY_DIR}/fetchcontent-${suffix}"
            --build-generator "${CMAKE_GENERATOR}" --build-options
            "-DBEMAN_MONADICS_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" ${ARGN} --test-command
            "${CMAKE_CURRENT_BINARY_DIR}/fetchcontent-${suffix}/beman.monadics.integration.fetchcontent"
    )
endfunction()

beman_add_fetchcontent_test(default)
beman_add_fetchcontent_test(
    strict
    "-DCMAKE_CXX_EXTENSIONS=OFF"
    "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
)
