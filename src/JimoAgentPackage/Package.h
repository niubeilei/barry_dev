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
#ifndef Aos_JimoAgentPackage_Package_h
#define Aos_JimoAgentPackage_Package_h

#include "Util/Buff.h"
#include "UtilComm/TcpClient.h"
#include "JimoAgentPackage/Ptrs.h"

class AosPackage : virtual public OmnRCObject
{
public:
	virtual bool serializeTo(const AosBuffPtr &buff) = 0;
	virtual bool serializeFrom(const AosBuffPtr &buff) = 0;
	virtual bool proc() = 0;
	virtual void setClient(const OmnTcpClientPtr &client) = 0;
	virtual int64_t getCrtOffset() = 0;
	virtual int64_t getLength() = 0;
	virtual bool getRslt() = 0;

};
#endif
