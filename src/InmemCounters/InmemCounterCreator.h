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
#ifndef Aos_DataxCacher_InmemCounterCreator_h
#define Aos_DataxCacher_InmemCounterCreator_h

#include "SEInterfaces/InmemCounterCreatorObj.h"


class AosInmemCounterCreator : public AosInmemCounterCreatorObj
{
	OmnDefineRCObject;

public:
	virtual bool registerCounter(const OmnString &name, AosInmemCounterObj *cacher);
	
	virtual AosInmemCounterObjPtr createCounter(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

private:
	bool init(const AosRundataPtr &rdata);
};
#endif

