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
// 2013/03/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysConsole_ServerOperations_h
#define Aos_SysConsole_ServerOperations_h

#include "ReqProc/ReqProc.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"
#include "Util/String.h"

class AosServerOperations: public AosSeRequestProc
{
public:
	AosServerOperations(const bool);
	~AosServerOperations() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	
private:
};
#endif

