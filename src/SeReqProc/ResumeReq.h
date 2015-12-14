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
// 10/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
Chen Ding, 01/27/2012
This is not used anymore
#ifndef Omn_ReqProc_ResumeReq_h
#define Omn_ReqProc_ResumeReq_h	

#include "ReqProc/ReqProc.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosResumeReq: public AosSeRequestProc
{
public:
	enum
	{
		eMaxPendingReq = 300,
	};

public:
	AosResumeReq(const bool);
	~AosResumeReq() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	static void procInvalidSession(
					const AosWebRequestPtr &req, 
					const AosXmlTagPtr &root,
					const OmnString &loginvpd, 
					const AosRundataPtr &rdata);
	
private:
	bool resendLoginReq(
					const AosWebRequestPtr &req, 
					const u32 idx, 
					const AosRundataPtr &rdata);
	AosXmlTagPtr getPendingReq(
					const AosXmlTagPtr &root, 
					u32 &idx, 
					const AosRundataPtr &rdata);
};
#endif
#endif
