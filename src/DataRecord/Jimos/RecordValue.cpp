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
// 2015/01/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataRecord/RecordValue.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosRecordValue_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosRecordValue(version);
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



AosRecordValue::AosRecordValue(const int version)
:
AosDataRecord(AosDataRecordType::eValue,
	AOSRECORDTYPE_VALUE, false AosMemoryCheckerFileLine),
mRecordLen(0),
{
}


AosRecordValue::~AosRecordValue()
{
}


bool
AosRecordValue::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	// 	<conf field_name="xxx" data_type="ddd"/>
	aos_assert_r(def, false);
	mFieldName = def->getAttrStr("field_name");	
	OmnString str = def->getAttrStr("data_type");
	AosDataType::E data_type = AosDataType::toEnum(str);
	if (AosDataType::isValid(data_type))
	{
		AosLogUserError(rdata, "invalid_data_type") 
			<< "Data Type: " << str << enderr;
		return false;
	}

	mValue.setDataType(data_type);
	return true;
}


AosDataRecordObjPtr 
AosRecordValue::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordValue(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordValue::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordValue(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


bool		
AosRecordValue::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata)
{
	value = mValue;
	return true;
}


bool		
AosRecordValue::setFieldValue(
		const int idx, 
		const AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	outofmem = false;
	mValue.setValueOnly(value);
	return true;
}


int
AosRecordValue::getRecordLen()
{
	return mValue.getSize();
}
	

char *
AosRecordValue::getData(AosRundata *rdata)
{
	return mMemory;
}


void
AosRecordValue::clear()
{
	AosDataRecord::clear();
	mMemory = 0;
	mValue.reset();
}


bool
AosRecordValue::setData(
		char *data,
		const int len,
		AosBuffData *metaData,
		const int64_t offset)
{
	mMemory = data;
	mValue.setValueFromBuff(mMemory);
	return true;
}


AosXmlTagPtr
AosRecordValue::serializeToXmlDoc(
		const char *data, 
		const int data_len,
		AosRundata* rdata)
{
	if (mFieldName == "")
	{
		AosLogUserError(rdata, "fieldname_empty") << enderr;
		return 0;
	}

	bool rslt = setData((char*)data, data_len, 0, 0);
	aos_assert_r(rslt, 0);

	OmnString docstr = "<record>";
	docstr << "<" << mFieldName << ">" << mValue << "</" << mFieldName << ">"
		<< "</record>";

	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

	return doc;
}
#endif
