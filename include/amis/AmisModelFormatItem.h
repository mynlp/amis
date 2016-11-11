//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: AmisModelFormatItem.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_AmisModelFormatItem_h_

#define Amis_AmisModelFormatItem_h_

#include <amis/ModelFormatLauncher.h>
#include <amis/Property.h>

AMIS_NAMESPACE_BEGIN

/**
 * This class defines ModelFormatLauncherItem specialized
 * for AmisModelFormat and its derivatives.
 */

template < class ModelFormat >
class AmisModelFormatItem : public ModelFormatLauncherItem {
public:
  /// Constructor
  AmisModelFormatItem( const std::string& name, const std::string& desc = "" )
    : ModelFormatLauncherItem( name, desc ) {}
  /// Make a new model format
  ModelFormatPtr launch( Property* property ) {
    return ModelFormatPtr( new ModelFormat(property->getFeatureWeightType()));
  }
};

AMIS_NAMESPACE_END

#endif // Amis_AmisModelFormatItem_h_
// end of AmisModelFormatItem.h
