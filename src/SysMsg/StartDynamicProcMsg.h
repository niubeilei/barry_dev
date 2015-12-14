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
#ifndef Aos_SysMsg_StartDynamicProcMsg_h
#define Aos_SysMsg_StartDynamicProcMsg_h

#include "TransBasic/AppMsg.h"
#include "API/AosApi.h"
#include "SEInterfaces/ProcessType.h"

class AosStartDynamicProcMsg : public AosAppMsg
{

private:
	AosProcessType::E	mProcType;
	int					mListenSvrId;

public:
	AosStartDynamicProcMsg(const bool regflag);
	AosStartDynamicProcMsg(const AosProcessType::E proc_tp); 
	AosStartDynamicProcMsg(
			const AosProcessType::E proc_tp, 
			const int listen_svr_id,
			const int to_svr_id);
	~AosStartDynamicProcMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eStartDynamicProc"; };
	virtual bool proc();
	
};
#endif

