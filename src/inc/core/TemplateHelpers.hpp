/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef COW_CHECK_HPP
#define COW_CHECK_HPP

#include <type_traits>

namespace cow {
namespace core {

template < bool... Args >
struct And {
  static const bool value = true;
};

template < bool T, bool... Args >
struct And< T, Args... > {
  static const bool value = ::std::conditional< T,
               And< Args... >, ::std::false_type >::type::value;
};

template < bool... Args >
struct Or {
  static const bool value = false;
};

template < bool T, bool... Args >
struct Or< T, Args... > {
  static const bool value = ::std::conditional< T,
               ::std::true_type, Or< Args... > >::type::value;

};

} // namespace core
} // namespace cow

#endif
