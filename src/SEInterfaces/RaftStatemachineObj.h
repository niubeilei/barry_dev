////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/06/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_RaftStateMachineObj_h
#define Aos_SEInterfaces_RaftStateMachineObj_h

#include "Jimo/Jimo.h"


class AosRaftStatemachineObj : virtual public AosJimo
{
protected:

public:
	virtual ~AosRaftStatemachineObj();

	virtual void setASEID(const u32 aseid) = 0;
	virtual void setJPID(const u64 jpid) = 0;
	virtual void setJSID(const u64 jSid) = 0;
	virtual bool start(AosRundata *rdata) = 0;
};

#endif

