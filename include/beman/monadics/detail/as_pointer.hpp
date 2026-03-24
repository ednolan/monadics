// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_AS_POINTER_HPP
#define BEMAN_MONADICS_DETAIL_AS_POINTER_HPP

#if !defined(BEMAN_USE_MODULES) || defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)
#include <type_traits>
#endif

namespace beman::monadics::detail {

template <typename T>
inline constexpr auto as_pointer = static_cast<std::remove_cvref_t<T>*>(nullptr);

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_AS_POINTER_HPP
