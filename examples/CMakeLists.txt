# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Usage:
# CXX=clang++ cmake -S . -B build -G Ninja --fresh
#
cmake_minimum_required(VERSION 4.2...4.3)

project(beman.monadics.example LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS ON)
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)
    set(BEMAN_USE_MODULES ON)
    set(beman_monadics_target beman::monadics_module)

    find_package(beman.monadics REQUIRED)

    enable_testing()
endif()

set(ALL_EXAMPLES monadics_std_usage)

message("Examples to be built: ${ALL_EXAMPLES}")

foreach(example ${ALL_EXAMPLES})
    add_executable(beman.monadics.examples.${example})
    target_sources(beman.monadics.examples.${example} PRIVATE ${example}.cpp)
    target_link_libraries(
        beman.monadics.examples.${example}
        PRIVATE ${beman_monadics_target}
    )

    set_target_properties(
        beman.monadics.examples.${example}
        PROPERTIES
            # TODO(CK): why? CXX_EXTENSIONS OFF
            CXX_STANDARD_REQUIRED ON
            CXX_SCAN_FOR_MODULES ${BEMAN_USE_MODULES}
    )

    add_test(
        NAME beman.monadics.examples.${example}
        COMMAND beman.monadics.examples.${example}
    )
endforeach()
