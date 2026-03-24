# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.30)

project(beman.monadics.integration.fetchcontent CXX)

include(FetchContent)

if(NOT DEFINED BEMAN_MONADICS_SOURCE_DIR)
    message(
        FATAL_ERROR
        "BEMAN_MONADICS_SOURCE_DIR must be set to the root of the beman.monadics source tree."
    )
endif()

option(BEMAN_USE_MODULES "" OFF)

FetchContent_Declare(beman_monadics SOURCE_DIR "${BEMAN_MONADICS_SOURCE_DIR}")
FetchContent_MakeAvailable(beman_monadics)

get_target_property(
    beman_monadics_verify_headers
    beman.monadics
    VERIFY_INTERFACE_HEADER_SETS
)
if(beman_monadics_verify_headers)
    message(
        FATAL_ERROR
        "beman.monadics must not set VERIFY_INTERFACE_HEADER_SETS=ON for consumers."
    )
endif()

get_target_property(
    beman_monadics_interface_features
    beman.monadics
    INTERFACE_COMPILE_FEATURES
)
if(NOT "cxx_std_20" IN_LIST beman_monadics_interface_features)
    message(
        FATAL_ERROR
        "beman.monadics must advertise cxx_std_20 via INTERFACE_COMPILE_FEATURES"
    )
endif()

if(BEMAN_USE_MODULES)
    set(consumer_target beman::monadics_module)
    get_target_property(
        defs
        beman::monadics_module
        INTERFACE_COMPILE_DEFINITIONS
    )
    if(NOT "BEMAN_USE_MODULES" IN_LIST defs)
        message(
            FATAL_ERROR
            "beman::monadics_module must propagate BEMAN_USE_MODULES"
        )
    endif()
else()
    set(consumer_target beman::monadics)
endif()

add_executable(beman.monadics.integration.fetchcontent)
target_sources(
    beman.monadics.integration.fetchcontent
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../main.cpp"
)
target_link_libraries(
    beman.monadics.integration.fetchcontent
    PRIVATE ${consumer_target}
)
set_target_properties(
    beman.monadics.integration.fetchcontent
    PROPERTIES CXX_SCAN_FOR_MODULES ${BEMAN_USE_MODULES}
)
