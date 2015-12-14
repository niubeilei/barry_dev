////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 03/06/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_QueryUtil_QueryCondInfo_h
#define AOS_QueryUtil_QueryCondInfo_h

#include "Util/Opr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"


struct AosQueryCondInfo : virtual public OmnRCObject
{
	OmnDefineRCObject;

	OmnString 		mContainer;
	OmnString		mAname;
	AosOpr			mOpr;
	OmnString		mValue;
	bool			mReverse;
	bool			mOrder;

	AosQueryCondInfo(
			const OmnString container,
			const OmnString &aname,
			const AosOpr opr,
			const OmnString &value,
			const bool reverse, 
			const bool order)
	:
	mContainer(container),
	mAname(aname),
	mOpr(opr),
	mValue(value),
	mReverse(reverse),
	mOrder(order)
	{
	}

	AosQueryCondInfo(const AosXmlTagPtr &term)
	:
	mOpr(eAosOpr_Invalid),
	mReverse(false),
	mOrder(false)
	{
		// 'term' should be in the following format
		// 		<term AOSTAG_QUERY_CONTAINER_OBJID="xxx"
		// 			AOSTAG_QUERY_ATTRNAME="xxx"
		// 			AOSTAG_QUERY_OPR="xxx"
		// 			AOSTAG_QUERY_REVERSE="true/false"
		// 			AOSTAG_QUERY_ORDER="true/false">
		// 			value
		// 		</term>
		aos_assert(term);

		mContainer = term->getAttrStr(AOSTAG_QUERY_CONTAINER_OBJID);
		mAname = term->getAttrStr(AOSTAG_QUERY_ATTRNAME);
		mOpr = AosOpr_toEnum(term->getAttrStr(AOSTAG_QUERY_OPR));
		mValue = term->getNodeText();
		mReverse = term->getAttrBool(AOSTAG_QUERY_REVERSE, false);
		mOrder = term->getAttrBool(AOSTAG_QUERY_ORDER, false);
	}
};
#endif
