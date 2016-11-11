//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Profile.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Profile_h_

#define Amis_Profile_h_

#include <amis/configure.h>


#include <string>

#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <ctime>
#else // TIME_WITH_SYS_TIME
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else // HAVE_SYS_TIME_H
#include <ctime>
#endif // HAVE_SYS_TIME_H
#endif // TIME_WITH_SYS_TIME

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif // HAVE_SYS_RESOURCE_H



#include <csignal>
#include <amis/StringStream.h>
#include <iomanip>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>ProfUnit</name>
/// <overview>A basic unit for profiling</overview>
/// <desc>
/// The class provides a unit for passive profiling.
/// </desc>
/// <body>

//class ProfUnit;

// struct profunit_hash : public hash< size_t > {
//   size_t operator()( const ProfUnit* unit ) const {
//     return hash< size_t >::operator()( reinterpret_cast< size_t >( unit ) );
//   }
// };

// struct profname_hash : public hash< size_t > {
//   size_t operator()( const char* name ) const {
//     return hash< size_t >::operator()( reinterpret_cast< size_t >( name ) );
//   }
// };

class ProfUnit {
public:
  typedef unsigned int ProfTick;
  /// A measure for profile time
  typedef long ProfCount;
  /// A measure for profile count

  static const long PROFILE_INTERVAL;
  /// An interval of passive profiling

protected:
  

private:
  std::string name;
  /// The name of this unit
  ProfCount pass_count;
  /// The count of entering this unit
  ProfCount reentrant_count;
  /// The number of reentrancy to this unit
  ProfTick total_tick;
  /// The total tick of this unit
  ProfTick child_tick;
  /// The number of ticks the children of this unit consumed
  //hash_map< const ProfUnit*, ProfTick, profunit_hash > call_graph;
  amis_hash_map< size_t, ProfTick > call_graph;
  /// Each tick count for children

public:
  ProfUnit( const std::string& n ) : name( n ), call_graph() {
    pass_count = 0;
    reentrant_count = 0;
    total_tick = 0;
    child_tick = 0;
  }
  /// Construct a new profile unit
  ~ProfUnit() {}
  /// Destructor

public:
  const std::string& getName() const {
    return name;
  }
  /// Get the name of this unit
  ProfCount passCount() const {
    return pass_count;
  }
  /// Get the current pass count of this unit
  ProfCount reentrantCount() const {
    return reentrant_count;
  }
  /// Get the number of reentrancy to this unit
  ProfTick tickSec( ProfTick tick ) const {
    return tick / (1000000 / PROFILE_INTERVAL );
  }
  /// Convert the tick count to seconds
  ProfTick tickMSec( ProfTick tick ) const {
    return tick % (1000000 / PROFILE_INTERVAL );
  }
  /// Convert the tick count to milli-seconds
  ProfTick totalTick() const {
    return total_tick;
  }
  /// Get the total tick count of this unit
  ProfTick childTick() const {
    return child_tick;
  }
  /// Get the tick count consumed by children
//   const hash_map< const ProfUnit*, ProfTick, profunit_hash >& callGraph() const {
//     return call_graph;
//   }
  const amis_hash_map< size_t, ProfTick >& callGraph() const {
    return call_graph;
  }
  /// Each tick count consumed by children

public:
  std::string tickString( ProfTick tick ) const {
    OStringStream os;
    os << std::setw( 7 ) << tickSec( tick ) << '.'
       << std::setfill( '0' ) << std::setw( 2 ) << tickMSec( tick ) << std::setfill( ' ' );
    return os.str();
  }
  /// Convert the tick count to a string expression
  std::string totalTickString() const {
    return tickString( totalTick() );
  }
  /// Get the total tick count in a string expression
  std::string childTickString() const {
    return tickString( childTick() );
  }
  /// Get the tick count consumed by children in a string expression

public:
  void enter() {
    ++pass_count;
    ++reentrant_count;
  }
  /// Enter this unit
  bool leave() {
    return --reentrant_count == 0;
  }
  /// Leave this unit
  void addTick( ProfTick tick ) {
    total_tick += tick;
  }
  /// Add a tick count of this unit
  void addChildTick( const ProfUnit* child, ProfTick tick ) {
    child_tick += tick;
    call_graph[ reinterpret_cast< size_t >( child ) ] += tick;
  }
  /// Add a tick count consumed by a child
};

/// </body>
/// </classdef>

/// <classdef>
/// <name>Profile</name>
/// <overview>Profiling tools</overview>
/// <desc>
/// The class provides profiling tools by passive profiling.
/// If you make a new instance of this class, the time from construction to
/// destruction is measured by passive profiling.
/// You should use PROF1, ...,  macros defined in the end of this file.
/// </desc>
/// <body>

class Profile {
public:
  typedef ProfUnit::ProfTick ProfTick;
  /// A measure for profile time
  typedef ProfUnit::ProfCount ProfCount;
  /// A measure for profile count

#ifdef AMIS_PROFILE_LEVEL
#ifdef __CYGWIN__
  static const int ITIMER_WHICH = ITIMER_REAL;
  static const int SIGACTION_SIGNUM = SIGALRM;
#else // __CYGWIN__
  static const int ITIMER_WHICH = ITIMER_PROF;
  static const int SIGACTION_SIGNUM = SIGPROF;
#endif // __CYGWIN__
#endif // AMIS_PROFILE_LEVEL

private:
  static int profile_enabled;
  /// Whether a profiling is enabled
  //static hash_map< const char*, ProfUnit*, profname_hash > unit_table;
  static amis_hash_map< size_t, ProfUnit* > unit_table;
  /// A table of profiling units
  static Profile* current_profile;
  /// A currently enabled profile
#ifdef AMIS_PROFILE_LEVEL
  static struct sigaction action;
  /// A structure of sigaction for SIGPROF
  static struct itimerval timer;
  /// A structure of itimerval for setitimer
  static struct sigaction old_action;
  /// An old structure of sigaction for SIGPROF
  static struct itimerval old_timer;
  /// An old structure of itimerval for setitimer
#endif // AMIS_PROFILE_LEVEL

  class CompTick {
  public:
    bool operator()( const ProfUnit* arg1, const ProfUnit* arg2 ) {
      return arg1->totalTick() > arg2->totalTick();
    }
  };
  /// Comparing function of ProfUnit*

protected:
  static void signalHandler( int );
  /// A signal handler for SIGPROF
  static ProfUnit* findProfUnit( const char* name ) {
    //hash_map< const char*, ProfUnit*, profname_hash >::const_iterator it
    amis_hash_map< size_t, ProfUnit* >::const_iterator it
      = unit_table.find( reinterpret_cast< size_t >( name ) );
    if ( it == unit_table.end() ) {
      ProfUnit* new_unit = new ProfUnit( name );
      unit_table[ reinterpret_cast< size_t >( name ) ] = new_unit;
      return new_unit;
    } else {
      return it->second;
    }
  }
  /// Find an instance of ProfUnit of a specified name

public:
  static void initializeProfile();
  /// Initialize profiling
  static void finalizeProfile();
  /// Finalize profiling
  static std::string profileResult();
  /// Show the profiling result

  static void enableProfile() {
    --profile_enabled;
  }
  /// Enable profiling
  static void disableProfile() {
    ++profile_enabled;
  }
  /// Disable profiling
  static bool profileEnabled() {
    return profile_enabled == 0;
  }
  /// Whether profiling is enabled

private:
  Profile* parent;
  /// Parent of this profile block
  ProfUnit* unit;
  /// ProfUnit this profile belongs to
  ProfTick tick;
  /// Current total tick of this block

public:
  Profile( const char* name ) {
    parent = current_profile;
    current_profile = NULL;
    tick = 0;
    unit = findProfUnit( name );
    unit->enter();
    current_profile = this;
  }
  /// Start a new profile block
  ~Profile() {
    current_profile = NULL;
    if ( unit->leave() ) {
      unit->addTick( tick );
    }
    if ( parent != NULL ) {
      parent->tick += tick;
      parent->unit->addChildTick( unit, tick );
    }
    current_profile = parent;
  }
  /// Finish this profile block

  void incTick() {
    ++tick;
  }
  /// Increment the tick count
};

/// </body>
/// </classdef>

//////////////////////////////////////////////////////////////////////

/// <classdef>
/// <name>ProfTimer</name>
/// <overview>Timer</overview>
/// <desc>
/// The class provides a timer to measure an executing time.
/// </desc>
/// <body>

class ProfTimer {
private:
  timeval start_time;
#ifdef HAVE_SYS_RESOURCE_H
  rusage cpu_usage;
#endif // HAVE_SYS_RESOURCE_H
  long real_sec;
  long real_usec;
  long user_sec;
  long user_usec;
  long system_sec;
  long system_usec;
  long memory_size;

protected:
  std::string timeToStr( long sec, long usec ) const;
  /// Convert a time into a readable-format

public:
  ProfTimer() { initProf(); }
  /// Constructor
  virtual ~ProfTimer() {}
  /// Destructor

  void initProf();
  /// Initialized with the current time
  void startProf();
  /// Start the timer
  void stopProf();
  /// Stop the timer
  long realSec() const { return real_sec; }
  /// Get the seconds passed
  double realSecP() const { return real_sec + (real_usec/100000.0); }
  long realUSec() const { return real_usec; }
  /// Get the micro-seconds passed
  
  long userSec() const { return user_sec; }
  /// Get the seconds used by a user
  double userSecP() const { return user_sec + (user_usec/100000.0); }
  
  long userUSec() const { return user_usec; }
  /// Get the micro-seconds used by a user
  
  long systemSec() const { return system_sec; }
  /// Get the seconds used by a system
  double systemSecP() const { return system_sec + (system_usec/100000.0); }
  long systemUSec() const { return system_usec; }
  /// Get the micro-seconds used by a system
  
  
  long memorySize() const { return memory_size; }
  std::string realTimeString() const { return timeToStr( real_sec, real_usec ); }
  /// Get the time passed in a readable-format
  std::string userTimeString() const { return timeToStr( user_sec, user_usec ); }
  /// Get the time used by a user in a readable-format
  std::string systemTimeString() const { return timeToStr( system_sec, system_usec ); }
  /// Get the time used by a user in a readable-format
};

/// </body>
/// </classdef>

// Macros for profiling
#ifdef AMIS_PROFILE_LEVEL

#define AMIS_PROFILE_START Profile::initializeProfile()
#define AMIS_PROFILE_FINISH Profile::finalizeProfile()

#define AMIS_PROFILE_XX( level, line, name ) \
Profile profile_##level##_##line( name )
#define AMIS_PROFILE_X( level, line, name ) \
AMIS_PROFILE_XX( level, line, name )

#if AMIS_PROFILE_LEVEL >= 1
#define AMIS_PROF1( name ) AMIS_PROFILE_X( 1, __LINE__, name )
#else
#define AMIS_PROF1( name )
#endif
#if AMIS_PROFILE_LEVEL >= 2
#define AMIS_PROF2( name ) AMIS_PROFILE_X( 2, __LINE__, name )
#else
#define AMIS_PROF2( name )
#endif
#if AMIS_PROFILE_LEVEL >= 3
#define AMIS_PROF3( name ) AMIS_PROFILE_X( 3, __LINE__, name )
#else
#define AMIS_PROF3( name )
#endif
#if AMIS_PROFILE_LEVEL >= 4
#define AMIS_PROF4( name ) AMIS_PROFILE_X( 4, __LINE__, name )
#else
#define AMIS_PROF4( name )
#endif
#if AMIS_PROFILE_LEVEL >= 5
#define AMIS_PROF5( name ) AMIS_PROFILE_X( 5, __LINE__, name )
#else
#define AMIS_PROF5( name )
#endif

#else // AMIS_PROFILE_LEVEL

#define AMIS_PROFILE_START
#define AMIS_PROFILE_FINISH
#define AMIS_PROF1( name )
#define AMIS_PROF2( name )
#define AMIS_PROF3( name )
#define AMIS_PROF4( name )
#define AMIS_PROF5( name )

#endif // AMIS_PROFILE_LEVEL

AMIS_NAMESPACE_END

#endif // Profile_h_
// end of Profile.h
