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
// 08/15/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_AddVersion_h
#define Omn_ReqProc_AddVersion_h	

#include "ReqProc/ReqProc.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/SeRequestProc.h"

class AosAddVersion: public AosSeRequestProc
{
public:
	AosAddVersion(const bool);
	~AosAddVersion() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
};
#endif

