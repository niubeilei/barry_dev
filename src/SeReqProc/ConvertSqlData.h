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
// 06/15/2011	Created by Tracy 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_ConvertSqlData_h
#define Omn_ReqProc_ConvertSqlData_h	

#include "ReqProc/ReqProc.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosConvertSqlData: public AosSeRequestProc
{
	//ConvertSqlant: This class shoud not have any member data, it's not thead safe. 
	
public:
	AosConvertSqlData(const bool);
	~AosConvertSqlData() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	
private:
};
#endif

