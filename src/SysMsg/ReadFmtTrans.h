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
#ifndef Aos_SysMsg_ReadFmtTrans_h
#define Aos_SysMsg_ReadFmtTrans_h

#include "TransUtil/TaskTrans.h"

class AosReadFmtTrans : virtual public AosTaskTrans
{

private:
	u32			mCubeGrpId;
	int			mFileSeq;
	u64			mBegFmtId;
	u64			mEndFmtId;

public:
	AosReadFmtTrans(const bool regflag);
	AosReadFmtTrans(
			const int svr_id,
			const u32 cube_grp,
			const int file_seq,
			const u64 beg_fmt_id,
			const u64 end_fmt_id);
	~AosReadFmtTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eReadFmt"; };
	//virtual AosConnMsgPtr clone();
	virtual AosTransPtr clone();
	virtual bool proc();
	
private:

};
#endif

