//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: DataFormat.cc,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////
#include <amis/DataFormat.h>

AMIS_NAMESPACE_BEGIN

Initializer< Launcher< const DataFormat&, const Property* >* >* DataFormatLauncher::queue = NULL;

AMIS_NAMESPACE_END

// end of DataFormat.cc
