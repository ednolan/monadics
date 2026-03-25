// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_META_EXTRACT_VALUE_TYPE_HPP
#define BEMAN_MONADICS_DETAIL_META_EXTRACT_VALUE_TYPE_HPP

#if defined(BEMAN_USE_MODULES) && !defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)
import beman.monadics.detail;
#else

#ifndef BEMAN_MONADICS_MODULE_INTERFACE
#include <beman/monadics/detail/decomposable.hpp>
#include <type_traits>
#endif

namespace beman::monadics::detail {

template <decomposable<1> Box>
[[nodiscard]] consteval auto meta_extract_value_type() noexcept {
    return []<template <typename...> typename T, typename V, typename... Args>(T<V, Args...>*) {
        return std::type_identity<V>{};
    }(as_pointer<Box>);
};

} // namespace beman::monadics::detail

#endif // defined(BEMAN_USE_MODULES) && !defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)

#endif // BEMAN_MONADICS_DETAIL_META_EXTRACT_VALUE_TYPE_HPP
