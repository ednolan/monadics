// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
#define BEMAN_MONADICS_DETAIL_OR_ELSE_HPP

#if !defined(BEMAN_USE_MODULES) || defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/rebox_value.hpp>
#include <beman/monadics/detail/invoke_with_error.hpp>
#include <beman/monadics/detail/same_box.hpp>
#include <utility>
#endif

namespace beman::monadics::detail {

template <typename NewBox, typename OldBox>
concept or_elseable_return =
    (same_box<NewBox, OldBox> || on_error<"Should return the same type of Box">)
    && (!has_error_channel<OldBox>
        || (same_box_and_value<NewBox, OldBox> || on_error<"Should return the Box with same value_type">));

template <typename Box, typename Fn>
concept or_elseable_impl =
    or_elseable_return<decltype(invoke_with_error(std::declval<Fn>(), std::declval<Box>())), Box>;

struct or_else_t {
    template <typename Fn>
    struct action {
        Fn fn;

        template <is_box Box, same_unqualified_as<action> A, typename Traits = get_box_traits<Box>>
        [[nodiscard]] friend constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept
            requires or_elseable_impl<decltype(box), decltype(std::forward<A>(a).fn)>
        {
            using NewBox = decltype(invoke_with_error(std::forward<A>(a).fn, std::forward<Box>(box)));
            if (!Traits::has_value(box)) {
                return invoke_with_error(std::forward<A>(a).fn, std::forward<Box>(box));
            }

            return rebox_value<NewBox>(std::forward<Box>(box));
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }
};

inline constexpr or_else_t or_else{};

template <typename Box, typename Fn>
concept or_elseable = requires(Box&& box, Fn&& fn) { std::forward<Box>(box) | or_else(std::forward<Fn>(fn)); };

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
