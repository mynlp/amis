//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: FeatureParser.h,v 1.2 2008-07-29 06:05:57 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#ifndef Amis_FeatureParser_h_

#define Amis_FeatureParser_h_

#include <amis/configure.h>
#include <amis/Feature.h>
#include <amis/NameTable.h>
#include <amis/Tokenizer.h>
#include <string>
#include <vector>

#include <amis/Profile.h>

AMIS_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////

template < class Feature >
class FeatureParser {
public:
  typedef typename Feature::FeatureFreq FeatureFreq;

private:
  mutable std::string feature;
  mutable FeatureFreq freq;

public:
  bool inputFeature( Tokenizer& t, std::string& name, FeatureFreq& freq ) const
  {
    AMIS_PROF2( "FeatureParser::inputFeature" );
    if ( t.nextToken( name, ":" ) ) {
      if ( t.currentDelimiter() == ':' ) {
        AMIS_PROF2( "FeatureParser::inputFeature::extractFreq" );
        if ( ! t.nextToken( freq ) ) {
          throw IllegalFeatureError( "Cannot read feature frequency" );
        }
//         if ( freq <= static_cast< FeatureFreq >( 0 ) ) {
//           throw IllegalFeatureError( "Feature frequency must be positive" );
//         }
      } else {
        // feature frequency is not specified
        freq = static_cast< FeatureFreq >( 1 );
      }
      return true;
    }
    else {
      return false;
    }
  }
  /// Input a feature

  void inputFeatureList( NameTable& table, Tokenizer& t, std::vector< Feature >& fl ) const {
    AMIS_PROF2( "FeatureParser::inputFeatureList" );
    while ( inputFeature( t, feature, freq ) ) {
      AMIS_PROF2( "FeatureParser::inputFeatureList::push_back" );
      fl.push_back( Feature( table.featureID( feature ), freq ) );
    }
  }
  /// Input a feature list
};

AMIS_NAMESPACE_END

#endif // Amis_FeatureParser_h_
// end of FeatureParser.h
