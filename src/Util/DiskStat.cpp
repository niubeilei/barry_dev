////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/11/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/DiskStat.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"



bool 
AosDiskStat::setMetadata(const AosXmlTagPtr &doc)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosDiskStat::addStat(const AosDiskStat &stat)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosDiskStat::setServerIsDown(const bool server_down)
{
	err_no = eSvrDown;
	//server_is_down = server_down;
	return true;
}
