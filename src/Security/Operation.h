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
// 12/23/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Security_Operation_h
#define Omn_Security_Operation_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

class AosSecReq;

class AosOperation
{
	enum
	{
		eMaxConds = 50
	};

public:
	static OmnString determineOperation(AosSecReq &sec_req, const AosRundataPtr &rdata);
};
#endif

