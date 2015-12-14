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
#ifndef Aos_JimoAgentPackage_JimoAgentFileToLocal_h
#define Aos_JimoAgentPackage_JimoAgentFileToLocal_h

#include "JimoAgentPackage/JimoAgentPackage.h"
#include "Util/Buff.h"
#include "Util/String.h"

class AosJimoAgentFileToLocal: public AosJimoAgentPackage
{
public:
	enum
	{
		eBlockSize = 1*1024*1024
	};
private:
	OmnString			mUser;
	OmnString			mRemoteFile;
	OmnString			mLocalDir;
	int64_t				mOffset;
public:
	AosJimoAgentFileToLocal();
	AosJimoAgentFileToLocal(
			const OmnString &user, 
			const OmnString &remote_file,
			const OmnString &local_dir,
			const int64_t &offset);
	~AosJimoAgentFileToLocal(){}
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool proc();
private:
	bool	sendResp(const OmnString &resp, const bool rslt);
};
#endif
