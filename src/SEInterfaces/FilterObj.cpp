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
// 08/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/FilterObj.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/FilterCreatorObj.h"
#include "XmlUtil/XmlTag.h"



AosFilterCreatorObjPtr AosFilterObj::smCreator;

AosFilterObj::AosFilterObj(
		const OmnString &name, 
		const AosFilterType::E type, 
		const bool flag)
:
mType(type)
{
	if (flag)
	{
		AosFilterObjPtr thisptr(this, false);
		if (!AosFilterObj::registerFilter(name, thisptr))
		{
			OmnThrowException("failed_registering");
			return;
		}
	}
}


AosFilterObj::AosFilterObj()
{
}

AosFilterObj::~AosFilterObj()
{
}


AosFilterObjPtr 
AosFilterObj::createFilter(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->createFilter(def, rdata);
}
	

bool
AosFilterObj::registerFilter(const OmnString &name, const AosFilterObjPtr &filter)
{
	aos_assert_r(smCreator, false);
	return smCreator->registerFilter(name, filter);
}

