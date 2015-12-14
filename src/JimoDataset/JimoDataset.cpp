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
// 2013/07/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoDataset/JimoDataset.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>


AosJimoDataset::AosJimoDataset(
		const int version,
		const AosJimoType::E type)
{
	mJimoVersion = version;
	mJimoType = AosJimoType::eDataset;
}

AosJimoDataset::~AosJimoDataset()
{
}
