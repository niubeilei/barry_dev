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
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_TransSvrDeathMsg_h
#define AOS_TransUtil_TransSvrDeathMsg_h

#include "TransBasic/AppMsg.h"
#include "Util/TransId.h"
#include "Util/Buff.h"

class AosTransSvrDeathMsg: public AosAppMsg
{

private:
	AosTransId 	mReqId;
	int			mDeathSvrId;

public:
	AosTransSvrDeathMsg(const bool reg_flag);
	AosTransSvrDeathMsg(
		const int to_svrid,
		const u32 to_proc_id,
		const AosTransId &req_id,
		const int death_svr_id);
	~AosTransSvrDeathMsg();
	
	virtual bool 	serializeTo(const AosBuffPtr &buff);
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eTransSvrDeathMsg"; };
	virtual bool	proc();
	
};

#endif
