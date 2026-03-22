// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("not-supported") {
    STATIC_REQUIRE_FALSE(transform_errorable<std::shared_ptr<int>, decltype([](std::nullptr_t) { return nullptr; })>);
}

} // namespace beman::monadics::tests
