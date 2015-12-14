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
#ifndef Aos_SysMsg_ProcDeathMsg_h
#define Aos_SysMsg_ProcDeathMsg_h

#include "TransBasic/AppMsg.h"

class AosProcDeathMsg : public AosAppMsg
{

private:	
	int		mDeathSvrId;
	u64		mDeathProcId;
	AosProcessType::E	mProcType;

public:
	AosProcDeathMsg(const bool regflag);
	AosProcDeathMsg(
			const AosProcessType::E proc_type,
			const int to_svr_id,
			const u32 to_proc_id,
			const int death_sid,
			const u32 death_pid);
	~AosProcDeathMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eProcDeathMsg"; };
	virtual bool proc();
	
};
#endif

