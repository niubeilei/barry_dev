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
#ifndef Aos_EventMgr_EventCreator_h
#define Aos_EventMgr_EventCreator_h

#include "SEInterfaces/EventCreatorObj.h"


class AosEventCreator : public AosEventCreatorObj
{
	OmnDefineRCObject;

public:
	virtual bool registerEvent(const OmnString &name, AosEventObj *cacher);
	
	virtual AosEventObjPtr createEvent(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

	virtual AosEventObjPtr serializeFrom(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata);
private:
	bool init(const AosRundataPtr &rdata);
};
#endif

