module;

export module beman.monadics;

import beman.monadics.detail;

#define BEMAN_MONADICS_MODULE_INTERFACE

export namespace beman::monadics {

using detail::and_then;
using detail::box_traits;
using detail::get_box_traits;
using detail::or_else;
using detail::transform;
using detail::transform_error;

} // namespace beman::monadics

// export extern "C++" {
// #include "beman/monadics/monadics.hpp"
// }
