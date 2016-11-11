//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: AmisModelFormat.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_AmisModelFormat_h_

#define Amis_AmisModelFormat_h_

#include <amis/configure.h>
#include <amis/ModelFormat.h>
#include <amis/Model.h>
#include <amis/NameTable.h>
#include <amis/Tokenizer.h>

//////////////////////////////////////////////////////////////////////

AMIS_NAMESPACE_BEGIN

/**
 * The class imports model data from Amis-style model files.
 * The Amis format enables non-binary features.
 * For details, see README.
 * @see Model, ModelFormat
 */

template < class Name = std::string >
class AmisModelFormat : public ModelFormat {
public:
  /// Constructor
  AmisModelFormat(FeatureWeightType fwt) : ModelFormat(fwt) {}
  /// Destructor
  virtual ~AmisModelFormat() {}

  /// Name of the model format
  std::string modelFormatName() const {
    return "AmisModel";
  }
  
  virtual void modelInitialize( NameTable& name_table_base, ModelBase& model_base ) {
    // do nothing
  }
  
  /// Interface defined by ModelFormat
  void inputModel( std::istream& is, NameTable& name_table, ModelBase& model_base ) {
    Model* model = dynamic_cast< Model* >( &model_base );
    if ( model == NULL ) {
      throw IllegalModelFormatError( "AmisModelFormat can be used only for Model class", 0 );
    }
    inputModel( is, name_table, *model );
  }

  /// Input model data from an input stream
  virtual void inputModel( std::istream& is, NameTable& name_table, Model& model ) {
    Tokenizer t( is );
    while ( ! t.endOfStream() ) {
      Name feature;
      if ( ! t.nextToken( feature, ":" ) ) continue; // empty line
      if ( t.currentDelimiter() == ':' ) {
        throw IllegalModelFormatError( "Feature name cannot contain \":\"", t.lineNumber() );
      }
      Real lambda;
      if ( ! t.nextToken( lambda ) ) {
        OStringStream os;
        os << "Lambda value not found for Feature " << feature;
        throw IllegalModelFormatError( os.str(), t.lineNumber() );
      }
      FeatureID id = model.newFeature( inputLambda( lambda ) );
      try {
        name_table.registerNewFeature( feature, id );
      } catch ( IllegalFeatureError& e ) {
        throw IllegalModelFormatError( e.message(), t.lineNumber() );
      }
      std::string dummy;
      if ( t.nextToken( dummy ) ) {
        OStringStream os;
        os << "Too many tokens for Feature " << feature;
        throw IllegalModelFormatError( os.str(), t.lineNumber() );
      }
    }
  }

  /// Interface defined by ModelFormat
  void outputModel( const NameTable& name_table, const ModelBase& model_base, std::ostream& os, int precision = DEFAULT_PRECISION, bool do_filter = false ) const {
    const Model* model = dynamic_cast< const Model* >( &model_base );
    if ( model == NULL ) {
      throw IllegalModelFormatError( "AmisModelFormat can be used only for Model class", 0 );
    }
    outputModel( name_table, *model, os, precision, do_filter );
  }

  /// Output model data into an output stream
  virtual void outputModel( const NameTable& name_table, const Model& model, std::ostream& os, int precision = DEFAULT_PRECISION, bool do_filter = false) const {
    os.precision( precision );
    os.setf( std::ios::scientific );

    size_t num_active = 0;
    Real threshold = pow(0.1, precision+1);

    for ( size_t i = 0; i < model.numFeatures(); ++i ) {
      Real lambda = model.getLambda(i);
      if ( fabs(lambda) >= threshold) {
	++num_active;
	os << name_table.featureName( i ) << '\t' << outputLambda( lambda ) << '\n';
      }
      else {
	if (!do_filter) {
	  os << name_table.featureName( i ) << '\t' << outputLambda( lambda ) << '\n';
	}
      }
    }
    AMIS_PROF_MESSAGE( num_active << " active features" << std::endl );
  }
};

AMIS_NAMESPACE_END

#endif // AmisModelFormat_h_
// end of AmisModelFormat.h
