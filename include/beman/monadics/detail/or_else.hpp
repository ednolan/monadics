// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
#define BEMAN_MONADICS_DETAIL_OR_ELSE_HPP

#if defined(BEMAN_USE_MODULES) && !defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)
import beman.monadics.detail;
#else

    #include <beman/monadics/detail/get_box_traits.hpp>
    #include <beman/monadics/detail/invoke_with_error.hpp>
    #include <beman/monadics/detail/rebox_value.hpp>
    #include <beman/monadics/detail/same_box.hpp>

namespace beman::monadics::detail {

    // ChatGPT: Many modern libraries (ranges-style) avoid friend entirely by using
    // CPOs (customization point objects) like this:
    #undef USE_CPO
    #ifdef USE_CPO

struct or_else_fn {
    template <class E, class F>
    auto operator()(E&& e, F&& f) const {
        if (!e.has_value()) {
            return f(e.error());
        }
        return std::forward<E>(e);
    }
};

inline constexpr or_else_fn or_else{};

    #else

struct or_else_t {
    template <typename Fn>
    struct action {
        Fn fn;

        template <is_box Box, same_unqualified_as<action> A, typename Traits = get_box_traits<Box> >
        [[nodiscard]] friend constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept
            requires requires {
                { invoke_with_error(std::forward<A>(a).fn, std::forward<Box>(box)) } -> same_box<Box>;
            }
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

    #endif

} // namespace beman::monadics::detail

#endif // defined(BEMAN_USE_MODULES) && !defined(BEMAN_MONADICS_DETAIL_MODULE_INTERFACE)

#endif // BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
