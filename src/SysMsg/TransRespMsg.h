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
// 2013/07/23	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysMsg_TransRespMsg_h
#define Aos_SysMsg_TransRespMsg_h

#include "TransBasic/AppMsg.h"
#include "Util/TransId.h"

// this resp is for SvrProxy.
class AosTransRespMsg : public AosAppMsg
{

private:
	AosTransId 	mReqId;
	AosBuffPtr  mResp;

public:
	AosTransRespMsg(const bool reg_flag);
	AosTransRespMsg(
		const int to_svr_id,
		const u32 to_proc_id,
		const AosTransId req_id,
		const AosBuffPtr &resp);
	~AosTransRespMsg();

	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eTransRespMsg"; };
	virtual AosConnMsgPtr clone2();
	virtual bool proc();
	
};
#endif

