<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

# Tutorial

## Introduction

`beman.monadics` provides four monadic operations — `and_then`, `transform`, `or_else`,
`transform_error` — as free functions that work uniformly across any type that models a "box":
a container that either holds a value or signals absence/failure.

```cpp
#include <beman/monadics/monadics.hpp>
namespace bms = beman::monadics;
```

## Opting In a Type

A type becomes a box by specializing `beman::monadics::box_traits<T>`. How much you need to
provide depends on how well-structured the type already is.

### Case 1: Auto-deduction (empty specialization)

If the type already exposes a compatible interface — member `has_value()`, `value()`, `error()`,
nested `value_type` / `error_type`, and `rebind` / `rebind_error` — an empty specialization is
all that is needed. The library deduces everything automatically.

`std::expected` is the canonical example:

```cpp
template <typename T, typename E>
struct beman::monadics::box_traits<std::expected<T, E>> {};
```

All four operations are immediately available:

```cpp
using E = std::expected<int, std::string>;

auto result =
    E{42}
    | bms::and_then([](int v)            { return E{v * 2}; })
    | bms::transform([](int v)           { return v + 1; })
    | bms::or_else([](const std::string&){ return E{0}; })
    | bms::transform_error([](auto e)    { return "error: " + e; });
```

### Case 2: Partial customization

If the type is mostly well-structured but misses one capability, provide only that piece. The
rest is still deduced.

`std::optional` has `has_value()`, `value()`, and `value_type`, but has no `error()` member and
no `error_type`. Provide those two things:

```cpp
template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] static constexpr auto error() noexcept { return std::nullopt; }
};
```

`and_then`, `transform`, and `or_else` now work. `transform_error` is automatically disabled
because `optional` has no real error channel.

```cpp
auto parse = [](const std::string& s) -> std::optional<int> {
    try { return std::stoi(s); } catch (...) { return {}; }
};

auto result =
    std::optional<std::string>{"42"}
    | bms::and_then(parse)
    | bms::transform([](int v) { return v * 2; })
    | bms::or_else([]()        { return std::optional{0}; }); // optional{84}
```

`std::shared_ptr` is another partial case: it has no `error()`, no `error_type`, no `rebind` or
`rebind_error`, but `has_value` and `value` can be wired up:

```cpp
template <typename T>
struct beman::monadics::box_traits<std::shared_ptr<T>> {
    [[nodiscard]] static bool has_value(const std::shared_ptr<T>& p) noexcept {
        return static_cast<bool>(p);
    }

    [[nodiscard]] static decltype(auto) value(auto&& p) noexcept {
        return *std::forward<decltype(p)>(p);
    }

    [[nodiscard]] static std::shared_ptr<T> make(T v) {
        return std::make_shared<T>(std::move(v));
    }

    [[nodiscard]] static std::shared_ptr<T> error() noexcept { return nullptr; }
};
```

`and_then`, `transform`, and `or_else` work; `transform_error` is disabled.

```cpp
auto ptr = std::make_shared<int>(10);

auto result =
    ptr
    | bms::and_then([](int v)  { return std::make_shared<int>(v * 2); })
    | bms::transform([](int v) { return v + 3; })
    | bms::or_else([]()        { return std::make_shared<int>(0); }); // 23
```

### Case 3: Fully custom type

For types that expose no compatible interface at all, provide the full set of static members.

```cpp
template <typename T>
struct Result {
    bool ok;
    T    data;
    int  err;
};

template <typename T>
struct beman::monadics::box_traits<Result<T>> {
    using value_type = T;
    using error_type = int;

    template <typename U> using rebind       = Result<U>;
    template <typename E> using rebind_error = Result<T>;

    [[nodiscard]] static bool has_value(const Result<T>& r) noexcept { return r.ok; }

    [[nodiscard]] static decltype(auto) value(auto&& r) noexcept {
        return std::forward<decltype(r)>(r).data;
    }

    [[nodiscard]] static int error(const Result<T>& r) noexcept { return r.err; }

    [[nodiscard]] static Result<T> make(T v)       { return {true,  std::move(v), 0}; }
    [[nodiscard]] static Result<T> make_error(int e) { return {false, {},          e}; }
};
```

All four operations are now available:

```cpp
Result<int> r{true, 10, 0};

auto result =
    r
    | bms::transform([](int v)  { return v * 2; })          // Result{true, 20, 0}
    | bms::and_then([](int v)   { return Result<int>{true, v + 1, 0}; }) // Result{true, 21, 0}
    | bms::transform_error([](int e) { return e + 100; });  // error unchanged (has value)
```

## Operations Reference

### `and_then` — flat-map over the value

Calls `fn(value)` when the box is non-empty; propagates absence/error unchanged. `fn` must
return the same box template with the same error type.

```cpp
auto result = std::optional{42}
    | bms::and_then([](int v) { return std::optional{v * 2}; }); // optional{84}

auto empty = std::optional<int>{}
    | bms::and_then([](int v) { return std::optional{v * 2}; }); // nullopt
```

### `transform` — map over the value

Calls `fn(value)` and rewraps the result. `fn` returns a plain value, not a box.

```cpp
auto result = std::optional{10}
    | bms::transform([](int v) { return v + 1.5; }); // optional<double>{11.5}
```

### `or_else` — recover from absence/error

Calls `fn` when the box is empty or in an error state; passes through the value unchanged.
`fn` must return the same box template with the same value type.

For boxes without an error channel, `fn` takes no arguments:

```cpp
auto result = std::optional<int>{}
    | bms::or_else([]() { return std::optional{0}; }); // optional{0}
```

For boxes with an error channel, `fn` receives the error:

```cpp
using E = std::expected<int, std::string>;
auto result = E{std::unexpect, "oops"}
    | bms::or_else([](const std::string&) { return E{-1}; }); // expected{-1}
```

### `transform_error` — map over the error

Calls `fn(error)` and replaces the error. Only available for box types with a real error
channel (`std::expected`-like types; not `std::optional` or pointers).

```cpp
using E = std::expected<int, int>;
auto result = E{std::unexpect, 42}
    | bms::transform_error([](int e) { return e * 2; }); // expected{unexpect, 84}
```

## Pipe Composition

All operations compose with `|`:

```cpp
const auto result =
    std::optional{10}
    | bms::and_then([](int v)    { return std::optional{v * 2.0}; })
    | bms::transform([](double v){ return static_cast<int>(v); })
    | bms::and_then([](int)      { return std::optional<char>{}; }) // fails here
    | bms::or_else([]()          { return std::optional{0}; });     // recovers
```

## Writing Your Own Operations

Because `box_traits` and `get_box_traits` are public, you can write new operations that work
across all adapted types without touching the library or the box types. See
[Design Rationale — Extensibility](design_rationale.md#extensibility) for details and examples.
