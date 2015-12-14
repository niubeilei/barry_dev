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
#ifndef Aos_SysMsg_StartProcFinishMsg_h
#define Aos_SysMsg_StartProcFinishMsg_h

#include "TransBasic/AppMsg.h"
#include "API/AosApi.h"
#include "SEInterfaces/ProcessType.h"

class AosStartProcFinishMsg : public AosAppMsg
{

private:
	AosProcessType::E	mProcType;
	u32					mStartProcId;
	int					mStartProcPid;	

public:
	AosStartProcFinishMsg(const bool regflag);
	AosStartProcFinishMsg(
			const u32 to_proc_id,
			const AosProcessType::E proc_tp,
			const u32 start_proc_id,
			const int start_proc_pid,
			const int to_svr_id);
	AosStartProcFinishMsg(
			const u32 to_proc_id,
			const AosProcessType::E proc_tp,
			const u32 start_proc_id,
			const int start_proc_pid);
	~AosStartProcFinishMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eStartProcFinish"; };
	virtual bool proc();
	
};
#endif

