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
#include "JimoAgentPackage/JimoAgentFileToRemoteResp.h"


AosJimoAgentFileToRemoteResp::AosJimoAgentFileToRemoteResp()
:
AosJimoAgentPackage(AosAgentPackageType::eFileToRemoteResp, AOSTAG_AGENTPACKAGE_FILETOREMOTERESP)
{
}

AosJimoAgentFileToRemoteResp::AosJimoAgentFileToRemoteResp(const OmnString &resp, const bool rslt)
:
AosJimoAgentPackage(AosAgentPackageType::eFileToRemoteResp, AOSTAG_AGENTPACKAGE_FILETOREMOTERESP),
mResp(resp),
mRslt(rslt)
{
}

bool
AosJimoAgentFileToRemoteResp::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mResp);
	buff->setU8(mRslt);
	return true;
}

bool 
AosJimoAgentFileToRemoteResp::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mResp = buff->getOmnStr("");
	mRslt = buff->getU8(0);
	return true;
}

bool
AosJimoAgentFileToRemoteResp::proc()
{
	printf("%s", mResp.data());
	return mRslt;
}


