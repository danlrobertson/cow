/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef COW_PTRIMPL_HPP
#define COW_PTRIMPL_HPP

#include <type_traits>
#include "../err/ErrorClasses.hpp"

namespace cow {
namespace core {

// prototype impl type
template < typename T, typename P >
class cow_ptr_base {
private:
  typename P::rc_type m_rc;

public:
  // must remove extent for array type
  typedef typename ::std::remove_extent< T >::type value_type;
  typename P::rc_type rc() { return m_rc; }

  cow_ptr_base( const cow_ptr_base& ) = delete;
  cow_ptr_base& operator=( const cow_ptr_base& ) = delete;

  // default ctor
  cow_ptr_base() : m_rc( 1 ) { }

  // ref counting functions
  typename P::rc_type decRef() { return P::decRef( m_rc ); }
  void                incRef() { return P::incRef( m_rc ); }

  // interface
  virtual bool        gc() = 0;
  virtual cow_ptr_base*  clone() = 0;
  virtual T*          get() = 0;
  virtual             ~cow_ptr_base() { }
};

// basic default by value implementation of cow_ptr_base.
// this is the prefered type as there are fewer heap
// allocs when this impl is used
template < typename T, typename P >
class cow_ptr_val : public cow_ptr_base< T, P > {
public:
  T m_impl;

  cow_ptr_val(): m_impl() { }

  cow_ptr_val( const T& src ) : m_impl( src ) { }

  virtual cow_ptr_val* clone() { return new cow_ptr_val( m_impl ); }

  virtual T* get() { return &m_impl; }

  virtual bool gc();
};

// all abstract base class passed to cow_ptr_ptr must
// implement T* clone() and have a virtual destructor
template < typename T, typename P >
class cow_ptr_ptr : public cow_ptr_base< T, P > {
private:
  cow_ptr_ptr( T* src ) : m_impl( src ) { }

public:
  T* m_impl;

  cow_ptr_ptr() : m_impl() { }

  template < typename U >
  cow_ptr_ptr( const U& src ) : m_impl( new U( src ) ) { }

  virtual cow_ptr_ptr* clone() { return new cow_ptr_ptr( m_impl->clone() ); }

  virtual T* get() { return m_impl; }

  virtual bool gc();
};

// array implementation of the cow_ptr_base type
template < typename T, typename P >
struct cow_ptr_arr : public cow_ptr_base< T, P > {
private:
  cow_ptr_arr( T* ptr, size_t s ): m_impl( ptr ), sz( s ) { }

public:
  T*     m_impl;
  size_t sz;

  cow_ptr_arr(): m_impl( nullptr ), sz( 0 ) { }
  cow_ptr_arr( size_t s ): m_impl( new T[ s ]() ), sz( s ) { }

  virtual cow_ptr_arr* clone();

  virtual T* get() { return m_impl; }

  virtual bool gc();
};

template< typename T, typename P >
bool cow_ptr_val< T, P >::gc() {
  if( !cow_ptr_base< T, P >::decRef() ) {
    delete this;
    return true;
  }
  return false;
}

template< typename T, typename P >
bool cow_ptr_ptr< T, P >::gc() {
  if( !cow_ptr_base< T, P >::decRef() ) {
    delete m_impl;
    delete this;
    return true;
  }
  return false;
}

template< typename T, typename P >
cow_ptr_arr< T, P >* cow_ptr_arr< T, P >::clone() {
  T* tmp = new T[ sz ];
  for( size_t i = 0; i < sz; ++i ) {
    tmp[i] = m_impl[i];
  }
  return new cow_ptr_arr( tmp, sz );
}

template< typename T, typename P >
bool cow_ptr_arr< T, P >::gc() {
  if( !cow_ptr_base< T, P >::decRef() ) {
    delete[] m_impl;
    delete this;
    return true;
  }
  return false;
}

} // namespace core
} // namespace cow

#endif
