////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
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
#ifndef Aos_JimoAgentPackage_JimoAgentFileToRemoteResp_h
#define Aos_JimoAgentPackage_JimoAgentFileToRemoteResp_h

#include "JimoAgentPackage/JimoAgentPackage.h"
#include "Util/Buff.h"
#include "Util/String.h"

class AosJimoAgentFileToRemoteResp: public AosJimoAgentPackage
{
private:
	OmnString			mResp;
	bool				mRslt;
public:
	AosJimoAgentFileToRemoteResp();
	AosJimoAgentFileToRemoteResp(const OmnString &command, const bool rslt);
	~AosJimoAgentFileToRemoteResp(){}
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool proc();
	virtual bool getRslt() { return mRslt; }
};
#endif
