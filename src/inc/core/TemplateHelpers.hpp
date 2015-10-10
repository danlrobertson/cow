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

template < typename T >
struct has_clone {
private:
  template< typename U, U > struct signature_check { };
  struct flag { char one; char two; };
  template < typename U > static flag test(...);
  template < typename U > static char test( signature_check< T*(T::*)(), &U::clone >* = 0 );
public:
  static const bool value = sizeof( test< T >(0) ) == 1;
  typedef T type;
};

} // namespace core
} // namespace cow

#endif
