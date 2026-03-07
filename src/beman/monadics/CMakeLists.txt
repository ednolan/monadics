# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BEMAN_USE_MODULES)
    add_library(beman.monadics.detail)
    target_compile_features(beman.monadics.detail PUBLIC cxx_std_20)
    target_compile_definitions(beman.monadics.detail PUBLIC BEMAN_USE_MODULES)
    target_sources(
        beman.monadics.detail
        PUBLIC FILE_SET CXX_MODULES FILES detail.cppm
    )

    set_target_properties(
        beman.monadics.detail
        PROPERTIES
            CXX_EXTENSIONS OFF
            CXX_VISIBILITY_PRESET hidden
            VERIFY_INTERFACE_HEADER_SETS ON
            VISIBILITY_INLINES_HIDDEN ON
    )

    add_library(beman.monadics)
    target_compile_features(beman.monadics PUBLIC cxx_std_20)
    target_compile_definitions(beman.monadics PUBLIC BEMAN_USE_MODULES)
    target_sources(
        beman.monadics
        PUBLIC FILE_SET CXX_MODULES FILES monadics.cppm
    )

    target_link_libraries(beman.monadics PRIVATE beman.monadics.detail)

    set_source_files_properties(
        detail.cppm
        monadics.cppm
        PROPERTIES COMPILE_OPTIONS "-Wno-include-angled-in-module-purview"
    )

    set(beman_monadics_target beman.monadics)
    set(beman_monadics_detail_target beman.monadics.detail)
else()
    add_library(beman.monadics INTERFACE)
    target_compile_features(beman.monadics INTERFACE cxx_std_20)

    set(beman_monadics_target beman.monadics)
    set(beman_monadics_detail_target beman.monadics)
endif()

add_library(beman::monadics ALIAS beman.monadics)

# target_compile_definitions(
# ${BEMAN_EXECUTION_TARGET_PREFIX}
# PUBLIC BEMAN_HAS_IMPORT_STD
# )
# set_target_properties(
# ${BEMAN_EXECUTION_TARGET_PREFIX}
# PROPERTIES CXX_MODULE_STD ON
# )

set_target_properties(
    ${beman_monadics_target}
    PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
        VERIFY_INTERFACE_HEADER_SETS ON
        VISIBILITY_INLINES_HIDDEN ON
)

target_sources(
    ${beman_monadics_detail_target}
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS ${PROJECT_SOURCE_DIR}/include
            FILES
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/and_then.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/as_pointer.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/box_traits.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/config.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/decomposable.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_box_traits.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_error_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_error_type.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_make_error_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_make_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_rebind.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_rebind_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_value_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_value_query_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_value_type.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/instance_of.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/invoke_with_value.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/invoke_with_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/meta_extract_value_type.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/meta_rebind_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/meta_rebind_value.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/or_else.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/rebox_value.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/rebox_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/same_box.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/same_template.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/same_unqualified_as.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/transform.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/transform_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/utility.hpp
)

target_sources(
    ${beman_monadics_target}
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS ${PROJECT_SOURCE_DIR}/include
            FILES ${PROJECT_SOURCE_DIR}/include/beman/monadics/monadics.hpp
)
