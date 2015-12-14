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
// 07/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_TaskTransCreatorObj_h
#define Aos_SEInterfaces_TaskTransCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosTaskTransCreatorObj : virtual public OmnRCObject
{
public:
	virtual bool registerTaskTrans(const AosTaskTransObjPtr &cacher) = 0;

	virtual AosTaskTransObjPtr createTaskTrans(
					const AosJobObjPtr &job,
					const AosXmlTagPtr &tag, 
					const AosRundataPtr &rdata) = 0;

	virtual AosTaskTransObjPtr serializeFrom(
					const AosBuffPtr &buff, 
					const AosRundataPtr &rdata) = 0;
};
#endif

