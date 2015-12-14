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
#ifndef Aos_SysMsg_SwitchToMasterFinishMsg_h
#define Aos_SysMsg_SwitchToMasterFinishMsg_h

#include "TransBasic/AppMsg.h"

class AosSwitchToMasterFinishMsg : public AosAppMsg
{

private:
	u32		mFinishProcId;
	u32		mCubeGrpId;

public:
	AosSwitchToMasterFinishMsg(const bool regflag);
	AosSwitchToMasterFinishMsg(
		const int to_svr_id,
		const u32 to_proc_id,
		const u32 finish_proc_id,
		const u32 cube_grp_id);
	~AosSwitchToMasterFinishMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eSwitchToMasterFinishMsg"; };
	virtual bool proc();
	
};
#endif

