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
// 2014/05/22 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordStat.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterUtil/CounterUtil.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"


AosRecordStat::AosRecordStat(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eStat, AOSRECORDTYPE_STAT, flag AosMemoryCheckerFileLine),
mRecordLen(0),
mSplitFieldIdx(-1),
mNumFields(-1)
{
}


AosRecordStat::AosRecordStat(
		const AosRecordStat &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mRecordLen(rhs.mRecordLen),
mSplitFieldIdx(rhs.mSplitFieldIdx),
mNumFields(rhs.mNumFields)
{
	AosDataFieldObjPtr field;
	for (u32 i = 0; i < rhs.mChildFields.size(); i++)
	{
		field = rhs.mChildFields[i]->clone(rdata);
		if (!field)
		{
			AosSetEntityError(rdata, "datarecord_missing_field", 
				"Data Record", mName) 
				<< "Field Index: " << i << enderr;
			OmnThrowException("failed cloning field");
			return;
		}
		mChildFields.push_back(field);
	}
	
	mChildFieldIdxs = rhs.mChildFieldIdxs;
	if (mChildFields.size() != mChildFieldIdxs.size())
	{
		AosSetEntityError(rdata, "datarecord_num_fields_mismatch", 
				"Data Record", mName) << enderr;
		OmnThrowException("error");
		return;
	}
}


AosRecordStat::~AosRecordStat()
{
}


bool
AosRecordStat::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);
	//<datafield type="str" zky_name="str" zky_offset="0" zky_length="120" zky_datatooshortplc="cstr">
	//	<datafield type="str" zky_name="f11"/>
	//	<datafield type="str" zky_name="f21"/>
	//	<datafield type="str" zky_name="f22"/>
	//	<datafield type="str" zky_name="f31"/>
	//	<datafield type="str" zky_name="f32"/>
	//	<datafield type="str" zky_name="f33"/>
	//</datafield>
	//<datafield type="bin_u64" zky_name="epochday" zky_offset="120" zky_length="8"/>
	//<datafield type="bin_u64" zky_name="value" zky_offset="128" zky_length="8"/>

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);

	mNumFields = mFields.size();

	AosDataFieldObjPtr elem;
	AosXmlTagPtr elems = def->getFirstChild("datafields");
	if (!elems) return true;

	AosXmlTagPtr elemtag = elems->getFirstChild(true);
	while (elemtag)
	{
		AosXmlTagPtr elem_child = elemtag->getFirstChild("datafield");
		if (!elem_child)
		{
			elemtag = elems->getNextChild();
			continue;
		}

		OmnString nn = elemtag->getAttrStr(AOSTAG_NAME);
		mSplitFieldIdx = AosDataRecord::getFieldIdx(nn, rdata);

		OmnString name;
		map<OmnString, int>::iterator itr;
		while(elem_child)
		{
			elem = AosDataFieldObj::createDataFieldStatic(elem_child, this, rdata);

			if (!elem)
			{
				AosSetEntityError(rdata, "datarecord_internal_error", 
						"Data Record", "dd") << enderr;
				return false;
			}

			name = elem->getName();
			itr = mChildFieldIdxs.find(name);
			if(itr != mChildFieldIdxs.end())
			{
				AosSetEntityError(rdata, "datarecord_field_already_exist", 
						"Data Record", name) << ""<< enderr;
				return false;
			}

			mChildFieldIdxs[name] = mChildFields.size() + mNumFields; 
			mChildFields.push_back(elem);
			elem_child = elemtag->getNextChild();
		}
		elemtag = elems->getNextChild();
	}

	mNumFields += mChildFields.size();
	aos_assert_r(mNumFields >= 0 &&(u32)mNumFields ==  mFields.size() + mChildFields.size(), false);

	mRecordLen = def->getAttrInt(AOSTAG_LENGTH, 0);
	if (mRecordLen <= 0)
	{
		AosSetErrorUser3(rdata, "invalid_record_len", "recordfixbin_invalid_recordlen")
			<< def << enderr;
		return false;
	}

	return true;
}


AosDataRecordObjPtr 
AosRecordStat::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordStat(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordStat::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordStat(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordStat::create(
		const AosXmlTagPtr &def,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordStat * record = OmnNew AosRecordStat(false AosMemoryCheckerFileLine);
	record->setTaskDocid(task_docid);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordStat::getFieldValue(
		const int idx, 
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata) 
{
	aos_assert_r(mMemory && mMemLen > 0, false);
	aos_assert_r(idx >= 0 && idx < mNumFields, false);
	if (mFieldValues.find(idx) != mFieldValues.end())
	{
		value = mFieldValues[idx];
		return true;
	}

	int index = 0;
	bool rslt = false;
	if (idx >= 0 && (u32)idx < mFields.size())
	{
		rslt = mFields[idx]->getValueFromRecord(
			this, mMemory, mMemLen, index, value, copy_flag, rdata);
		aos_assert_r(rslt, false);

		mFieldValues[idx] = value;
		return true;
	}

	aos_assert_r(mSplitFieldIdx >= 0 && (u32)mSplitFieldIdx < mFields.size(), false);
	AosValueRslt valueRslt;
	map<int, AosValueRslt>::iterator itr = mFieldValues.find(mSplitFieldIdx);
	if (itr == mFieldValues.end())
	{
		rslt = mFields[mSplitFieldIdx]->getValueFromRecord(
				this, mMemory, mMemLen, index, valueRslt, copy_flag, rdata);
		aos_assert_r(rslt, false);
		mFieldValues[mSplitFieldIdx] = valueRslt;
	}
	else
	{
		valueRslt = itr->second;
	}

	OmnString split_str = valueRslt.getStr();
//OmnScreen << "EEEE split_str:" << split_str << ";" << endl;
	split_str.normalizeWhiteSpace(true, true);  // trim
	aos_assert_r(split_str != "", false);
	rslt = splitStatKey(split_str);
	aos_assert_r(rslt, false);

	aos_assert_r((u32)idx >= mFields.size() && idx < mNumFields, false);
	value = mFieldValues[idx];
	return true;
}


void
AosRecordStat::clear()
{
	AosDataRecord::clear();
}


bool 		
AosRecordStat::setData(
		char *data,
		const int len,
		AosMetaData *metaData,
		const int64_t offset)
{
	aos_assert_r(data && len > 0 && len >= mRecordLen, false);
	mMemory = data;
	mMemLen = len;
	if (metaData)
	{
		mMetaData = metaData;
		mMetaDataRaw = metaData;
	}
	mOffset = offset;

	return true;
}


bool
AosRecordStat::determineRecordLen(
		char *data,
		const int64_t &len,
		int &record_len,
		int &status)
{
	aos_assert_r(len > 0, false);
	return true;
}


int
AosRecordStat::getFieldIdx(
		const OmnString &name,
		AosRundata *rdata)
{
	map<OmnString, int>::iterator itr = mFieldIdxs.find(name);
	if(itr != mFieldIdxs.end())
	{
		return itr->second;
	}

	itr = mChildFieldIdxs.find(name);
	if(itr != mChildFieldIdxs.end())
	{
		return itr->second;
	}
	return -1;
}


bool
AosRecordStat::splitStatKey(const OmnString &keys)
{
	aos_assert_r(keys != "", false);

	vector<OmnString> substrs;
	int num = AosStrSplit::splitStrBySubstr(keys, AOS_COUNTER_SEGMENT_SEP2, substrs, 100);
	aos_assert_r(num >= 0 && (u32)num == mChildFields.size(), false);

	int idx = mFieldIdxs.size();
	for (u32 i = 0; i < substrs.size(); i++)
	{
		aos_assert_r(idx < mNumFields, false);

		AosValueRslt value;
		aos_assert_r(substrs[i] != "", false);
		value.setStr(substrs[i]);
		mFieldValues[idx] = value;
		idx ++;
	}
	aos_assert_r(mFieldValues.size() > mChildFields.size(), false);
	return true;
}


