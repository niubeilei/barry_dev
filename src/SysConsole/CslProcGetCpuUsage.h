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
#ifndef Aos_SysConsole_CslProcGetCpuUsage_h
#define Aos_SysConsole_CslProcGetCpuUsage_h

#include "CloudCube/MsgProc.h"

class AosCslProcGetCpuUsage : public AosConsoleProc
{
	OmnDefineRCObject;

private:

public:
	AosCslProcGetCpuUsage(const bool flag = false);
	~AosCslProcGetCpuUsage(); 

	virtual bool procReq(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &entry, 
					OmnString &resp, 
					bool &finished);
};
#endif

