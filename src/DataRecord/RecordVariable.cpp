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
// This class simulates the records of the following format:
// 		ddd$ddd$ddd
// where '$' is a substring that serves as the separator that separates
// fields in the record. The separator is configurable. 
//
// When data are set to this record, it separates the fields into mFields.
// There shall be an AosDataElem for each field. The AosDataElem is responsible
// for how to interpret the field.
//
// Modification History:
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordVariable.h"

#if 0

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"


AosRecordVariable::AosRecordVariable(const bool flag)
:
AosDataRecord(AosDataRecordType::eVariable, 
	AOSRECORDTYPE_VARIABLE, flag),
mEstimateRecordLen(0),
mIsDataParsed(false)
{
}


AosRecordVariable::AosRecordVariable(
		const AosRecordVariable &rhs,
		AosRundata *rdata)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mEstimateRecordLen(rhs.mEstimateRecordLen),
mRowDelimiter(rhs.mRowDelimiter),
mFieldDelimiter(rhs.mFieldDelimiter),
mTextQualifier(rhs.mTextQualifier),
mIsDataParsed(false)
{
}


AosRecordVariable::~AosRecordVariable()
{
}


bool
AosRecordVariable::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);
	
	mRowDelimiter = AosParseRowDelimiter(def->getAttrStr(AOSTAG_ROW_DELIMITER));
	aos_assert_r(mRowDelimiter != "", false);

	mFieldDelimiter = AosParseFieldDelimiter(def->getAttrStr(AOSTAG_FIELD_DELIMITER));
	aos_assert_r(mFieldDelimiter != "", false);

	mTextQualifier = AosParseTextQualifier(def->getAttrStr(AOSTAG_TEXT_QUALIFIER));

	if (mFields.size() <= 0)
	{
		AosSetEntityError(rdata, "recordcsv_missing_field", 
			"Record", mName) << def->toString() << enderr;
		return false;
	}

	mEstimateRecordLen = 0;
	for (u32 i=0; i<mFields.size(); i++)
	{
		if (!mFields[i]->needValueFromField() && !mFields[i]->isConst())
		{
			mEstimateRecordLen += eEstimateEachFieldLen;
		}
	}

	return true;
}


AosDataRecordObjPtr 
AosRecordVariable::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordVariable(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordVariable::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordVariable(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordVariable::create(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordVariable * record = OmnNew AosRecordVariable(false AosMemoryCheckerFileLine);
	record->setTask(task);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordVariable::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);

	bool rslt = true;
	if (!mIsDataParsed)
	{
		rslt = parseData(rdata);
		aos_assert_r(rslt, false);
	}

	if (mFieldValues.find(idx) != mFieldValues.end())
	{
		value = mFieldValues[idx];
		return true;
	}

	OmnAlarm << "error" << enderr;
	return false;
}


void
AosRecordVariable::clear()
{
	AosDataRecord::clear();
}


bool
AosRecordVariable::setData(
		char *data,
		const int len,
		AosBuffData *metaData,
		const int64_t offset)
{
	aos_assert_r(data && len > 0, false);
	mMemory = data;
	mMemLen = len;
	mMetaData = metaData;
	mOffset = offset;

	mIsDataParsed = false;

	return true;
}


bool
AosRecordVariable::parseData(AosRundata* rdata)
{
	aos_assert_r(!mIsDataParsed, false);
	aos_assert_r(mMemory && mMemLen > 0, false);

	mIsDataParsed = true;
	mFieldValues.clear();
	
	bool rslt = true;
	int idx = 0;
	for (u32 i=0; i < mFields.size(); i++)
	{
		AosValueRslt value; 
		rslt = mFields[i]->getValueFromRecord(
			this, mMemory, mMemLen, idx, value, true, rdata);
		aos_assert_r(rslt, false);
		
		if (idx < mMemLen && !mFields[i]->needValueFromField() && !mFields[i]->isConst())
		{
			aos_assert_r(strncmp(&mMemory[idx], mFieldDelimiter.data(), mFieldDelimiter.length()) == 0, false);
			idx += mFieldDelimiter.length(); 
		}

		mFieldValues[i] = value;
	}

	return true;
}

#endif

