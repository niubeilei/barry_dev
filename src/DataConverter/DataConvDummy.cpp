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
// 2013/11/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataConverter/DataConvDummy.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"



AosDataConvDummy::AosDataConvDummy(const OmnString &version)
:
AosDataConverter("dummy", version)
{
}


AosDataConvDummy::~AosDataConvDummy()
{
}


