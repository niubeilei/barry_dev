////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 11/12/2014 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordXmlSimple.h"

#include "alarm_c/alarm.h" 
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"

AosRecordXmlSimple::AosRecordXmlSimple(AosMemoryCheckDeclBegin)
:
AosDataRecord(AosDataRecordType::eSimpleXml, 
		AOSRECORDTYPE_SIMPLE_XML, 
		false AosMemoryCheckerFileLine)
{
}

AosRecordXmlSimple::~AosRecordXmlSimple()
{
}
	
bool
AosRecordXmlSimple::getFieldValue(
		const OmnString &field_name,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	OmnString field_value = "";

	aos_assert_r(mRecord, false);

	//OmnString field_value = mRecord->getNodeText(field_name);
	AosXmlTagPtr child = mRecord->getFirstChild(field_name);
	if (child)
	{
		field_value = child->getNodeText();
	}
	else
	{
		field_value = mRecord->getAttrStr(field_name, "");
	}

	aos_assert_r(field_value != "", false);

	value.setStr(field_value);
	return true;
}

void AosRecordXmlSimple::setDoc(AosXmlTagPtr record)
{
	mRecord = record;
}
