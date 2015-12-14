////////////////////////////////////////////////////////////////////////////
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
// 2013/12/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordIIL.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataField/DataFieldStr.h"
#include "DataField/FieldBinU64.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"


AosRecordIIL::AosRecordIIL(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eIIL, AOSRECORDTYPE_IIL, flag AosMemoryCheckerFileLine),
mRecordLen(0),
mIsDataParsed(false)
{
	mValueField = OmnNew AosDataFieldStr(false);
	mDocidField = OmnNew AosFieldBinU64(false);
}


AosRecordIIL::AosRecordIIL(
		const AosRecordIIL &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mRecordLen(rhs.mRecordLen),
mIsDataParsed(false)
{
	if (rhs.mValueField) mValueField = rhs.mValueField->clone(rdata);
	if (rhs.mDocidField) mDocidField = rhs.mDocidField->clone(rdata);
}


AosRecordIIL::~AosRecordIIL()
{
}


bool
AosRecordIIL::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);

	//mRecordLen = def->getAttrInt(AOSTAG_LENGTH, 0);
	//if (mRecordLen <= 0)
	//{
	//	AosSetErrorUser3(rdata, "invalid_record_len", "recordfixbin_invalid_recordlen")
	//		<< def << enderr;
	//	return false;
	//}

	AosXmlTagPtr fieldsNode = def->getFirstChild("datafields");
	aos_assert_r(fieldsNode, false);
	AosXmlTagPtr fieldNode = fieldsNode->getFirstChild("datafield");
	while (fieldNode)
	{
		AosXmlTagPtr valueNode = fieldNode->getFirstChild(true);
		while (valueNode)
		{
			AosDataFieldObjPtr elem = AosDataFieldObj::createDataFieldStatic(valueNode, this, rdata);
			aos_assert_r(elem, false);
			appendField(rdata, elem);

			valueNode = fieldNode->getNextChild();
		}
		fieldNode = fieldsNode->getNextChild();
	}

	return true;
}


AosDataRecordObjPtr 
AosRecordIIL::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordIIL(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordIIL::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordIIL(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordIIL::create(
		const AosXmlTagPtr &def,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordIIL * record = OmnNew AosRecordIIL(false AosMemoryCheckerFileLine);
	record->setTaskDocid(task_docid);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordIIL::getFieldValue(
		const int idx, 
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(idx >= 0, false);

	if (mFieldValFlags[idx])
	{
		value = mFieldValues[idx];
		return true;
	}

	if (!mIsDataParsed)
	{
		aos_assert_r(mMemory && mMemLen > 0, false);

		mIsDataParsed = true;
		memset(mFieldValFlags, 0, mNumFields); // clear field values.

		int crt_idx = 0;
		int len = *(int *)&mMemory[crt_idx];
		aos_assert_r(len < mMemLen, false);

		crt_idx += sizeof(int);

		// 1. key 
		mFieldsRaw[0]->mFieldInfo.setPosition(crt_idx, len);
		//mFieldsRaw[0]->setFixedFlag(true);

		// 1.1. sub fields parse data
		if (mNumFields > 2)
		{
			parseIILKey(rdata, mMemory, crt_idx, len);
		}

		crt_idx += len;

		// 2. docid 
		len = sizeof(u64);
		mFieldsRaw[1]->mFieldInfo.setPosition(crt_idx, len);
		//mFieldsRaw[1]->setFixedFlag(true);
	}

	int index = 0;
	bool rslt = mFieldsRaw[idx]->getValueFromRecord(
		this, mMemory, mMemLen, index, value, true, rdata);
	aos_assert_r(rslt, false);
	mFieldValues[idx] = value;
	mFieldValFlags[idx] = true;
	return true;
}


void
AosRecordIIL::clear()
{
	AosDataRecord::clear();

	mRecordLen = 0;
	mIsDataParsed = false;
}


bool 		
AosRecordIIL::setData(
		char *data,
		const int len,
		AosMetaData *metaData,
		int &status)
		//const int64_t offset)
{
	//aos_assert_r(data && len > 0 && len >= mRecordLen, false);
	aos_assert_r(data && len > 0, false);
	mMemory = data;
	mMemLen = len;
	if (metaData)
	{
		mMetaData = metaData;
		mMetaDataRaw = metaData;
	}
	//mOffset = offset;
	int record_len = 0;
	determineRecordLen(data,len,record_len,status);
	mRecordLen = record_len;
	return true;
}


bool
AosRecordIIL::determineRecordLen(
		char *data,
		const int64_t &len,
		int &record_len,
		int &status)
{
	// felicia, 2014/11/06
	aos_assert_r(len > 0, false);
	record_len = 0;
	if (len < sizeof(int))
	{
		record_len = -1;
		status = -1;
		return true;
	}

	int key_len = *(int *)data;
	if (sizeof(int) + key_len + sizeof(u64) > len)
	{
		record_len = -1;
		status = -1;
		return true;
	}
	record_len += sizeof(int);
	record_len += *(int *)data;
	record_len += sizeof(u64);
	status = 0;
	return true;
}


bool
AosRecordIIL::parseIILKey(AosRundata *rdata, 
		char *data, int crt_idx, int len)
{
	int init_idx = crt_idx; 
	char endChar = data[crt_idx + len]; 
	data[crt_idx + len] = '\0';
	char sep = 0x01;
	for (size_t field_idx=2; field_idx<mNumFields; field_idx++)
	{
		int field_len = 0;
		char *sep_ptr = strchr(&data[crt_idx], sep); 
		if (sep_ptr)
		{
			field_len = sep_ptr - &data[crt_idx];
		}
		else
		{
			field_len = init_idx + len - crt_idx;
		}

		mFieldsRaw[field_idx]->mFieldInfo.setPosition(crt_idx, field_len);
		//mFieldsRaw[field_idx]->setFixedFlag(true);

		//if (data[crt_idx + field_len + 1] == '\0') break;
		crt_idx += field_len + 1;
	}

	data[init_idx + len] = endChar;

	return true;
}


