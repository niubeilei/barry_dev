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
// 2014/08/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataRecord/RecordBSON.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"


AosRecordBSON::AosRecordBSON(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eBSON,
	AOSRECORDTYPE_BSON, flag AosMemoryCheckerFileLine),
mRecordLen(0),
mIsDataParsed(false)
{
}


AosRecordBSON::AosRecordBSON(
		const AosRecordBSON &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mRecordLen(rhs.mRecordLen),
mIsDataParsed(false)
{
}


AosRecordBSON::~AosRecordBSON()
{
}


bool
AosRecordBSON::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


AosDataRecordObjPtr 
AosRecordBSON::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordBSON(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordBSON::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordBSON(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordBSON::create(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordBSON * record = OmnNew AosRecordBSON(false);
	record->setTask(task);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordBSON::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata)
{
	u64 ss1 = OmnGetTimestamp();
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);
	aos_assert_r(mFieldVals, false);

	if (mFieldValFlags[idx] && !mNeedEscapeQualifier[idx])
	{
		value = mFieldVals[idx];
		gRecordTime1 += OmnGetTimestamp() - ss1;
		return true;
	}
	
	u64 ss2 = OmnGetTimestamp();
	gRecordTime1 += ss2 - ss1;
	ss1 = ss2;

	bool rslt = true;
	int index = 0;
	if (mFieldsRaw[idx]->isConst())
	{
		rslt = mFieldsRaw[idx]->getValueFromRecord(
			this, mMemory, mMemLen, index, value, copy_flag, rdata);
		aos_assert_r(rslt, false);

		mFieldValFlags[idx] = true;
		mFieldVals[idx] = value;
		gRecordTime2 += OmnGetTimestamp() - ss1;
		return true;
	}
	ss2 = OmnGetTimestamp();
	gRecordTime2 += ss2-ss1;
	ss1 = ss2;

	if (!mIsDataParsed)
	{
		rslt = parseData(rdata);
		aos_assert_r(rslt, false);
	}
	ss2 = OmnGetTimestamp();
	gRecordTime3 += ss2-ss1;
	ss1 = ss2;

	if (mFieldValFlags[idx])
	{
		if (mNeedEscapeQualifier[idx])
		{
			OmnString str = mFieldVals[idx].getValueStr1();
			aos_assert_r(str.length() > 2, false);

			char c = str[0];
			OmnString str_temp(&str.data()[1], str.length() - 2);
			OmnString new_str(&c, 1);
			str.setLength(2);
			str.getBuffer()[0] = mBackslashEscape ? '\\' : c; 
			str.getBuffer()[1] = c;
			str_temp.replace(str, new_str, true);
			mFieldVals[idx].setValue(str_temp);
			mNeedEscapeQualifier[idx] = false;
		}
		
		value = mFieldVals[idx];
		gRecordTime4 += OmnGetTimestamp() - ss1;
		return true;
	}
	ss2 = OmnGetTimestamp();
	gRecordTime4 += ss2-ss1;
	ss1 = ss2;

	index = 0;
	rslt = mFieldsRaw[idx]->getValueFromRecord(
		this, mMemory, mMemLen, index, value, true, rdata);
	aos_assert_r(rslt, false);

	mFieldValFlags[idx] = true;
	mFieldVals[idx] = value;
	ss2 = OmnGetTimestamp();
	gRecordTime5 += ss2-ss1;
	return true;
}


bool		
AosRecordBSON::setFieldValue(
		const int idx, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	outofmem = false;
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);
	aos_assert_r(mMemory && mMemLen > 0, false);

	AosDataRecord::setFieldValue(idx, value, outofmem, rdata);

	mFieldValFlags[idx] = true;
	mFieldVals[idx] = value;
	mComposeFinished = false;
	return true;
}


bool
AosRecordBSON::composeData()
{
	if (mComposeFinished) return true;

	OmnString rslt_str, str;
	u32 num_fields = mFields.size();
	u32 idx = moveToNextField(0);
	while (idx < num_fields)
	{
		if (rslt_str != "")
		{
			rslt_str << mFieldDelimiter;
		}

		str = "";
		if (mFieldValFlags[idx])
		{
			str = mFieldVals[idx].getValueStr1();
			str.normalizeWhiteSpace(true, true);  // trim
		}

		if (mTextQualifier)
		{
			rslt_str << mTextQualifier;
			if (str != "" && str.indexOf(0, mTextQualifier) != -1)
			{
				OmnString pattern(&mTextQualifier, 1);
				OmnString new_str(2, mTextQualifier, true);
				if (mBackslashEscape) new_str.getBuffer()[0] = '\\';
				str.replace(pattern, new_str, true);
			}
			rslt_str << str << mTextQualifier;
		}
		else
		{
			rslt_str << str;
		}

		idx++;
		idx = moveToNextField(idx);
	}
	rslt_str << mRowDelimiter;
	mRecordLen = rslt_str.length();
	mComposeFinished = true;

	if (mMemLen < mRecordLen) return false;
	memcpy(mMemory, rslt_str.data(), mRecordLen);
	return true;
}


int
AosRecordBSON::getRecordLen()
{
	if (!mComposeFinished) composeData();
	return mRecordLen;
}
	

char *
AosRecordBSON::getData(AosRundata *rdata)
{
	if (!mComposeFinished) composeData();
	return mMemory;
}


void
AosRecordBSON::clear()
{
	AosDataRecord::clear();

	mRecordLen = 0;
	mComposeFinished = true;
	memset(mFieldValFlags, 0, mFields.size());
}


bool
AosRecordBSON::setData(
		char *data,
		const int len,
		AosBuffData *metaData,
		const int64_t offset)
{
	aos_assert_r(data && len > 0, false);
	mMemory = data;
	mMemLen = len;
	mMetaData = metaData;
	mMetaDataRaw = metaData;
	mOffset = offset;

	mRecordLen = len;
	mIsDataParsed = false;
	mComposeFinished = true;

	return true;
}


bool
AosRecordBSON::parseData(AosRundata *rdata_raw)
{
	// This function scans the data to create the field index:
	// 		[offset, length]
	// 		[offset, length]
	// 		...
	// 		[offset, length]
	// One for each field. These are used by fields to retrieve their values.
	// The parsing process will determine the field's start and length, thus
	// converting a variable length records into fixed length records.
//	u64 start_time = OmnGetTimestamp();

	aos_assert_r(!mIsDataParsed, false);
	aos_assert_r(mMemory && mMemLen > 0, false);

	mIsDataParsed = true;
	char * data = mMemory;
	int len = mMemLen;

	int rd_len = mRowDelimiter.length();
	if (mRowDelimiter != "" && strncmp(&data[len - rd_len], mRowDelimiter.data(), rd_len) == 0)
	{
		len -= rd_len;
	}
	
	register int idx = 0;
	register char c = 0;
	char qualifier = 0;
	bool needEscapeQualifier = false;
	u32 num_fields = mFields.size();
	u32 field_idx = moveToNextField(0);
	int field_start, field_end, field_length;

//	u64 ss2 = OmnGetTimestamp();
//	u64 ss3;
//	gTimeCount8 += ss2 - start_time;

	while (idx < len)
	{
		// Looking for:
		// 1. Field delimiter: the end of the current field
		// 2. The quotation mark for the field, if any
		c = data[idx];

		// 1. Check whether it is an empty field
		if (c == mFieldDelimiter)
		{
			// It is an empty field.
			mFieldsRaw[field_idx]->setFieldPos(idx, 0);
			mFieldsRaw[field_idx]->setNull();			// Chen Ding, 2014/08/16
			field_idx++;
			field_idx = moveToNextField(field_idx);
			if (field_idx >= num_fields) break;
			idx++;
//			ss3 = OmnGetTimestamp();
//			gTimeCount1 += ss3 - ss2;
//			ss2 = ss3;
			continue;
		}

//		ss3 = OmnGetTimestamp();
//		gTimeCount1 += ss3 - ss2;
//		ss2 = ss3;

		// 2. Check whether it is qualifier.
		qualifier = 0;
		if (!mNoTextQualifier)
		{
			if (mTextQualifier)
			{
				if (c == mTextQualifier) qualifier = c;
			}
			else
			{
				if (c == '"' || c == '\'') qualifier = c;
			}
		}

		// 3. It is not qualifiered.
		// Scan all the way to the field_separator
		if (!qualifier)
		{
			field_start = idx;
			while (idx < len && ((c = data[idx]) != mFieldDelimiter))
			{
				idx++;
			}

			field_length = idx - field_start;
			mFieldsRaw[field_idx]->setFieldPos(field_start, field_length);
			field_idx++;
			field_idx = moveToNextField(field_idx);
			if (field_idx >= num_fields) 
			{
//				ss3 = OmnGetTimestamp();
//				gTimeCount2 += ss3 - ss2;
//				gTimeCount6 += ss3 - start_time;
				return true;
			}
			idx++;
//			ss3 = OmnGetTimestamp();
//			gTimeCount2 += ss3 - ss2;
//			ss2 = ss3;
			continue;	
		}
//		ss3 = OmnGetTimestamp();
//		gTimeCount2 += ss3 - ss2;
//		ss2 = ss3;
			
		// 4. It is qualifier field.
		// Scan all the way to the closing qualifier.
		// Will consider escapes. Contents inside the
		// qualifier are blindly skipped.
		idx++;
		field_start = idx;
		needEscapeQualifier = false;
		while (idx < len)
		{
			while (idx < len && data[idx] != qualifier) idx++;
			if (idx >= len) break;

			/*
			c = data[idx];
			if (c != qualifier)
			{
				idx++;
				continue;
			}
			*/

			// A qualifier is found. Check whether it is escaped 
			if (!mBackslashEscape)
			{
				if (idx < len-2 && data[idx+1] == qualifier)
				{
					// It is escaped
					needEscapeQualifier = true;
					idx += 2;
					continue;
				}
			}
			else
			{
				// It is escaped by backslash
				if (idx > field_start && data[idx-1] == '\\')
				{
					// It is escaped
					needEscapeQualifier = true;
					idx++;
					continue;
				}
			}

			// The closing qualifier is found. 
			// Skip white spaces after the qualifier.
			field_end = idx;
			idx++;
			break;
		}

//		ss3 = OmnGetTimestamp();
//		gTimeCount3 += ss3 - ss2;
//		ss2 = ss3;
			
		// It either runs rout of the data or the next is the field separator.
		if ((idx < len && data[idx] != mFieldDelimiter) || 
			 field_end >= len || field_end < field_start)
		{
			c = data[len-1];
			data[len-1] = 0;
			AosSetErrorUser(rdata_raw, "recordcsv_invalid_record") << data << enderr;
			OmnAlarm << "recordcsv_invalid_record " << enderr;
			data[len-1] = c;
			return false;
		}

		if (needEscapeQualifier)
		{
			mNeedEscapeQualifier[field_idx] = true;
			field_start--;
			field_end++;
		}

		field_length = field_end - field_start;
		mFieldsRaw[field_idx]->setFieldPos(field_start, field_length);
		field_idx++;
		field_idx = moveToNextField(field_idx);
		if (field_idx >= num_fields) 
		{
//			ss3 = OmnGetTimestamp();
//			gTimeCount4 += ss3 - ss2;
//			gTimeCount6 += ss3 - start_time;
			return true;
		}
		idx++;
//		ss3 = OmnGetTimestamp();
//		gTimeCount4 += ss3 - ss2;
//		ss2 = ss3;
	}
//	ss3 = OmnGetTimestamp();
//	gTimeCount7 += ss3 - start_time;
//	ss2 = ss3;

	while(field_idx < num_fields)
	{
		mFieldsRaw[field_idx]->setFieldPos(0, 0);
		field_idx++;
		field_idx = moveToNextField(field_idx);
		if (field_idx >= num_fields) 
		{
//			u64 sss = OmnGetTimestamp();
//			gTimeCount5 += sss - ss2;
//			gTimeCount6 += sss - start_time;
			return true;
		}
	}

//	ss3 = OmnGetTimestamp();
//	gTimeCount5 += ss3 - ss2;
//	gTimeCount6 += ss3 - start_time;

	return true;
}


AosXmlTagPtr
AosRecordBSON::serializeToXmlDoc(
		const char *data, 
		const int data_len,
		AosRundata* rdata)
{
	bool rslt = setData((char*)data, data_len, 0, 0);
	aos_assert_r(rslt, 0);

	OmnString docstr = "<record>";

	OmnString subtagname, valuestr;
	for (u32 i=0; i<mFields.size(); i++)
	{
		if (mFieldsRaw[i]->isIgnoreSerialize()) continue; 
		
		subtagname = mFieldsRaw[i]->getName();
		aos_assert_r(subtagname != "", 0);

		AosValueRslt value;
		rslt = getFieldValue(i, value, false, rdata);
		aos_assert_r(rslt, 0);

		valuestr = value.getValueStr1();
		valuestr.normalizeWhiteSpace(true, true);
		
		docstr << "<" << subtagname
			   << "><![CDATA[" << valuestr
			   << "]]></" << subtagname << ">";
	}
	
	docstr << "</record>";

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

	return doc;
}


void
AosRecordBSON::flush(const bool clean_memory)
{
	if (!mComposeFinished) composeData();
}


int	
AosRecordBSON::getFieldIdx( 
		const OmnString &name,
		AosRundata *rdata)
{
}


AosDataFieldObj*	
AosRecordBSON::getFieldByIdx1(const u32 idx)
{
}
#endif
