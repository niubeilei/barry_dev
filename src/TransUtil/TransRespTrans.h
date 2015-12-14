////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Trans.h
// Description:
//	This is the super class for transactions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransUtil_TransRespTrans_h
#define Aos_TransUtil_TransRespTrans_h

#include "TransUtil/TaskTrans.h"


class AosTransRespTrans : public AosTaskTrans 
{

private:
	AosTransId 	mReqId;
	bool		mReqIsSyncResp;
	AosBuffPtr  mResp;

public:
	AosTransRespTrans(const bool reg_flag);
	AosTransRespTrans(
		const int to_svr_id,
		const u32 to_proc_id,
		const AosTransId req_id,
		const bool req_is_sync_resp,
		const AosBuffPtr &resp,
		const bool need_save);
	~AosTransRespTrans();

	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eTransResp"; };
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool directProc();
	
	AosTransId	getReqId(){ return mReqId; };

};
#endif

