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
// This is the AosBuff implementation of data records:
// 			Name			OmnString
// 			Type			1 byte
// 			Contents
// Below are the types supported now.
//		u8
//		u16
//		u32
//		u64
//		char
//		i16
//		i32
//		i64
//		float
//		double
//		string
//		vector
//
// data are saved in network order (big endian)
// 
// Modification History:
// 2013/12/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataRecord/RecordBuff.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataField/DataField.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"




AosRecordBuff::AosRecordBuff()
:
AosDataRecord("AosRecordBuff", "0.0")
{
}


AosRecordBuff::~AosRecordBuff()
{
}


bool
AosRecordBuff::config(const AosXmlTagPtr &def, AosRundata *rdata)
{
	return true;
}


void
AosRecordBuff::clear()
{
	mInternalData.setLength(mRecordLen+10);
	mData = (char *)mInternalData.data();
	memset(mData, 0, mRecordLen);
	mBuffIdx = 0;

	AosDataRecord::clear();
}


AosDataRecordObjPtr 
AosRecordBuff::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordBuff(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordBuff::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordBuff(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


bool		
AosRecordBuff::getFieldValue(
		const int idx, 
		AosValueRslt &value, 
		AosRundata *rdata) 
{
	aos_assert_r(idx >= 0 && (u32)idx < mFieldVec.size(), false);

	if (!mIsDataParsed)
	{
		bool rslt = parseData(rdata);
		aos_assert_r(rslt, false);
	}

	Entry &entry = mFieldVec[idx];
	switch (entry.type)
	{
	case AosDataType::eU8:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setU8(mBuffRaw->getU8(0));
		 return true;

	case AosDataType::eU16:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setU16(mBuffRaw->getU16(0));
		 return true;

	case AosDataType::eU32:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setU32(mBuffRaw->getU32(0));
		 return true;

	case AosDataType::eU64:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setU64(mBuffRaw->getU64(0));
		 return true;

	case AosDataType::eChar:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setChar(mBuffRaw->getCHar(0));
		 return true;

	case AosDataType::eInt16:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setInt16(mBuffRaw->getI16(0));
		 return true;

	case AosDataType::eInt32:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setInt32(mBuffRaw->getInt32(0));
		 return true;

	case AosDataType::eInt64:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setInt64(mBuffRaw->getI64(0));
		 return true;

	case AosDataType::eIntFloat:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setFloat(mBuffRaw->getFloat(0));
		 return true;

	case AosDataType::eIntDouble:
		 mBuffRaw->setCrtIdx(entry.pos);
		 value.setDouble(mBuffRaw->getDouble(0));
		 return true;

	case AosDataType::eIntDouble:
		 mBuffRaw->setCrtIdx(entry.pos);
		 data = mBuffRaw->getCharStr(len);
		 value.setCharStr(data, len);
		 return true;

	default:
		 OmnAlarm << "Unrecognized type: " << type << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


void
AosRecordBuff::clear()
{
	mInternalData = "";
	mData = (char *)mInternalData.data();
	mDataLen = 0;

	AosDataRecord::clear();
	mFieldVec.clear();
	mFieldMap.clear();
}


bool 		
AosRecordBuff::setData(
		char *data,
		const int len,
		AosBuffData *metaData,
		const int64_t offset)
{
	if (!data || len <= 0) return false;
	mIsDataParsed = false;
	mMetaData = metaData;
	mMetaDataRaw = metaData;
	mOffset = offset;

	mData = data;
	mDataLen = len;
	mBuff.setData(mData, mDataLen);
	return true;
}


bool
AosRecordBuff::parseData(AosRundata *rdata)
{
	// Data are saved as:
	// 		DocType			(1 byte)
	// 		Length			(variable)
	// 		FieldName		OmnString
	// 		FieldType		1 byte
	// 		FieldContents
	// 		...
	// 		FieldName		OmnString
	// 		FieldType		1 byte
	// 		FieldContents
	// Only the fields that are defined in mFieldDefs are accepted.
	aos_assert_r(mBuffRaw, false);
	mBuffRaw->reset();
	char *name;
	int len;
	AosDataType::E type;
	mFieldVec.clear();
	int field_idx = 0;
	mFieldValues.clear();
	mFieldNames.clear();
	while (mBuffRaw->hasMore())
	{
		name = mBuffRaw->getCharStr(len);
		if (!name) return true;
		itr = mFieldDef.find(name);
		if (itr == mFieldDef.end())
		{
			field_idx = -1;
		}
		else
		{
			field_idx = itr->second;
		}
		type = (AosDataType::E)mBuffRaw->getU8(0);
		i32 crt_pos = mBuffRaw->getCrtIdx();
		switch (type)
		{
		case AosDataType::eU8:
			 mBuffRaw->getU8(0);
			 if (field_idx >= 0) 
			 {
				 mFieldPos[field_idx] = crt_pos;
			 }
			 break;

		case AosDataType::eU16:
			 mBuffRaw->getU16(0);
			 mFieldVec.push_back(Entry(crt_pos, type));
			 mFieldMap[name] = field_idx++;
			 break;

		case AosDataType::eU32:
			 mBuffRaw->getU32(0);
			 mFieldVec.push_back(Entry(crt_pos, type));
			 mFieldMap[name] = field_idx++;
			 break;

		case AosDataType::eU64:
			 mBuffRaw->getU64(0);
			 mFieldVec.push_back(Entry(crt_pos, type));
			 mFieldMap[name] = field_idx++;
			 break;

		case AosDataType::eChar:
			 mBuffRaw->getChar(0);
			 mFieldVec.push_back(Entry(crt_pos, type));
			 mFieldMap[name] = field_idx++;
			 break;

		case AosDataType::eInt16:
			 mBuffRaw->getI16(0);
			 mFieldVec.push_back(Entry(crt_pos, type));
			 mFieldMap[name] = field_idx++;
			 break;

		case AosDataType::eInt32:
			 mBuffRaw->getInt32(0);
			 mFieldVec.push_back(Entry(crt_pos, type));
			 mFieldMap[name] = field_idx++;
			 break;

		case AosDataType::eInt64:
			 mBuffRaw->getI64(0);
			 mFieldVec.push_back(Entry(crt_pos, type));
			 mFieldMap[name] = field_idx++;
			 break;

		case AosDataType::eFloat:
			 mBuffRaw->getFloat(0);
			 mFieldVec.push_back(Entry(crt_pos, type));
			 mFieldMap[name] = field_idx++;
			 break;

		case AosDataType::eDouble:
			 mBuffRaw->getDouble(0);
			 mFieldVec.push_back(Entry(crt_pos, type));
			 mFieldMap[name] = field_idx++;
			 break;

		case AosDataType::eCharStr:
			 data = mBuffRaw->get(len);
			 mFieldVec.push_back(Entry(crt_pos, type, len));
			 mFieldMap[name] = field_idx++;
			 break;

		default:
			 OmnAlarm << "Unrecognized type: " << type << enderr;
			 return false;
		}
	}
	return true;
}


bool 		
AosRecordBuff::setFieldValue( 
		const int idx, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	outofmem = false;
	aos_assert_r(idx >= 0 && (u32)idx < mFieldVec.size(), false);
	AosDataRecord::setFieldValue(idx, value, outofmem, rdata);	// Ketty 2014/01/17 
	Entry &entry = mFieldVec[idx];
	if (entry.pos == -1)
	{
		mFieldValues[entry.field_idx] = value;
		return true;
	}

	aos_assert_r(entry.pos > 0, false);
	entry.pos = -1;
	return true;
}


bool 		
AosRecordBuff::setFieldValue( 
		const OmnString &field_name, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	outofmem = false;
	mapitr_t itr = mFieldMap.find(field_name.data());
	if (itr == mFieldMap.end())
	{
		// Field does not exist
		mFieldNames.push_back(field_name);
		mFieldValues.push_back(value);
		mFieldVec.push_back(Entry(mFieldValues.size()-1));
		mFieldMap[mFieldNames[mFieldNames.size()-1].data()] = mFieldValues.size()-1;
		return true;
	}
	
	Entry &entry = mFieldVec[itr->second];
	if (entry.pos == -1)
	{
		mFieldValues[entry.field_idx] = value;
		return true;
	}
	
	aos_assert_r(entry.pos >= 0, false);
	entry.pos = -1;
	entry.data_type = AosDataType::eInvalid;
	mFieldNames.push_back(field_name);
	mFieldValues.push_back(value);
	mFieldVec.push_back(Entry(mFieldValues.size()-1));
	mFieldMap[mFieldNames[mFieldNames.size()-1].data()] = mFieldValues.size()-1;
	return true;
}


char *
AosRecordBuff::getData(AosRundata *rdata)
{
	// It packs all the fields in mFieldVec.
	char *data = mData;
	mBuff.reset();
	for (u32 i=0; i<mFieldVec.size(); i++)
	{
		Entry &entry = mFieldVec[i];
		if (entry.pos >= 0)
		{

		}
		switch (entry.data_type)
		{
		case AosDataType::eU8:
			 mBuff.setU8((u8)data[entry.pos]);
			 break;
			 
		case AosDataType::eU16:
			 mBuff.setU16(hton(*(u16*)&data[entry.pos]));
			 break;
			 
		case AosDataType::eU32:
			 mBuff.setU32(hton(*(u32*)&data[entry.pos]));
			 break;
			 
		case AosDataType::eU64:
			 mBuff.setU64(hton(*(u64*)&data[entry.pos]));
			 break;
			 
		case AosDataType::eChar:
			 mBuff.setChar(data[entry.pos]);
			 break;
			 
		case AosDataType::eInt16:
			 mBuff.setInt16(hton(*(int16_t*)&data[entry.pos]));
			 break;
			 
		case AosDataType::eInt32:
			 mBuff.setInt32(hton(*(int32_t*)&data[entry.pos]));
			 break;
			 
		case AosDataType::eInt64:
			 mBuff.setInt64(hton(*(int64_t*)&data[entry.pos]));
			 break;
			 
		case AosDataType::eFloat:
			 mBuff.setFloat(hton(*(float*)&data[entry.pos]));
			 break;
			 
		case AosDataType::eDouble:
			 mBuff.setDouble(hton(*(double*)&data[entry.pos]));
			 break;
			 
		case AosDataType::eCharStr:
			 mBuff.setCharStr(&data[entry.pos], entry.len);
			 break;
			 
		default:
			 if (entry.pos == -1)
			 {
				 // The value is encoded in mFieldValues.
				 aos_assert_r(entry.field_idx >= 0 && (u32)entry.field_idx < mFieldValues.zize(), false);
				 mFieldValues.serializeToBuff(mBuff);
			 }
			 else
			 {
			 	OmnAlarm << "Unrecognized data type: " << entry.data_type << enderr;
			 	return 0;
			 }
		}
	}

	return mBuff.data();
}
#endif
