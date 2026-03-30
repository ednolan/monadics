<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

# Design Rationale

## Goals

`beman.monadics` has two related goals:

1. **Validate a unifying abstraction.** Demonstrate that the monadic vocabulary (`and_then`,
   `transform`, `or_else`, `transform_error`) can be expressed as free functions over a single
   "box" abstraction that covers `std::optional`, `std::expected`, smart pointers, raw pointers,
   and arbitrary user types. This is intended to inform a future C++ standardization paper.

2. **Provide an open extension point.** Give users a foundation on which they can build
   additional operations — `flatten`, `join`, `value_or`, `sequence`, and others — that
   automatically work for every adapted box type, without modifying the library or the box type
   itself.

## Why Free Functions?

C++23 adds monadic operations as member functions of `std::optional` and `std::expected`. This
works for those types, but member functions cannot be added retroactively to existing types
(`std::shared_ptr`, raw pointers, third-party types), and they cannot be added by users at all
without modifying the type.

Free functions in a namespace make the operations open for extension:

```cpp
// works with any adapted type — no changes to the type required
ptr | and_then(f) | transform(g) | or_else(h);
```

## The Box Abstraction

A "box" is any type that can hold a value or signal absence/failure. Its capabilities are
expressed as a traits class users specialize: `box_traits<T>`.

The full interface — what a from-scratch specialization looks like:

```cpp
template <typename T>
struct beman::monadics::box_traits<MyBox<T>> {
    // --- Types -----------------------------------------------------------

    using value_type = T;
    using error_type = int;

    template <typename U> using rebind       = MyBox<U>;
    template <typename E> using rebind_error = MyBox<T, E>;

    // --- Queries ---------------------------------------------------------

    // Reports whether the box holds a value.
    [[nodiscard]] static bool has_value(const MyBox<T>&) noexcept;

    // Accesses the value; forwards the box's value category.
    [[nodiscard]] static decltype(auto) value(auto&& box) noexcept;

    // Accesses the error or absence sentinel. Two forms:
    //   nullary — sentinel is a constant (e.g. std::nullopt, nullptr)
    //   unary   — error is stored inside the box (e.g. std::expected)
    [[nodiscard]] static auto           error() noexcept;           // nullary
    [[nodiscard]] static decltype(auto) error(auto&& box) noexcept; // unary

    // --- Construction ----------------------------------------------------

    // Constructs a box holding a value.
    [[nodiscard]] static MyBox<T>    make(T v);

    // Constructs a box holding an error. Enables transform_error.
    [[nodiscard]] static MyBox<T, E> make_error(int e);
};
```

## Adapting an Existing Type

Most real types already expose part of this interface. The library runs a deduction cascade
for each capability: it checks the explicit specialization first, then the box type's own
members, then the template parameters. You only need to supply what it cannot find on its own.

### When everything is deducible — empty specialization

`std::expected<T, E>` already has `has_value()`, `value()`, `error()`, nested `value_type`
and `error_type`, and two template parameters that the library uses for rebind deduction.
There is nothing left to provide:

```cpp
template <typename T, typename E>
struct beman::monadics::box_traits<std::expected<T, E>> {};
```

All four operations work immediately.

### When a few pieces are missing — minimal specialization

`std::optional<T>` has `has_value()`, `value()`, and `value_type`, but it has no `error()`
member and no `error_type`. The absence sentinel `std::nullopt` is a compile-time constant,
not something stored in the box, so the library cannot derive it. One member is enough:

```cpp
template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] static constexpr auto error() noexcept { return std::nullopt; }
};
```

From this single function the library derives `error_type` (`std::nullopt_t`), and because
`optional` has only one template parameter every `rebind_error<E>` maps back to `optional<T>`,
so `has_error_channel<optional<T>>` is false and `transform_error` is automatically disabled.

## Why a Traits Class, Not a Concept or CRTP Base?

The central requirement is that the library must work retroactively — for types that already
exist and cannot be modified. Consider `std::shared_ptr<T>`. It has no `error()` member, no
`value_type`, no `make_error`. A concept-gated free function can constrain usage but cannot
supply missing members. A CRTP base requires inheritance, which is impossible for standard
library types and breaks for any `final` class.

A traits specialization adds the missing pieces without touching `std::shared_ptr` at all:

```cpp
template <typename T>
struct beman::monadics::box_traits<std::shared_ptr<T>> {
    static bool          has_value(const std::shared_ptr<T>& p) noexcept { return static_cast<bool>(p); }
    static decltype(auto) value(auto&& p) noexcept                       { return *std::forward<decltype(p)>(p); }
    static std::shared_ptr<T> error() noexcept                           { return nullptr; }
    static std::shared_ptr<T> make(T v)                                  { return std::make_shared<T>(std::move(v)); }
};
```

| Approach | Problem |
|---|---|
| Member-function concept | Requires the type to already expose the right interface; cannot retrofit |
| CRTP base | Requires inheritance; impossible for `final` classes and third-party types |
| **Traits specialization** | **Non-intrusive, retroactively applicable to any type** |

The traits pattern follows the precedent of `std::iterator_traits`, `std::char_traits`, and
`std::numeric_limits`. It separates the "what" (the box type) from the "how" (the trait
implementation) without touching the type itself.

## Extensibility

The second goal — an open extension point — is what distinguishes this library from simply
adding more member functions to standard types.

`get_box_traits<Box>` is a public type alias that exposes the full assembled traits for any
adapted box: `value_type`, `error_type`, `has_value`, `value`, `error`, `make`, `make_error`,
`rebind`, and `rebind_error`. Anyone can use it to write a new operation that works immediately
for every existing and future box type.

### Example: `value_or`

```cpp
template <beman::monadics::is_box Box>
auto value_or(Box&& box, typename beman::monadics::get_box_traits<Box>::value_type fallback) {
    using Traits = beman::monadics::get_box_traits<Box>;
    if (Traits::has_value(box))
        return Traits::value(std::forward<Box>(box));
    return fallback;
}
```

Works for `std::optional`, `std::expected`, `std::shared_ptr`, and any user-adapted type —
zero changes to the library or to the box types.

### Example: `flatten`

```cpp
template <beman::monadics::is_box Box>
    requires beman::monadics::is_box<typename beman::monadics::get_box_traits<Box>::value_type>
auto flatten(Box&& box) {
    using Traits    = beman::monadics::get_box_traits<Box>;
    using InnerBox  = typename Traits::value_type;
    using ITraits   = beman::monadics::get_box_traits<InnerBox>;

    if (!Traits::has_value(box))
        return ITraits::make_error(Traits::error(std::forward<Box>(box)));
    return Traits::value(std::forward<Box>(box));
}
```

`flatten(optional<optional<int>>{optional{42}})` → `optional{42}`.

### Example: pipe-compatible operations via `operator|`

New operations can also support the `|` pipe syntax by following the same `action` wrapper
pattern used by the library's own operations:

```cpp
struct value_or_t {
    template <typename T>
    struct action {
        T fallback;

        template <beman::monadics::is_box Box>
        friend auto operator|(Box&& box, action a) {
            using Traits = beman::monadics::get_box_traits<Box>;
            if (Traits::has_value(box))
                return Traits::value(std::forward<Box>(box));
            return std::move(a.fallback);
        }
    };

    template <typename T>
    auto operator()(T fallback) const { return action<T>{std::move(fallback)}; }
};

inline constexpr value_or_t value_or{};
```

```cpp
auto v = std::optional<int>{} | value_or(42); // 42
```

This pattern requires no changes to `beman.monadics` and no changes to `std::optional`.

## Value-Category Preservation

All four operations preserve the value category of the box. Passing an rvalue box forwards the
contained value as an rvalue to the callback, enabling move semantics through the chain:

```cpp
std::optional<std::string>{"hello"}
    | and_then([](std::string&& s) {       // s is rvalue
          return std::optional{std::move(s)};
      });
```

This is enforced via `invoke_with_value` and `invoke_with_error`, which forward the box using
the same value category as the input.

## Concept Constraints and Error Messages

The `operator|` for each operation is constrained by a named concept
(`and_thenable_impl`, `or_elseable_impl`, etc.) rather than an inline `requires` expression.
Named concepts surface human-readable `on_error<"msg">` diagnostics when a constraint fails,
whereas inline `requires` expressions produce only "expression not well-formed".

If `and_then` receives a callable that returns a box with a different error type, the compiler
reports:

```
constraint 'and_thenable_return<..., ...>' not satisfied:
  'Should return the Box with same error_type'
```

The constraint hierarchy is:

```
and_thenable_impl<Box, Fn>
  └── and_thenable_return<NewBox, OldBox>
        ├── same_box<NewBox, OldBox>           // same box template
        └── same_box_and_error<NewBox, OldBox> // same error type
```

## The `or_else` / `and_then` Asymmetry

`and_then` must preserve the **error type**: the callback replaces the value but the error
channel stays the same.

`or_else` must preserve the **value type**: the callback replaces the error but the value
channel stays the same.

This mirrors `std::expected`:

- `and_then(f)` — `f: T -> expected<U, E>` (E is fixed)
- `or_else(f)` — `f: E -> expected<T, F>` (T is fixed)

## Error Channel Detection

Not all boxes have a real error channel. `std::optional` has no error type — absence is
represented structurally. The library detects this via `has_error_channel`: a box has a real
error channel if and only if `rebind_error<int>` and `rebind_error<long>` produce different
types. For `optional`, both produce `optional` regardless of the argument, so
`has_error_channel<optional<T>>` is false and `transform_error` is disabled.

## Non-Goals

- **Runtime overhead**: all operations are `constexpr` and `noexcept`; no virtual dispatch,
  no allocation, no type erasure.
- **Heterogeneous chaining**: `and_then` does not allow changing the box template (only the
  value type). Mixing `optional` and `expected` in a single chain is not supported.
- **Lazy evaluation**: operations execute eagerly. Lazy/deferred evaluation is out of scope.
