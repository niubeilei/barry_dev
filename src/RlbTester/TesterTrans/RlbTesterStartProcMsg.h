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
#ifndef Aos_RlbTester_TesterTrans_RlbTesterStartProcMsg_h
#define Aos_RlbTester_TesterTrans_RlbTesterStartProcMsg_h

#include "TransBasic/AppMsg.h"

class AosRlbTesterStartProcMsg : public AosAppMsg
{

private:
	u32			mStartProcId;

public:
	AosRlbTesterStartProcMsg(const bool regflag);
	AosRlbTesterStartProcMsg(
			const int to_svr_id,
			const u32 to_proc_id,
			const u32 start_proc_id);
	~AosRlbTesterStartProcMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eRlbTesterStartProc"; };
	virtual bool proc();
	
};
#endif

