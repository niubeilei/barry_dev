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
#ifndef Aos_JimoAgentPackage_JimoAgentPackage_h
#define Aos_JimoAgentPackage_JimoAgentPackage_h

#include "JimoAgentPackage/AgentPackageType.h"
#include "JimoAgentPackage/Ptrs.h"
#include "JimoAgentPackage/Package.h"
#include "UtilComm/TcpClient.h"
#include "Util/Buff.h"
#include "Util/String.h"

class AosJimoAgentPackage : public AosPackage
{
	OmnDefineRCObject;
protected:
	AosAgentPackageType::E 	mType;
	OmnString				mName;	
	OmnTcpClientPtr			mClient;
public:
	AosJimoAgentPackage(
			const AosAgentPackageType::E type,
			const OmnString &name);
	~AosJimoAgentPackage(){};
	bool registerAgentPackage(const AosJimoAgentPackage* package);
	static AosPackagePtr serializeFromStatic(const AosBuffPtr &buff);
	static bool staticInit();
	virtual void setClient(const OmnTcpClientPtr &client) { mClient = client; }
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool proc() { return true; }
	virtual int64_t getCrtOffset() { return -1; }
	virtual int64_t getLength() { return -1; }
	virtual bool getRslt() { return false; }
	static AosPackagePtr getPackage(const AosAgentPackageType::E type);
};
#endif
