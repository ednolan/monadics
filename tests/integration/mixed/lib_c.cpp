// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "lib_c.hpp"

#include "lib_a.hpp"
#include "lib_b.hpp"

#include <optional>

namespace integration {

int combined_result() {
    // process_with_headers: and_then doubles → optional{5} → optional{10}
    const auto from_a = process_with_headers(std::optional{5});
    // process_with_modules: transform adds 10 → optional{5} → optional{15}
    const auto from_b = process_with_modules(std::optional{5});
    return from_a.value_or(0) + from_b.value_or(0); // 10 + 15 = 25
}

} // namespace integration
