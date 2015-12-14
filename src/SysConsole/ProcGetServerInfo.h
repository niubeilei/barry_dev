////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2013/03/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysConsole_ProcGetServerInfo_h
#define Aos_SysConsole_ProcGetServerInfo_h

#include "CloudCube/MsgProc.h"

class AosMsgProcGetServerInfo : virtual public AosIILQueryProc
{
	OmnDefineRCObject;

private:

public:
	AosMsgProcGetServerInfo();
	~AosMsgProcGetServerInfo(); 

	virtual bool procMsg(
					const AosRundataPtr &rdata,
					const AosBuffMsgPtr &msg);
};
#endif
