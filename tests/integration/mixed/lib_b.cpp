// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// import must appear before any declarations in a non-module translation unit.
import beman.monadics;

#include "lib_b.hpp"

#include "optional_box_traits.hpp"

namespace integration {

std::optional<int> process_with_modules(std::optional<int> input) {
    namespace bms = beman::monadics;
    return input | bms::transform([](int v) { return v + 10; });
}

} // namespace integration
