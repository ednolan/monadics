// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include <beman/monadics/detail/meta_rebind_error.hpp>

template <typename T, typename E>
struct myexpected {};

namespace beman::monadics::detail::tests {

TEST_CASE("get_meta_rebind_error") {
    using rebinder = typename decltype(get_meta_rebind_error<myexpected<int, double>>())::type;
    STATIC_REQUIRE(std::same_as<rebinder::rebind_error<float>, myexpected<int, float>>);
}

TEST_CASE("get_meta_rebind_error-preserves-value-type") {
    using rebinder = typename decltype(get_meta_rebind_error<myexpected<double, int>>())::type;
    STATIC_REQUIRE(std::same_as<rebinder::rebind_error<float>, myexpected<double, float>>);
}

template <typename T>
concept rebind_error_extractable = requires { get_meta_rebind_error<T>(); };

TEST_CASE("not-callable-for-non-decomposable-with-2-args") {
    STATIC_REQUIRE_FALSE(rebind_error_extractable<int>);
    STATIC_REQUIRE_FALSE(rebind_error_extractable<std::optional<int>>);
}

} // namespace beman::monadics::detail::tests
