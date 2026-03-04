# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.30...4.3)

set(CMAKE_LINK_DEPENDS_NO_SHARED ${BUILD_SHARED_LIBS})

# if(BEMAN_USE_MODULES AND CMAKE_VERSION VERSION_GREATER_EQUAL 4.2 AND CMAKE_VERSION VERSION_LESS 4.3)
# set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
# endif()

project(
    beman.monadics
    DESCRIPTION "Freestanding monadic operations"
    LANGUAGES CXX
    VERSION 0.0.1
)

# [CMAKE.SKIP_TESTS]
option(
    BEMAN_MONADICS_BUILD_TESTS
    "Enable building tests and test infrastructure. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

# [CMAKE.SKIP_EXAMPLES]
option(
    BEMAN_MONADICS_BUILD_EXAMPLES
    "Enable building examples. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

# for find of beman-install-library
# include(infra/cmake/beman-install-library-config.cmake)

add_subdirectory(src/beman/monadics)

# beman_install_library(beman.monadics)

if(BEMAN_MONADICS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/beman/monadics)
endif()

if(BEMAN_MONADICS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
