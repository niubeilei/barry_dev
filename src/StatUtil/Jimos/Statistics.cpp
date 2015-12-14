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
#include "StatUtil/Jimos/Statistics.h"

#include "SEInterfaces/StatModelObj.h"
#include "SEInterfaces/StatMeasureObj.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Rundata/Rundata.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosStatistics_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosStatistics(version);
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
AosStatistics::cloneJimo() const
{
	return OmnNew AosStatistics(*this);
}


AosStatistics::AosStatistics(const int version)
:AosStatisticsObj(version)
{
}


AosStatistics::~AosStatistics()
{
}

vector<AosStatModelObjPtr> 
AosStatistics::getStatModels(AosRundata *rdata)
{
}

AosStatModelObjPtr 
AosStatistics::getStatModel(
		AosRundata *rdata, 
		const OmnString &name)
{
}

AosStatisticsObjPtr 
AosStatistics::createStatistics(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
}

bool 
AosStatistics::addStatModel(
		AosRundata *rdata, 
		const OmnString &name,
		const vector<OmnString> &keys,
		const OmnString &file_fname, 
		const vector<AosStatMeasureObjPtr> &measures)
{
}

bool 
AosStatistics::removeStatModel(
		AosRundata *rdata, 
		const OmnString &name)
{
}
