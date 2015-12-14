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
#ifndef Aos_SysMsg_SendFmtTrans_h
#define Aos_SysMsg_SendFmtTrans_h

#include "TransUtil/TaskTrans.h"

class AosSendFmtTrans : virtual public AosTaskTrans
{

private:
	u32			mCubeGrpId;
	AosBuffPtr	mFmtBuff;

public:
	AosSendFmtTrans(const bool regflag);
	AosSendFmtTrans(
			const int svr_id,
			const u32 cube_grp_id,
			const AosBuffPtr &fmt_buff);
	~AosSendFmtTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eSendFmt"; };
	//virtual AosConnMsgPtr clone();
	virtual AosTransPtr clone();
	virtual bool proc();
	
};
#endif

