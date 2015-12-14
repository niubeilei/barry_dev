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
#include "JimoDataProc/JimoDataProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>



AosJimoDataProc::AosJimoDataProc(const AosJimoDataProc &rhs)
:
AosDataProc(rhs)
{
	mPipEnd = rhs.mPipEnd;
	mProcDataCount = rhs.mProcDataCount;
	mOutputCount = rhs.mOutputCount;
}

AosJimoDataProc::AosJimoDataProc(
		const int version,
		const AosJimoType::E type)
:
AosDataProc(AOSDATAPROC_JIMO, AosDataProcId::eJimo, false)
{
	mPipEnd = true;
	mProcDataCount = 0;
	mOutputCount = 0;
}


AosJimoDataProc::~AosJimoDataProc()
{
}
	

bool
AosJimoDataProc::flushRecord(
		AosDataRecordObj **output_records, 
		AosDataRecordObj *record,
		AosRundata *rdata_raw)
{
	if (!isPipEnd())
	{
		output_records[0] = record;
	}
	else
	{
		aos_assert_r(record, false);
		record->flushRecord(rdata_raw);
	}

	return true;
}


AosDataFieldType::E
AosJimoDataProc::convertToDataFieldType(
		AosRundata* rdata,
		const AosExprObjPtr &expr,
		const AosDataRecordObjPtr &record)
{
	AosDataType::E datatype = expr->getDataType(rdata, record.getPtr());
	OmnString str_type = AosDataType::getTypeStr(datatype);

	AosDataFieldType::E type = AosDataFieldType::toEnum(str_type);
	//aos_assert_r(type != AosDataFieldType::eInvalid, AosDataFieldType::eInvalid);
	switch(type)
	{
		case AosDataFieldType::eStr:
			break;
		case AosDataFieldType::eBinU64:
			break;
		case AosDataFieldType::eBinDouble:
			break;
		case AosDataFieldType::eU64:
			type = AosDataFieldType::eBinU64;
			break;
		case AosDataFieldType::eDouble:
		case AosDataFieldType::eNumber:
			type = AosDataFieldType::eBinDouble;
			break;
		case AosDataFieldType::eDateTime:
			type = AosDataFieldType::eBinDateTime;
			break;
		case AosDataFieldType::eInt64:
			type = AosDataFieldType::eBinInt64;
			break;
		case AosDataFieldType::eInvalid:
			break;

		default:
			OmnAlarm << "not handle this data field type: " << str_type << enderr;
			break;
	}

	return type;
}


AosDataFieldType::E
AosJimoDataProc::convertToDataFieldType(
		AosRundata* rdata,
		const OmnString &str_type)
{
	AosDataFieldType::E type = AosDataFieldType::toEnum(str_type);
	aos_assert_r(type != AosDataFieldType::eInvalid, AosDataFieldType::eInvalid);
	switch(type)
	{
		case AosDataFieldType::eStr:
			break;
		case AosDataFieldType::eBinU64:
			break;
		case AosDataFieldType::eBinDouble:
			break;
		case AosDataFieldType::eU64:
			type = AosDataFieldType::eBinU64;
			break;
		case AosDataFieldType::eDouble:
		case AosDataFieldType::eNumber:
			type = AosDataFieldType::eBinDouble;
			break;
		case AosDataFieldType::eDateTime:
			type = AosDataFieldType::eBinDateTime;
			break;
		case AosDataFieldType::eInt64:
			type = AosDataFieldType::eBinInt64;
			break;
		default:
			OmnAlarm << "not handle this data field type: " << str_type << enderr;
			break;
	}

	return type;
}

AosExprObjPtr
AosJimoDataProc::convertToExpr(
		const OmnString &name,
		const AosRundataPtr &rdata)
{
	OmnString str = name;
	str << ";";

	OmnString errmsg;
	AosExprObjPtr expr = AosParseExpr(str, errmsg, rdata.getPtr());
	if (!expr)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " 
			<< "" << enderr;

		OmnAlarm << name << enderr;
		return NULL;
	}
	return expr;
}


void
AosJimoDataProc::resetDataFieldLen(
		const AosDataFieldType::E type,
		int &len)
{
	if (type == AosDataFieldType::eBinU64 || type == AosDataFieldType::eBinDouble)
		len = 8;
}

OmnString 
AosJimoDataProc::getDataProcName()
{
	return mName;
}

bool	
AosJimoDataProc::dumpRecord(
		const AosDataRecordObjPtr &rec,
		const OmnString &msg)
{
	aos_assert_r(rec, false);
	OmnString msg1 = "";
	
	msg1 << "[" << mName << "]" << msg1;
	return rec->dumpData(true, msg);
}


/*
void
AosJimoDataProc::showDataProcInfo()
{
	if (mProcDataCount == 0)
	{
		mTimeStamp = OmnGetTimestamp();
	}
	else if(mProcDataCount % eMaxPrintSize == 0)
	{
		u64 time = OmnGetTimestamp();

		OmnJobInfo << "!-! DataProcInfo: " << mName << ", num: " << eMaxPrintSize
			<< ", time: " << time - mTimeStamp << "(us)" << endl;
		mTimeStamp = time;
	}
}
*/
