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
// 2013/09/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_TransCallerObj_h
#define Aos_SEInterfaces_TransCallerObj_h

#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosTransCallerObj : public OmnRCObject
{
public:
	virtual bool transFinished(
						const AosRundataPtr &rdata, 
						const u64 trans_id,
						const u64 caller_id,
						const bool status, 
						const OmnString &errmsg,
						const AosBuffPtr &data) = 0;
};
#endif
