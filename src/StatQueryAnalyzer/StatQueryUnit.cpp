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

#include "StatQueryAnalyzer/StatQueryUnit.h"

#include "CounterUtil/CounterUtil.h"
#include "SEInterfaces/DatasetObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/QueryTermObj.h"
#include "Util/ValueRslt.h"
#include "Util/StrParser.h"
#include "StatUtil/StatDefineDoc.h"

AosStatQueryUnit::AosStatQueryUnit(
		const AosStatDefineDocPtr &stat_def,
		vector<AosQrUtil::FieldDef> &select_fields)
:
mStatDef(stat_def),
mSelectedFields(select_fields),
mGroupByTimeUnit(AosStatTimeUnit::eInvalid)
{
}


AosStatQueryUnit::~AosStatQueryUnit()
{
}


bool
AosStatQueryUnit::initMeasureInfo(
		set<AosStatMeasure> &measures,
		vector<OmnString> &orig_grpby_fields)
{
	set<AosStatMeasure>::iterator itr = measures.begin();
	for(; itr != measures.end(); itr++)
	{
		mMeasures.push_back(*itr);
	}
	
	bool rslt = mStatDef->adjustGroupByFields(orig_grpby_fields,
			mGroupByKeyFields, mGroupByTimeUnit);
	aos_assert_r(rslt, false);
	if(!AosStatTimeUnit::isValid(mGroupByTimeUnit))
	{
		mGroupByTimeUnit = AosStatTimeUnit::eAllTime;
	}

	return true;
}

bool
AosStatQueryUnit::initStatKeyFields(AosInternalStat &stat)
{
	mStatKeys = stat.getKeyFields();
	return true;
}


bool
AosStatQueryUnit::initStatQryTimeConds(
		const AosRundataPtr &rdata,
		vector<AosQueryTermObjPtr> &cond_terms)
{
	/*
	int start_days[2], end_days[2];
	int num_matched;
	bool rslt = cond_terms[0]->getTimeCond(rdata, "_time", num_matched, start_days, end_days);
	aos_assert_r(rslt && num_matched <=2 , 0);

	mValueNum = 0;
	for(int i=0; i<num_matched; i++)
	{
		AosStatTimeArea info;
		info.start_time = start_days[i];
		info.end_time = end_days[i];
		mStatQryConds.push_back(info);
		
		mValueNum += (end_days[i] - start_days[i] + 1); 
	}
	*/
		
	bool rslt;
	AosStatTimeArea cond;
	cond.time_unit = mStatDef->getTimeUnit();
	
	aos_assert_r(cond_terms.size() <= 2, false);
	for(u32 i=0; i<cond_terms.size(); i++)
	{
		aos_assert_r(cond_terms[i]->isTimeTerm(), false);
		rslt = cond_terms[i]->initStatTimeCond(rdata, cond);
		aos_assert_r(rslt, false);
	}
	if(cond_terms.size())	mStatQryConds.push_back(cond);

	aos_assert_r(rslt, false);
	return true;
}


bool
AosStatQueryUnit::initStatQryValueConds(
		const AosRundataPtr &rdata,
		vector<AosQueryTermObjPtr> &cond_terms)
{
	if(cond_terms.size() != 0)
	{
		OmnNotImplementedYet;
		return false;
	}
	return true;
}


bool
AosStatQueryUnit::initStatDataset(
		const AosRundataPtr &rdata,
		const u32 docid_num,
		const AosBuffPtr &docids_buff)
{
	u64 t1 = OmnGetTimestamp(); 
	AosXmlTagPtr dataset_xml = generateDatasetXml(rdata, docid_num, docids_buff);
	aos_assert_r(dataset_xml, false);

	mStatDataset = AosCreateDataset(rdata, dataset_xml);
	aos_assert_r(mStatDataset, false);
	
	OmnScreen << "!!!!!!!! init dataset time:" << OmnGetTimestamp() - t1 << endl;
	return true;
}


AosXmlTagPtr
AosStatQueryUnit::generateDatasetXml(
		const AosRundataPtr &rdata,
		const u32 docid_num,
		const AosBuffPtr &docids_buff)
{
	OmnString dataset_str = "<dataset zky_objid=\"stat_dataset\" zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_dataset\" jimo_type=\"jimo_dataset\" current_version=\"0\" zky_classname=\"AosDatasetByStat\" >";
	dataset_str << "<versions><ver_0>libDatasetJimos.so</ver_0></versions>"
			<< "<recordset />"
			<< createSchemaConfStr(rdata) 
			<< "<datascanner buff_cache_num=\"1\" zky_objid=\"stat_datascanner\" zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_data_scanner\" jimo_type=\"jimo_data_scanner\" current_version=\"0\" zky_classname=\"AosDataScannerConnector\" >"
			<< "<versions><ver_0>libScannerJimos.so</ver_0></versions>"
			<< "<dataconnector doc_type=\"stat_doc\" zky_objid=\"jimo_datacube_readdoc_by_sort\" zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" jimo_type=\"jimo_datacube\" current_version=\"0\" zky_classname=\"AosDataCubeReadDocBySort\" >"
			<< "<versions><ver_0>libDataCubicJimos.so</ver_0></versions>";

	dataset_str << "<docids num=\"" << docid_num << "\" >";
	AosXmlTag::addBinaryData(dataset_str, "id_buff", docids_buff);
	dataset_str << "</docids>";
	dataset_str << createStatConfStr(rdata);
	dataset_str << "</dataconnector>";
	dataset_str << "</datascanner>";
	dataset_str << "</dataset>";

	AosXmlTagPtr dataset_xml = AosXmlParser::parse(dataset_str AosMemoryCheckerArgs);
	aos_assert_r(dataset_xml, 0);
	return dataset_xml;
}

	
OmnString
AosStatQueryUnit::createSchemaConfStr(const AosRundataPtr &rdata)
{
	OmnString str = "<dataschema zky_objid=\"jimo_schema_unilength\" zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_schema\" jimo_type=\"jimo_schema\" current_version=\"0\" zky_classname=\"AosSchemaStatRecord\" >";
	
	str << "<versions><ver_0>libSchemaJimos.so</ver_0></versions>";


	str << "<datarecord type=\"ctnr\" zky_name=\"stat_rcd\" >"
		<< "<datarecord type=\"buff\" zky_name=\"stat_rcd\" >"
		<< "<datafields>";
	for(u32 i=0; i<mSelectedFields.size(); i++)
	{
		if(mSelectedFields[i].isTimeField())
		{
			str << "<element type=\"u64\" zky_name=\"_time\" />";
			continue;
		}
		
		if(!mSelectedFields[i].isStatValueField())
		{
			str << "<element type=\"varbuff\" zky_name=\"" << mSelectedFields[i].oname << "\" />";
			continue;
		}

		str << "<element type=\"int64\" zky_name=\"" << mSelectedFields[i].oname << "\" />";
	}

	str << "</datafields>"
		<< "</datarecord>"
		<< "</datarecord>";


	str << "<stat_keys>";
	for(u32 i=0; i<mStatKeys.size(); i++)
	{
		str << "<key_field name=\"" << mStatKeys[i] << "\" />";
	}
	str << "</stat_keys>";

	str << "<measures>";
	for(u32 i=0; i<mMeasures.size(); i++)
	{
		str << mMeasures[i].toXmlString(); 
	}
	str << "</measures>";

	str << "<group_by_keys>";
	for(u32 i=0; i<mGroupByKeyFields.size(); i++)
	{
		str << "<field field_name=\"" << mGroupByKeyFields[i] << "\" />";
	}
	str << "</group_by_keys>"; 
			
	str << "</dataschema>";
	
	return str;
}


OmnString
AosStatQueryUnit::createStatConfStr(const AosRundataPtr &rdata)
{
	AosXmlTagPtr stat_def_conf = mStatDef->getGlobalVt2dConf(rdata);
	aos_assert_r(stat_def_conf, "");
	OmnString str = "<stat_config>";
	str << "<stat_def_conf_ctnr>" << stat_def_conf->toString() << "</stat_def_conf_ctnr>";
	
	str << "<stat_keys>";
	for(u32 i=0; i<mStatKeys.size(); i++)
	{
		str << "<key_field name=\"" << mStatKeys[i] << "\" />";
	}
	str << "</stat_keys>";


	str << "<measures>";
	for(u32 i=0; i<mMeasures.size(); i++)
	{
		str << mMeasures[i].toXmlString(); 
	}
	str << "</measures>";
	
	str << "<group_by_keys>";
	for(u32 i=0; i<mGroupByKeyFields.size(); i++)
	{
		str << "<field field_name=\"" << mGroupByKeyFields[i] << "\" />";
	}
	str << "</group_by_keys>"; 

	str << "<group_by_time time_unit=\"" << AosStatTimeUnit::toStr(mGroupByTimeUnit) << "\" />";
	
	str << "<times>";
	for(u32 i=0; i<mStatQryConds.size(); i++)
	{
		str << mStatQryConds[i].toXmlString();
	}
	str << "</times>";

	str << "</stat_config>";
	return str;
}


bool
AosStatQueryUnit::run(const AosRundataPtr &rdata)
{
	aos_assert_r(mStatDataset, false);
	mStatDataset->sendStart(rdata);
	
	return true;
}


bool
AosStatQueryUnit::generateRslt(
		const AosRundataPtr &rdata,
		OmnString &content)
{
	bool rslt;
	u32 rcd_num = 0;
	OmnString rcds_str;

	AosRecordsetObjPtr record_set;
	AosDataRecordObj *data_rcd;
	while (1)
	{
		rslt = mStatDataset->nextRecordset(rdata, record_set);
		aos_assert_r(rslt, false);

		if (AosRecordsetObj::checkEmpty(record_set)) break;
		
		while (1)
		{
			rslt = record_set->nextRecord(rdata, data_rcd);
			aos_assert_r(rslt, false);
			if (!data_rcd) break;
		
			rslt = generateEachEntry(rdata, data_rcd, rcds_str);	
			aos_assert_r(rslt, false);

			rcd_num++;
		}
	}

	content = "<Contents total=\"";
	content << rcd_num << "\">"
		<< rcds_str
		<< "</Contents>";
	
	content << "</Contents>";
	return true;
}


bool
AosStatQueryUnit::generateEachEntry(
		const AosRundataPtr &rdata,
		AosDataRecordObj *data_rcd,
		OmnString &rcd_str)
{
	OmnAlarm << "finish!" << enderr;
	return true;
}


