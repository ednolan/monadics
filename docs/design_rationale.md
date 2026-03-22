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

A "box" is any type that:

- reports whether it holds a value (`has_value`)
- provides access to the value (`value`)
- provides access to the error or absence sentinel (`error`)
- can be reconstructed from a value (`make`) or an error (`make_error`)
- knows how to rebind its value type (`rebind<U>`) and error type (`rebind_error<E>`)

These capabilities are expressed as a traits class users specialize: `box_traits<T>`.

## Why a Traits Class, Not a Concept or CRTP Base?

| Approach | Problem |
|---|---|
| Member-function concept | Cannot add members to existing types; intrusive |
| CRTP base | Requires inheritance; breaks for `final` classes and third-party types |
| Traits specialization | Non-intrusive, retroactively applicable to any type |

The traits pattern follows the precedent of `std::iterator_traits`, `std::char_traits`, and
`std::numeric_limits`. It separates the "what" (the box type) from the "how" (the trait
implementation) without touching the type itself.

## Auto-Deduction

For types that already expose a compatible interface, an empty specialization suffices:

```cpp
template <typename T, typename E>
struct beman::monadics::box_traits<std::expected<T, E>> {};
```

The library internally runs a deduction cascade (`get_value_fn`, `get_error_fn`, etc.) that
probes the type for each capability in priority order: explicit trait override first, then
member function, then deduced from structure. This keeps the opt-in lightweight for
well-structured types while remaining fully customizable for others.

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
    struct action { T fallback; };

    template <typename T>
    auto operator()(T fallback) const { return action<T>{std::move(fallback)}; }
};

template <beman::monadics::is_box Box, typename T>
auto operator|(Box&& box, value_or_t::action<T> a) {
    using Traits = beman::monadics::get_box_traits<Box>;
    if (Traits::has_value(box))
        return Traits::value(std::forward<Box>(box));
    return std::move(a.fallback);
}

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
