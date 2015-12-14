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
//
// Modification History:
// 03/25/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Proggie/ReqDistr/Samples/Processor.h"

#include "Debug/Debug.h"
#include "Proggie/ProggieUtil/NetRequest.h"
#include "UtilComm/ConnBuff.h"


bool	
AosMyProcessor::procRequest(const OmnConnBuffPtr &req)
{
	OmnTrace << "To process the request: " << req->getData() << endl;
	return true;
}

