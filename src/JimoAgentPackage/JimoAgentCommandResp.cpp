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
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "JimoAgentPackage/JimoAgentCommandResp.h"

AosJimoAgentCommandResp::AosJimoAgentCommandResp()
:
AosJimoAgentPackage(AosAgentPackageType::eCommandResp, AOSTAG_AGENTPACKAGE_COMMANDRESP)
{
}

AosJimoAgentCommandResp::AosJimoAgentCommandResp(const OmnString &resp)
:
AosJimoAgentPackage(AosAgentPackageType::eCommandResp, AOSTAG_AGENTPACKAGE_COMMANDRESP),
mResp(resp)
{
}

bool
AosJimoAgentCommandResp::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mResp);
	return true;
}

bool 
AosJimoAgentCommandResp::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mResp = buff->getOmnStr("");
	return true;
}

bool
AosJimoAgentCommandResp::proc()
{
	printf("%s", mResp.data());
	return true;
}
