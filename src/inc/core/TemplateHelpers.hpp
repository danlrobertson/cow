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
  typedef std::true_type type;
  static const bool value = type::value;
};

template < bool T, bool... Args >
struct And< T, Args... > {
  typedef typename ::std::conditional< T,
          And< Args... >, ::std::false_type >::type type;
  static const bool value = type::value;
};

template < bool... Args >
struct Or {
  typedef std::false_type type;
  static const bool value = type::value;
};

template < bool T, bool... Args >
struct Or< T, Args... > {
  typedef typename ::std::conditional< T,
               ::std::true_type, Or< Args... > >::type type;
  static const bool value = type::value;
};


namespace helpers {
  template < typename T >
  struct true_if_clone {
      static const bool value = true;
  };

  template< typename T >
  static true_if_clone<decltype(std::declval<T>().clone())> test_has_clone(int);

  template< typename T >
  static ::std::false_type test_has_clone(long);
}

template<class T>
struct has_clone : decltype(helpers::test_has_clone<T>(0)){};

} // namespace core
} // namespace cow

#endif
