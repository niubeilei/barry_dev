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
// 2013/03/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_IILCache_MsgProcPhase2Req_h
#define Aos_IILCache_MsgProcPhase2Req_h


class AosMsgProcPhase2Req : virtual public AosMsgProc
{
	OmnDefineRCObject;

private:

public:
	AosMsgProcPhase2Req();
	~AosMsgProcPhase2Req(); 

	virtual bool procMsg(
					const AosRundataPtr &rdata,
					const AosBuffMsgPtr &msg);
};
#endif
#endif
