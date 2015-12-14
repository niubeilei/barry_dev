////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_SendShortMsg_h
#define Omn_ReqProc_SendShortMsg_h	

#include "ReqProc/ReqProc.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosSendShortMsg: public AosSeRequestProc
{
public:
	AosSendShortMsg(const bool);
	~AosSendShortMsg() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	
private:
	bool sendShortMsgNoResp(
					const AosXmlTagPtr &doc, 
					const AosRundataPtr &rdata);

	bool sendShortMsgResp(
					const AosXmlTagPtr &doc, 
					const AosRundataPtr &rdata);
};
#endif

