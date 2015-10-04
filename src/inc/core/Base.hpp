/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef COW_BASE_HPP
#define COW_BASE_HPP

#include <type_traits>

#include "../err/ErrorClasses.hpp"
#include "BaseImpl.hpp"
#include "TemplateHelpers.hpp"

namespace cow {
namespace core {

// create our basic ref counting policy
struct BasicRC {
  typedef unsigned int rc_type;
  static void incRef( rc_type& n ) { ++n; }
  static rc_type decRef( rc_type& n ) { return --n; }
};

template < typename T, typename P >
struct _cow_base_type_check {
  typedef typename ::std::conditional< ::std::is_array< T >::value,
      _cow_base_arr< typename ::std::remove_extent< T >::type, P >,
      typename ::std::conditional< ::std::is_abstract< T >::value,
                                   _cow_base_ptr< T, P >, _cow_base_val< T, P >
                                  >::type >::type type;
};

template < typename T, typename P = BasicRC >
class cow_base {
public:
  typedef typename _cow_base_type_check< T, P >::type impl_t;
  typedef typename impl_t::value_type                 value_type;

private:
  impl_t* m_pimpl;

public:
  cow_base() : m_pimpl( new impl_t ) { }

  template < typename U >
  cow_base( const U& src ) : m_pimpl( new impl_t( src ) ) { }

  explicit cow_base( const cow_base& );

  explicit cow_base( cow_base&& );

  cow_base& operator=( const cow_base& );

  cow_base& operator=( cow_base&& );

  value_type* make_unique();

  typename P::rc_type use_count() const { return m_pimpl->rc(); }

  const value_type* get() const { return m_pimpl->get(); }
  value_type* get() { return make_unique(); }

  value_type& operator*() { return *make_unique(); }
  const value_type& operator*() const { return *( m_pimpl->get() ); }

  value_type* operator->() { return make_unique(); }
  const value_type* operator->() const { return m_pimpl->get(); }

  bool same_instance( const cow_base& ) const;

  bool operator==( const cow_base& ) const;

  bool operator!=( const cow_base& ) const;

  ~cow_base();
};

template < typename T, typename P >
cow_base< T, P >::cow_base( const cow_base& src ) : m_pimpl( src.m_pimpl ) {
  m_pimpl->incRef();
}

template < typename T, typename P >
cow_base< T, P >::cow_base( cow_base&& src ) : m_pimpl( src.m_pimpl ) {
  src.m_pimpl = nullptr;
}

template < typename T, typename P >
cow_base< T, P >& cow_base< T, P >::operator=( const cow_base& src ) {
  if( src.m_pimpl )
    src.m_pimpl->incRef();
  m_pimpl->gc();

  m_pimpl = src.m_pimpl;

  return *this;
}

template < typename T, typename P >
cow_base< T, P >& cow_base< T, P >::operator=( cow_base&& src ) {
  m_pimpl->gc();

  m_pimpl = src.m_pimpl;

  src.m_pimpl = nullptr;
  return *this;
}

template < typename T, typename P >
bool cow_base< T, P >::same_instance( const cow_base& rhs ) const {
  return m_pimpl && m_pimpl == rhs.m_pimpl;
}

template < typename T, typename P >
bool cow_base< T, P >::operator==( const cow_base& rhs ) const {
  return same_instance( rhs );
  // TODO: add something like the following to add a layer
  // to eqality
  // && ( *get() == *rhs );
}

template < typename T, typename P >
bool cow_base< T, P >::operator!=( const cow_base& rhs ) const {
  return !same_instance( rhs );
  // TODO: add something like the following to add a layer
  // to ineqality
  // || ( *get() == *rhs ) );
}

template< typename T, typename P >
typename cow_base< T, P >::value_type* cow_base< T, P >::make_unique() {
  if( m_pimpl && m_pimpl->rc() > 1 ) {
    impl_t* impl = m_pimpl->clone();
    m_pimpl->gc();
    m_pimpl = impl;
  }
  return m_pimpl->get();
}

template < typename T, typename P >
cow_base< T, P >::~cow_base() {
  if( m_pimpl ) {
    m_pimpl->gc();
  }
}

} // namespace core
} // namespace cow

#endif
