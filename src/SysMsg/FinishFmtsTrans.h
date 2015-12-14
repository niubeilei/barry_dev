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
#ifndef Aos_SysMsg_FinishFmtsTrans_h
#define Aos_SysMsg_FinishFmtsTrans_h

#include "TransUtil/TaskTrans.h"

class AosFinishFmtsTrans : virtual public AosTaskTrans
{

private:
	u64			mMaxFmtId;
	int			mCrtSvrId;

public:
	AosFinishFmtsTrans(const bool regflag);
	AosFinishFmtsTrans(
			const int svr_id,
			const u32 cube_grp_id,
			const u64 max_fmt_id,
			const int crt_svr_id);
	~AosFinishFmtsTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eFinishFmts"; };
	//virtual AosConnMsgPtr clone();
	virtual AosTransPtr clone();
	virtual bool proc();
	
private:

};
#endif

