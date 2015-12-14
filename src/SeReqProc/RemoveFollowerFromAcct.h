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
// Modification History:
// 02/22/2012	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_RemoveFollowerFromAcct_h
#define Omn_ReqProc_RemoveFollowerFromAcct_h	

#include "ReqProc/ReqProc.h"
#include "SearchEngine/DocServer.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosRemoveFollowerFromAcct: public AosSeRequestProc
{
	//Important: This class shoud not have any member data, it's not thead safe. 
	
public:
	AosRemoveFollowerFromAcct(const bool);
	~AosRemoveFollowerFromAcct() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	
private:
};
#endif

