////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This data record uses AosBuff to retrieve values. As an example, 
// if the data is created using AosBuff:
// 		buff->setU32(...);
// 		buff->setU64(...);
// 		buff->setStr(...);
// 		...
// We can use this data record to retrieve field values.
//
// Modification History:
// 2013/12/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataRecord/Jimos/RecordByBuff.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataField/DataField.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Value/Value.h"
#include "XmlUtil/XmlTag.h"

static OmnString sgCurrentVersion = "1.0";

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosRecordByBuff(const AosRundataPtr &rdata, const OmnString &version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosRecordByBuff(version);
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


AosRecordByBuff::AosRecordByBuff(const OmnString &version)
:
AosDataRecord(AosDataRecordType::eJimoRecord, 
		AOSRECORDTYPE_JIMO_RECORD, version, false AosMemoryCheckerArgs)
{
}


AosRecordByBuff::~AosRecordByBuff()
{
}


bool
AosRecordByBuff::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	if (worker_doc)
	{
		bool rslt = AosDataRecord::config(worker_doc, rdata);
		aos_assert_r(rslt, false);
	}
	
	return true;
}


AosDataRecordObjPtr 
AosRecordByBuff::clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordByBuff(*this);
	}

	catch (...)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordByBuff::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordByBuff(*this);
	}

	catch (...)
	{
		OmnAlarm << "internal error" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordByBuff::create(
		const AosXmlTagPtr &worker_doc,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata AosMemoryCheckDecl) const
{
	try
	{
		AosRecordByBuff * record = OmnNew AosRecordByBuff(sgCurrentVersion);
		record->setTask(task);
		bool rslt = record->config(rdata, worker_doc, 0);
		aos_assert_r(rslt, 0);
		return record;
	}

	catch (...)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}
}


AosValuePtr
AosRecordByBuff::getFieldValue(
		const AosRundataPtr &rdata, 
		const int field_idx)
{
	if (field_idx <= 0 || (u32)field_idx >= mFields.size())
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	if (!mIsDataParsed) if (!parseData(rdata)) return 0;

	if (!mFields[field_idx])
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	return mFields[field_idx]->getFieldValue(rdata);
}


bool		
AosRecordByBuff::getFieldValue(
		const int idx, 
		AosValueRslt &value, 
		const AosRundataPtr &rdata) 
{
	if (idx <= 0 || (u32)idx >= mFields.size())
	{
		AosSetError(rdata, "internal_error") << enderr;
		return false;
	}

	if (!mIsDataParsed) if (!parseData(rdata)) return false;

	if (!mFields[idx])
	{
		AosSetError(rdata, "internal_error") << enderr;
		return false;
	}

	AosValuePtr vv = mFields[idx]->getFieldValue(rdata);
	if (!vv)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return false;
	}

	value.setValue(vv->getU64(0));
	return true;
}


void
AosRecordByBuff::clear()
{
	mData = 0;
	mDataLen = 0;
	mDataBuff = 0;

	AosDataRecord::clear();
}


bool 		
AosRecordByBuff::setData(
		char *data,
		const int len,
		const AosBuffDataPtr &metaData,
		const int64_t offset,
		const bool need_copy)
{
	if (!data || len <= 0) return false;
	mIsDataParsed = false;
	mMetaData = metaData;
	mOffset = offset;

	if (!mDataBuff)
	{
		mDataBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	}

	if (need_copy)
	{
		mDataBuff->resize(len);
		mDataBuff->reset();
		mDataBuff->setCharStr(data, len, true);
	}
	else
	{
		mDataBuff->replaceMemory(data, len);
	}
	 	
	mDataLen = len;
	return true;
}


bool
AosRecordByBuff::parseData(const AosRundataPtr &rdata)
{
	// This data record uses a 
	aos_assert_rr(!mIsDataParsed, rdata, false);
	aos_assert_rr(mDataBuff, rdata, false);
	mIsDataParsed = true;
	mDataBuff->setCrtIdx(mStartIdx);

	for (u32 i=0; i < mFields.size(); i++)
	{
		bool rslt = mFields[i]->getValueFromBuff(rdata, mDataBuff);
		aos_assert_r(rslt, false);
	}

	return true;
}	
#endif
