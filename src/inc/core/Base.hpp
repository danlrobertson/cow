/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef COW_PTR_HPP
#define COW_PTR_HPP

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
struct cow_ptr_type_check {
  typedef typename ::std::conditional< ::std::is_array< T >::value,
      cow_ptr_arr< typename ::std::remove_extent< T >::type, P >,
      typename ::std::conditional< ::std::is_abstract< T >::value,
                                   cow_ptr_ptr< T, P >, cow_ptr_val< T, P >
                                  >::type >::type type;
};

template < typename T, typename P = BasicRC >
class cow_ptr {
public:
  typedef typename cow_ptr_type_check< T, P >::type  impl_t;
  typedef typename impl_t::value_type                value_type;

private:
  impl_t* m_pimpl;

public:
  cow_ptr() : m_pimpl( new impl_t ) { }

  template < typename U >
  cow_ptr( const U& src ) : m_pimpl( new impl_t( src ) ) { }

  explicit cow_ptr( const cow_ptr& );

  explicit cow_ptr( cow_ptr&& );

  cow_ptr& operator=( const cow_ptr& );

  cow_ptr& operator=( cow_ptr&& );

  value_type* make_unique();

  typename P::rc_type use_count() const { return m_pimpl->rc(); }

  const value_type* get() const { return m_pimpl->get(); }
  value_type* get() { return make_unique(); }

  value_type& operator*() { return *make_unique(); }
  const value_type& operator*() const { return *( m_pimpl->get() ); }

  value_type* operator->() { return make_unique(); }
  const value_type* operator->() const { return m_pimpl->get(); }

  bool same_instance( const cow_ptr& ) const;

  bool operator==( const cow_ptr& ) const;

  bool operator!=( const cow_ptr& ) const;

  ~cow_ptr();
};

template < typename T, typename P >
cow_ptr< T, P >::cow_ptr( const cow_ptr& src ) : m_pimpl( src.m_pimpl ) {
  m_pimpl->incRef();
}

template < typename T, typename P >
cow_ptr< T, P >::cow_ptr( cow_ptr&& src ) : m_pimpl( src.m_pimpl ) {
  src.m_pimpl = nullptr;
}

template < typename T, typename P >
cow_ptr< T, P >& cow_ptr< T, P >::operator=( const cow_ptr& src ) {
  if( src.m_pimpl )
    src.m_pimpl->incRef();
  m_pimpl->gc();

  m_pimpl = src.m_pimpl;

  return *this;
}

template < typename T, typename P >
cow_ptr< T, P >& cow_ptr< T, P >::operator=( cow_ptr&& src ) {
  m_pimpl->gc();

  m_pimpl = src.m_pimpl;

  src.m_pimpl = nullptr;
  return *this;
}

template < typename T, typename P >
bool cow_ptr< T, P >::same_instance( const cow_ptr& rhs ) const {
  return m_pimpl && m_pimpl == rhs.m_pimpl;
}

template < typename T, typename P >
bool cow_ptr< T, P >::operator==( const cow_ptr& rhs ) const {
  return same_instance( rhs );
  // TODO: add something like the following to add a layer
  // to eqality
  // && ( *get() == *rhs );
}

template < typename T, typename P >
bool cow_ptr< T, P >::operator!=( const cow_ptr& rhs ) const {
  return !same_instance( rhs );
  // TODO: add something like the following to add a layer
  // to ineqality
  // || ( *get() == *rhs ) );
}

template< typename T, typename P >
typename cow_ptr< T, P >::value_type* cow_ptr< T, P >::make_unique() {
  if( m_pimpl && m_pimpl->rc() > 1 ) {
    impl_t* impl = m_pimpl->clone();
    m_pimpl->gc();
    m_pimpl = impl;
  }
  return m_pimpl->get();
}

template < typename T, typename P >
cow_ptr< T, P >::~cow_ptr() {
  if( m_pimpl ) {
    m_pimpl->gc();
  }
}

} // namespace core
} // namespace cow

#endif
