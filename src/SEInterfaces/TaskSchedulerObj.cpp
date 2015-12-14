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
// 2013/10/31 Created by Linda Lin
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/TaskSchedulerObj.h"

#include "SEInterfaces/JimoType.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosTaskSchedulerObj::AosTaskSchedulerObj()
:
AosJimo(AosJimoType::eTaskScheduler, 0)
{
}

AosTaskSchedulerObj::AosTaskSchedulerObj(const u32 version)
:
AosJimo(AosJimoType::eTaskScheduler, version)
{
}


AosTaskSchedulerObj::~AosTaskSchedulerObj()
{
}

