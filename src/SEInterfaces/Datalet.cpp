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
// 2013/03/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/Datalet.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"


AosDatalet::AosDatalet(const AosDataletType::E type, const u32 version)
:
AosJimo(AosJimoType::eDatalet, 1),
mDataletType(type)
{
}


AosDatalet::~AosDatalet()
{
}



