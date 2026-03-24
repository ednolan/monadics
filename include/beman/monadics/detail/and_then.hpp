// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_AND_THEN_HPP
#define BEMAN_MONADICS_DETAIL_AND_THEN_HPP

#if !defined(BEMAN_USE_MODULES) || defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/rebox_error.hpp>
#include <beman/monadics/detail/invoke_with_value.hpp>
#include <beman/monadics/detail/same_box.hpp>
#include <utility>
#endif

namespace beman::monadics::detail {

template <typename NewBox, typename OldBox>
concept and_thenable_return =
    (same_box<NewBox, OldBox> || on_error<"Should return the same type of Box">)
    && (same_box_and_error<NewBox, OldBox> || on_error<"Should return the Box with same error_type">);

template <typename Box, typename Fn>
concept and_thenable_impl =
    and_thenable_return<decltype(invoke_with_value(std::declval<Fn>(), std::declval<Box>())), Box>;

struct and_then_t {
    template <typename Fn>
    struct action {
        Fn fn;

        template <is_box Box, same_unqualified_as<action> A, typename Traits = get_box_traits<Box>>
        [[nodiscard]] friend constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept
            requires and_thenable_impl<decltype(box), decltype(std::forward<A>(a).fn)>
        {
            if (Traits::has_value(box)) {
                return invoke_with_value(std::forward<A>(a).fn, std::forward<Box>(box));
            }

            using NewBox = decltype(invoke_with_value(std::forward<A>(a).fn, std::forward<Box>(box)));

            return rebox_error<NewBox>(std::forward<Box>(box));
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }
};

inline constexpr and_then_t and_then{};

template <typename Box, typename Fn>
concept and_thenable = requires(Box&& box, Fn&& fn) {
    { std::forward<Box>(box) | and_then(std::forward<Fn>(fn)) };
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_AND_THEN_HPP
