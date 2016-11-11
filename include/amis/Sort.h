//////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Yusuke Miyao
//  You may distribute under the terms of the Artistic License.
//
//  Amis - A maximum entropy estimator
//
//  Author: Yusuke Miyao (yusuke@is.s.u-tokyo.ac.jp)
//  $Id: Sort.h,v 1.1.1.1 2008-07-28 19:25:53 kyoshida Exp $
//
//////////////////////////////////////////////////////////////////////

#ifndef Amis_Sort_h_
#define Amis_Sort_h_

AMIS_NAMESPACE_BEGIN

template<class Index, class Value>
class IndexValuePairCmpByValue {
	public:
	int operator()(const std::pair<Index,Value>& p1, const std::pair<Index,Value>& p2) const {
		return p1.second < p2.second;
	}
};

template<class Index, class Value>
class IndexValuePairCmpByIndex {
	public:
	int operator()(const std::pair<Index,Value>& p1, const std::pair<Index,Value>& p2) const {
		return p1.first < p2.first;
	}
};

AMIS_NAMESPACE_END

#endif
