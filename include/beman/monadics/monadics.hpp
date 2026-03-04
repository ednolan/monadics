// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_MONADICS_HPP
#define BEMAN_MONADICS_MONADICS_HPP

#include <beman/monadics/detail/config.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/and_then.hpp>
#include <beman/monadics/detail/or_else.hpp>
#include <beman/monadics/detail/transform.hpp>
#include <beman/monadics/detail/transform_error.hpp>

namespace beman::monadics {

BEMAN_MONADICS_MODULE_EXPORT using detail::and_then;
BEMAN_MONADICS_MODULE_EXPORT using detail::box_traits;
BEMAN_MONADICS_MODULE_EXPORT using detail::get_box_traits;
BEMAN_MONADICS_MODULE_EXPORT using detail::or_else;
BEMAN_MONADICS_MODULE_EXPORT using detail::transform;
BEMAN_MONADICS_MODULE_EXPORT using detail::transform_error;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_MONADICS_HPP
