#include <type_traits>
#include "../err/ErrorClasses.hpp"

namespace cow {
namespace core {

// prototype impl type
template < typename T, typename P >
class _cow_base {
private:
  typename P::rc_type m_rc;

public:
  // must remove extent for array type
  typedef typename ::std::remove_extent< T >::type value_type;
  typename P::rc_type rc() { return m_rc; }

  _cow_base( const _cow_base& ) = delete;
  _cow_base& operator=( const _cow_base& ) = delete;

  // default ctor
  _cow_base() : m_rc( 1 ) { }

  // ref counting functions
  typename P::rc_type decRef() { return P::decRef( m_rc ); }
  void                incRef() { return P::incRef( m_rc ); }

  // interface
  virtual bool        gc() = 0;
  virtual _cow_base*  clone() = 0;
  virtual T*          get() = 0;
  virtual             ~_cow_base() { }
};

// basic default by value implementation of _cow_base.
// this is the prefered type as there are fewer heap
// allocs when this impl is used
template < typename T, typename P >
class _cow_base_val : public _cow_base< T, P > {
public:
  T m_impl;

  _cow_base_val(): m_impl() { }

  _cow_base_val( const T& src ) : m_impl( src ) { }

  virtual _cow_base_val* clone() { return new _cow_base_val( m_impl ); }

  virtual T* get() { return &m_impl; }

  virtual bool gc();
};

// all abstract base class passed to _cow_base_ptr must
// implement T* clone() and have a virtual destructor
template < typename T, typename P >
class _cow_base_ptr : public _cow_base< T, P > {
private:
  _cow_base_ptr( T* src ) : m_impl( src ) { }

public:
  T* m_impl;

  _cow_base_ptr() : m_impl() { }

  template < typename U >
  _cow_base_ptr( const U& src ) : m_impl( new U( src ) ) { }

  virtual _cow_base_ptr* clone() { return new _cow_base_ptr( m_impl->clone() ); }

  virtual T* get() { return m_impl; }

  virtual bool gc();
};

// array implementation of the _cow_base type
template < typename T, typename P >
struct _cow_base_arr : public _cow_base< T, P > {
private:
  _cow_base_arr( T* ptr, size_t s ): m_impl( ptr ), sz( s ) { }

public:
  T*     m_impl;
  size_t sz;

  _cow_base_arr(): m_impl( nullptr ), sz( 0 ) { }
  _cow_base_arr( size_t s ): m_impl( new T[ s ]() ), sz( s ) { }

  virtual _cow_base_arr* clone();

  virtual T* get() { return m_impl; }

  virtual bool gc();
};

template< typename T, typename P >
bool _cow_base_val< T, P >::gc() {
  if( !_cow_base< T, P >::decRef() ) {
    delete this;
    return true;
  }
  return false;
}

template< typename T, typename P >
bool _cow_base_ptr< T, P >::gc() {
  if( !_cow_base< T, P >::decRef() ) {
    delete m_impl;
    delete this;
    return true;
  }
  return false;
}

template< typename T, typename P >
_cow_base_arr< T, P >* _cow_base_arr< T, P >::clone() {
  T* tmp = new T[ sz ];
  for( size_t i = 0; i < sz; ++i ) {
    tmp[i] = m_impl[i];
  }
  return new _cow_base_arr( tmp, sz );
}

template< typename T, typename P >
bool _cow_base_arr< T, P >::gc() {
  if( !_cow_base< T, P >::decRef() ) {
    delete[] m_impl;
    delete this;
    return true;
  }
  return false;
}

} // namespace core
} // namespace cow
