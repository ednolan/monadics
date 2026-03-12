// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_MONADICS_HPP
#define BEMAN_MONADICS_MONADICS_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/and_then.hpp>
#include <beman/monadics/detail/or_else.hpp>
#include <beman/monadics/detail/transform.hpp>
#include <beman/monadics/detail/transform_error.hpp>

namespace beman::monadics {

using detail::box_traits;

template <typename T>
concept is_box = detail::is_box<T>;

template <typename T>
using get_box_traits = detail::get_box_traits<T>;

using detail::and_then;
using detail::or_else;
using detail::transform;
using detail::transform_error;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_MONADICS_HPP
