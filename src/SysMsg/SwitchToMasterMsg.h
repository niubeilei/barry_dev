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
#ifndef Aos_SysMsg_SwitchToMasterMsg_h
#define Aos_SysMsg_SwitchToMasterMsg_h

#include "TransBasic/AppMsg.h"

class AosSwitchToMasterMsg : public AosAppMsg
{

private:
	u32		mCubeGrpId;

public:
	AosSwitchToMasterMsg(const bool regflag);
	AosSwitchToMasterMsg(const int to_svr_id, const u32 cube_grp_id);
	AosSwitchToMasterMsg(
		const int to_svr_id,
		const int to_proc_id,
		const u32 cube_grp_id);
	~AosSwitchToMasterMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eSwitchToMasterMsg"; };
	virtual bool proc();
	
};
#endif

