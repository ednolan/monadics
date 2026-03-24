# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.30)

project(beman.monadics.integration.find_package CXX)

find_package(beman.monadics CONFIG REQUIRED)

get_target_property(
    beman_monadics_verify_headers
    beman::monadics
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
    beman::monadics
    INTERFACE_COMPILE_FEATURES
)
if(NOT "cxx_std_20" IN_LIST beman_monadics_interface_features)
    message(
        FATAL_ERROR
        "beman.monadics must advertise cxx_std_20 via INTERFACE_COMPILE_FEATURES"
    )
endif()

add_executable(beman.monadics.integration.find_package)
target_sources(
    beman.monadics.integration.find_package
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../main.cpp"
)
target_link_libraries(
    beman.monadics.integration.find_package
    PRIVATE beman::monadics
)
