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
// 09/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_EventCreatorObj_h
#define Aos_SEInterfaces_EventCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosEventCreatorObj : virtual public OmnRCObject
{
public:
	virtual bool registerEvent(const OmnString &name, AosEventObj *cacher) = 0;
	virtual AosEventObjPtr createEvent(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;

	virtual AosEventObjPtr serializeFrom(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata) = 0;
};
#endif

