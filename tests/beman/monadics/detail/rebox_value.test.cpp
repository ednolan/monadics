// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include <beman/monadics/detail/rebox_value.hpp>

#include <utility>

template <typename T, typename E>
struct TestBox {
    bool has_val;
    T    val{};
    E    err{};
};

namespace beman::monadics::detail {

template <typename T, typename E>
struct box_traits<TestBox<T, E>> {
    using value_type = T;
    using error_type = E;

    template <typename U>
    using rebind = TestBox<U, E>;

    template <typename F>
    using rebind_error = TestBox<T, F>;

    static bool has_value(const TestBox<T, E>& b) noexcept { return b.has_val; }
    static T    value(TestBox<T, E>&& b) { return std::move(b.val); }
    static E    error(TestBox<T, E>&& b) { return std::move(b.err); }

    static TestBox<T, E> make(T v) { return {true, std::move(v), {}}; }
    static TestBox<T, E> make_error(E e) { return {false, {}, std::move(e)}; }
};

} // namespace beman::monadics::detail

namespace beman::monadics::detail::tests {

TEST_CASE("copies value into new box type") {
    TestBox<int, double> src{true, 5, 0.0};
    const auto           result = rebox_value<TestBox<double, double>>(std::move(src));
    REQUIRE(result.has_val);
    REQUIRE(result.val == 5.0);
}

TEST_CASE("same value type roundtrip") {
    TestBox<int, double> src{true, 42, 0.0};
    const auto           result = rebox_value<TestBox<int, double>>(std::move(src));
    REQUIRE(result.has_val);
    REQUIRE(result.val == 42);
}

} // namespace beman::monadics::detail::tests
