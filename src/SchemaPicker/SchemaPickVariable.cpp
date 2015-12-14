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
// This schema picker picks schemas based on:
// 1. Based on a field in the data 
//
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SchemaPicker/SchemaPickVariable.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "API/AosApi.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaPickVariable_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchemaPickVariable(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
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


AosSchemaPickVariable::AosSchemaPickVariable(const int version)
:
AosSchemaPicker(AOSSCHEMAPICKER_VARIABLE, version)
{
}


AosSchemaPickVariable::~AosSchemaPickVariable()
{
}


bool
AosSchemaPickVariable::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	aos_assert_r(worker_doc, false);
	mKeyIdx = worker_doc->getAttrInt("start_pos", -1);
	if (mKeyIdx == -1)
	{
		AosSetEntityError(rdata, "schemaPickConst_miss_key_index", "schemapicker", "")
			    << worker_doc << enderr;
		return false;
	}

	mKeyLen = worker_doc->getAttrInt("length", -1);
	if (mKeyLen == -1)
	{
		AosSetEntityError(rdata, "schemaPickConst_miss_key_length", "schemapicker", "")
			    << worker_doc << enderr;
		return false;
	}
	return true;
}


void
AosSchemaPickVariable::reset()
{
	mMap.clear();
}

bool
AosSchemaPickVariable::holdRecords(
		const vector<AosDataRecordObjPtr> &records,
		const vector<OmnString> &keys,
		AosRundata *rdata)
{
	aos_assert_r(records.size() == keys.size(), false);
	for(size_t i=0; i<keys.size(); i++)
	{
		OmnString key = keys[i];
		aos_assert_r(mMap.find(key) == mMap.end(), false);

		mMap[key] = records[i].getPtr();
	}
	return true;
}


/*
bool 
AosSchemaPickVariable::addSchema(
		AosDataRecordObj * schema,
		const AosXmlTagPtr &conf,
		AosRundata *rdata)
{
	OmnString conf_name = conf->getAttrStr(AOSTAG_XMLEDITOR_NODE_INDEX, "");
	OmnString key = conf->getAttrStr(AOSTAG_KEY, "");
	if (key == "")
	{
		AosSetEntityError(rdata, "schemaPickConst_miss_key", "schemapicker", conf_name)
			    << conf << enderr;
		return false;
	}
	
	if (mMap.find(key) != mMap.end())
	{
		AosSetEntityError(rdata, "schemaPickConst_key_is_in_map", "schemapicker", conf_name)
			    << conf << enderr;
		return false;
	}

	mMap[key] = schema;
	return true;
}
*/
AosDataRecordObj *
AosSchemaPickVariable::pickSchema(
		AosRundata *rdata,
		const char *data,
		const int rcd_len)
{
	// This schema picker maintains a map. It maps the field contents
	// to a schema. 
	if (rcd_len < mKeyIdx + mKeyLen) return 0;

	OmnString key(&data[mKeyIdx], mKeyLen);
	map<OmnString, AosDataRecordObj *>::iterator itr = mMap.find(key);

	if (itr == mMap.end()) return 0;

	return itr->second;
}


AosJimoPtr 
AosSchemaPickVariable::cloneJimo()  const
{
	try
	{
		return OmnNew AosSchemaPickVariable(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


