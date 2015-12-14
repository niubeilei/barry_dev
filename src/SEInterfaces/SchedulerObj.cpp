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
#include "SEInterfaces/SchedulerObj.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosSchedulerObj::AosSchedulerObj(const u32 version)
:
AosJimo(AosJimoType::eScheduler, version)
{
}


AosSchedulerObj::~AosSchedulerObj()
{
}
