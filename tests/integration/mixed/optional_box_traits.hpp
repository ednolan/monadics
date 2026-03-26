// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

// When consumed via headers, include the primary template.
// When consumed via modules, the primary template is already available from the import.
#include <beman/monadics/monadics.hpp>

#include <optional>

template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }
};
