// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include <beman/monadics/detail/rebox_error.hpp>

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

TEST_CASE("copies error into new box type") {
    TestBox<int, int> src{false, 0, 42};
    const auto        result = rebox_error<TestBox<int, double>>(std::move(src));
    REQUIRE(!result.has_val);
    REQUIRE(result.err == 42.0);
}

TEST_CASE("same error type roundtrip") {
    TestBox<int, double> src{false, 0, 3.14};
    const auto           result = rebox_error<TestBox<int, double>>(std::move(src));
    REQUIRE(!result.has_val);
    REQUIRE(result.err == 3.14);
}

} // namespace beman::monadics::detail::tests
