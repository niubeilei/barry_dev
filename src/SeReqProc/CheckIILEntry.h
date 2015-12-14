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
// 2013/08/20 Created by Ken Lee 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_CheckIILEntry_h
#define Omn_ReqProc_CheckIILEntry_h	

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"


class AosCheckIILEntry: public AosSeRequestProc
{

public:
	AosCheckIILEntry(const bool);
	~AosCheckIILEntry() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	
};

#endif

