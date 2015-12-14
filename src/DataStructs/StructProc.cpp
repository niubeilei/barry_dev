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
// 2013/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/StructProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"

AosStructProc::AosStructProc(
		const OmnString &type,
		const int version)
:
AosStructProcObj(version),
mType(type),
mDftValue(0)
{
}


AosStructProc::~AosStructProc()
{
}


vector<AosValueRslt> & 
AosStructProc::getNewValues()
{
	return mNewValueRslt;
}


vector<AosValueRslt> & 
AosStructProc::getOldValues()
{
	return mOldValueRslt;
}


void 
AosStructProc::clearValuesRslt()
{
	mOldValueRslt.clear(); 
	mNewValueRslt.clear();
}


void 
AosStructProc::config(const AosXmlTagPtr &conf)
{
	OmnShouldNeverComeHere;
}


bool 
AosStructProc::setFieldValue(
		char *array,
		const int64_t data_len,
		const int pos,
		const int64_t &value,
		AosDataType::E data_type,
		const u64 &stat_id)
{
	OmnShouldNeverComeHere;
	return false;
}


OmnString 
AosStructProc::getJimoClassname(const OmnString &type)
{
	OmnString classname = "";
	if (type == AOSSTRUCTPROC_SUM) 
	{
		classname = "AosStructProcSum";
	}

	if (type == AOSSTRUCTPROC_MAX) 
	{
		classname = "AosStructProcMax";
	}

	if (type == AOSSTRUCTPROC_MIN) 
	{
		classname = "AosStructProcMin";
	}
	return classname;
}


int64_t 
AosStructProc::calculateGeneralValue(const int64_t &new_value, const int64_t &old_value)
{
	OmnShouldNeverComeHere;
	return -1;
}

int 
AosStructProc::getDftValue()
{
	OmnShouldNeverComeHere;
	return -1;
}
