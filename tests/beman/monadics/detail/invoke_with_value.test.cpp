// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include <beman/monadics/detail/invoke_with_value.hpp>

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

TEST_CASE("calls fn with the box value") {
    TestBox<int, double> box{true, 7, 0.0};
    const auto           result = invoke_with_value([](int v) { return v * 2; }, std::move(box));
    REQUIRE(result == 14);
}

TEST_CASE("return type follows fn return type") {
    TestBox<int, double> box{true, 3, 0.0};
    const auto           result = invoke_with_value([](int v) -> double { return v * 1.5; }, std::move(box));
    REQUIRE(result == 4.5);
}

} // namespace beman::monadics::detail::tests
