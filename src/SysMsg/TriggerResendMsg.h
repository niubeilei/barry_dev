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
#ifndef Aos_SysMsg_TriggerResendMsg_h
#define Aos_SysMsg_TriggerResendMsg_h

#include "TransBasic/AppMsg.h"

class AosTriggerResendMsg : public AosAppMsg
{

private:
	OmnString mReason;
	bool	mNeedResendEnd;
	int		mTargetSvrId;
	u32		mTargetProcId;
	bool	mIsIpcConn;

public:
	AosTriggerResendMsg(const bool regflag);
	AosTriggerResendMsg(const OmnString reason);
	AosTriggerResendMsg(
		const OmnString &reason, 
		const int target_svr_id,
		const u32 target_proc_id,
		const bool is_ipc_conn);
	~AosTriggerResendMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eTriggerResendMsg"; };
	virtual bool proc();

	OmnString	getReason(){ return mReason; };
	bool	isNeedResendEnd(){ return mNeedResendEnd; };
	bool	isIpcConn(){ return mIsIpcConn; };
	int		getTargetSvrId(){ return mTargetSvrId; };
	u32		getTargetProcId(){ return mTargetProcId; };
	
};
#endif

