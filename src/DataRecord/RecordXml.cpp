////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2014/03/31 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordXml.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"


AosRecordXml::AosRecordXml(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eXml,
	AOSRECORDTYPE_XML, flag AosMemoryCheckerFileLine),
mXml(0)
{
}


AosRecordXml::AosRecordXml(
		const AosRecordXml &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine)
{
	if (rhs.mXml)
	{
		mXml = rhs.mXml->clone(AosMemoryCheckerArgsBegin);
	}
}


AosRecordXml::~AosRecordXml()
{
}


bool
AosRecordXml::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


AosDataRecordObjPtr 
AosRecordXml::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordXml(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordXml::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordXml(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordXml::create(
		const AosXmlTagPtr &def,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordXml * record = OmnNew AosRecordXml(false AosMemoryCheckerFileLine);
	record->setTaskDocid(task_docid);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool
AosRecordXml::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);

	if (mFieldValFlags[idx])
	{
		value = mFieldValues[idx];
		return true;
	}

	int index = 0;
	bool rslt = mFieldsRaw[idx]->getValueFromRecord(
		this, mMemory, mMemLen, index, value, copy_flag, rdata);
	aos_assert_r(rslt, false);

	mFieldValues[idx] = value;
	return true;
}


bool		
AosRecordXml::getFieldValue(
		const OmnString &field_name,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(field_name != "", false);
	if (!mXml) createEmptyXml();

	if (field_name == "all_xml")
	{
		OmnString str = mXml->toString();
		value.setStr(str);
		return true;
	}

	int idx = getFieldIdx(field_name, rdata);
	return getFieldValue(idx, value, copy_flag, rdata);
}


bool
AosRecordXml::setFieldValue(
		const int idx, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	outofmem = false;
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);
	AosDataRecord::setFieldValue(idx, value, outofmem, rdata);

	if (!mXml) createEmptyXml();

	aos_assert_r(mFields[idx]->getType() == AosDataFieldType::eXpath, false);

	//AosDataRecordObjPtr rcd = this->clone(rdata AosMemoryCheckerArgs);
	mFields[idx]->setValueToRecord(this, value, outofmem, rdata);

	mFieldValues[idx] = value;
	mFieldValFlags[idx] = true;

	if (mXml->getDataLength() > mMemLen) outofmem = true;
	return true;
}
	

void
AosRecordXml::clear()
{
	mXml = 0;
	AosDataRecord::clear();
}


bool
AosRecordXml::setData(
		char *data,
		const int len,
		AosMetaData *metaData,
		int &status)
		//const int64_t offset)
{
	aos_assert_r(data && len > 0, false);

	OmnString str(data, len);
	mXml = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(mXml, false);

	mMemory = data;
	mMemLen = len;
	if (metaData)
	{
		mMetaData = metaData;
		mMetaDataRaw = metaData;
	}
	//mOffset = offset;
	status = 0;

	return true;
}


int
AosRecordXml::getRecordLen() 
{
	if (!mXml) createEmptyXml();
	return mXml->getDataLength();
}


char *
AosRecordXml::getData(AosRundata *rdata) 
{
	if (!mXml) createEmptyXml();
	int len;
	return (char *)mXml->getData(len);
}

	
OmnString
AosRecordXml::xpathQuery(
		const OmnString &xpath,
		AosRundata *rdata)
{
	if (xpath == "") return "";
	if (!mXml) createEmptyXml();
	return mXml->xpathQuery(xpath);
}
	

bool
AosRecordXml::xpathSetAttr(
		const OmnString &xpath,
		const OmnString &value,
		AosRundata *rdata)
{
	if (xpath == "" || value == "") return "";
	if (!mXml) createEmptyXml();
	return mXml->xpathSetAttr(xpath, value, true);
}


void
AosRecordXml::flush(const bool clean_memory)
{
	if (!mMemory) return;
	if (!mXml) createEmptyXml();
	OmnString str = mXml->toString();
	if (mMemLen < str.length()) return;
	memcpy(mMemory, str.data(), str.length());
}


void
AosRecordXml::createEmptyXml()
{
	OmnString str = "<record/>";
	mXml = AosXmlParser::parse(str AosMemoryCheckerArgs);
}

