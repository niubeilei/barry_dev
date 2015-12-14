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
#include "SchemaPicker/SchemaPickFixBin.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "API/AosApi.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaPickFixBin_0(
 		const AosRundataPtr &rdata, 
 		const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchemaPickFixBin(version);
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



AosSchemaPickFixBin::AosSchemaPickFixBin(const u32 version)
:
AosSchemaPicker(AOSSCHEMAPICKER_FIXBIN, version)
{
	/*
	if (!config(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	*/
}


AosSchemaPickFixBin::~AosSchemaPickFixBin()
{
}


bool
AosSchemaPickFixBin::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	// 	<conf start_pos="xxx" len="xxx">
	// 	</conf>
	
	aos_assert_r(worker_doc, false);
	mKeyIdx = worker_doc->getAttrInt("start_pos", -1);
	//aos_assert_r(mKeyIdx != -1, false);
	if (mKeyIdx == -1)
	{
		AosSetEntityError(rdata, "schemaPickConst_miss_key_index", "schemapicker", "")
			    << worker_doc<< enderr;
		return false;
	}

	mKeyLen = worker_doc->getAttrInt("length", -1);
	//aos_assert_r(mKeyLen != -1, false);
	if (mKeyLen == -1)
	{
		AosSetEntityError(rdata, "schemaPickConst_miss_key_length", "schemapicker", "")
			    << worker_doc << enderr;
		return false;
	}
/*
	AosXmlTagPtr entry_conf = worker_doc->getFirstChild("rawdata_parse");
	if (!entry_conf)
	{
		AosSetEntityError(rdata, "schemaPickConst_miss_entries", "schemapicker", "")
			    << worker_doc << enderr;
		return false;
	}

	mEntryConf = entry_conf->clone(AosMemoryCheckerArgsBegin);
*/
	return true;
}

void
AosSchemaPickFixBin::reset()
{
	mMap.clear();	
}
/*
bool
AosSchemaPickFixBin::holdRecords(
		const vector<AosDataRecordObjPtr> &records,
		AosRundata *rdata)
{
	aos_assert_r(records.size() > 0, false);

	map<OmnString, AosDataRecordObj *> rcdMap;
	bool rslt = getMap(records, rcdMap);
	aos_assert_r(rslt, false);
	aos_assert_r(rcdMap.size() > 0, false);
	
	AosXmlTagPtr tag = mEntryConf->getFirstChild("entry");
	OmnString key, record_key;
	int len;
	map<OmnString, AosDataRecordObj *>::iterator itr;
	while(tag)
	{
		// <entry key="vt"  len="752"  record_key="vc"/>
		key = tag->getAttrStr("key");
		aos_assert_r(key != "", false);

		len = tag->getAttrInt("len", 0);
		aos_assert_r(len > 0, false);

		record_key = tag->getAttrStr("zky_input_record_name");
		aos_assert_r(record_key != "", false);

		itr = rcdMap.find(record_key);
		aos_assert_r(itr != rcdMap.end(), false);

		key << "_" << len;
		mMap[key] = itr->second;

		tag = mEntryConf->getNextChild("entry");
	}
	
	return true;
}

bool 
AosSchemaPickFixBin::getMap(
		const vector<AosDataRecordObjPtr> &records,
		map<OmnString, AosDataRecordObj *> &rcdMap)
{
	OmnString key;
	for(size_t i=0; i<records.size(); i++)
	{
		key = records[i]->getRecordName();
		aos_assert_r(key != "", false);
		rcdMap[key]= records[i].getPtr();
	}

	return true;
}

AosDataRecordObj *
AosSchemaPickFixBin::pickSchema(
		AosRundata *rdata,
		const char *data,
		const int rcd_len)
{
	// This schema picker maintains a map. It maps the field contents
	// to a schema. 
	if (rcd_len < mKeyIdx + mKeyLen) return 0;

	OmnString key(&data[mKeyIdx], mKeyLen);
	key << "_" << rcd_len;
	map<OmnString, AosDataRecordObj *>::iterator itr = mMap.find(key);
	if (itr == mMap.end())
	{
		// This is a special case for Anhui Unicom!!!!!!!!!!!!!!!
		key.assign(&data[mKeyIdx], mKeyLen);
		key << "_" << (rcd_len + 40);
		itr = mMap.find(key);
	}

	if (itr == mMap.end()) return 0;

	return itr->second;
}
*/

bool
AosSchemaPickFixBin::addSchemaIndex(
		const AosXmlTagPtr &tag,
		int index)
{
	OmnString key = tag->getAttrStr("key");
	aos_assert_r(key != "", false);

	int len = tag->getAttrInt("len", 0);
	aos_assert_r(len > 0, false);

	key << "_" << len;
	mMap[key] = index;

	return true;
}

int
AosSchemaPickFixBin::pickSchemaIndex(
		AosRundata *rdata,
		const char *data,
		const int rcd_len)
{
	// This schema picker maintains a map. It maps the field contents
	// to a schema. 
	if (rcd_len < mKeyIdx + mKeyLen) return -1;

	OmnString key(&data[mKeyIdx], mKeyLen);
	key << "_" << rcd_len;
	map<OmnString, int>::iterator itr = mMap.find(key);
	if (itr == mMap.end())
	{
		// This is a special case for Anhui Unicom!!!!!!!!!!!!!!!
		key.assign(&data[mKeyIdx], mKeyLen);
		key << "_" << (rcd_len + 40);
		itr = mMap.find(key);
	}

	if (itr == mMap.end()) return -1;

	return itr->second;
}

AosJimoPtr 
AosSchemaPickFixBin::cloneJimo()  const
{
	try
	{
		return OmnNew AosSchemaPickFixBin(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
