#ifndef Amis_configure_h_
#define Amis_configure_h_

#include <amis/config_amis.h>

#ifndef AMIS_NAMESPACE_BEGIN
#define AMIS_NAMESPACE_BEGIN namespace amis {
#define AMIS_NAMESPACE_END   }
#endif

#ifndef STD_NAMESPACE_BEGIN
#define STD_NAMESPACE_BEGIN namespace std {
#define STD_NAMESPACE_END   }
#endif

#ifndef GNU_NAMESPACE_BEGIN
#define GNU_NAMESPACE_BEGIN namespace __gnu_cxx {
#define GNU_NAMESPACE_END   }
#endif

#ifdef AMIS_DEBUG_LEVEL
#define AMIS_DEBUG_MODE( level ) ( level <= AMIS_DEBUG_LEVEL )
#define AMIS_DEBUG_MESSAGE( level, message ) \
    if ( level <= AMIS_DEBUG_LEVEL ) std::cerr << message;
#else // AMIS_DEBUG_LEVEL
#define AMIS_DEBUG_MODE( level ) 0
#define AMIS_DEBUG_MESSAGE( level, message )
#endif // AMIS_DEBUG_LEVEL




#if __GNUC__ == 4 && __GNUC_MINOR__ == 0 // hack for gcc 4.0.x
  #include <ext/hash_map>
  #include <string>
  GNU_NAMESPACE_BEGIN
  template <> struct hash< std::string > {
    size_t operator()( const std::string& s ) const {
      return hash< const char* >()( s.c_str() );
    }
  };
  GNU_NAMESPACE_END

  AMIS_NAMESPACE_BEGIN
  template < class KEY, class VALUE >
  class amis_hash_map : public __gnu_cxx::hash_map< KEY, VALUE > {};
  AMIS_NAMESPACE_END

#elif defined(HAVE_TR1_UNORDERED_MAP) // GCC 4.3.0

  #include <tr1/unordered_map>
  #include <string>
  AMIS_NAMESPACE_BEGIN
  template < class KEY, class VALUE >
  class amis_hash_map : public std::tr1::unordered_map< KEY, VALUE > {};

  AMIS_NAMESPACE_END

#elif defined(HAVE_EXT_HASH_MAP) // GCC 3.x

  #include <ext/hash_map>
  #include <string>


  #if __GNUC__ == 3 && __GNUC_MINOR__ == 0 // hack for gcc 3.0.x
    STD_NAMESPACE_BEGIN
    template <> struct hash< string > {
      size_t operator()( const string& s ) const {
        return hash< const char* >()( s.c_str() );
      }
    };
    STD_NAMESPACE_END

  #else // __GNUC_MAJOR == 3 && __GNUC_MINOR__ == 0
    GNU_NAMESPACE_BEGIN
    template <> struct hash< std::string > {
      size_t operator()( const std::string& s ) const {
        return hash< const char* >()( s.c_str() );
      }
    };
    GNU_NAMESPACE_END
  #endif // __GNUC_MAJOR == 3 && __GNUC_MINOR__ == 0

    AMIS_NAMESPACE_BEGIN
    template < class KEY, class VALUE >
    class amis_hash_map : public __gnu_cxx::hash_map< KEY, VALUE > {};
    AMIS_NAMESPACE_END

#else // HAVE_EXT_HASH_MAP

  #include <hash_map>
  #include <string>
  STD_NAMESPACE_BEGIN
  template <> struct hash< string > {
    size_t operator()( const string& s ) const {
      return hash< const char* >()( s.c_str() );
    }
  };
  STD_NAMESPACE_END
  AMIS_NAMESPACE_BEGIN
  template < class KEY, class VALUE >
  class amis_hash_map : public std::hash_map< KEY, VALUE > {};
  AMIS_NAMESPACE_END

#endif // HAVE_EXT_HASH_MAP



#ifdef HAVE_EXT_POOL_ALLOCATOR_H
#include <ext/pool_allocator.h>
AMIS_NAMESPACE_BEGIN
template < typename T >
class Allocator : public __gnu_cxx::__pool_alloc< T > {
};
AMIS_NAMESPACE_END
#else // HAVE_EXT_POOL_ALLOCATOR_H
template < typename T >
class Allocator : public std::allocator< T > {
};
#endif // HAVE_EXT_POOL_ALLOCATOR_H

#ifdef  AMIS_PARALLEL
#define _REENTRANT
#endif

#endif // configure_h_
// end of configure.h
