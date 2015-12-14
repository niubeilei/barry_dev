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
#ifndef Aos_JimoAgentPackage_JimoAgentFileToRemote_h
#define Aos_JimoAgentPackage_JimoAgentFileToRemote_h

#include "JimoAgentPackage/JimoAgentPackage.h"
#include "Util/Buff.h"
#include "Util/String.h"

class AosJimoAgentFileToRemote: public AosJimoAgentPackage
{
private:
	OmnString			mUser;
	OmnString			mRemoteDir;
	OmnString			mFileName;
	int64_t				mOffset;
	int64_t				mLength;
	AosBuffPtr			mBuff;
public:
	AosJimoAgentFileToRemote();
	AosJimoAgentFileToRemote(
			const OmnString &user, 
			const OmnString &remote_dir,
			const OmnString &name,
			const int64_t &offset,
			const int64_t &length,
			const AosBuffPtr &buff);
	~AosJimoAgentFileToRemote(){}
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool proc();
private:
	bool sendResp(const OmnString &resp, const bool rslt);
};
#endif
