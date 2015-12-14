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
#ifndef Aos_SysMsg_ResendEndMsg_h
#define Aos_SysMsg_ResendEndMsg_h

#include "TransBasic/AppMsg.h"

class AosResendEndMsg : public AosAppMsg
{

private:	
	int		mTargetSid;
	u32		mTargetPid;
	bool	mIsIpcConn;
	int		mFromSid;
	u32		mFromPid;

public:
	AosResendEndMsg(const bool regflag);
	AosResendEndMsg(
		const int target_sid, 
		const u32 target_pid,
		const bool is_ipc_conn,
		const int from_sid,
		const u32 from_pid);
	~AosResendEndMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eResendEndMsg"; };
	virtual bool proc();
	
};
#endif

