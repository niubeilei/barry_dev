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
#ifndef Aos_JimoAgentPackage_JimoAgentFileToLocalResp_h
#define Aos_JimoAgentPackage_JimoAgentFileToLocalResp_h

#include "JimoAgentPackage/JimoAgentPackage.h"
#include "Util/Buff.h"
#include "Util/String.h"

class AosJimoAgentFileToLocalResp: public AosJimoAgentPackage
{
public:
	enum
	{
		eBlockSize = 1*1024*1024
	};
private:
	bool				mRslt;
	OmnString			mResp;
	OmnString			mUser;
	OmnString			mLocalDir;
	OmnString			mFileName;
	int64_t				mLength;
	int64_t				mOffset;
	AosBuffPtr			mBuff;
	int64_t				mCrtOffset;
public:
	AosJimoAgentFileToLocalResp();
	AosJimoAgentFileToLocalResp(const OmnString &resp, const bool rlst);
	AosJimoAgentFileToLocalResp(
			const OmnString &user,
			const OmnString &local_dir,
			const OmnString &name,
			const int64_t &length,
			const int64_t &offset,
			const AosBuffPtr &buff);
	~AosJimoAgentFileToLocalResp(){}
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool proc();
	virtual int64_t getCrtOffset() { return mCrtOffset; }
	virtual int64_t getLength() { return mLength; }
	virtual bool getRslt() { return mRslt; }
};
#endif
