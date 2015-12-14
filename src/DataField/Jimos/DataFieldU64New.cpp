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
// Chen Ding, 2014/08/16
// Null Values
// -----------
//
// Modification History:
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataField/Jimos/DataFieldU64New.h"

#include "API/AosApi.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataFieldU64New_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataFieldU64New(version);
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



AosDataFieldU64New::AosDataFieldU64New(const int version)
:
AosDataField(version),
mParsed(false),
mValue(0)
{
}


AosDataFieldU64New::AosDataFieldU64New(const AosDataFieldU64New &rhs)
:
AosJimoDataField(rhs),
mParsed(rhs.mParsed),
mValue(rhs.mValue)
{
}


AosDataFieldU64New::~AosDataFieldU64New()
{
}


bool
AosDataFieldU64New::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	aos_assert_r(mValueFromFieldIdx < 0, false);

	if (mIsConst) 
	{
		mValue = AosStr2U64(mConstValueStr); 
	}

	return true;
}


bool
AosDataFieldU64New::serializeToXmlDoc(
		OmnString &docstr,
		int &idx,
		const char *data, 
		const int datalen,
		AosRundata* rdata)
{
	// It converts the data from 'data' to 'docstr'. The value in 'data'
	// starts at 'idx', and its length is 'mFieldInfo.field_data_len'. 
	// If needed, it will do the data conversion (not supported yet)
	// from the original form to the target form.
	if (mIgnoreSerialize) return true;

	// Chen Ding, 2014/08/16
	if (mIsNull) return true;
	
	aos_assert_rr(mName != "", rdata, false);
	aos_assert_rr(mParsed, rdata, false);

	docstr << "<" << mName << "><![CDATA[" << mValue << "]]></" << mName << ">";
	return true;
}


bool
AosDataFieldU64New::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataFieldU64New::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	// Chen Ding, 2014/08/16
	if (mIsNull)
	{
		value.setNullValue();
		return true;
	}

	mParsed = true;
	if (mIsConst)
	{
		value = mValue;
		return true;
	}

	bool rslt;
	switch (mSourceDataType)
	{
	case AosDataType::eString:
		 // The data is stored as strings.
		 if (mFieldInfo.field_data_len == 0)
		 {
			 mIsNull = true;
			 value.setNullValue();
			 return true;
		 }

		 rslt = AosStr2U64(&data[mFieldInfo.field_offset], mFieldInfo.field_data_len, true, value);
		 aos_assert_rr(rslt, rdata, false);
		 idx += mFieldInfo.field_data_len;
		 return true;

	case AosDataType::eU64:
		 // The data is stored as binary u64. 
		 aos_assert_rr(mFieldInfo.field_data_len == sizeof(u64), rdata, false);
		 value.setValue(*(u64 *)&data[mFieldInfo.field_offset]);
		 idx += sizeof(u64);
		 return true;

	// case AosDataType::eVarInt:
	// 	 OmnNotImplementedYet;
	// 	 return false;

	default:
		 AosLogError1(rdata, true, "data_type_not_supported") 
			 << "Source Type: " << mSourceDataType << enderr;
		 mParsed = false;
		 return false;
	}

	return true;
}


bool
AosDataFieldU64New::setValueToRecord(
		AosDataRecordObj* record,
		const AosValueRslt &value,
		bool &outofmem,
		AosRundata* rdata)
{
	// This function shall be called for records that already know the
	// field's offset, such as fixed length records. CSV records shall 
	// not call this function. 
	outofmem = false;

	// Chen Ding, 2014/08/16
	mIsNull = true;
	if (value.isNull())
	{
		return true;
	}

	aos_assert_r(record, false);
	aos_assert_r(mFieldInfo.field_offset >= 0, false);

	int record_len = record->getMemoryLen();
	if (record_len < mFieldInfo.field_offset + mFieldInfo.field_data_len)
	{
		outofmem = true;
		return true;
	}

	mIsNull = false;
	outofmem = true;
	OmnString ss;
	char *record_data = (char *)record->getData(rdata);

	switch (mSourceDataType)
	{
	case AosDataType::eString:
		 // Values are stored as strings. 
		 ss << mValue;
		 aos_assert_rr(ss.length() <= mFieldInfo.field_data_len, rdata, false);
		 memcpy(&record_data[mFieldInfo.field_offset], ss.data(), ss.length());
		 return true;

	case AosDataType::eU64:
		 aos_assert_rr(mFieldInfo.field_data_len == sizeof(u64), rdata, false);
		 *(u64*)&record_data[mFieldInfo.field_offset] = mValue;
		 return true;

	// case AosDataType::eVarInt:
	// 	 OmnNotImplementedYet;
	// 	 return false;

	default:
		 AosSetErrorUser(rdata, "data_type_not_supported") << mSourceDataType << enderr;
		 return false;
	}

}


AosDataFieldObjPtr 
AosDataFieldU64New::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldU64New(*this);
}


AosDataFieldObjPtr
AosDataFieldU64New::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldU64New * field = OmnNew AosDataFieldU64New(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}
#endif

