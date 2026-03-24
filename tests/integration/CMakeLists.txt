# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_test(
    NAME beman.monadics.integration.fetchcontent
    COMMAND
        ${CMAKE_CTEST_COMMAND} --build-and-test
        "${CMAKE_CURRENT_SOURCE_DIR}/fetchcontent"
        "${CMAKE_CURRENT_BINARY_DIR}/fetchcontent" --build-generator
        "${CMAKE_GENERATOR}" --build-options
        "-DBEMAN_MONADICS_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" --test-command
        "${CMAKE_CURRENT_BINARY_DIR}/fetchcontent/beman.monadics.integration.fetchcontent"
)
