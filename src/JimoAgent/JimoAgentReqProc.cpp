//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
////////////////////////////////////////////////////////////////////////////
#include "JimoAgent/JimoAgentReqProc.h"
#include "Util/OmnNew.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Ptrs.h"
#include "JimoAgentPackage/JimoAgentPackage.h"
AosJimoAgentReqProc::AosJimoAgentReqProc()
{
}


AosJimoAgentReqProc::~AosJimoAgentReqProc()
{
}


AosNetReqProcPtr
AosJimoAgentReqProc::clone()
{
	return OmnNew AosJimoAgentReqProc();
}

bool
AosJimoAgentReqProc::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr client = buff->getConn();
	AosBuffPtr recv_buff = OmnNew AosBuff(buff AosMemoryCheckerArgs);
	AosPackagePtr package = AosJimoAgentPackage::serializeFromStatic(recv_buff);
	aos_assert_r(package, false);
	package->setClient(client);
	return package->proc();
}
