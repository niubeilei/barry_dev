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
// 2013/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/DataConverters/JimoDataConverters.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoDataConverters(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &worker_doc, 
 		const AosXmlTagPtr &jimo_doc) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosJimoDataConverters(rdata, worker_doc, jimo_doc);
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


bool
AosJimoDataConverters_getEpochYear(
		const AosRundataPtr &rdata,
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc)
{
	AosJimoDataConverters *jimo = (AosJimoDataConverters*)abs_jimo;
	int epochYear = jimo->getEpochYear(); 
	OmnScreen << "--------------- EpcohYear :" << epochYear << endl;
	return true;
}


bool 
AosJimoDataConverters_getEpochMonth(
		const AosRundataPtr &rdata,
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc)
{
	AosJimoDataConverters *jimo = (AosJimoDataConverters*)abs_jimo;
	int epochMonth = jimo->getEpochMonth(); 
	OmnScreen << "--------------- EpcohMonth :" << epochMonth<< endl;
	return true;
}


bool
AosJimoDataConverters_getEpochDay(
		const AosRundataPtr &rdata,
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc)
{
	AosJimoDataConverters *jimo = (AosJimoDataConverters*)abs_jimo;
	int epochDay= jimo->getEpochDay(); 
	OmnScreen << "--------------- EpcohDay:" << epochDay << endl;
	return true;
}


bool
AosJimoDataConverters_getEpochHour(
		const AosRundataPtr &rdata,
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc)
{
	AosJimoDataConverters *jimo = (AosJimoDataConverters*)abs_jimo;
	int epochHour= jimo->getEpochHour(); 
	OmnScreen << "--------------- EpcohHour:" << epochHour << endl;
	return true;
}


bool
AosJimoDataConverters_getEpochTime(
		const AosRundataPtr &rdata,
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc)
{
	AosJimoDataConverters *jimo = (AosJimoDataConverters*)abs_jimo;
	int epochTime= jimo->getEpochTime(); 
	OmnScreen << "--------------- EpcohTime:" << epochTime << endl;
	return true;
}


bool
AosJimoDataConverters_getStartTime(
		const AosRundataPtr &rdata,
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc)
{
	AosJimoDataConverters *jimo = (AosJimoDataConverters*)abs_jimo;
	OmnString startTime = jimo->getStartTime(); 
	OmnScreen << "--------------- StartTime:" << startTime << endl;
	return true;
}

