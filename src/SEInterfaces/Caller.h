////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2014/01/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_Caller_h
#define Aos_SEInterfaces_Caller_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DatasetId.h"
#include "SEInterfaces/Ptrs.h"


class AosCaller : virtual public OmnRCObject
{
public:
	virtual bool callerReqFinished(
						const AosRundataPtr &rdata,
						const void *userdata) = 0;

	virtual bool callerReqFailed(
						const AosRundataPtr &rdata,
						const void *userdata) = 0;

	// virtual bool callerReqProgressReport(
	// 					const AosRundataPtr &rdata,
	// 					const u32 progress) = 0;
};
#endif

