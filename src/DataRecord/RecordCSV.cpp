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
// 2013/12/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordCSV.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "MetaData/MetaData.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "API/AosApiC.h"


AosRecordCSV::AosRecordCSV(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eCSV,
AOSRECORDTYPE_CSV, flag AosMemoryCheckerFileLine),
mRecordLen(0),
mEstimateRecordLen(0),
mNoTextQualifier(false),
mIsDataParsed(false),
mNeedEscapeQualifier(0),
mBackslashEscape(true),
mCrtRcdLen(0), 
mCrtFieldIdx(0)
{
}


AosRecordCSV::AosRecordCSV(
		const AosRecordCSV &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mRecordLen(rhs.mRecordLen),
mEstimateRecordLen(rhs.mEstimateRecordLen),
mRowDelimiter(rhs.mRowDelimiter),
mFieldDelimiter(rhs.mFieldDelimiter),
mTextQualifier(rhs.mTextQualifier),
mNoTextQualifier(rhs.mNoTextQualifier),
mIsDataParsed(false),
mNeedEscapeQualifier(0),
mBackslashEscape(rhs.mBackslashEscape),
mCrtRcdLen(0),
mCrtFieldIdx(0),
mRowDelimiterChar(rhs.mRowDelimiterChar),
mFieldsNum(rhs.mFieldsNum)
{
	if (mNumFields > 0)
	{
		mNeedEscapeQualifier = OmnNew bool[mNumFields];
		memset(mNeedEscapeQualifier, 0, mNumFields);
	}
}


AosRecordCSV::~AosRecordCSV()
{
	delete [] mNeedEscapeQualifier;
	mNeedEscapeQualifier = 0;
}


bool
AosRecordCSV::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);
	OmnCout << def->toString() << endl;

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);
	
	OmnString rowDelimiter = AosParseRowDelimiter(def->getAttrStr(AOSTAG_ROW_DELIMITER));
	aos_assert_r(rowDelimiter != "", false);
	
	mRowDelimiter = rowDelimiter;

	if (rowDelimiter.length() == 2)
	{
		aos_assert_r(rowDelimiter == "\r\n", false);
	}
	else
	{
		aos_assert_r(rowDelimiter.length() == 1, false);
	}
	mRowDelimiterChar = mRowDelimiter.data()[0]; 

	OmnString field_delimiter = AosParseFieldDelimiter(def->getAttrStr(AOSTAG_FIELD_DELIMITER));

	AosConvertAsciiBinary(field_delimiter);
	aos_assert_r(field_delimiter.length() == 1, false);


	mFieldDelimiter = field_delimiter.data()[0];
	aos_assert_r(mFieldDelimiter, false);

	mTextQualifier = 0;
	mBackslashEscape = false;
	mNoTextQualifier = def->getAttrBool("no_text_qualifier", false);
	if (!mNoTextQualifier)
	{
		OmnString text_qualify_str = AosParseTextQualifier(def->getAttrStr(AOSTAG_TEXT_QUALIFIER));
		aos_assert_r(text_qualify_str.length() <= 1, false);
		if (text_qualify_str != "")
		{
			mTextQualifier = text_qualify_str[0];
		}
		if (mTextQualifier == '\1')
		{
			mNoTextQualifier = true;
		}

		OmnString ss = def->getAttrStr("quote_escape");
		if (ss == "backslash") mBackslashEscape = true;
	}

	if (mNumFields <= 0)
	{
		AosSetEntityError(rdata, "recordcsv_missing_field", 
			"Record", mName) << def->toString() << enderr;
		return false;
	}

	int field_num = mNumFields;
	mEstimateRecordLen = 0;
	for (u32 i=0; i<mNumFields; i++)
	{
		if (!mFieldsRaw[i]->needValueFromField())
		{
			int field_len = mFieldsRaw[i]->mFieldInfo.field_len;
			if (field_len < eEstimateEachFieldLen)
			{
				field_len = eEstimateEachFieldLen;
			}
			mEstimateRecordLen += field_len;
		}
		if (mFieldsRaw[i]->isVirtualField())
		{
			field_num --;
		}
	}
	mFieldsNum = field_num; 

	if (mNeedEscapeQualifier)
	{
		AosSetError(rdata, "internal_error") << enderr;
		delete [] mNeedEscapeQualifier;
		mNeedEscapeQualifier = 0;
	}

	/*
	for (u32 i=0; i<mNumFields; i++)
	{
		mFieldsRaw[i]->setFixedFlag(true);
	}
	*/

	mNeedEscapeQualifier = OmnNew bool[mNumFields];
	memset(mNeedEscapeQualifier, 0, mNumFields);

	return true;
}


AosDataRecordObjPtr 
AosRecordCSV::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordCSV(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordCSV::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordCSV(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordCSV::create(
		const AosXmlTagPtr &def,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordCSV * record = OmnNew AosRecordCSV(false AosMemoryCheckerFileLine);
	record->setTaskDocid(task_docid);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordCSV::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata)
{
	//OmnTagFuncInfo << endl;
	aos_assert_r(idx >= 0 && (u32)idx < mNumFields, false);
	aos_assert_r(mFieldValues, false);

	if (mFieldValFlags[idx])
	{
		value = mFieldValues[idx];
		return true;
	}

	bool rslt = true;
	int index = 0;
	/*
	if (mFieldsRaw[idx]->isConst())
	{
		rslt = mFieldsRaw[idx]->getValueFromRecord(
			this, mMemory, mMemLen, index, value, copy_flag, rdata);
		aos_assert_r(rslt, false);

		mFieldValFlags[idx] = true;
		mFieldValues[idx] = value;
		return true;
	}
	*/

	if (!mIsDataParsed)
	{
		int record_len;
		int status;
		rslt = parseData(mMemory, mMemLen, record_len, status);
		aos_assert_r(rslt, false);
	}

	index = 0;
	rslt = mFieldsRaw[idx]->getValueFromRecord(
		this, mMemory, mMemLen, index, value, true, rdata);
	aos_assert_r(rslt, false);

	mFieldValues[idx] = value;
	mFieldValFlags[idx] = true;

	if (mNeedEscapeQualifier[idx])
	{
		OmnString str = mFieldValues[idx].getStr();
		aos_assert_r(str.length() > 2, false);

		char c = str[0];
		OmnString str_temp(&str.data()[1], str.length() - 2);
		OmnString new_str(&c, 1);
		str.setLength(2);
		str.getBuffer()[0] = mBackslashEscape ? '\\' : c; 
		str.getBuffer()[1] = c;
		str_temp.replace(str, new_str, true);
		mFieldValues[idx].setStr(str_temp);
		value = mFieldValues[idx];
		mNeedEscapeQualifier[idx] = false;
	}

	return true;
}


bool		
AosRecordCSV::setFieldValue(
		const int idx, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	aos_assert_r(idx == mCrtFieldIdx, false);

	outofmem = false;
	aos_assert_r(idx >= 0 && (u32)idx < mFieldsNum, false);
	aos_assert_r(mMemory && mMemLen > 0, false);
	int offset = mCrtRcdLen;

	if (idx != 0) 
	{
		if (offset + 1 >= mMemLen)
		{
			outofmem = true;
			return true;
		}
		mMemory[offset] = mFieldDelimiter; 
		offset++;
	}

	AosValueRslt new_value = value;
	AosDataRecord::setFieldValue(idx, value, outofmem, rdata);
	if(!value.isNull()) 
	{
		AosDataType::E fieldDataType = mFields[idx]->getDataType(rdata, this);
		if (fieldDataType == AosDataType::eString)
		{
			// 1. Text Qualifier
			if (!mNoTextQualifier && mTextQualifier) 
			{
				if (offset + 1 >= mMemLen)
				{
					outofmem = true;
					return true;
				}
				mMemory[offset] = mTextQualifier;
				offset++;
			}

			// 2. escape 
			OmnString str = value.getStr();
			if (str != "" && str.indexOf(0, mTextQualifier) != -1)    
			{                                                         
				OmnString pattern(&mTextQualifier, 1);                
				OmnString new_str(2, mTextQualifier, true);           
				if (mBackslashEscape) new_str.getBuffer()[0] = '\\';  
				str.replace(pattern, new_str, true);                  
			}                                                         
			new_value.setStr(str);
		}

		mFields[idx]->mFieldInfo.field_offset = offset;
		mFields[idx]->setValueToRecord(mMemory, mMemLen, new_value, outofmem, rdata); 
		if (outofmem) return true;
		offset += mFields[idx]->mFieldInfo.field_len;
		if (fieldDataType == AosDataType::eString)
		{
			if (!mNoTextQualifier && mTextQualifier) 
			{
				if (offset + 1 >= mMemLen)
				{
					outofmem = true;
					return true;
				}
				mMemory[offset] = mTextQualifier;
				offset++;
			}
		}
	}

	//if (idx == (i64)(mNumFields - 1) && mRowDelimiter != "")
	if (idx == (i64)(mFieldsNum - 1) && mRowDelimiter != "")
	{                                          
		if (offset + mRowDelimiter.length() >= mMemLen)
		{
			outofmem = true;
			return true;
		}
		memcpy(&mMemory[offset], mRowDelimiter.data(), mRowDelimiter.length());
		offset += mRowDelimiter.length(); 
		mRecordLen = offset;
	} 
	mCrtRcdLen = offset;
	mCrtFieldIdx++;
	return true;
}


int
AosRecordCSV::getRecordLen()
{
	return mRecordLen;
}
	

char *
AosRecordCSV::getData(AosRundata *rdata)
{
	return mMemory;
}


void
AosRecordCSV::clear()
{
	AosDataRecord::clear();

	mCrtRcdLen = 0;
	mRecordLen = 0;
	mCrtFieldIdx = 0;
	memset(mFieldValFlags, 0, mNumFields);
	memset(mNeedEscapeQualifier, 0, mNumFields);
	for (int i=0; i<mNumFields; i++)
	{
		mFieldsRaw[i]->setNotNull();
	}
}


bool
AosRecordCSV::setData(
		char *data,
		const int len,
		AosMetaData *metaData,
		int &status)
		//const int64_t offset)
{
	aos_assert_r(data && len > 0, false);
	int rcd_len = 0;
	parseData(data, len, rcd_len, status);
	mMemory = data;
	mMemLen = len;
	if(metaData)
	{
		mMetaData = metaData;
		mMetaDataRaw = metaData;
	}
	//mOffset = offset;

	mRecordLen = rcd_len;
	mIsDataParsed = false;

	return true;
}

/*
bool
AosRecordCSV::parseData(AosRundata *rdata_raw)
{
	// This function scans the data to create the field index:
	// 		[offset, length]
	// 		[offset, length]
	// 		...
	// 		[offset, length]
	// One for each field. These are used by fields to retrieve their values.
	// The parsing process will determine the field's start and length, thus
	// converting a variable length records into fixed length records.
	aos_assert_r(!mIsDataParsed, false);
	aos_assert_r(mMemory && mMemLen > 0, false);

	//memset(mFieldValFlags, 0, mNumFields);

	mIsDataParsed = true;
	char * data = mMemory;
	int len = mMemLen;

	int rd_len = mRowDelimiter.length();
	if (mRowDelimiter != "" && strncmp(&data[len - rd_len], mRowDelimiter.data(), rd_len) == 0)
	{
		len -= rd_len;
	}
	
	int idx = 0;
	char c = 0;
	char qualifier = 0;
	bool needEscapeQualifier = false;
	u32 field_idx = moveToNextField(0);
	u32 pre_field_idx = field_idx;
	int field_start, field_end, field_length;
	while (idx < len)
	{
		c = data[idx];

		// 1. Check whether it is an empty field
		// It is an empty field
		if (c == mFieldDelimiter)
		{
			mFieldsRaw[field_idx]->mFieldInfo.setPosition(idx, 0);
			mFieldsRaw[field_idx]->setNull();
			field_idx++;
			field_idx = moveToNextField(field_idx);
			if (field_idx >= mNumFields) break;
			idx++;
			continue;
		}

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
			pre_field_idx = field_idx;
			mFieldsRaw[field_idx]->setNotNull();
			mFieldsRaw[field_idx]->mFieldInfo.setPosition(field_start, field_length);
			if (field_length == 0) mFieldsRaw[field_idx]->setNull();

			field_idx++;
			field_idx = moveToNextField(field_idx);
			if (field_idx >= mNumFields) 
			{
				//felicia, 2014/11/12 for \r
				if (data[idx] == '\n' && data[idx-1] == '\r')
				{
					mFieldsRaw[pre_field_idx]->setNotNull();
					mFieldsRaw[pre_field_idx]->mFieldInfo.setPosition(field_start, field_length-1);
					if (field_length-1 == 0)
						mFieldsRaw[pre_field_idx]->setNull();

				}
				return true;
			}
			idx++;
			continue;	
		}
			
		// 4. It is qualifier field.
		// Scan all the way to the closing qualifier.
		// Will consider escapes. Contents inside the
		// qualifier are blindly skipped.
		idx++;
		field_start = idx;
		needEscapeQualifier = false;
		while (idx < len)
		{
			c = data[idx];
			if (c != qualifier)
			{
				idx++;
				continue;
			}

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
			
		// It either runs rout of the data or the next is the field separator.
		if ((idx < len && data[idx] != mFieldDelimiter) || 
			 field_end >= len || field_end < field_start)
		{
			if (len - idx == 1 && data[idx] == '\r')
			{
				// Do Nothing
			}
			else
			{
				c = data[len-1];
				data[len-1] = 0;
				//AosSetErrorUser(rdata_raw, "recordcsv_invalid_record") << data << enderr;
				//OmnAlarm << "recordcsv_invalid_record " << enderr;
				data[len-1] = c;
				return true;
			}
		}

		if (needEscapeQualifier)
		{
			mNeedEscapeQualifier[field_idx] = true;
			field_start--;
			field_end++;
		}

		field_length = field_end - field_start;
		pre_field_idx = field_idx;

		mFieldsRaw[field_idx]->setNotNull();
		mFieldsRaw[field_idx]->mFieldInfo.setPosition(field_start, field_length);
		if (field_length == 0) mFieldsRaw[field_idx]->setNull();

		field_idx++;
		field_idx = moveToNextField(field_idx);
		if (field_idx >= mNumFields) 
		{
			//felicia, 2014/11/12 for \r
			if (data[idx] == '\n' && data[idx-1] == '\r')
			{
				mFieldsRaw[pre_field_idx]->setNotNull();
				mFieldsRaw[pre_field_idx]->mFieldInfo.setPosition(field_start, field_length-1);
				if (field_length-1 == 0)
					mFieldsRaw[pre_field_idx]->setNull();
			}
			return true;
		}
		idx++;
	}

	while(field_idx < mNumFields)
	{
		mFieldsRaw[field_idx]->mFieldInfo.setPosition(0, 0);
		mFieldsRaw[field_idx]->setNull();
		field_idx++;
		field_idx = moveToNextField(field_idx);
		if (field_idx >= mNumFields) return true;
	}

	return true;
}
*/

bool
AosRecordCSV::parseData(
		char *data,
		const int64_t &len,
		int &record_len,
		int &status)
{
	// this function is to scan all the data until find one record
	// eg: there are three fields, the following is error case
	// 1. abc, bcd\n
	// 2. abc, bcd, def, hij\n
	// 3. abc, b"\n"d, def\n
	// 4. abc, "abc"d, def\n
	//
	// the following is correct case:
	// 1. abc, bcd, def\n
	// 2. abc, "bcd\n", def\n
	//
	// if field has '\000', it must replace to ' '
	// eg : abc\000, bcd ===> abc , bcd
	
	
	// jimodb-953, 2015.10.14
	// Optimize RecordCSV 
	int crtidx = 0;
	int numFields = 0;
	int field_idx = 0;
	int pre_field_pos = 0, field_end = 0;
	int crt_status = eFieldBegin;
	mFieldsRaw[field_idx]->setNotNull();
	int rcd_len = mRowDelimiter.length();
	bool needEscapeQualifier = false;   
	
	while(crtidx<len)
	{
		if (data[crtidx] == 0) data[crtidx]= ' ';

		switch(crt_status)
		{
			case eFieldBegin: 
				if (data[crtidx] == mFieldDelimiter)
				{
					crt_status = eFieldEnd;
					mFieldsRaw[field_idx]->setNull();
					break;
				}

				if (data[crtidx] == mRowDelimiterChar || '\r' == data[crtidx])
				{
					crt_status = eRecordEnd;
					mFieldsRaw[field_idx]->setNull();
					break;
				}
				if (!mNoTextQualifier && data[crtidx] == mTextQualifier)
				{
					crt_status = eStrField;
				}

				else
				{
					crt_status = eField;
				}
				crtidx++;
				break;

			case eField: 
				if (data[crtidx] == mFieldDelimiter)
				{
					crt_status = eFieldEnd;
					break;
				}

				if (data[crtidx] == mRowDelimiterChar || '\r' == data[crtidx])
				{
					crt_status = eRecordEnd;
					break;
				}

				crtidx++;
				break;

			case eStrField: 
				if (data[crtidx] == mTextQualifier)
				{
					crtidx++;
					if (crtidx>=len)
					{
						record_len = -1;
						status = -1;
						return true;
					}
					if (data[crtidx] != mTextQualifier)
					{
						if (data[crtidx] == mFieldDelimiter)
						{
							crt_status = eFieldEnd;
							if (crtidx-pre_field_pos == 2)
							{
								mFieldsRaw[field_idx]->setNull();
							}
							break;
						}

						else if (mRowDelimiterChar == data[crtidx] || '\r' == data[crtidx])
						{
							crt_status = eRecordEnd;
							if(crtidx-pre_field_pos == 2)
							{
								mFieldsRaw[field_idx]->setNull();
							}
							break;
						}
						else
						{
							OmnScreen << "invalid record" << endl;
							status =-2;
							record_len = ++crtidx;
							return true;
						}
					}
					else if (!mBackslashEscape)
					{
						// It is escaped
						needEscapeQualifier = true;
						crtidx++;
						continue;
					}
				}
				crtidx++;
				break;

			case eFieldEnd: 
				crt_status = eFieldBegin;
				field_end = crtidx;
				if (!mNoTextQualifier && mTextQualifier && data[pre_field_pos] == mTextQualifier)
				{
					if (needEscapeQualifier)                     
					{                                            
						mNeedEscapeQualifier[field_idx] = true;  
						field_end++;                             
						pre_field_pos--;                         
					}                                            
					mFieldsRaw[field_idx]->mFieldInfo.field_offset = pre_field_pos+1;
					//crtidx++;
					//mFieldsRaw[field_idx]->mFieldInfo.field_len = crtidx - pre_field_pos;
					mFieldsRaw[field_idx]->mFieldInfo.field_data_len = field_end - pre_field_pos -2;
				}

				else
				{
					mFieldsRaw[field_idx]->mFieldInfo.field_offset = pre_field_pos;
					//crtidx++;
					//mFieldsRaw[field_idx]->mFieldInfo.field_len = crtidx - pre_field_pos;
					mFieldsRaw[field_idx]->mFieldInfo.field_data_len = field_end - pre_field_pos;
				}

				crtidx++;
				field_idx ++;
				//mFieldsRaw[field_idx]->setNotNull();
				if (field_idx >= mFieldsNum) 
				{                            
					status = -2;             
					record_len = crtidx;     
					return true;             
				}                            
				numFields++; 
				pre_field_pos = crtidx;
				needEscapeQualifier = false;
				break;

			case eRecordEnd: 
				numFields++;
				if (data[crtidx] == '\r')
				{
					crtidx++;
					if (crtidx < len)
					{
						if (data[crtidx] != '\n')
						{
							OmnScreen << "invalid record" << endl;
							status = -2;
							record_len = ++crtidx;
							return true;
						}
						rcd_len = 2;
					}
					else
					{
						status = -1;
						record_len = -1;
						return true;
					}
				}
				if (numFields == mFieldsNum)
				{
					//crtidx += mRowDelimiter.length();
					//crtidx++;
					field_end = crtidx;
					if (!mNoTextQualifier && mTextQualifier && data[pre_field_pos] == mTextQualifier)
					{
						if (needEscapeQualifier)                    
						{                                           
							mNeedEscapeQualifier[field_idx] = true; 
							field_end ++;                           
							pre_field_pos--;                        
						}                                           
						mFieldsRaw[field_idx]->mFieldInfo.field_offset = pre_field_pos+1;
						//crtidx++;
						//mFieldsRaw[field_idx]->mFieldInfo.field_len = crtidx - pre_field_pos;
						mFieldsRaw[field_idx]->mFieldInfo.field_data_len = field_end - pre_field_pos -rcd_len -1;
					}

					else
					{
						mFieldsRaw[field_idx]->mFieldInfo.field_offset = pre_field_pos;
						//crtidx++;
						//mFieldsRaw[field_idx]->mFieldInfo.field_len = crtidx - pre_field_pos;
						mFieldsRaw[field_idx]->mFieldInfo.field_data_len= field_end - pre_field_pos - rcd_len +1;
					}
					crtidx++;
					status = 0;
					record_len = crtidx;
					crtidx = len;
					needEscapeQualifier = false;  
					return true;
				} 
				else 
				{
					OmnScreen << "invalid record" << endl;
					status = -2;
					record_len = ++crtidx;
					return true;
				}

			default:
				status = -2;
				record_len = ++crtidx;
				return true;
		};
	}
	if (crtidx >= len)
	{
		record_len = -1;
		status = -1;
		return true;
	}

#if 0
	status = 0;
	record_len = 0;
	int idx = 0;
	char c = 0;
	char pre_c = 0;
	char qualifier = 0;
	u32 field_idx = moveToNextField(0);
	int field_start, field_end;
	int long_len = 0;
	while (idx < len)
	{
		if (idx - long_len > 100000) //record_len > 100K is too long
		{
			OmnString str(&data[long_len], idx-long_len);
			OmnScreen << " too long record : " << str << endl;
			long_len = idx;
//OmnScreen << "111111111111" << endl;
OmnScreen << "invalid record" << endl;
			status = -2;
		}
		c = data[idx];

		// 1. Check whether it is an empty field
		// It is an empty field
		if (c == mFieldDelimiter)
		{
			//filter record
			if (field_idx >= mNumFields)
			{
OmnScreen << "invalid record" << endl;
				status = -2;
				break;
			}
			if(mFieldsRaw[field_idx]->mFieldInfo.notnull)
			{
OmnScreen << "invalid record" << endl;
				status = -2;      
			}
			field_idx++;
			field_idx = moveToNextField(field_idx);
			if (field_idx >= mNumFields) break;
			idx++;
			continue;
		}
			
		// check whether it is RowDelimiter
		int k = 0;
		while (k < mRowDelimiter.length())
		{
			if (data[idx] != mRowDelimiter[k]) break;
			idx++;
			if (k < mRowDelimiter.length() && idx > len)
			{
				// incomplete record
				record_len = -1;		
				return true;
			}
			k++;
		}

		if (k == mRowDelimiter.length())
		{
			record_len = idx;
			field_idx++;
			field_idx = moveToNextField(field_idx);
			if (field_idx >= mNumFields)
			{
				return true;
			}

			// invalid record
			// 1. abc, bcd\n
			// 2. abc, bcd, def, fhj\n
OmnScreen << "invalid record" << endl;
			status = -2;
			return true;
		}

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
		// check whether there is mRowDelimiter
		// check whether incomplete record
		if (!qualifier)
		{
			while (idx < len && ((c = data[idx]) != mFieldDelimiter))
			{
				//scan where has mRowDelimiter
				if (c == 0)
				{
					data[idx] = ' ';
				}
				
				int k = 0;
				while (k < mRowDelimiter.length())
				{
					if (data[idx] != mRowDelimiter[k]) break;
					idx++;
					if (k < mRowDelimiter.length() && idx > len)
					{
						// incomplete record
						record_len = -1;		
						return true;
					}
					k++;
				}
				
				if (k == mRowDelimiter.length())
				{
					record_len = idx;
					field_idx++;
					field_idx = moveToNextField(field_idx);
					if (field_idx >= mNumFields)
					{
						return true;
					}

//OmnScreen << "333333333333333" << endl;
OmnScreen << "invalid record" << endl;
					status = -2;
					return true;
				}

				idx++;
			}

			if (idx >= len)
			{
				// incomplete record
			//	if (idx == len)
			//	{
			//		record_len = idx;
			//		return true;
			//	}
				record_len = -1;		
				return true;
			}

			field_idx++;
			field_idx = moveToNextField(field_idx);
			idx++;
			continue;	
		}
			
		// 4. It is qualifier field.
		// Scan all the way to the closing qualifier.
		// Will consider escapes. Contents inside the
		// qualifier are blindly skipped.
		idx++;
		field_start = idx;
		while (idx < len)
		{
			c = data[idx];
			if (c == 0)
			{
				data[idx] = ' ';
			}
		
			/*
			//felicia,2015/11/02 JIMODB-1013
			if (idx + 1 == len)
			{
				record_len = -1;
				return true;
			}
			*/

			if (c != qualifier)
			{
				idx++;
				continue;
			}

			// A qualifier is found. Check whether it is escaped 
			if (!mBackslashEscape)
			{
				//if (idx < len-2 && data[idx+1] == qualifier)
				if (data[idx+1] == qualifier)
				{
					// It is escaped
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
			
		/// It either runs rout of the data or the next is the field separator.
		if (field_end >= len)
		{
			record_len = -1;
			return true;
		}

		if (idx < len && data[idx] != mFieldDelimiter)
		{
			// exmple : "abc"d,xxxxxx
//OmnScreen << "44444444444444" << endl;
			status = -2;
			while (idx < len && ((c = data[idx]) != mFieldDelimiter))
			{
				//scan where has mRowDelimiter
				pre_c = data[idx-1];
				if (c == 0)
				{
					data[idx] = ' ';
				}
				int k = 0;
				while (k < mRowDelimiter.length())
				{
					if (data[idx] != mRowDelimiter[k]) break;
					idx++;
					if (k < mRowDelimiter.length() && idx > len)
					{
						// incomplete record
						record_len = -1;		
						return true;
					}
					k++;
				}
				
				if (k == mRowDelimiter.length())
				{
					field_idx++;
					field_idx = moveToNextField(field_idx);
					if (field_idx >= mNumFields && pre_c == qualifier)
					//if (field_idx >= mNumFields)
					{
						status = 0;
					}
					record_len = idx;
					return true;
				}
				idx++;
			}

			if (idx >= len)
			{
				// incomplete record
				record_len = -1;		
				return true;
			}

			field_idx++;
			field_idx = moveToNextField(field_idx);
			idx++;
			continue;	
		}
		
		field_idx++;
		field_idx = moveToNextField(field_idx);
		//if (field_idx >= mNumFields) return true;
		idx++;
	}

	if (field_idx < mNumFields)
	{
		record_len = -1;
		return true;
	}

//OmnScreen << "5555555555555555555555555" << endl;
	if (idx >= len)
	{
		status = -1;
		record_len = -1;
	}
	else
	{
OmnScreen << "invalid record" << endl;
		status = -2;
		record_len = idx; 
	}
	return true;
	
	/*
	while(field_idx < mNumFields)
	{
		mFieldsRaw[field_idx]->setFieldPos(0, 0);
		field_idx++;
		field_idx = moveToNextField(field_idx);
		if (field_idx >= mNumFields) return true;
	}
	*/
#endif
}


AosXmlTagPtr
AosRecordCSV::serializeToXmlDoc(
		const char *data, 
		const int data_len,
		AosRundata* rdata)
{
	int status;
	bool rslt = setData((char*)data, data_len, 0, status);
	aos_assert_r(rslt, 0);

	OmnString docstr = "<record>";

	OmnString subtagname, valuestr;
	for (u32 i=0; i<mNumFields; i++)
	{
		if (mFieldsRaw[i]->isIgnoreSerialize()) continue; 
		
		subtagname = mFieldsRaw[i]->getName();
		aos_assert_r(subtagname != "", 0);

		AosValueRslt value;
		rslt = getFieldValue(i, value, false, rdata);
		aos_assert_r(rslt, 0);

		valuestr = value.getStr();
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
AosRecordCSV::flush(const bool clean_memory)
{
	aos_assert(mCrtFieldIdx == (int)mFieldsNum); 
	//aos_assert(mCrtFieldIdx == (int)mNumFields); 
	
}


bool 
AosRecordCSV::appendField(
		AosRundata *rdata, 
		const OmnString &name,
		const AosDataType::E type,
		const AosStrValueInfo &info)
{
	OmnString datatype = AosDataType::getTypeStr(type);
	if (datatype == "string") datatype = "str";
	if (name == "docid") datatype = "docid";

	OmnString str = "<datafield zky_name=\"";
	str << name << "\" type=\"" << datatype << "\"/>";

	AosXmlTagPtr xml = AosStr2Xml(rdata, str AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	AosDataFieldObjPtr field = AosDataFieldObj::createDataFieldStatic(xml, this, rdata);
	aos_assert_r(field, false);

	bool rslt = AosDataRecord::appendField(rdata, field);
	aos_assert_r(rslt, false);

	OmnDelete [] mNeedEscapeQualifier;
	bool *needEscapeQualifier = OmnNew bool[mNumFields];
	memset(needEscapeQualifier, 0, mNumFields);
	mNeedEscapeQualifier = needEscapeQualifier;

	return true;
}


bool 
AosRecordCSV::appendField(
		AosRundata *rdata, 
		const AosDataFieldObjPtr &field)
{
	aos_assert_r(field, false);

	bool rslt = AosDataRecord::appendField(rdata, field);
	aos_assert_r(rslt, false);

	OmnDelete [] mNeedEscapeQualifier;
	bool *needEscapeQualifier = OmnNew bool[mNumFields];
	memset(needEscapeQualifier, 0, mNumFields);
	mNeedEscapeQualifier = needEscapeQualifier;

	return true;
}


bool 
AosRecordCSV::removeFields()
{
	AosDataRecord::removeFields();

	delete [] mNeedEscapeQualifier;
	mNeedEscapeQualifier = 0;

	return true;
}
