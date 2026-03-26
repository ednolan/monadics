# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(_integration_common_options
    "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
    "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

set(_integration_strict_options
    "-DCMAKE_CXX_EXTENSIONS=OFF"
    "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
)

function(beman_add_fetchcontent_test suffix)
    add_test(
        NAME "beman.monadics.integration.fetchcontent.${suffix}"
        COMMAND
            ${CMAKE_CTEST_COMMAND} --build-and-test
            "${CMAKE_CURRENT_SOURCE_DIR}/fetchcontent"
            "${CMAKE_CURRENT_BINARY_DIR}/fetchcontent-${suffix}"
            --build-generator "${CMAKE_GENERATOR}" --build-options
            "-DBEMAN_MONADICS_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            ${_integration_common_options} ${ARGN} --test-command
            "${CMAKE_CURRENT_BINARY_DIR}/fetchcontent-${suffix}/beman.monadics.integration.fetchcontent"
    )
endfunction()

beman_add_fetchcontent_test(default)
beman_add_fetchcontent_test(strict ${_integration_strict_options})
beman_add_fetchcontent_test(modules "-DBEMAN_USE_MODULES=ON")

set(_find_package_prefix "${CMAKE_CURRENT_BINARY_DIR}/find_package-prefix")

add_test(
    NAME beman.monadics.integration.find_package.install
    COMMAND
        ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --prefix
        "${_find_package_prefix}"
)
set_tests_properties(
    beman.monadics.integration.find_package.install
    PROPERTIES FIXTURES_SETUP beman.monadics.integration.find_package.fixture
)

function(beman_add_find_package_test suffix)
    add_test(
        NAME "beman.monadics.integration.find_package.${suffix}"
        COMMAND
            ${CMAKE_CTEST_COMMAND} --build-and-test
            "${CMAKE_CURRENT_SOURCE_DIR}/find_package"
            "${CMAKE_CURRENT_BINARY_DIR}/find_package-${suffix}"
            --build-generator "${CMAKE_GENERATOR}" --build-options
            "-DCMAKE_PREFIX_PATH=${_find_package_prefix}"
            ${_integration_common_options} ${ARGN} --test-command
            "${CMAKE_CURRENT_BINARY_DIR}/find_package-${suffix}/beman.monadics.integration.find_package"
    )
    set_tests_properties(
        "beman.monadics.integration.find_package.${suffix}"
        PROPERTIES
            FIXTURES_REQUIRED beman.monadics.integration.find_package.fixture
    )
endfunction()

beman_add_find_package_test(default)
beman_add_find_package_test(strict ${_integration_strict_options})
beman_add_find_package_test(modules "-DBEMAN_USE_MODULES=ON")

# Mixed: lib A (headers) + lib B (modules) → lib C → executable D.
# BEMAN_USE_MODULES is forced ON inside the inner project.
if(BEMAN_USE_MODULES)
    add_test(
        NAME beman.monadics.integration.mixed
        COMMAND
            ${CMAKE_CTEST_COMMAND} --build-and-test
            "${CMAKE_CURRENT_SOURCE_DIR}/mixed"
            "${CMAKE_CURRENT_BINARY_DIR}/mixed" --build-generator
            "${CMAKE_GENERATOR}" --build-options
            "-DBEMAN_MONADICS_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            ${_integration_common_options} --test-command
            "${CMAKE_CURRENT_BINARY_DIR}/mixed/beman.monadics.integration.mixed"
    )
endif()
