//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Array.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Array_h_

#define Amis_Array_h_

#include <amis/configure.h>
#include <vector>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>Array</name>
/// <overview>Fixed-size array</overview>
/// <body>

template < typename T, typename Alloc = Allocator< T > >
class Array {
public:
  typedef Alloc allocator_type;
  typedef typename allocator_type::size_type size_type;

private:
  T* head, * tail;

public:
  typedef const T* const_iterator;
  typedef T* iterator;

  const_iterator begin() const { return head; }
  const_iterator end() const { return tail; }
  iterator begin() { return head; }
  iterator end() { return tail; }
  
protected:
  static T* allocate( size_type n ) {
    return Alloc().allocate( n );
  }
  static void deallocate( T* p, size_type n ) {
    Alloc().deallocate( p, n );
  }

public:
  size_type size() const { return static_cast< size_type >( tail - head ); }
  bool empty() const { return size() == 0; }
  const T& operator[]( size_type n ) const { return *( head + n ); }
  T& operator[]( size_type n ) { return *( head + n ); }

  Array() {
    head = tail = NULL;
  }

  Array( const std::vector< T >& vec ) {
    if ( vec.size() == 0 ) {
      head = tail = NULL;
    } else {
      head = allocate( vec.size() );
      tail = head + vec.size();
      for ( size_type i = 0; i < vec.size(); ++i ) {
        head[ i ] = vec[ i ];
      }
    }
  }
  
  Array( const Array& x ) {
    if ( x.head == x.tail ) {
      head = tail = NULL;
    } else {
      head = allocate( x.size() );
      T* it = head;
      for ( T* it2 = x.head; it2 != x.tail; ++it, ++it2 ) {
        *it = *it2;
      }
      tail = it;
    }
  }

  ~Array() {
    if ( head != NULL ) {
      deallocate( head, tail - head );
    }
  }

  Array& operator=( const Array& x ) {
    if ( head != NULL ) deallocate( head, tail - head );
    if ( x.head == x.tail ) {
      head = tail = NULL;
    } else {
      head = allocate( x.size() );
      T* it = head;
      for ( T* it2 = x.head; it2 != x.tail; ++it, ++it2 ) {
        *it = *it2;
      }
      tail = it;
    }
    return *this;
  }
  
  void resize( size_type n ) {
    if ( head != NULL ) deallocate( head, tail - head );
    if ( n ) {
      head = allocate( n );
      tail = head + n;
    } else {
      head = tail = NULL;
    }
  }

  void swap( T& x ) {
    std::swap( head, x.head );
    std::swap( tail, x.tail );
  }
  
public:
  void readObject( objstream& is ) {
    if ( head != NULL ) deallocate( head, tail - head );
    size_type num;
    is >> num;
    if ( num == 0 ) {
      head = tail = NULL;
    } else {
      head = allocate( num );
      tail = head + num;
      for ( T* it = head; it != tail; ++it ) {
        it->readObject( is );
      }
    }
  }

  void writeObject( objstream& os ) const {
    os << size();
    for( T* it = head; it != tail; ++it ) {
      it->writeObject( os );
    }
  }
};

AMIS_NAMESPACE_END

/// </body>
/// </classdef>

#endif // Array_h_
// end of Array.h
