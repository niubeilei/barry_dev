////////////////////////////////////////////////////////////////////////////
//
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
#ifndef Aos_SysMsg_SetNewMasterTrans_h
#define Aos_SysMsg_SetNewMasterTrans_h

#include "TransUtil/TaskTrans.h"

class AosSetNewMasterTrans : virtual public AosTaskTrans
{

private:
	u32			mCubeGrpId;
	int			mMaster;

public:
	AosSetNewMasterTrans(const bool regflag);
	AosSetNewMasterTrans(
			const int svr_id,
			const u32 proc_id,
			const u32 cube_grp_id,
			const int master);
	~AosSetNewMasterTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eSetNewMaster"; };
	//virtual AosConnMsgPtr clone();
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

