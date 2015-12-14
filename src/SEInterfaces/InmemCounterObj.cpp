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
// 09/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/InmemCounterObj.h"

#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/InmemCounterCreatorObj.h"
#include "XmlUtil/XmlTag.h"


AosInmemCounterCreatorObjPtr AosInmemCounterObj::smCreator;

AosInmemCounterObj::AosInmemCounterObj(
		const OmnString &name, 
		const AosInmemCounterType::E type, 
		const bool flag)
:
mType(type)
{
	if (flag)
	{
		if (!registerInmemCounter(name, this))
		{
			OmnThrowException("failed_registering");
			return;
		}
	}
}


AosInmemCounterObj::~AosInmemCounterObj()
{
}


AosInmemCounterObjPtr 
AosInmemCounterObj::createInmemCounter(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->createCounter(def, rdata);
}
	

bool
AosInmemCounterObj::registerInmemCounter(const OmnString &name, AosInmemCounterObj *cacher)
{
	aos_assert_r(smCreator, false);
	return smCreator->registerCounter(name, cacher);
}

