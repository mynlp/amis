//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: AmisModelFormat.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/AmisModelFormat.h>
#include <amis/AmisModelFormatItem.h>

AMIS_NAMESPACE_BEGIN

Initializer< Launcher< ModelFormatPtr, Property* >* >* ModelFormatLauncher::queue = NULL;

AMIS_NAMESPACE_END

namespace {

  using namespace amis;

  AmisModelFormatItem< AmisModelFormat<> >
  amis_model_format( "Model", "Amis-style model format" );
  
  AmisModelFormatItem< AmisModelFormat<> >
  amis_modeld_format( "ModelD", "Amis-style model format" );

}

// end of AmisModelFormat.cc
