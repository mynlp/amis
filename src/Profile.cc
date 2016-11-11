//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Profile.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/configure.h>
#include <amis/Profile.h>
#include <amis/StringStream.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif // HAVE_UNISTD_H

#include <algorithm>
#include <iostream>
#include <vector>

AMIS_NAMESPACE_BEGIN

using namespace std;

//////////////////////////////////////////////////////////////////////

const long ProfUnit::PROFILE_INTERVAL = 10000;

void Profile::signalHandler( int ) {
#ifdef AMIS_PROFILE_LEVEL
  if ( current_profile != NULL ) {
    current_profile->incTick();
  }
  ::sigaction( SIGACTION_SIGNUM, &action, &old_action );
  setitimer( ITIMER_WHICH, &timer, &old_timer );
#endif // AMIS_PROFILE_LEVEL
}

//////////////////////////////////////////////////////////////////////

int Profile::profile_enabled = 0;
amis_hash_map< size_t, ProfUnit* > Profile::unit_table;
Profile* Profile::current_profile = NULL;

#ifdef AMIS_PROFILE_LEVEL
struct sigaction Profile::action;
struct itimerval Profile::timer;
struct sigaction Profile::old_action;
struct itimerval Profile::old_timer;
#endif // AMIS_PROFILE_LEVEL

void Profile::initializeProfile( void ) {
#ifdef AMIS_PROFILE_LEVEL
  action.sa_handler = signalHandler;
  sigemptyset( &action.sa_mask );
  action.sa_flags = 0;
  action.sa_flags |= SA_RESTART | SA_RESETHAND;
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = ProfUnit::PROFILE_INTERVAL;
  timer.it_interval.tv_sec = 0;
  //timer.it_interval.tv_usec = ProfUnit::PROFILE_INTERVAL;
  timer.it_interval.tv_usec = 0;
  ::sigaction( SIGACTION_SIGNUM, &action, &old_action );
  setitimer( ITIMER_WHICH, &timer, &old_timer );
#endif // AMIS_PROFILE_LEVEL
}
void Profile::finalizeProfile( void ) {
#ifdef AMIS_PROFILE_LEVEL
  setitimer( ITIMER_WHICH, &old_timer, NULL );
  ::sigaction( SIGACTION_SIGNUM, &old_action, NULL );
  for ( hash_map< size_t, ProfUnit* >::iterator it = unit_table.begin();
        it != unit_table.end();
        ++it ) {
    delete it->second;
  }
#endif // AMIS_PROFILE_LEVEL
}

string Profile::profileResult( void ) {
  OStringStream os;
  if ( ! unit_table.empty() ) {
    os << "Profiling result:\n";
    os << "Pass Count    Total sec.    Child sec.    Block\n";
    std::vector< const ProfUnit* > unit_vector( unit_table.size() );
    amis_hash_map< size_t, ProfUnit* >::const_iterator hash_it = unit_table.begin();
    for ( vector< const ProfUnit* >::iterator it = unit_vector.begin();
          it != unit_vector.end();
          ++it, ++hash_it ) {
      *it = hash_it->second;
    }
    std::stable_sort( unit_vector.begin(), unit_vector.end(), CompTick() );
    for ( vector< const ProfUnit* >::const_iterator unit = unit_vector.begin();
          unit != unit_vector.end();
          ++unit ) {
      os << setw( 10 ) << (*unit)->passCount() << "    "
         << (*unit)->totalTickString() << "    "
         << (*unit)->childTickString() << "    "
         << (*unit)->getName() << '\n';
    }
    os << "Call graph:\n";
    for ( vector< const ProfUnit* >::const_iterator it = unit_vector.begin();
          it != unit_vector.end();
          ++it ) {
      if ( ! (*it)->callGraph().empty() ) {
        os << (*it)->getName() << '\n';
        os << "  Pass Count    Total sec.    Block\n";
        os << "* " << setw( 10 ) << (*it)->passCount() << "    "
           << (*it)->totalTickString() << "    "
           << (*it)->getName() << '\n';
        for ( amis_hash_map< size_t, ProfTick >::const_iterator child = (*it)->callGraph().begin();
              child != (*it)->callGraph().end();
              ++child ) {
          const ProfUnit* child_prof = reinterpret_cast< const ProfUnit* >( child->first );
          os << "  " << setw( 10 ) << child_prof->passCount() << "    "
             << child_prof->tickString( child->second ) << "    "
             << child_prof->getName() << '\n';
        }
      }
    }
  }
  return os.str();
}

//////////////////////////////////////////////////////////////////////

void ProfTimer::initProf() {
#ifdef HAVE_GETTIMEOFDAY
  (void)gettimeofday( &start_time, NULL );
#endif // HAVE_GETTIMEOFDAY
#ifdef HAVE_SYS_RESOURCE_H
#ifdef HAVE_GETRUSAGE
  (void)getrusage( RUSAGE_SELF, &cpu_usage );
#endif // HAVE_GETRUSAGE
#endif // HAVE_SYS_RESOURCE_H
  real_sec = 0;
  real_usec = 0;
  user_sec = 0;
  user_usec = 0;
  system_sec = 0;
  system_usec = 0;
  memory_size = 0;
}

void ProfTimer::startProf( void ) {
#ifdef HAVE_GETTIMEOFDAY
  (void)gettimeofday( &start_time, NULL );
#endif // HAVE_GETTIMEOFDAY
#ifdef HAVE_SYS_RESOURCE_H
#ifdef HAVE_GETRUSAGE
  (void)getrusage( RUSAGE_SELF, &cpu_usage );
#endif // HAVE_GETRUSAGE
#endif // HAVE_SYS_RESOURCE_H
  real_sec = 0;
  real_usec = 0;
  user_sec = 0;
  user_usec = 0;
  system_sec = 0;
  system_usec = 0;
  memory_size = 0;
}

void ProfTimer::stopProf( void ) {
#ifdef HAVE_GETTIMEOFDAY
  timeval current_time;
  (void)gettimeofday( &current_time, NULL );
  real_sec = current_time.tv_sec - start_time.tv_sec;
  real_usec = current_time.tv_usec - start_time.tv_usec;
#endif // HAVE_GETTIMEOFDAY
#ifdef HAVE_SYS_RESOURCE_H
#ifdef HAVE_GETRUSAGE
  rusage current_usage;
  (void)getrusage( RUSAGE_SELF, &current_usage );
  user_sec  = current_usage.ru_utime.tv_sec - cpu_usage.ru_utime.tv_sec;
  user_usec = current_usage.ru_utime.tv_usec - cpu_usage.ru_utime.tv_usec;
  system_sec = current_usage.ru_stime.tv_sec - cpu_usage.ru_stime.tv_sec;
  system_usec  = current_usage.ru_stime.tv_usec - cpu_usage.ru_stime.tv_usec;
  memory_size = current_usage.ru_maxrss;
#endif // HAVE_GETRUSAGE
#endif // HAVE_SYS_RESOURCE_H
}

string ProfTimer::timeToStr( long sec, long usec ) const {
  OStringStream os;
  if ( usec < 0 ) {
    --sec;
    usec += 1000000;
  }
  if ( sec > 60 ) {
    long min = sec / 60;
    sec -= min * 60;
    if ( min > 60 ) {
      long hour = min / 60;
      min -= hour * 60;
      os << hour << " hour ";
    }
    os << min << " min ";
  }
  os << sec << " sec " << usec;
  return os.str();
}

AMIS_NAMESPACE_END

// end of Profile.cc
