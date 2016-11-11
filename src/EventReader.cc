//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: EventReader.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/EventReader.h>
#include <amis/EventReaderInst.h>
#include <amis/EventReaderLauncher.h>
#include <amis/EventReaderLauncherItem.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

Initializer< Launcher< EventReaderPtr, Property*, EventReaderIdentifier >* >* EventReaderLauncher::queue = NULL;

AMIS_NAMESPACE_END

namespace {

  using namespace amis;

  EventReaderLauncherItem< AmisEventReader< BinaryFeature, AlphaValue, false > >
  amis_amis_binary_alpha_false( EventReaderIdentifier("Amis", "binary", "alpha", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisEventReader< IntegerFeature, AlphaValue, false > >
  amis_amis_integer_alpha_false( EventReaderIdentifier("Amis", "integer", "alpha", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisEventReader< RealFeature, AlphaValue, false > >
  amis_amis_real_alpha_false( EventReaderIdentifier("Amis", "real", "alpha", false), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisEventReader< BinaryFeature, LambdaValue, false > >
  amis_amis_binary_lambda_false( EventReaderIdentifier("Amis", "binary", "lambda", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisEventReader< IntegerFeature, LambdaValue, false > >
  amis_amis_integer_lambda_false( EventReaderIdentifier("Amis", "integer", "lambda", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisEventReader< RealFeature, LambdaValue, false > >
  amis_amis_real_lambda_false( EventReaderIdentifier("Amis", "real", "lambda", false), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisEventReader< BinaryFeature, AlphaValue, true > >
  amis_amis_binary_alpha_true( EventReaderIdentifier("Amis", "binary", "alpha", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisEventReader< IntegerFeature, AlphaValue, true > >
  amis_amis_integer_alpha_true( EventReaderIdentifier("Amis", "integer", "alpha", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisEventReader< RealFeature, AlphaValue, true > >
  amis_amis_real_alpha_true( EventReaderIdentifier("Amis", "real", "alpha", true), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisEventReader< BinaryFeature, LambdaValue, true > >
  amis_amis_binary_lambda_true( EventReaderIdentifier("Amis", "binary", "lambda", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisEventReader< IntegerFeature, LambdaValue, true > >
  amis_amis_integer_lambda_true( EventReaderIdentifier("Amis", "integer", "lambda", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisEventReader< RealFeature, LambdaValue, true > >
  amis_amis_real_lambda_true( EventReaderIdentifier("Amis", "real", "lambda", true), "Amis-style standard event format (binary feature)" );


  EventReaderLauncherItem< AmisTreeEventReader< BinaryFeature, AlphaValue, false > >
  amis_tree_binary_alpha_false( EventReaderIdentifier("AmisTree", "binary", "alpha", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisTreeEventReader< IntegerFeature, AlphaValue, false > >
  amis_tree_integer_alpha_false( EventReaderIdentifier("AmisTree", "integer", "alpha", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisTreeEventReader< RealFeature, AlphaValue, false > >
  amis_tree_real_alpha_false( EventReaderIdentifier("AmisTree", "real", "alpha", false), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisTreeEventReader< BinaryFeature, LambdaValue, false > >
  amis_tree_binary_lambda_false( EventReaderIdentifier("AmisTree", "binary", "lambda", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisTreeEventReader< IntegerFeature, LambdaValue, false > >
  amis_tree_integer_lambda_false( EventReaderIdentifier("AmisTree", "integer", "lambda", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisTreeEventReader< RealFeature, LambdaValue, false > >
  amis_tree_real_lambda_false( EventReaderIdentifier("AmisTree", "real", "lambda", false), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisTreeEventReader< BinaryFeature, AlphaValue, true > >
  amis_tree_binary_alpha_true( EventReaderIdentifier("AmisTree", "binary", "alpha", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisTreeEventReader< IntegerFeature, AlphaValue, true > >
  amis_tree_integer_alpha_true( EventReaderIdentifier("AmisTree", "integer", "alpha", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisTreeEventReader< RealFeature, AlphaValue, true > >
  amis_tree_real_alpha_true( EventReaderIdentifier("AmisTree", "real", "alpha", true), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisTreeEventReader< BinaryFeature, LambdaValue, true > >
  amis_tree_binary_lambda_true( EventReaderIdentifier("AmisTree", "binary", "lambda", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisTreeEventReader< IntegerFeature, LambdaValue, true > >
  amis_tree_integer_lambda_true( EventReaderIdentifier("AmisTree", "integer", "lambda", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisTreeEventReader< RealFeature, LambdaValue, true > >
  amis_tree_real_lambda_true( EventReaderIdentifier("AmisTree", "real", "lambda", true), "Amis-style standard event format (binary feature)" );


  EventReaderLauncherItem< AmisFixEventReader< BinaryFeature, AlphaValue, false > >
  amis_fix_binary_alpha_false( EventReaderIdentifier("AmisFix", "binary", "alpha", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisFixEventReader< IntegerFeature, AlphaValue, false > >
  amis_fix_integer_alpha_false( EventReaderIdentifier("AmisFix", "integer", "alpha", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisFixEventReader< RealFeature, AlphaValue, false > >
  amis_fix_real_alpha_false( EventReaderIdentifier("AmisFix", "real", "alpha", false), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisFixEventReader< BinaryFeature, LambdaValue, false > >
  amis_fix_binary_lambda_false( EventReaderIdentifier("AmisFix", "binary", "lambda", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisFixEventReader< IntegerFeature, LambdaValue, false > >
  amis_fix_integer_lambda_false( EventReaderIdentifier("AmisFix", "integer", "lambda", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisFixEventReader< RealFeature, LambdaValue, false > >
  amis_fix_real_lambda_false( EventReaderIdentifier("AmisFix", "real", "lambda", false), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisFixEventReader< BinaryFeature, AlphaValue, true > >
  amis_fix_binary_alpha_true( EventReaderIdentifier("AmisFix", "binary", "alpha", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisFixEventReader< IntegerFeature, AlphaValue, true > >
  amis_fix_integer_alpha_true( EventReaderIdentifier("AmisFix", "integer", "alpha", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisFixEventReader< RealFeature, AlphaValue, true > >
  amis_fix_real_alpha_true( EventReaderIdentifier("AmisFix", "real", "alpha", true), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisFixEventReader< BinaryFeature, LambdaValue, true > >
  amis_fix_binary_lambda_true( EventReaderIdentifier("AmisFix", "binary", "lambda", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisFixEventReader< IntegerFeature, LambdaValue, true > >
  amis_fix_integer_lambda_true( EventReaderIdentifier("AmisFix", "integer", "lambda", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisFixEventReader< RealFeature, LambdaValue, true > >
  amis_fix_real_lambda_true( EventReaderIdentifier("AmisFix", "real", "lambda", true), "Amis-style standard event format (binary feature)" );


  EventReaderLauncherItem< AmisCRFEventReader< BinaryFeature, AlphaValue, false > >
  amis_crf_binary_alpha_false( EventReaderIdentifier("AmisCRF", "binary", "alpha", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisCRFEventReader< IntegerFeature, AlphaValue, false > >
  amis_crf_integer_alpha_false( EventReaderIdentifier("AmisCRF", "integer", "alpha", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisCRFEventReader< RealFeature, AlphaValue, false > >
  amis_crf_real_alpha_false( EventReaderIdentifier("AmisCRF", "real", "alpha", false), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisCRFEventReader< BinaryFeature, LambdaValue, false > >
  amis_crf_binary_lambda_false( EventReaderIdentifier("AmisCRF", "binary", "lambda", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisCRFEventReader< IntegerFeature, LambdaValue, false > >
  amis_crf_integer_lambda_false( EventReaderIdentifier("AmisCRF", "integer", "lambda", false), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisCRFEventReader< RealFeature, LambdaValue, false > >
  amis_crf_real_lambda_false( EventReaderIdentifier("AmisCRF", "real", "lambda", false), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisCRFEventReader< BinaryFeature, AlphaValue, true > >
  amis_crf_binary_alpha_true( EventReaderIdentifier("AmisCRF", "binary", "alpha", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisCRFEventReader< IntegerFeature, AlphaValue, true > >
  amis_crf_integer_alpha_true( EventReaderIdentifier("AmisCRF", "integer", "alpha", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisCRFEventReader< RealFeature, AlphaValue, true > >
  amis_crf_real_alpha_true( EventReaderIdentifier("AmisCRF", "real", "alpha", true), "Amis-style standard event format (binary feature)" );

  EventReaderLauncherItem< AmisCRFEventReader< BinaryFeature, LambdaValue, true > >
  amis_crf_binary_lambda_true( EventReaderIdentifier("AmisCRF", "binary", "lambda", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisCRFEventReader< IntegerFeature, LambdaValue, true > >
  amis_crf_integer_lambda_true( EventReaderIdentifier("AmisCRF", "integer", "lambda", true), "Amis-style standard event format (binary feature)" );
  EventReaderLauncherItem< AmisCRFEventReader< RealFeature, LambdaValue, true > >
  amis_crf_real_lambda_true( EventReaderIdentifier("AmisCRF", "real", "lambda", true), "Amis-style standard event format (binary feature)" );

}

// end of EventReader.cc
