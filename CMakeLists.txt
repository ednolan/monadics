# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.30...4.3)

# Infrastructure for future import std support
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.3 AND CMAKE_VERSION VERSION_LESS 4.4)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
        "451f2fe2-a8a2-47c3-bc32-94786d8fc91b"
    )
elseif(
    CMAKE_VERSION VERSION_GREATER_EQUAL 4.2
    AND CMAKE_VERSION VERSION_LESS 4.3
)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
        "d0edc3af-4c50-42ea-a356-e2862fe7a444"
    )
endif()

set(CMAKE_LINK_DEPENDS_NO_SHARED ${BUILD_SHARED_LIBS})

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

option(
    BEMAN_USE_MODULES
    "Enable C++20 module targets (beman::monadics_module). Default: OFF. Values: { ON, OFF }."
    OFF
)

include(infra/cmake/beman-install-library.cmake)

add_subdirectory(src/beman/monadics)

if(BEMAN_USE_MODULES)
    set(beman_monadics_target beman.monadics_module)
else()
    set(beman_monadics_target beman.monadics)
endif()

if(BEMAN_USE_MODULES)
    beman_install_library(beman.monadics TARGETS beman.monadics beman.monadics_detail beman.monadics_module)
else()
    beman_install_library(beman.monadics TARGETS beman.monadics)
endif()

if(BEMAN_MONADICS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BEMAN_MONADICS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
