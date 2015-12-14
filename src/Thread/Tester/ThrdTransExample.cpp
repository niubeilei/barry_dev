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
// 4/16/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "Thread/Tester/ThrdTransExample.h"

#include "UtilComm/ConnBuff.h"



AosThrdTransExample::AosThrdTransExample(const u32 transId)
:
OmnThrdTrans(transId)
{
}


AosThrdTransExample::~AosThrdTransExample()
{
}


bool    
AosThrdTransExample::isSuccess() const
{
	return true;
}


bool    
AosThrdTransExample::msgRcved(const OmnConnBuffPtr &buff)
{
	return true;
}

