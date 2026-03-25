// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP

#if defined(BEMAN_USE_MODULES) && !defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)
import beman.monadics.detail;
#else

#ifndef BEMAN_MONADICS_MODULE_INTERFACE
#include <beman/monadics/detail/or_else.hpp>
#include <utility>
#endif

namespace beman::monadics::detail {

template <typename NewError, typename Box>
concept transform_errorable_return = same_box<Box, typename get_box_traits<Box>::template rebind_error<NewError> >
                                  || on_error<"transform_error: fn must return a type compatible with rebind_error">;

template <typename Box, typename Fn>
concept transform_errorable_impl =
    (has_error_channel<Box> || on_error<"transform_error requires has_error_channel">)
    && transform_errorable_return<decltype(invoke_with_error(std::declval<Fn>(), std::declval<Box>())), Box>;

struct transform_error_t {
    template <typename Fn>
    struct action {
        Fn fn;

        template <is_box Box, same_unqualified_as<action> A, typename Traits = get_box_traits<Box> >
        [[nodiscard]] friend constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept
            requires transform_errorable_impl<decltype(box), decltype(std::forward<A>(a).fn)>
        {
            using NewError     = decltype(invoke_with_error(std::forward<A>(a).fn, std::forward<Box>(box)));
            using NewBox       = typename Traits::template rebind_error<NewError>;
            using NewBoxTraits = get_box_traits<NewBox>;

            // transform_error does not make sense if you don't have error
            return std::forward<Box>(box) | or_else([f = std::forward<A>(a).fn](auto&& e) {
                       return NewBoxTraits::make_error(f(std::forward<decltype(e)>(e)));
                   });
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }
};

inline constexpr transform_error_t transform_error{};

template <typename Box, typename Fn>
concept transform_errorable =
    requires(Box&& box, Fn&& fn) { std::forward<Box>(box) | transform_error(std::forward<Fn>(fn)); };

} // namespace beman::monadics::detail

#endif // defined(BEMAN_USE_MODULES) && !defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
