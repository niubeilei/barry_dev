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
// Modification History:
// 2014/08/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DiskMgr_DiskMgr_h
#define Aos_DiskMgr_DiskMgr_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DiskMgrObj.h"

class AosDiskMgr : public AosDiskMgrObj
{
	OmnDefineRCObject;

private:

public:
	AosDiskMgr();
	~AosDiskMgr();
};
#endif

