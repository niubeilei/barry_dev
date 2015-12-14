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
#ifndef Omn_ReqProc_RunIndexMgr_h
#define Omn_ReqProc_RunIndexMgr_h	

#include "ReqProc/ReqProc.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/Caller.h"

class AosRunIndexMgr: public AosSeRequestProc
{
	//Important: This class shoud not have any member data, it's not thead safe. 
public:
	enum E
	{
		eMaxFields = 50		
	};

public:
	AosRunIndexMgr(const bool);
	~AosRunIndexMgr() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
};
#endif

