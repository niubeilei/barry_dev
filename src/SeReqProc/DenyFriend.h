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
// 06/22/2011	Created by Michael
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_DenyFriend_h
#define Omn_ReqProc_DenyFriend_h	

#include "ReqProc/ReqProc.h"
#include "SearchEngine/DocServer.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SeReqProc/SeRequestProc.h"

class AosDenyFriend: public AosSeRequestProc
{
	//Important: This class shoud not have any member data, it's not thead safe. 
	
public:
	AosDenyFriend(const bool);
	~AosDenyFriend() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	
private:
};
#endif

