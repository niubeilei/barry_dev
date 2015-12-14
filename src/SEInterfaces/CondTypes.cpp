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
// 05/10/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/CondTypes.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"

OmnMutex 		AosCondType::smLock;
AosStr2U32_t	AosCondType::smNameMap;
OmnString 		AosCondType::smEnumMap[AosCondType::eMax];
bool			AosCondType::smInited = false;

AosCondType::E
AosCondType::getRandomType()
{
	int nn = getNumValidEntries();
	aos_assert_r(nn > 0, (E)0);
	int code = OmnRandom::nextInt1(0, nn-1);
	return (E)(code + getFirstEntry());
}
	
