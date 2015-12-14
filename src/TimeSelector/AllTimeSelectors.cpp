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
// 11/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TimeSelector/AllTimeSelectors.h"

#include "TimeSelector/Ptrs.h"
#include "TimeSelector/TimeSelectorType.h"
#include "TimeSelector/TimeSelector.h"
#include "TimeSelector/TimeAdditive.h"
#include "TimeSelector/TimeGetEpoch.h"
#include "TimeSelector/TimeSubtraction.h"
#include "TimeSelector/TimeOfDayHour2Sec.h"
#include "TimeSelector/TimeStr2Epoch.h"
#include "TimeSelector/TimeEpoch2Str.h"

AosStr2U32_t   		AosTimeSelectorType::smNameMap55;
AosTimeSelectorPtr	sgTimeSelectors[AosTimeSelectorType::eMax+1];
AosAllTimeSelectors sgAllTimeSelector;

AosAllTimeSelectors::AosAllTimeSelectors()
{
static AosTimeAdditive 			sgTimeAdditive(true);
static AosTimeGetEpoch			sgTimeGetEpoch(true);
static AosTimeOfDayHour2Sec		sgTimeOfDayHour2Sec(true);
static AosTimeSubtraction		sgTimeSubtraction(true);
static AosTimeStr2Epoch			sgTimeStr2Epoch(true);
static AosTimeEpoch2Str			sgTimeEpoch2Str(true);
}

