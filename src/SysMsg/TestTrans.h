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
#ifndef Aos_SysMsg_TestTrans_h
#define Aos_SysMsg_TestTrans_h

#include "TransUtil/CubicTrans.h"

class AosTestTrans : virtual public AosCubicTrans
{

private:
	u32			mCubeGrpId;
	AosBuffPtr	mFmtBuff;

public:
	AosTestTrans(const bool regflag);
	AosTestTrans(
			const u32 cube_grp_id,
			const AosBuffPtr &fmt_buff);
	~AosTestTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eTest"; };
	//virtual AosConnMsgPtr clone();
	virtual AosTransPtr clone();
	virtual bool directProc();
	virtual bool proc();
	
};
#endif

