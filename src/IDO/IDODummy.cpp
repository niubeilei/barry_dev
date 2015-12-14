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
// 2015/03/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IDO/IDODummy.h"

#include "JimoCall/JimoCall.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"


AosIDODummy::AosIDODummy()
:
AosIDO(1)
{
}


AosIDODummy::~AosIDODummy()
{
}


AosJimoPtr
AosIDODummy::cloneJimo() const
{
	return OmnNew AosIDODummy(*this);
}


bool 
AosIDODummy::serializeFrom(AosRundata *rdata, AosBuff *buff)
{
	return true;
}


bool 
AosIDODummy::serializeTo(AosRundata *rdata, AosBuff *buff) const
{
	return true;
}


bool 
AosIDODummy::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}

