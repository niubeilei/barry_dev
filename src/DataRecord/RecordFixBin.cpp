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
// This is binary fixed length record:
// 	String		fixed length
// 	char *		fixed length
// 	bool		one byte
// 	char		one byte
// 	int			sizeof(int)
// 	int64_t		sizeof(int64_t)
// 	u32			sizeof(u32)
// 	u64			sizeof(u64)
// 	double		sizeof(double)
//
// Modification History:
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordFixBin.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

#include "API/AosApiS.h"

AosRecordFixBin::AosRecordFixBin(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eFixedBinary, 
	AOSRECORDTYPE_FIXBIN, flag AosMemoryCheckerFileLine),
mRecordLen(0),
mWithFieldCache(true),
mTrimCondition(AosRecordFixBin::eInvalid)
{
}


AosRecordFixBin::AosRecordFixBin(
		const AosRecordFixBin &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mRecordLen(rhs.mRecordLen),
mWithFieldCache(true),
mTrimCondition(rhs.mTrimCondition)
{
}


AosRecordFixBin::~AosRecordFixBin()
{
}


bool
AosRecordFixBin::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);

	mRecordLen = def->getAttrInt(AOSTAG_LENGTH, 0);
	if (mRecordLen <= 0)
	{
		AosSetErrorUser3(rdata, "invalid_record_len", "recordfixbin_invalid_recordlen")
			<< def << enderr;
		return false;
	}
	
	mWithFieldCache = def->getAttrBool("with_field_cache", mWithFieldCache);
	if (mWithFieldCache) 
	{
		OmnScreen << "Caching fields" << endl;
	}
	else
	{
		OmnScreen << "No field caching" << endl;
	}

	OmnString str = def->getAttrStr("trim");
	if (str == "ALL") mTrimCondition = eAll;
	if (str == "HEAD") mTrimCondition = eHead;
	if (str == "TAIL") mTrimCondition = eTail;
	return true;
}


AosDataRecordObjPtr 
AosRecordFixBin::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordFixBin(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordFixBin::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordFixBin(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordFixBin::create(
		const AosXmlTagPtr &def,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordFixBin * record = OmnNew AosRecordFixBin(false AosMemoryCheckerFileLine);
	record->setTaskDocid(task_docid);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordFixBin::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);

	if (mWithFieldCache && mFieldValFlags[idx])
	{
		value = mFieldValues[idx];
  		return true;
 	}

	int index = 0;
	bool rslt = mFieldsRaw[idx]->getValueFromRecord(
		this, mMemory, mMemLen, index, value, copy_flag, rdata);

	if (!value.isNull() && mTrimCondition != eInvalid && mFieldsRaw[idx]->getType() == AosDataFieldType::eStr)            
	{                                                                    
		OmnString str = value.getStr();
		if (mTrimCondition == eAll)
		{
			str.removeWhiteSpaces(); //trim
		}
		else if(mTrimCondition == eHead)
		{
			str.removeLeadingWhiteSpace();
		}
		else if(mTrimCondition == eTail)
		{
			str.removeTailWhiteSpace();
		}
		value.setStr(str);
	}

	if (mWithFieldCache)
	{
		mFieldValues[idx] = value;
		mFieldValFlags[idx] = true;
	}

	return rslt;
}


bool		
AosRecordFixBin::setFieldValue(
		const int idx, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	outofmem = false;
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);
	AosDataRecord::setFieldValue(idx, value, outofmem, rdata);	// Ketty 2014/01/17 

	mFieldsRaw[idx]->setValueToRecord(mMemory, mMemLen, value, outofmem, rdata);
	if (outofmem) return true;
	int len = mFieldsRaw[idx]->mFieldInfo.field_data_len-mFieldsRaw[idx]->mFieldInfo.field_len;
	if (len > 0)
	{
		memset(mMemory + mFieldsRaw[idx]->mFieldInfo.field_offset+mFieldsRaw[idx]->mFieldInfo.field_len, '\0', len);
	}
	return true;
}
	

void
AosRecordFixBin::clear()
{
	AosDataRecord::clear();
}


bool
AosRecordFixBin::setData(
		char *data,
		const int len,
		AosMetaData *metaData,
		int &status)
		//const int64_t offset)
{
	aos_assert_r(data && len > 0, false);
	if(len < mRecordLen)
	{
		status = -1;
		return true;
	}
	mMemory = data;
	mMemLen = len;
	if (metaData)
	{
		mMetaData = metaData;
		mMetaDataRaw = metaData;
	}
	status = 0;
	//mOffset = offset;
	return true;
}


AosXmlTagPtr
AosRecordFixBin::serializeToXmlDoc(
		const char *data, 
		const int data_len,
		AosRundata* rdata)
{
	aos_assert_r(data_len >= mRecordLen, 0);

	OmnString docstr = "<record>";

	bool rslt = true;
	int idx = 0;
	for (u32 i=0; i<mFields.size(); i++)
	{
		rslt = mFieldsRaw[i]->serializeToXmlDoc(docstr, idx, data, data_len, rdata);
		aos_assert_r(rslt, 0);
	}
	
	docstr << "</record>";

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

	return doc;
}


AosBuffPtr
AosRecordFixBin::serializeToBuff(
		const AosXmlTagPtr &doc,
		AosRundata *rdata)
{
	aos_assert_r(doc, 0);
		
	bool rslt = true;
	AosBuffPtr buff = OmnNew AosBuff(eBuffInitSize AosMemoryCheckerArgs);
	for (u32 i=0; i<mFields.size(); i++)
	{
		rslt = mFieldsRaw[i]->serializeToBuff(doc, buff, rdata);
		aos_assert_r(rslt, 0);
	}
	return buff;	
}


bool
AosRecordFixBin::createRandomDoc(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	bool rslt = true;
	for (u32 i=0; i<mFields.size(); i++)
	{
		rslt = mFieldsRaw[i]->createRandomValue(buff, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool 
AosRecordFixBin::appendField(
		AosRundata *rdata, 
		const AosDataFieldObjPtr &field)

{
	aos_assert_r(field, false);
	bool rslt = AosDataRecord::appendField(rdata, field);
	aos_assert_r(rslt, false);
	return true;
}

