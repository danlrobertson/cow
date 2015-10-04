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
class _cow_ptr {
private:
  typename P::rc_type m_rc;

public:
  // must remove extent for array type
  typedef typename ::std::remove_extent< T >::type value_type;
  typename P::rc_type rc() { return m_rc; }

  _cow_ptr( const _cow_ptr& ) = delete;
  _cow_ptr& operator=( const _cow_ptr& ) = delete;

  // default ctor
  _cow_ptr() : m_rc( 1 ) { }

  // ref counting functions
  typename P::rc_type decRef() { return P::decRef( m_rc ); }
  void                incRef() { return P::incRef( m_rc ); }

  // interface
  virtual bool        gc() = 0;
  virtual _cow_ptr*  clone() = 0;
  virtual T*          get() = 0;
  virtual             ~_cow_ptr() { }
};

// basic default by value implementation of _cow_ptr.
// this is the prefered type as there are fewer heap
// allocs when this impl is used
template < typename T, typename P >
class _cow_ptr_val : public _cow_ptr< T, P > {
public:
  T m_impl;

  _cow_ptr_val(): m_impl() { }

  _cow_ptr_val( const T& src ) : m_impl( src ) { }

  virtual _cow_ptr_val* clone() { return new _cow_ptr_val( m_impl ); }

  virtual T* get() { return &m_impl; }

  virtual bool gc();
};

// all abstract base class passed to _cow_ptr_ptr must
// implement T* clone() and have a virtual destructor
template < typename T, typename P >
class _cow_ptr_ptr : public _cow_ptr< T, P > {
private:
  _cow_ptr_ptr( T* src ) : m_impl( src ) { }

public:
  T* m_impl;

  _cow_ptr_ptr() : m_impl() { }

  template < typename U >
  _cow_ptr_ptr( const U& src ) : m_impl( new U( src ) ) { }

  virtual _cow_ptr_ptr* clone() { return new _cow_ptr_ptr( m_impl->clone() ); }

  virtual T* get() { return m_impl; }

  virtual bool gc();
};

// array implementation of the _cow_ptr type
template < typename T, typename P >
struct _cow_ptr_arr : public _cow_ptr< T, P > {
private:
  _cow_ptr_arr( T* ptr, size_t s ): m_impl( ptr ), sz( s ) { }

public:
  T*     m_impl;
  size_t sz;

  _cow_ptr_arr(): m_impl( nullptr ), sz( 0 ) { }
  _cow_ptr_arr( size_t s ): m_impl( new T[ s ]() ), sz( s ) { }

  virtual _cow_ptr_arr* clone();

  virtual T* get() { return m_impl; }

  virtual bool gc();
};

template< typename T, typename P >
bool _cow_ptr_val< T, P >::gc() {
  if( !_cow_ptr< T, P >::decRef() ) {
    delete this;
    return true;
  }
  return false;
}

template< typename T, typename P >
bool _cow_ptr_ptr< T, P >::gc() {
  if( !_cow_ptr< T, P >::decRef() ) {
    delete m_impl;
    delete this;
    return true;
  }
  return false;
}

template< typename T, typename P >
_cow_ptr_arr< T, P >* _cow_ptr_arr< T, P >::clone() {
  T* tmp = new T[ sz ];
  for( size_t i = 0; i < sz; ++i ) {
    tmp[i] = m_impl[i];
  }
  return new _cow_ptr_arr( tmp, sz );
}

template< typename T, typename P >
bool _cow_ptr_arr< T, P >::gc() {
  if( !_cow_ptr< T, P >::decRef() ) {
    delete[] m_impl;
    delete this;
    return true;
  }
  return false;
}

} // namespace core
} // namespace cow

#endif
