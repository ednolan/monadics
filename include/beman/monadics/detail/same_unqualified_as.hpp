// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_SAME_UNQUALIFIED_AS_HPP
#define BEMAN_MONADICS_DETAIL_SAME_UNQUALIFIED_AS_HPP

#if !defined(BEMAN_USE_MODULES) || defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)
#include <concepts>
#include <type_traits>
#endif

namespace beman::monadics::detail {

template <typename T, typename U>
concept same_unqualified_as = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_SAME_UNQUALIFIED_AS_HPP
