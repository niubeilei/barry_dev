// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysMsg_ProcUpMsg_h
#define Aos_SysMsg_ProcUpMsg_h

#include "TransBasic/AppMsg.h"

class AosProcUpMsg : public AosAppMsg
{

private:	
	int		mUpSvrId;
	u64		mUpProcId;

public:
	AosProcUpMsg(const bool regflag);
	AosProcUpMsg(
			const int to_svr_id,
			const u32 to_proc_id,
			const int up_sid,
			const u32 up_pid);
	~AosProcUpMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eProcUpMsg"; };
	virtual bool proc();
	
};
#endif

