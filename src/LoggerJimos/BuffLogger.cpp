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
// This class serves as the super class for all buff based records.
//
// Modification History:
// 2013/12/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LoggerJimos/BuffLogger.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataField/DataField.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"

static OmnMutex sgLock;

AosBuffLogger::AosBuffLogger(
		const OmnString &name, 
		const OmnString &version)
:
AosDataRecord(name, version)
{
}


AosBuffLogger::~AosBuffLogger()
{
}


void
AosBuffLogger::clear()
{
	mInternalData.setLength(mRecordLen+10);
	mData = (char *)mInternalData.data();
	memset(mData, 0, mRecordLen);
	mBuffIdx = 0;

	AosDataRecord::clear();
}


AosDataRecordObjPtr 
AosBuffLogger::create(
	 const AosXmlTagPtr &def,
	 const AosTaskObjPtr &task,
	 const AosRundataPtr &rdata AosMemoryCheckDecl) const
{
	try
	{
		AosJimoPtr jimo = cloneJimo();
		if (jimo->getJimoType() != AOS_JIMOTYPE_DATA_RECORD)
		{
			AosSetEntityError(rdata, "bufflogger_invalid_datarecord",
					"Logger", mName) << def << enderr;
			return 0;
		}

		AosDataRecord *record = dynamic_cast<AosDataRecord*>(jimo.getPtr());
		if (!record)
		{
			AosSetEntityError(rdata, "internal_error",
					"Logger", mName) << def << enderr;
			return 0;
		}

		record->setTask(task);
		if (!record->config(def, rdata)) return 0;
		return record;
	}

	catch (...)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}
}


bool 		
AosBuffLogger::setData(
		char *data, 
		const int len, 
		const AosBuffDataPtr &metaData, 
		const int64_t offset,
		const bool need_copy)
{
	aos_assert_r(data && len > 0, false);
	mIsDataParsed = false;
	mMetaData = metaData;
	mOffset = offset;

	if (need_copy)
	{
		mInternalData.assign(data, len);
		mData = (char *)mInternalData.data();
		mDataLen = len;
		return true;
	}
	else
	{
		mData = data;
		mDataLen = len;
	}

	return true;
}


bool		
AosBuffLogger::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	if (!mIsDataParsed) 
	{
		if (!parseData(rdata)) return false;
	}

	aos_assert_rr(idx >= 0 && (u32)idx < mFields.size(), rdata, false);
	value = mValues[idx];
	return true;
}


AosDataRecordObjPtr 
AosBuffLogger::clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const
{
	AosJimoPtr jimo = cloneJimo();
	aos_assert_rr(jimo->getJimoType() == AOS_JIMOTYPE_DATA_RECORD, rdata, 0);
	return dynamic_cast<AosDataRecordObj*>(jimo.getPtr());
}

