//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: OptionManager.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/OptionManager.h>
#include <amis/Option.h>
#include <amis/LogStream.h>
#include <iostream>

AMIS_NAMESPACE_BEGIN

using namespace std;

void OptionManager::initOption( OptionBase* opt ) {
  //if ( opt->getName().empty() ) throw IllegalOptionError( "Empty option name" );
  if( name_hash.find( opt->getName() ) != name_hash.end() ) {
    AMIS_WARNING_MESSAGE( "Warning: you are about to duplicate property: " << opt->getName() << '\n' );
  }
  if( long_hash.find( opt->getLongName() ) != long_hash.end() ) {
    AMIS_WARNING_MESSAGE( "Warning: you are about to duplicate long name: " << opt->getLongName() << '\n' );
  }
  if( short_hash.find( opt->getShortName() ) != short_hash.end() ) {
    AMIS_WARNING_MESSAGE( "Warning: you are about to duplicate short name: " << opt->getShortName() << '\n' );
  }
  option_vector.push_back( opt );
  if ( ! opt->getName().empty() ) name_hash[ opt->getName() ] = opt;
  if ( ! opt->getLongName().empty() ) long_hash[ opt->getLongName() ] = opt;
  if ( ! opt->getShortName().empty() ) short_hash[ opt->getShortName() ] = opt;
}

std::string OptionManager::getShortDescription() const {
  OStringStream os;
  os << "Options:\n";
  for ( std::map< std::string, OptionBase* >::const_iterator it = short_hash.begin();
        it != short_hash.end();
        ++it ) {
    os << it->second->getShortDescription() << '\n';
  }
  return os.str();
}

std::string OptionManager::getFullDescription() const {
  OStringStream os;
  os << "List of available options\n";
  os << "======================================================================\n";
  os << "PROPERTY_NAME <args>, command line longname <args>, shortname <args>\n";
  os << " [argument type description]\n";
  os << " [description]\n";
  os << " [default value description]\n";
  os << "======================================================================\n";
  for ( std::map< std::string, OptionBase* >::const_iterator it = name_hash.begin();
        it != name_hash.end();
        ++it ) {
    os << it->second->getFullDescription() << '\n';
  }
  return os.str();
}

std::string OptionManager::getSpecificationStatus() const {
  OStringStream os;
  for ( std::map< std::string, OptionBase* >::const_iterator it = name_hash.begin();
        it != name_hash.end();
        ++it ) {
    os << it->second->getSpecificationStatus() << '\n';
  }
  return os.str();
}

void OptionManager::setOption( const std::string& name, const std::string& value ) {
  std::map< std::string, OptionBase* >::iterator it = name_hash.find( name );
  if ( it == name_hash.end() ) {
    throw IllegalOptionError( "Unknown option: " + name );
  }
  it->second->setValueFromString( value );
}
void OptionManager::setOptionLongName( const std::string& name, const std::string& value ) {
  std::map< std::string, OptionBase* >::iterator it = long_hash.find( name );
  if ( it == long_hash.end() ) {
    throw IllegalOptionError( "Unknown option: " + name );
  }
  it->second->setValueFromString( value );
}
void OptionManager::setOptionShortName( const std::string& name, const std::string& value ) {
  std::map< std::string, OptionBase* >::iterator it = short_hash.find( name );
  if ( it == short_hash.end() ) {
    throw IllegalOptionError( "Unknown option: " + name );
  }
  it->second->setValueFromString( value );
}

void OptionManager::resetToInit() {
  for ( std::vector< OptionBase* >::iterator it = option_vector.begin();
	it != option_vector.end();
	++it ) {
    (*it)->resetToInit();
  }
}

void OptionManager::resetToUnspecified() {
  for ( std::vector< OptionBase* >::iterator it = option_vector.begin();
	it != option_vector.end();
	++it ) {
    (*it)->resetToUnspecified();
  }
}

AMIS_NAMESPACE_END

// end of OptionManager.cc
