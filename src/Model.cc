//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Model.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#include <amis/Model.h>
#include <amis/ModelLauncher.h>
#include <amis/DataFormat.h>
#include <climits>
#include <cmath>

AMIS_NAMESPACE_BEGIN

Initializer< Launcher< ModelPtr, Property*, ModelIdentifier >* >* ModelLauncher::queue = NULL;

AMIS_NAMESPACE_END

namespace {

  using namespace amis;

  DataFormat amis_data_format( "Amis", "NameTable", "Model", "Event", "Standard ME model" );
  DataFormat tree_data_format( "AmisTree", "NameTable", "Model", "EventTree", "Standard ME model" );
  DataFormat fix_data_format( "AmisFix", "NameTable", "Model", "EventFix", "Standard ME model" );
  DataFormat crf_data_format( "AmisCRF", "NameTable", "Model", "EventCRF", "Standard ME model" );


  ModelLauncherItem< Model, AlphaValue > model_alpha_item( ModelIdentifier("Model", "alpha"), "Standard model" );
  ModelLauncherItem< Model, LambdaValue > model_lambda_item( ModelIdentifier("Model", "lambda"), "Standard model" );

}

// end of Model.cc
