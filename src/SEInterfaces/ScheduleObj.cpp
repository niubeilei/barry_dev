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
#include "SEInterfaces/ScheduleObj.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosScheduleObj::AosScheduleObj(const u32 version)
:
AosJimo(AosJimoType::eSchedule, version)
{
}


AosScheduleObj::~AosScheduleObj()
{
}
