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
#ifndef Aos_SysMsg_StopSendFmtTrans_h
#define Aos_SysMsg_StopSendFmtTrans_h

#include "TransUtil/TaskTrans.h"

class AosStopSendFmtTrans : public AosTaskTrans
{

private:

public:
	AosStopSendFmtTrans(const bool regflag);
	AosStopSendFmtTrans(
			const int to_svr_id,
			const u32 cube_grp_id);
			//const u32 new_master);
	~AosStopSendFmtTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eStopSendFmt"; };
	//virtual AosConnMsgPtr clone();
	virtual AosTransPtr clone();
	virtual bool proc();
	
};
#endif

