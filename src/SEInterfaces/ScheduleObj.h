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
// 2013/10/25 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ScheduleObj_h
#define Aos_SEInterfaces_ScheduleObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosScheduleObj : virtual public AosJimo
{
protected:

public:
	AosScheduleObj(const u32 version);
	~AosScheduleObj();

};
#endif
