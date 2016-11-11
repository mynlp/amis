//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Option.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Option_h_
#define Amis_Option_h_

#include <amis/configure.h>
#include <amis/Initializer.h>
#include <amis/ErrorBase.h>
#include <amis/StringHash.h>
#include <amis/StringStream.h>
#include <amis/OptionManager.h>
#include <string>
#include <map>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

class OptionBase : public Initializer< OptionManager* > {
protected:
  std::string name;
  std::string long_name;
  std::string short_name;
  int num_args;
  std::string description;
  bool allow_overwrite;
  bool specified;

public:
  OptionBase( Initializer< OptionManager* >*& queue,
	      const std::string& n,
	      const std::string& ln,
	      const std::string& sn,
	      int ar,
	      const std::string& desc,
	      bool ao = true ) : Initializer< OptionManager* >( queue )
  {
    name = n;
    long_name = ln;
    short_name = sn;
    num_args = ar;
    description = desc;
    allow_overwrite = ao;
    specified = false;
  }
  virtual ~OptionBase() {}

public:
  void start( OptionManager* manager )
  {
    manager->initOption( this );
  }

public:
  const std::string& getName() const { return name; }
  const std::string& getLongName() const { return long_name; }
  const std::string& getShortName() const { return short_name; }
  int                getNumArguments() const { return num_args; }
  const std::string& getDescription() const { return description; }
  virtual std::string getAdditionalDescription() const {
    return "";
  }

public:
  std::string getNameDescription() const {
    OStringStream os;
    if ( ! getName().empty() ) {
      os << getName() << makeArgDescriptionString( num_args );
    }
    if ( ! getLongName().empty() ) {
      os << ", " << getLongName() << makeArgDescriptionString( num_args );
    }
    if ( ! getShortName().empty() ) {
      os << ", " << getShortName() << makeArgDescriptionString( num_args );
    }
    return os.str();
  }

  std::string getShortDescription() const {
    OStringStream os;
    if ( ! getShortName().empty() ) {
      os << getShortName();
      if ( ! getTypeString().empty() ) {
        os << " <" << getTypeString() << ">";
      }
      os << '\t' << getDescription();
      os << " [default: " << getDefaultValueString() << ']';
    }
    return os.str();
  }

  std::string getFullDescription() const {
    OStringStream os;
    os << getNameDescription() << '\n';
    os << " ARGUMENT TYPE: ";
    if ( getTypeString().empty() ) {
      os << "empty";
    } else {
      os << getTypeString();
    }
    os << '\n';
    os << " DESCRIPTION: " << getDescription() << '\n';
    std::string ad = getAdditionalDescription();
    if ( ad != "" ) {
      os << " " << ad << '\n';
    }
    os << " DEFAULT: " << getDefaultValueString() << std::endl;
    return os.str();
  }

  std::string getSpecificationStatus() const {
    OStringStream os;
    os << getName() << "=" << getCurrentValueString();
    if ( !specified ) {
      os << " ([default])";
    }
    return os.str();
  }

protected:
  static std::string makeArgDescriptionString( int num_args ) {
    OStringStream os;
    if ( num_args < 0 ) {
      os << " <1> <2> ... <n>";
    }
    else if ( num_args == 0 ) {
      return "";
    }
    else {
      os << " ";
      for ( int i = 0; i < num_args; i++ ) {
	os << "<" << (i+1) << ">";
	if ( i != num_args- 1 ) {
	  os << " ";
	}
      }
    }
    return os.str();
  }

public:
  virtual std::string getDefaultValueString() const = 0;
  virtual std::string getCurrentValueString() const = 0;
  virtual std::string getTypeString() const = 0;
  /// get the current value

  virtual void setValueFromString( const std::string& s ) = 0;
  /// set the value from the string

  virtual void resetToInit() = 0;
  /// reset the status of the specification to the inital state

  virtual void resetToUnspecified() {
    specified = false;
  }
};

//////////////////////////////////////////////////////////////////////

template<class T>
class Option : public OptionBase {
protected:
  T    default_value;
  T    value;

public:
  Option( Initializer< OptionManager* >*& queue,
	  const std::string& pn,
	  const std::string& ln,
	  const std::string& sn,
	  const T& def_val = T(),
	  const std::string& desc = "",
	  bool ao = true )
    : OptionBase( queue, pn, ln, sn, 1, desc, ao ) {
    default_value  = def_val;
    value          = def_val;
  }

  std::string getDefaultValueString() const {
    OStringStream os;
    os << default_value;
    return os.str();
  }
  std::string getCurrentValueString() const {
    OStringStream os;
    os << value;
    return os.str();
  }
  std::string getTypeString() const {
    //return TypeString<T>::getString();
    return "unknown";
  }
  const T& getValue() const {
    return value;
  }

  void setValueFromString( const std::string& s ) throw( IllegalOptionError ) {
    IStringStream is( s );
    T val;
    is >> val;
    if ( !is ) {
      throw IllegalOptionError( "The value of the option is illegal: " + getNameDescription() );
    }
    if ( !specified || allow_overwrite ) {
      value = val;
      specified = true;
    }
  }
  void resetToInit() {
    value = default_value;
    specified = false;
  }
};

//////////////////////////////////////////////////////////////////////

template <> inline std::string Option< int >::getTypeString() const {
  return "int";
}
template <> inline std::string Option< double >::getTypeString() const {
  return "double";
}
template <> inline std::string Option< std::string >::getTypeString() const {
  return "string";
}

//////////////////////////////////////////////////////////////////////

template <> inline std::string Option< bool >::getDefaultValueString() const {
  if ( default_value == true ) {
    return "true";
  } else {
    return "false";
  }
}
template <> inline std::string Option< bool >::getCurrentValueString() const {
  if ( value == true ) {
    return "true";
  } else {
    return "false";
  }
}
template <> inline std::string Option< bool >::getTypeString() const {
  return "bool";
}

template <> inline void Option< bool >::setValueFromString( const std::string& s ) throw( IllegalOptionError ) {
  bool val;
  if ( s == "true" || s == "1" || s == "yes" || s == "on" || s == "TRUE" || s == "YES" || s == "ON" ) {
    val = true;
  }
  else if ( s == "false" || s == "0" || s == "no" || s == "off" || s == "FALSE" || s == "NO" || s == "OFF" ) {
    val = false;
  }
  else {
    throw IllegalOptionError( "The value of the option must be one of \"true\", \"false\", \"1\", \"0\", \"yes\", \"no\", \"on\", and \"off\": " + getNameDescription() );
  }
  if ( !specified || allow_overwrite ) {
    value = val;
    specified = true;
  } 
}

template <> inline void Option< bool >::resetToInit() {
  value = default_value;
  specified = false;
}

//////////////////////////////////////////////////////////////////////

//  For accumulative options specified as "-opt a -opt b -opt c" to indicate [a, b, c] for a vector variable.
//  If a option of this type once specified, the value stored as the default value is discarded and the new accumulation begins.

template < class T >
class OptionVector : public OptionBase {
protected:
  std::vector< T >    default_value;
  std::vector< T >    value;

public:
  OptionVector( Initializer< OptionManager* >*& queue,
		const std::string& pn,
		const std::string& ln,
		const std::string& sn,
		const std::vector< T >& def_val = std::vector< T >(),
		const std::string& desc = "",
		bool ao = true )
    : OptionBase( queue, pn, ln, sn, -1, desc, true ) { // minus number of argumetns means any number
    default_value  = def_val;
    value = def_val;
  }

  std::string getDefaultValueString() const {
    OStringStream os;
    os << "[";
    for ( size_t i = 0; i < default_value.size(); ++i ) {
      os << default_value[i];
      if ( i != default_value.size()-1 ) {
	os << " ";
      }
    }
    os << "]";
    return os.str();
  }
  std::string getCurrentValueString() const {
    OStringStream os;
    os << "[";
    for ( size_t i = 0; i < value.size(); ++i ) {
      os << value[i];
      if ( i != value.size()-1 ) {
	os << " ";
      }
    }
    os << "]";
    return os.str();
  }
  std::string getTypeString() const {
    return "vector";
  }
  const std::vector< T >& getValue() const {
    return value;
  }

  void setValueFromString( const std::string& s ) throw( IllegalOptionError ) {
    if ( !specified ) {
      value.resize( 0 );
    }
    IStringStream is( s );
    T val;
    is >> val;
    if ( !is ) {
      throw IllegalOptionError( "The value of the option is illegal: " + getNameDescription() );
    }
    if ( !specified || allow_overwrite ) {
      value.push_back( val );
      specified = true;
    }
  }
  void resetToInit() {
    value = default_value;
    specified = false;
  }
};

//////////////////////////////////////////////////////////////////////

// For boolean options such as "-h", "-debug" and "-trace", which set the value to the revserse of the default value when specified. (i.e., false -> true and true->false)

class OptionToggle : public OptionBase {
 protected:
  bool    default_value;
  bool    value;

 public:
  OptionToggle( Initializer< OptionManager* >*& queue,
		const std::string& pn,
		const std::string& ln,
		const std::string& sn,
		const bool def_val = false,
		const std::string& desc = "",
		bool ao = true )
    : OptionBase( queue, pn, ln, sn, 0, desc, ao ) {
    default_value  = def_val;
    value = def_val;
  }

  std::string getDefaultValueString() const {
    if( default_value == true ) {
      return "true";
    } else {
      return "false";
    }
  }
  std::string getCurrentValueString() const {
    if( value == true ) {
      return "true";
    } else {
      return "false";
    }
  }
  std::string getTypeString() const {
    return "";
  }
  bool getValue() const {
    return value;
  }

  void setValueFromString( const std::string& s ) throw( IllegalOptionError ) {
    if ( !specified || allow_overwrite ) {
      if ( s == "" ) {
	value = !default_value;
	specified = true;
      }
      else {
	bool val;
	if ( s == "true" || s == "1" || s == "yes" || s == "on" || s == "TRUE" || s == "YES" || s == "ON" ) {
	  val = true;
	}
	else if ( s == "false" || s == "0" || s == "no" || s == "off" || s == "FALSE" || s == "NO" || s == "OFF" ) {
	  val = false;
	}
	else {
          throw IllegalOptionError( "The value of the option must be one of \"true\", \"false\", \"1\", \"0\", \"yes\", \"no\", \"on\", and \"off\": " + getNameDescription() );
	}
	value = val;
	specified = true;
      }
    }
  }
  void resetToInit() {
    value = default_value;
    specified = false;
  }
};

// For options such as "-map-type ["single"|"each"|"cnt_inv"], where identifiers "single", "each" and "cnt_inv" are tied to integer constants in the program such as CONST_SINGLE, CONST_EACH, and CONST_CNT_INV respectively for instance.
//  The mapping of name to integer is passed as constructor arguments.

template < class INT >
class OptionNamedInt : public OptionBase {
protected:
  INT                                 default_value;
  INT                                 value;
  StringHash<INT>                     name_to_int;
  std::map<INT, std::string>          int_to_name;

public:
  OptionNamedInt( Initializer< OptionManager* >*& queue,
		  const std::string& pn,
		  const std::string& ln,
		  const std::string& sn,
		  const StringHash< INT >& names,
		  INT def_val = 0,
		  const std::string& desc = "",
		  bool ao = true )
    : OptionBase( queue, pn, ln, sn, 1, desc, ao )
  {
    default_value = def_val;
    value = def_val;
    for ( typename StringHash< INT >::const_iterator itr = names.begin(); itr != names.end(); ++itr ) {
      std::string name = itr->first;
      INT         cons = itr->second;
      name_to_int[ name ] = cons;
      int_to_name[ cons ] = name;
    }
  }
  virtual ~OptionNamedInt() {}

  std::string getDefaultValueString() const {
    return int_to_name.find( default_value )->second;
  }

  std::string getCurrentValueString() const {
    return int_to_name.find( value )->second;
  }

  std::string getAdditionalDescription() const {
    OStringStream os;
    os << "SUPPORTED VALUE:";
    for ( typename StringHash<INT>::const_iterator itr = name_to_int.begin(); itr != name_to_int.end(); ++itr ) {
      os << " " << itr->first;
    }
    return os.str();
  }
  std::string getTypeString() const {
    return "NamedInt (string)";
  }
  INT getValue() const {
    return value;
  }

  void setValueFromString( const std::string& s ) throw( IllegalOptionError ) {
    INT val;
    if ( name_to_int.find( s ) != name_to_int.end() ) {
      val = name_to_int[ s ];
    } else {
      OStringStream oss;
      oss << s << " is not supported as the value for " << getNameDescription();
      throw IllegalOptionError( oss.str() );
    }
    if ( !specified || allow_overwrite ) {
      value = name_to_int[ s ];
      specified = true;
    }
  }

  void resetToInit() {
    value = default_value;
    specified = false;
  }
};

AMIS_NAMESPACE_END

#endif // Amis_Option_h_
// end of Option.h
