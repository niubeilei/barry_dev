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
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "Schema/Jimos/SchemaStatRecord.h"

#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "StatUtil/StatUtil.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaStatRecord_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchemaStatRecord(version);
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


AosSchemaStatRecord::AosSchemaStatRecord(
		const OmnString &type,
		const int version)
:
AosSchema(type, version)
{
}


AosSchemaStatRecord::AosSchemaStatRecord(const int version)
:
AosSchema(AOSSCHEMA_STAT_RECORD, version)
{
}

AosSchemaStatRecord::~AosSchemaStatRecord()
{
}


AosJimoPtr 
AosSchemaStatRecord::cloneJimo() const
{
	try
	{
		return OmnNew AosSchemaStatRecord(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosSchemaStatRecord::config( 
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	bool rslt = initDataRecord(rdata, worker_doc); 
	aos_assert_r(rslt, false);
	
	rslt = initStatFieldDef(worker_doc);
	aos_assert_r(rslt, false);
	
	rslt = initRcdFieldDef();
	aos_assert_r(rslt, false);

	return true;
}


bool
AosSchemaStatRecord::initDataRecord(const AosRundataPtr &rdata, const AosXmlTagPtr &conf)
{
	AosXmlTagPtr rcd_conf = conf->getFirstChild("datarecord");
	if (!rcd_conf)
	{
		OmnAlarm << "missing schema record" << enderr;
		return false;
	}
	
	OmnString rcd_ctnr_conf_str = "<record_ctnr type=\"ctnr\" zky_name=\"stat_rcd_ctnr\">"; 
	rcd_ctnr_conf_str << rcd_conf->toString()
		<< "</record_ctnr>";
	
	AosXmlTagPtr rcd_ctnr_conf = AosXmlParser::parse(rcd_ctnr_conf_str AosMemoryCheckerArgs);
	aos_assert_r(rcd_ctnr_conf, false);	

	mDataRecord = AosDataRecordObj::createDataRecordStatic(rcd_ctnr_conf, 0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(mDataRecord, false);
	mDataRecordRaw = mDataRecord.getPtr();
	OmnString rcd_name = rcd_conf->getAttrStr(AOSTAG_NAME, "");
	if (rcd_name != "")
	{
		mDataRecordRaw->setRecordName(rcd_name);
	}
	
	return true;
}


bool
AosSchemaStatRecord::initStatFieldDef(const AosXmlTagPtr &conf)
{
	AosXmlTagPtr stat_field_confs = conf->getFirstChild("stat_field_def");
	aos_assert_r(stat_field_confs , false);
	
	bool rslt;
	AosStatFieldDef field_def;
	AosXmlTagPtr field_def_conf = stat_field_confs->getFirstChild(true);
	while(field_def_conf)
	{
		rslt = field_def.config(field_def_conf);
		aos_assert_r(rslt, false);
		
		mStatFieldDefs.push_back(field_def);

		field_def_conf = stat_field_confs->getNextChild();
	}
	
	return true;
}


bool
AosSchemaStatRecord::initRcdFieldDef()
{
	map<OmnString, u32> idx_map;
	map<OmnString, u32>::iterator m_itr;
	for(u32 i=0; i<mStatFieldDefs.size(); i++)
	{
		idx_map.insert(make_pair(mStatFieldDefs[i].mFname, i));
	}

	u32 raw_idx;
	AosDataFieldObj *data_field;
	u32 field_num = mDataRecordRaw->getNumFields();
	OmnString field_name;
	for(u32 i=0; i<field_num; i++)
	{
		data_field = mDataRecordRaw->getFieldByIdx1(i);
		aos_assert_r(data_field, false);

		field_name = data_field->getName();
		aos_assert_r(field_name != "", false);	
		
		m_itr = idx_map.find(field_name);	
		aos_assert_r(m_itr != idx_map.end(), false);
		raw_idx = m_itr->second;
		aos_assert_r(raw_idx < mStatFieldDefs.size(), false);

		mRcdFieldDefs.push_back(mStatFieldDefs[raw_idx]);
	}
	
	return true;
}


bool
AosSchemaStatRecord::getNextRecord(
		const AosRundataPtr rdata,
		AosBuff * rcd_buff_data,
		AosDataRecordObj *record)
{
	//felicia, 2014/09/18
	//record->clear();
	
	AosValueRslt value_rslt;
	bool rslt, outofmem = false;
	for(u32 idx=0; idx<mRcdFieldDefs.size(); idx++)
	{
		rslt = getEntryFieldValue(rdata, rcd_buff_data, idx, value_rslt);
		aos_assert_r(rslt, false);
		
		outofmem = false;
		rslt = record->setFieldValue(idx, value_rslt, outofmem,
				rdata.getPtr());
		aos_assert_r(rslt, false);
		// aos_assert_r(!outofmem, false);
	}
	record->flush();
	return true;
}

bool
AosSchemaStatRecord::getEntryFieldValue(
		const AosRundataPtr &rdata,
		AosBuff * rcd_buff_data,
		const u32 idx,
		AosValueRslt &value_rslt)
{
	aos_assert_r(idx < mRcdFieldDefs.size(), false);
	
	char * entry = rcd_buff_data->data();
	if(mRcdFieldDefs[idx].mFieldType == AosStatFieldDef::eStatKey)
	{
		u32 key_pos = *(u32 *)(entry+ mRcdFieldDefs[idx].mFieldPos); 
		aos_assert_r(key_pos < rcd_buff_data->dataLen(), false);
		
		rcd_buff_data->setCrtIdx(key_pos);
		OmnString key_value = rcd_buff_data->getOmnStr("");
		aos_assert_r(key_value != "", false);

		value_rslt.setStr(key_value);
		return true;
	}
		
	if(mRcdFieldDefs[idx].mFieldType == AosStatFieldDef::eTime)
	{
		int64_t epoch_time = *(int64_t *)(entry + mRcdFieldDefs[idx].mFieldPos);	
		return getCalendarTime(epoch_time, value_rslt);	
	}

	// this is measures.
	char * data = entry + mRcdFieldDefs[idx].mFieldPos; 
	bool rslt = AosStatUtil::setStatValueToValueRslt(rdata, data, mRcdFieldDefs[idx].mDataType, value_rslt);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosSchemaStatRecord::getCalendarTime(
		const int64_t epoch_time,
		AosValueRslt &value_rslt)
{
	value_rslt.setI64(epoch_time);
	return true;
}


bool
AosSchemaStatRecord::getRecord(const OmnString &name, AosDataRecordObjPtr &record)
{
	record = mDataRecord;
	return true;
	//record = 0;
	//aos_assert_r(mDataRecord, false);
	//OmnString key = mDataRecord->getRecordName();
	//if (name == key)
	//{
	//	record = mDataRecord;
	//}
	//return true;
}


