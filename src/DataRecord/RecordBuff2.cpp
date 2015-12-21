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
// 
// Modification History:
// 2013/12/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordBuff2.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataField/DataField.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"

#include "DataField/DataFieldBitmap.h"

AosRecordBuff2::AosRecordBuff2(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eBuff, AOSRECORDTYPE_BUFF, flag AosMemoryCheckerFileLine),
mRecordLen(0),
mIsDataParsed(false),
mCrtFieldIdx(0),
mCrtFieldOffset(0)
{

}


AosRecordBuff2::AosRecordBuff2(
		const AosRecordBuff2 &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mRecordLen(rhs.mRecordLen),
mIsDataParsed(false),
mCrtFieldIdx(0),
mCrtFieldOffset(0)
{
}


AosRecordBuff2::~AosRecordBuff2()
{
}


bool
AosRecordBuff2::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)

{
	aos_assert_r(def, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);


	return true;
}


void
AosRecordBuff2::clear()
{
	AosDataRecord::clear();

	mRecordLen = 0;
	mIsDataParsed = false;
	mCrtFieldIdx = 0;
	mCrtFieldOffset = 0;
}


AosDataRecordObjPtr 
AosRecordBuff2::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordBuff2(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordBuff2::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordBuff2(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordBuff2::create(
		const AosXmlTagPtr &def,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordBuff2 * record = OmnNew AosRecordBuff2(false AosMemoryCheckerFileLine);
	record->setTaskDocid(task_docid);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}

void
AosRecordBuff2::flush(const bool clean_memory)
{
	if(mIsDirty)
		aos_assert(mCrtFieldIdx == (int)mNumFields); 
}


bool		
AosRecordBuff2::getFieldValue(
		const int idx, 
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(idx >= 0 && (u32)idx < mNumFields, false);

	bool rslt = true;
	if (!mIsDataParsed)
	{
		int record_len = 0;
		int status = -2;

		rslt = parseData(mMemory, mMemLen, record_len, status);

		aos_assert_r(rslt, false);
	}

	if (mFieldValFlags[idx])
	{
		value = mFieldValues[idx];
		return true;
	}
		
	int crt_idx = 0;
	rslt = mFieldsRaw[idx]->getValueFromRecord(
		this, mMemory, mMemLen, crt_idx, value, copy_flag, rdata);
	aos_assert_r(rslt, false);
	mFieldValues[idx] = value;
	mFieldValFlags[idx] = true;
	return true;
}
// jimodb-1301
bool
AosRecordBuff2::setData(
		char *data,
		const int len,
		AosMetaData *metaData,
		int &status)
		//const int64_t offset)
{
	int record_len = 0;
	//bool rslt = parseData(data, len, record_len, status);
	//aos_assert_r(rslt, false);

	aos_assert_r(data && len > 0, false);
	mMemory = data;
	mMemLen = len;
	if(metaData)
	{
		mMetaData = metaData;
		mMetaDataRaw = metaData;
	}
	//mOffset = offset;

	mRecordLen = record_len;
	mIsDataParsed = false;

	return true;
}
#if 0
bool 		
AosRecordBuff2::setData(
		char *data,
		const int len,
		AosMetaData *metaData,
		const int64_t offset)
{
	aos_assert_r(data && len > 0 && len >= mRecordLen, false);
	mMemory = data;
	mMemLen = len;
	mRecordLen = len;
	if (metaData)
	{
		mMetaData = metaData;
		mMetaDataRaw = metaData;
	}
	mOffset = offset;

	mIsDataParsed = false;
	mCrtFieldIdx = 0;
	mCrtFieldOffset = 0;
	
	return true;
}
#endif

bool 		
AosRecordBuff2::determineRecordLen(
		char *data,
		const int64_t &len,
		int &record_len,
		int &status)
{
	//aos_assert_r(mMemory && mMemLen > 0, false);
	aos_assert_r(len >= 0, false);
	mIsDataParsed = true;
	memset(mFieldValFlags, 0, mNumFields); // clear field values.
	int crt_pos = 0;
	int str_len = 0;
	AosDataFieldType::E type;

	//record_len = *(int *)(data-sizeof(int));
	//bool rslt = AosBuff::decodeRecordBuffLength(record_len);

	for (u32 i=0; i < mNumFields; i++)
	{
		if (mFieldsRaw[i]->isVirtualField())
			continue;
		type = mFieldsRaw[i]->getType();
		if (type == AosDataFieldType::eStr || type == AosDataFieldType::eBuff)
		{
			str_len = *(u32*)(data + crt_pos);
			mFieldsRaw[i]->mFieldInfo.field_data_len = str_len;
			mFieldsRaw[i]->mFieldInfo.field_len = str_len;
			crt_pos += sizeof(int) + sizeof(i8);
			mFieldsRaw[i]->mFieldInfo.field_offset = crt_pos;
			crt_pos += str_len;

			record_len += str_len + sizeof(i8);
			record_len += sizeof(int);
		}
		else
		{
			aos_assert_r(mFieldsRaw[i]->mFieldInfo.field_len > 0, false);
			mFieldsRaw[i]->mFieldInfo.field_offset = crt_pos + sizeof(i8);
			crt_pos += mFieldsRaw[i]->mFieldInfo.field_len + sizeof(i8);

			record_len += mFieldsRaw[i]->mFieldInfo.field_len + sizeof(i8);
		}
	}
	status = 0;
	return true;
#if 0
	// buff = len(u32) + buffContents
	aos_assert_r(len > 0, false);
	record_len = 0;
	AosDataFieldType::E type;
	for (u32 i=0; i<mNumFields; i++)
	{
		type = mFieldsRaw[i]->getType();
		if (type == AosDataFieldType::eStr || type == AosDataFieldType::eBuff)
		{
			 record_len += *(int*)(data+record_len);
			 record_len += sizeof(int);
		}
		else
		{
			aos_assert_r(mFieldsRaw[i]->mFieldInfo.field_len > 0, false);
			record_len += mFieldsRaw[i]->mFieldInfo.field_len;
		}
		if (record_len > len)
		{
			record_len = -1;
			break;
		}
	}
	return true;
#endif
}


bool
AosRecordBuff2::parseData(		
		char *data,
		const int64_t &len,
		int &record_len,
		int &status)
{
	aos_assert_r(!mIsDataParsed, false);
	aos_assert_r(data && len > 0, false);

	mIsDataParsed = true;
	memset(mFieldValFlags, 0, mNumFields); // clear field values.
	int crt_pos = 0;
	int str_len = 0;
	record_len = 0;
	AosDataFieldType::E type;

	//int recordLen = *(int *)(data-sizeof(int));
	//bool rslt = AosBuff::decodeRecordBuffLength(recordLen);
	

	for (u32 i=0; i < mNumFields; i++)
	{

		if (mFieldsRaw[i]->isVirtualField())
			continue;
aos_assert_r(crt_pos < len, false);
		type = mFieldsRaw[i]->getType();
		mFieldsRaw[i]->setNotNull();

		if (*(i8*)(data+crt_pos) == 1)
		{
			mFieldsRaw[i]->setNull();
			crt_pos += sizeof(i8);
			if (type == AosDataFieldType::eStr || type == AosDataFieldType::eBuff)
			{
				mFieldsRaw[i]->mFieldInfo.field_data_len = 0;
				crt_pos += sizeof(int);

				record_len += sizeof(i8);
				record_len += sizeof(int);
			}
			else
			{
				crt_pos += mFieldsRaw[i]->mFieldInfo.field_data_len;
				record_len +=  sizeof(i8) + mFieldsRaw[i]->mFieldInfo.field_data_len;
			}
			continue;
		}
		crt_pos += sizeof(i8);
		if (type == AosDataFieldType::eStr || type == AosDataFieldType::eBuff)
		{
			str_len = *(u32*)(data + crt_pos);
			mFieldsRaw[i]->mFieldInfo.field_data_len = str_len;
			mFieldsRaw[i]->mFieldInfo.field_len = str_len;
			crt_pos += sizeof(int);
			mFieldsRaw[i]->mFieldInfo.field_offset = crt_pos;
			crt_pos += str_len;

			record_len += str_len + sizeof(i8);
			record_len += sizeof(int);
		}
		else
		{
			aos_assert_r(mFieldsRaw[i]->mFieldInfo.field_len > 0, false);
			mFieldsRaw[i]->mFieldInfo.field_offset = crt_pos;
			crt_pos += mFieldsRaw[i]->mFieldInfo.field_len;

			record_len += mFieldsRaw[i]->mFieldInfo.field_len + sizeof(i8);
		}

		//if ((i < mNumFields-1 && record_len == len) || record_len > len)
		if (record_len > len)
		{
			record_len = -1;
			status = -1;
			return true;
		}
	}
	status = 0;
	return true;
}



bool		
AosRecordBuff2::setFieldValue(
		const int idx, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	aos_assert_r(idx >= 0 && (u32)idx < mNumFields, false);
	AosDataRecord::setFieldValue(idx, value, outofmem, rdata);

	aos_assert_r(mCrtFieldIdx == idx, false);
	aos_assert_r(mCrtFieldOffset >= 0, false);
	int offset = mCrtFieldOffset;

	if (mCrtFieldOffset + 10 > mMemLen)
	{
		outofmem = true;
		return true;

	}
	AosDataFieldType::E type = mFieldsRaw[mCrtFieldIdx]->getType();
	if(value.isNull())
	{
		*(i8*)(mMemory+offset) = 1;
		offset += sizeof(i8);
		if (type == AosDataFieldType::eStr)
		{
			*(int*)(mMemory + offset) = 0;
			offset += sizeof(int);
			mFieldsRaw[mCrtFieldIdx]->mFieldInfo.field_offset = offset;
			mFieldsRaw[mCrtFieldIdx]->mFieldInfo.field_data_len = 0;
		}
		else
		{
			mFieldsRaw[mCrtFieldIdx]->mFieldInfo.field_offset = offset;
			offset += mFieldsRaw[idx]->mFieldInfo.field_data_len;
		}
		mRecordLen = offset;
		mCrtFieldOffset = offset;
		mCrtFieldIdx++;
		return true;
	}
	else
	{
		*(i8*)(mMemory + offset) = 0;
		offset += sizeof(i8);
	}

	if (type == AosDataFieldType::eBuff || type == AosDataFieldType::eStr)
	{
		OmnString vv = value.getStr();
		int str_len = vv.length();
		*(u32*)(mMemory + offset) = str_len;
		mFieldsRaw[mCrtFieldIdx]->mFieldInfo.field_data_len = str_len;
		//mFieldsRaw[mCrtFieldIdx]->mFieldInfo.field_len = str_len;
		offset += sizeof(int);
	}

	mFieldsRaw[mCrtFieldIdx]->mFieldInfo.field_offset = offset;
	mFieldsRaw[mCrtFieldIdx]->setValueToRecord(mMemory, mMemLen, value, outofmem, rdata);
	if(outofmem) return true;
	mCrtFieldOffset = offset;
	mCrtFieldOffset += mFieldsRaw[idx]->mFieldInfo.field_data_len;
	mRecordLen = mCrtFieldOffset;
	mCrtFieldIdx++;
	return true;
}


// Chen Ding, 2015/01/24
bool 
AosRecordBuff2::appendField(
		AosRundata *rdata, 
		const OmnString &fname,
		const AosDataType::E type)
{
	map<OmnString, int>::iterator itr = mFieldIdxs.find(fname);
	if (itr != mFieldIdxs.end())
	{
		AosLogUserError(rdata, "field_already_exist") 
			<< "Name: " << fname << enderr;
		return false;
	}

	AosDataFieldObjPtr field_creator = AosDataFieldObj::getCreator();
	aos_assert_rr(field_creator, rdata, false);
	AosDataFieldObjPtr field = field_creator->createDataField(rdata, type);
	aos_assert_rr(field, rdata, false);
	field->setName(fname);
	mFieldIdxs[fname] = mNumFields;
	mFields.push_back(field);
	mNumFields++;

	OmnDelete [] mFieldsRaw;
	mFieldsRaw = OmnNew AosDataFieldObj*[mNumFields];
	for (u32 i = 0; i < mNumFields; i++)
	{
		mFieldsRaw[i] = mFields[i].getPtr();
	}

	mFieldValues = OmnNew AosValueRslt[mNumFields];
	mFieldValFlags = OmnNew bool[mNumFields];
	memset(mFieldValFlags, 0, mNumFields); 
	return true;
}

