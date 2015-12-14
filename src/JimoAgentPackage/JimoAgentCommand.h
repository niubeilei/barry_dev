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
#ifndef Aos_JimoAgentPackage_JimoAgentCommand_h
#define Aos_JimoAgentPackage_JimoAgentCommand_h

#include "JimoAgentPackage/JimoAgentPackage.h"
#include "Util/Buff.h"
#include "Util/String.h"

class AosJimoAgentCommand: public AosJimoAgentPackage
{
private:
	OmnString			mUser;
	OmnString			mCommand;
public:
	AosJimoAgentCommand();
	AosJimoAgentCommand(const OmnString &user, const OmnString &command);
	~AosJimoAgentCommand(){}
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool proc();
};
#endif
