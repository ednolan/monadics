// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>
#include <concepts>

namespace beman::monadics::tests {

TEST_CASE("box-trait-for") {
    using Traits = get_box_traits<std::optional<int>>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, std::nullopt_t>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<float>, std::optional<float>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<float>, std::optional<int>>);
    STATIC_REQUIRE(Traits::value(std::optional{5}) == 5);
    STATIC_REQUIRE(Traits::has_value(std::optional{5}));
    STATIC_REQUIRE(Traits::has_value(std::optional<int>{}) == false);
    STATIC_REQUIRE(std::same_as<decltype(Traits::error()), std::nullopt_t>);
    STATIC_REQUIRE(Traits::make(10) == std::optional{10});
    STATIC_REQUIRE(Traits::make_error(Traits::error()) == std::optional<int>{});
}

template <typename Box>
constexpr decltype(auto) value_or(Box&& box, auto&& defaultValue) noexcept {
    using Traits = detail::get_box_traits<Box>;

    if (Traits::has_value(box)) {
        return Traits::value(std::forward<Box>(box));
    }

    if constexpr (std::invocable<decltype(defaultValue)>) {
        return defaultValue();
    } else {
        return std::forward<decltype(defaultValue)>(defaultValue);
    }
}

TEST_CASE("value_or") {
    constexpr auto r = value_or(std::optional{10}, 15);
    STATIC_REQUIRE(r == 10);
}

} // namespace beman::monadics::tests
