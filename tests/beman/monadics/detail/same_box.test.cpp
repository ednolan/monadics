// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include <beman/monadics/detail/same_box.hpp>

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

TEST_CASE("identical instantiation") { STATIC_REQUIRE(same_box<TestBox<int, double>, TestBox<int, double>>); }

TEST_CASE("rebinding value type stays same_box") {
    STATIC_REQUIRE(same_box<TestBox<float, double>, TestBox<int, double>>);
}

TEST_CASE("rebinding error type stays same_box") {
    STATIC_REQUIRE(same_box<TestBox<int, float>, TestBox<int, double>>);
}

TEST_CASE("non-box type is not same_box") {
    STATIC_REQUIRE_FALSE(same_box<int, TestBox<int, double>>);
    STATIC_REQUIRE_FALSE(same_box<TestBox<int, double>, int>);
}

} // namespace beman::monadics::detail::tests
