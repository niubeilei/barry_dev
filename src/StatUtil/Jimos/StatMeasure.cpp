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
// 2015/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StatUtil/Jimos/StatMeasure.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Rundata/Rundata.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosStatMeasure_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosStatMeasure(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosJimoPtr 
AosStatMeasure::cloneJimo() const
{
	return OmnNew AosStatMeasure(*this);
}


AosStatMeasure::AosStatMeasure(const int version)
:AosStatMeasureObj(version)
{
}

AosStatMeasure::~AosStatMeasure()
{
}

AosStatMeasureObjPtr 
AosStatMeasure::createStatMeasure(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
}

