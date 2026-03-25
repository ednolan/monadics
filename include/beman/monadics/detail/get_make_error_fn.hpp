// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_MAKE_ERROR_FN_HPP
#define BEMAN_MONADICS_DETAIL_GET_MAKE_ERROR_FN_HPP

#if defined(BEMAN_USE_MODULES) && !defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)
import beman.monadics.detail;
#else

#ifndef BEMAN_MONADICS_MODULE_INTERFACE
#include <beman/monadics/detail/utility.hpp>
#include <concepts>
#include <utility>
#endif

namespace beman::monadics::detail {

template <typename Box, typename Traits, typename E>
[[nodiscard]] consteval decltype(auto) get_make_error_fn() noexcept {
    if constexpr (requires { Traits::make_error(std::declval<E>()); }) {
        return [](auto&& e) { return Traits::make_error(std::forward<decltype(e)>(e)); };
    } else if constexpr (std::constructible_from<Box, E>) {
        return [](auto&& e) { return Box{std::forward<decltype(e)>(e)}; };
    }
}

template <typename Box, typename Traits, typename E>
concept has_make_error_fn = requires {
    { get_make_error_fn<Box, Traits, E>() } -> deduced;
} || on_error<"provide Traits::make_error(E) or a Box{E} constructor">;

} // namespace beman::monadics::detail

#endif // defined(BEMAN_USE_MODULES) && !defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)

#endif // BEMAN_MONADICS_DETAIL_GET_MAKE_ERROR_FN_HPP
