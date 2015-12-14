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
// 06/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_CreateAccessRecord_h
#define Omn_ReqProc_CreateAccessRecord_h	

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "ReqProc/ReqProc.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"
#include "Util/String.h"

class AosCreateAccessRecord: public AosSeRequestProc
{
public:
	AosCreateAccessRecord(const bool);
	~AosCreateAccessRecord() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	
private:
};
#endif
