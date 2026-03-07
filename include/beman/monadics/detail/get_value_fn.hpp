// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_VALUE_FN_HPP
#define BEMAN_MONADICS_DETAIL_GET_VALUE_FN_HPP

#if defined(BEMAN_USE_MODULES) && !defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)
import beman.monadics.detail;
#else

#include <beman/monadics/detail/utility.hpp>

#include <utility>

namespace beman::monadics::detail {

template <typename Box, typename Traits>
[[nodiscard]] consteval decltype(auto) get_value_fn() noexcept {
    if constexpr (requires { Traits::value(std::declval<Box>()); })
        return [](auto&& b) -> decltype(auto) { return Traits::value(std::forward<decltype(b)>(b)); };
    else if constexpr (requires { std::declval<Box>().value(); })
        return [](auto&& b) -> decltype(auto) { return std::forward<decltype(b)>(b).value(); };
}

template <typename Box, typename Traits>
concept has_value_fn = requires {
    { get_value_fn<Box, Traits>() } -> deduced;
} || on_error<"provide Traits::value(Box) or Box::value()">;

} // namespace beman::monadics::detail

#endif // defined (BEMAN_USE_MODULES) && !defined (BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)

#endif // BEMAN_MONADICS_DETAIL_GET_VALUE_FN_HPP
