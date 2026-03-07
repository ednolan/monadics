// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include <beman/monadics/detail/same_unqualified_as.hpp>

namespace beman::monadics::detail::tests {

TEST_CASE("identical types") {
    STATIC_REQUIRE(same_unqualified_as<int, int>);
    STATIC_REQUIRE(same_unqualified_as<double, double>);
}

TEST_CASE("strips const") {
    STATIC_REQUIRE(same_unqualified_as<const int, int>);
    STATIC_REQUIRE(same_unqualified_as<int, const int>);
}

TEST_CASE("strips lvalue ref") {
    STATIC_REQUIRE(same_unqualified_as<int&, int>);
    STATIC_REQUIRE(same_unqualified_as<int, int&>);
}

TEST_CASE("strips rvalue ref") {
    STATIC_REQUIRE(same_unqualified_as<int&&, int>);
    STATIC_REQUIRE(same_unqualified_as<int, int&&>);
}

TEST_CASE("strips combined qualifiers") {
    STATIC_REQUIRE(same_unqualified_as<const int&, int>);
    STATIC_REQUIRE(same_unqualified_as<const int&&, int>);
    STATIC_REQUIRE(same_unqualified_as<volatile int&, int>);
}

TEST_CASE("different base types") {
    STATIC_REQUIRE_FALSE(same_unqualified_as<int, double>);
    STATIC_REQUIRE_FALSE(same_unqualified_as<const int&, double>);
}

} // namespace beman::monadics::detail::tests
