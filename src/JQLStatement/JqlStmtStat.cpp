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
// 2014/04/23	Created by Andy Zhang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "JQLStatement/JqlStmtStat.h"

#include "JQLStatement/JqlStmtDataset.h"
#include "AosConf/Stat/StatConf.h"
#include "AosConf/DataSet.h" 
#include "StatUtil/StatMeasure.h"
#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "XmlUtil/XmlTag.h"
#include "alarm_c/alarm.h"

AosJqlStmtStat::AosJqlStmtStat()
{
	mDataSetName = "";
	mStatDocName = "";
	mKeyFields = 0;
}

AosJqlStmtStat::~AosJqlStmtStat()
{
	OmnDelete(mKeyFields);
}

bool
AosJqlStmtStat::run(const AosRundataPtr run(const AosRundataPtr &rdata)rdata, const AosJimoProgObjPtr run(const AosRundataPtr &rdata)prog)
{
	if (mOp == JQLTypes::eRun) return runStat(rdata);

	AosSetEntityError(rdata, "JQL_index_run_err", "JQL Stat", "")      
		        << "opr is undefined!" << enderr;                               
	return false;                                                           
}
    

bool
AosJqlStmtStat::runStat(const AosRundataPtr &rdata)
{
	OmnString msg;
	/*
	AosXmlTagPtr stat_doc = getDocByObjid(mStatDocName);
	if (!stat_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	*/
	OmnString dataset_objid = AosJqlStmtDataset::getObjid(mDataSetName);
	mDatasetDoc = getDocByObjid(dataset_objid);
	if (!mDatasetDoc)
	{
		msg << "Failed to get datasetdoc " << mDataSetName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	bool rslt = AosJqlStmtDataset::convertDatasetConf(rdata, mDatasetDoc);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	AosXmlTagPtr schema = mDatasetDoc->getFirstChild("dataschema");
	if (!schema)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	mRecord = schema->getFirstChild("datarecord");
	if (!mRecord)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr stat_doc = getStatDoc(rdata);
	if (!stat_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr xml = stat_doc->getFirstChild("internal_statistics");
	if (!xml)
	{
		msg << "stat doc error : missing internal_statistics node.";
		rdata->setJqlMsg(msg);
		return false;
	}
	AosXmlTagPtr level0_stat = xml->getFirstChild("statistic");
	if (!level0_stat)
	{
		msg << "stat doc error : missing statistic node.";
		rdata->setJqlMsg(msg);
		return false;
	}

	AosXmlTagPtr level1_stat = xml->getNextChild();

	xml = level0_stat->getFirstChild("measures");
	if (!xml)
	{
		msg << "stat doc error : missing measures node.";
		rdata->setJqlMsg(msg);
		return false;
	}
	AosXmlTagPtr measure = xml->getFirstChild();
	if (!measure)
	{
		msg << "stat doc error : missing measure node.";
		rdata->setJqlMsg(msg);
		return false;
	}

	AosXmlTagPtr level1_measures, level1_measure;
	if (level1_stat)
	{
		level1_measures = level1_stat->getFirstChild("measures");
		if (!level1_measures)
		{
			msg << "stat doc error : missing level1 measures node.";
			rdata->setJqlMsg(msg);
			return false;
		}

		level1_measure = level1_measures->getFirstChild();
		if (!level1_measure)
		{
			msg << "stat doc error : missing level1 measure node.";
			rdata->setJqlMsg(msg);
			return false;
		}
	}

	AosXmlTagPtr root = stat_doc->clone(AosMemoryCheckerArgsBegin);
	if (level1_stat)
	{
		root->setAttr("stat_conf_level", "0-1");
		rslt = procDistinct(root, rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}
	else if (mKeyFields)
	{
		root->setAttr("stat_conf_level", "1");
	}

	OmnString objid, ctnr_objid, jobdoc_str;
	StatConf stat1(mDatasetDoc, root);
	jobdoc_str = stat1.createConfig(objid, ctnr_objid, mMaxThread);
	
	cout << " ===================job_doc============================= " << endl;
	cout << jobdoc_str << endl;
	cout << " ======================================================== " << endl;

	AosXmlTagPtr jobdoc = AosXmlParser::parse(jobdoc_str AosMemoryCheckerArgs);
	if (!jobdoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	// 2014/07/16 : Young
	jobdoc->setAttr("zky_job_name", mStatDocName);
	jobdoc->setAttr("zky_pctrs", "jobctnr");
	jobdoc->setAttr("zky_otype", "job");
	
	if ( !createDoc1(jobdoc_str) )
	{
		msg << "Failed to create Job doc.";
		rdata->setJqlMsg(msg);
		return false;
	}
	
	jobdoc_str = mJmoRundata.str_value;
	jobdoc = AosXmlParser::parse(jobdoc_str AosMemoryCheckerArgs);
	if (!jobdoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	objid = jobdoc->getAttrStr(AOSTAG_OBJID, "");
	if (!objid != "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	msg << "Job " << objid << " created successfully.";
	rdata->setJqlMsg(msg);

	if ( !startJob(objid) )
	{
		msg << "Failed to run Job .";
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "Job " << objid << " run successfully."; 
	rdata->setJqlMsg(msg);

/*
	int i=0;
	while(measure)
	{
		AosXmlTagPtr root = stat_doc->clone(AosMemoryCheckerArgsBegin);
		AosXmlTagPtr tag = root->getFirstChild("internal_statistics");
		AosXmlTagPtr level0_tag = tag->getFirstChild("statistic");
		AosXmlTagPtr level1_tag = tag->getNextChild();
		level0_tag->removeNode("measures", true, true);
		AosXmlTagPtr node = measure->clone(AosMemoryCheckerArgsBegin);
		level0_tag->addNode(node);

		if (level1_tag)
		{
			if (!level1_measure)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}
			level1_tag->removeNode("measures", true, true);
			node =  level1_measure->clone(AosMemoryCheckerArgsBegin);
			level1_tag->addNode(node);
			level1_measure = level1_measures->getNextChild();
			root->setAttr("stat_conf_level", "0-1");
		}
	
		OmnString objid, ctnr_objid, jobdoc_str;
		if (!mKeyFields)
		{
			StatConf stat1(mDatasetDoc, root);
			jobdoc_str = stat1.createConfig(objid, ctnr_objid, i);
		}
		else
		{
			AosXmlTagPtr measure_field = measure->getFirstChild("measure_field");
			if (!measure_field)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}
			OmnString field_name = measure_field->getAttrStr("field_name", "");
			if (!field_name != "")
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}
			//type << "(" << field_name << ")";
			//map<OmnString, AosXmlTagPtr>::iterator itr = mDatasetDocs.find(type);
			map<OmnString, AosXmlTagPtr>::iterator itr = mDatasetDocs.find(field_name);
			if (!itr != mDatasetDocs.end())
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}

			root->setAttr("stat_conf_level", "1");
			StatConf stat1(itr->second, root);
			jobdoc_str = stat1.createConfig(objid, ctnr_objid, i);
		}

		// createDoc
		{
			msg << "Failed to create Job doc.";
			rdata->setJqlMsg(msg);
			return false;
		}
cout << " ===================job_doc============================= " << endl;
cout << jobdoc_str << endl;
cout << " ======================================================== " << endl;

		msg << "Job " << objid << " created successfully.";
		rdata->setJqlMsg(msg);

		if ( !startJob(objid) )
		{
			msg << "Failed to run Job .";
			rdata->setJqlMsg(msg);
			return false;
		}

		msg << "Job " << objid << " run successfully."; 
		rdata->setJqlMsg(msg);

		i++;
		measure = xml->getNextChild();
	}
*/
	return false;
}

bool
AosJqlStmtStat::procDistinct(
		AosXmlTagPtr &stat_doc,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr xml = stat_doc->getFirstChild("internal_statistics");
	if (!xml)
	{
		OmnString msg = "stat doc error : missing internal_statistics node.";
		rdata->setJqlMsg(msg);
		return false;
	}
	
	AosXmlTagPtr statistic = xml->getChildByAttr("has_distinct", "true");
	if (!statistic) return true;

	AosXmlTagPtr key_fields = statistic->getFirstChild("key_fields");
	if (!key_fields)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString field_name, distinct_fields = "";
	AosXmlTagPtr field = key_fields->getFirstChild();
	while(field)
	{
		field_name = field->getAttrStr("field_name", "");
		if (!field_name != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		distinct_fields << field_name << ",";

		field = key_fields->getNextChild();
	}

	AosXmlTagPtr measures = statistic->getFirstChild("measures");
	if (!measures)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr dmeasure = measures->getChildByAttr("zky_isdistinct", "true");
	if (!dmeasure)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr distinct_measure = dmeasure->clone(AosMemoryCheckerArgsBegin);
	if (!distinct_measure)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr agr_type = distinct_measure->getFirstChild("agr_type");
	if (!agr_type)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	field_name = agr_type->getAttrStr("distinct_field", "");
	if (!field_name != "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	distinct_fields << field_name;

	measures->removeNode(dmeasure);

	AosXmlTagPtr distinct_stat = statistic->clone(AosMemoryCheckerArgsBegin);
	if (!distinct_stat)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	distinct_stat->removeAttr("has_distinct");

	distinct_stat->setAttr("zky_isdistinct", "true");
	AosXmlTagPtr distinct_measures = distinct_stat->getFirstChild("measures");
	if (!distinct_measures)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	bool rslt = distinct_measures->removeAllChildren();
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	distinct_measures->addNode(distinct_measure);

	AosXmlTagPtr level0_tag = xml->getChildByAttr("internal_id", "0");
	if (!level0_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	level0_tag->setAttr("distinct_fields", distinct_fields);

	xml->addNode(distinct_stat);
	return true;
}

AosXmlTagPtr
AosJqlStmtStat::getStatDoc(const AosRundataPtr &rdata)
{
	OmnString msg;
	AosXmlTagPtr stat_doc = getDocByObjid(mStatDocName);
	if (!stat_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	if (!mKeyFields) return stat_doc;
    
	AosXmlTagPtr internal_statistics = stat_doc->getFirstChild("internal_statistics");	
	if (!internal_statistics)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr xml = internal_statistics->getFirstChild("statistic");
	if (!xml)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	
	int size = mKeyFields->size();

	AosXmlTagPtr key_fields, init_tag;
	int fields_num;
	while(xml)
	{
		key_fields = xml->getFirstChild("key_fields");
		if (!key_fields)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}

		fields_num = key_fields->getNumSubtags();
		if (fields_num != size) 
		{
			xml = internal_statistics->getNextChild();
			continue;
		}
		
		if (!checkKeyFields(key_fields, rdata))
		{
			xml = internal_statistics->getNextChild();
			continue;
		}
		
		init_tag = xml->clone(AosMemoryCheckerArgsBegin);
		if (!init_tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		
		break;
	}
	
	if (!init_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	int internal_id = init_tag->getAttrInt("internal_id", -1);
	if (!internal_id != -1)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	
	mPreLevel = "";
	mPreLevel << internal_id -1;
	AosXmlTagPtr pre_level_tag  = internal_statistics->getChildByAttr("internal_id", mPreLevel);
	if (!pre_level_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	bool rslt = getDatasetDocs(pre_level_tag, stat_doc, rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	rslt = internal_statistics->removeAllChildren();
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr measures = init_tag->getFirstChild("measures");
	if (!measures)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr measure = measures->getFirstChild("measure");
	if (!measure)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}


	AosXmlTagPtr measure_field, agr_type;
	OmnString field_name, type, key;

	while(measure)
	{
		measure_field = measure->getFirstChild("measure_field");
		if (!measure_field)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		field_name = measure_field->getAttrStr("field_name", "");
		if (!field_name != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}

		agr_type = measure->getFirstChild("agr_type");
		if (!agr_type)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		type = agr_type->getAttrStr("type", "");
		if (!type != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}

		key = type;
		key << "(" << field_name << ")";
		AosJqlUtil::escape(key); 
		
		measure_field->setAttr("field_name", key);

		measure = measures->getNextChild();
	}
	internal_statistics->addNode(init_tag);

	return stat_doc;
}

bool
AosJqlStmtStat::getDatasetDocs(
		const AosXmlTagPtr &tag,
		const AosXmlTagPtr &stat_doc,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr vector2d = stat_doc->getFirstChild("vector2d");
	if (!vector2d)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString vector2d_str = vector2d->toString();
	OmnString time_unit = stat_doc->getAttrStr("time_unit", "");
	if (!time_unit != "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	OmnString time_field_name = stat_doc->getAttrStr("time_field_name", "");
	if (!time_field_name != "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	vector<string> field_names;

	AosXmlTagPtr key_fields = tag->getFirstChild("key_fields");
	if (!key_fields)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr field = key_fields->getFirstChild("field");
	if (!field)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	while(field)
	{
		string field_name = field->getAttrStr("field_name", "");
		if (!field_name != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		field_names.push_back(field_name);
		field = key_fields->getNextChild();
	}

	AosXmlTagPtr measures = tag->getFirstChild("measures");
	if (!measures)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr measure = measures->getFirstChild("measure");
	if (!measure)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr measure_field, agr_type, dataset_doc, xml;
	OmnString field_name, type, key;
	//map<OmnString, AosXmlTagPtr>::iterator itr;
	//mDatasetDocs.clear();

	AosXmlTagPtr data_record = mRecord->clone(AosMemoryCheckerArgsBegin);
	if (!data_record)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
//	bool rslt;
	map<OmnString, vector<OmnString> >  func_fields;
	map<OmnString, vector<OmnString> >::iterator func_itr;
	while(measure)
	{
		measure->setAttr("time_unit", time_unit);				
		
		measure_field = measure->getFirstChild("measure_field");
		if (!measure_field)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		field_name = measure_field->getAttrStr("field_name", "");
		if (!field_name != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		agr_type = measure->getFirstChild("agr_type");
		if (!agr_type)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		type = agr_type->getAttrStr("type", "");
		if (!type != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		
		key = type;
		key << "(" << field_name << ")";
		AosJqlUtil::escape(key); 

		func_itr = func_fields.find(field_name);
		if (func_itr == func_fields.end())
		{
			vector<OmnString> v;                                           
			pair<map<OmnString, vector<OmnString> >::iterator, bool> itr_rslt;
			itr_rslt = func_fields.insert(make_pair(field_name, v));
			if (!itr_rslt.second)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}
			func_itr = itr_rslt.first;
		}
		if (!func_itr != func_fields.end())
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		(func_itr->second).push_back(key);
		//xml = data_record->getFirstChild("datafields");
		//if (!xml)
		//{
		//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//	OmnAlarm << __func__ << enderr;
		//	return false;
		//}

		//xml = xml->getChildByAttr("zky_name", field_name);
		//if (!xml)
		//{
		//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//	OmnAlarm << __func__ << enderr;
		//	return false;
		//}

		//xml->setAttr("zky_name", key);

		//itr = mDatasetDocs.find(key);
		//if (!itr == mDatasetDocs.end())
		//{
		//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//	OmnAlarm << __func__ << enderr;
		//	return false;
		//}

		//mDatasetDocs.insert(make_pair(key, dataset_doc));

		measure = measures->getNextChild();
	}
	
	bool rslt = getStatRecord(data_record, field_names, time_field_name, func_fields);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	mDatasetDoc = createDataset(field_names, measures, time_field_name, vector2d_str, data_record, rdata);
	if (!mDatasetDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	return true;
}

bool
AosJqlStmtStat::getStatRecord(
		AosXmlTagPtr &data_record,
		const vector<string> &field_names,
		const OmnString &time_field_name,
		map<OmnString, vector<OmnString> > &func_fields)
{
	data_record->setAttr("type", "buff");
	//data_record->removeAttr("zky_length");
	AosXmlTagPtr fields = data_record->getFirstChild("datafields");
	if (!fields)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	vector<AosXmlTagPtr> data_fields;
	AosXmlTagPtr tag;
	for(size_t i=0; i<field_names.size(); i++)
	{
		tag = fields->getChildByAttr("zky_name", field_names[i]);
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		tag = tag->clone(AosMemoryCheckerArgsBegin);
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		tag->setAttr("type", "buff");
		//tag->removeAttr("zky_offset");
		//tag->removeAttr("zky_length");
		data_fields.push_back(tag);
	}
	
	// time_field_name
	tag = fields->getChildByAttr("zky_name", time_field_name);
	if (!tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	tag = tag->clone(AosMemoryCheckerArgsBegin);
	if (!tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	tag->setAttr("type", "buff");
	//tag->removeAttr("zky_offset");
	//tag->removeAttr("zky_length");
	data_fields.push_back(tag);
	
	// field_name
	map<OmnString, vector<OmnString> >::iterator itr = func_fields.begin();
	AosXmlTagPtr tmp_node;
	vector<OmnString> v;
	for(; itr != func_fields.end(); ++itr)
	{
		if (itr->first != "0x2a")
		{
			tag = fields->getChildByAttr("zky_name", itr->first);
			if (!tag)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}
		}

		tag = tag->clone(AosMemoryCheckerArgsBegin);
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		tag->setAttr("type", "buff");
		tag->removeAttr("zky_offset");
		tag->removeAttr("zky_length");

		v = itr->second;
		for(size_t k=0; k<v.size(); k++)
		{
			tmp_node = tag->clone(AosMemoryCheckerArgsBegin);
			if (!tmp_node)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}
			tmp_node->setAttr("zky_name", v[k]);
			data_fields.push_back(tmp_node);
		}
	}

	fields->removeAllChildren();
	for(size_t i=0; i<data_fields.size(); i++)
	{
		fields->addNode(data_fields[i]);
	}
	return true;
}

AosXmlTagPtr
AosJqlStmtStat::createDataset(
		const vector<string> &field_names,
		const AosXmlTagPtr &measures,
		const OmnString &time_field_name,
		const OmnString &vector2d_str,
		const AosXmlTagPtr &data_record,
		const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataSet> data_set = boost::make_shared<DataSet>();                   
	data_set->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");

	OmnString name = mStatDocName;
	name << "_level1_task001_input_dataset_all";
	data_set->setAttribute("zky_name", name);

	boost::shared_ptr<DataScanner> data_scanner = boost::make_shared<DataScanner>();        
	bool rslt = createDataScanner(field_names, measures, vector2d_str, data_scanner);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	data_set->setScanner(data_scanner);

	boost::shared_ptr<DataSchemaInternalStat> data_schema = boost::make_shared<DataSchemaInternalStat>();
	rslt = createDataSchema(field_names, measures, time_field_name, data_record, data_schema);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	data_set->setSchema(data_schema);

	OmnString dataset_str = data_set->getConfig();
	AosXmlTagPtr dataset_doc = AosXmlParser::parse(dataset_str AosMemoryCheckerArgs);
	if (!dataset_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	return dataset_doc;
}

bool
AosJqlStmtStat::createDataSchema(
		const vector<string> &field_names,
		const AosXmlTagPtr &measures,
		const OmnString &time_field_name,
		const AosXmlTagPtr &data_record,
		boost::shared_ptr<DataSchemaInternalStat> &data_schema)
{
	OmnString name = mStatDocName;
	name << "_level1_task001_schema_all";
	data_schema->setAttribute("zky_name", name);

	name = mStatDocName;
	name << "_level1_task001_input_rcd_all";
	boost::shared_ptr<DataRecord> record = boost::make_shared<DataRecord>(data_record);     
	record->setAttribute("zky_name", name);          
	data_schema->setRecord(record);
	
	data_schema->setStatKeyNum(field_names.size());
	
	vector<StatFieldDef> all_field_defs;
	bool rslt = initAllStatFieldDefs(field_names, measures, time_field_name,
			data_record, all_field_defs);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	for(u32 i=0; i<all_field_defs.size(); i++)
	{
		data_schema->addStatFieldDef(all_field_defs[i].toXmlString());
	}
	
	//for(u32 i=0; i<field_names.size(); i++)
	//{
	//	data_schema->addStatKey(field_names[i]);
	//}
	//data_schema->setTimeFieldName(time_field_name);

	AosXmlTagPtr measure = measures->getFirstChild("measure");
	if (!measure)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	while(measure)
	{
		data_schema->addMeasure(measure->toString());
		measure = measures->getNextChild();
	}
	return true;
}


bool
AosJqlStmtStat::initAllStatFieldDefs(
		const vector<string> &stat_keys,
		const AosXmlTagPtr &measures,
		const OmnString &time_field_name,
		const AosXmlTagPtr &data_record,
		vector<StatFieldDef> &all_field_defs)
{
	OmnString field_name;
	StatFieldDef field_def;
	bool rslt, inited;

	AosXmlTagPtr data_fields = data_record->getFirstChild("datafields");
	if (!data_fields)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}	
	AosXmlTagPtr field_conf = data_fields->getFirstChild();	
	u32 field_idx = 0;
	while(field_conf)
	{
		field_name = field_conf->getAttrStr("zky_name");
		if (!field_name != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		
		rslt = initStatFieldDef(field_name, field_idx, field_def, inited,
				stat_keys, measures, time_field_name);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		if(inited) all_field_defs.push_back(field_def);	

		field_idx++;
		field_conf = data_fields->getNextChild();
	}
	return true;
}

bool
AosJqlStmtStat::initStatFieldDef(
		const OmnString &fname,
		const u32 field_idx,
		StatFieldDef &field_def,
		bool &inited,
		const vector<string> &stat_keys,
		const AosXmlTagPtr &measures,
		const OmnString &time_field_name)
{
	inited = true;
	if (!fname != "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	int idx;
	field_def.mFname = fname;
	field_def.mFieldIdx = field_idx;
	if(fname == time_field_name)
	{
		field_def.mFieldType = StatFieldDef::eTime;
		field_def.mStatFieldIdx = 0;
		return true;
	}

	if(isStatKeyField(stat_keys, fname, idx))
	{
		if (!idx >= 0)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		field_def.mFieldType = StatFieldDef::eStatKey;
		field_def.mStatFieldIdx = idx;
		return true;
	}
	
	if(isMeasureField(measures, fname, idx))
	{
		if (!idx >= 0)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		
		field_def.mFieldType = StatFieldDef::eMeasure;
		field_def.mStatFieldIdx = idx;
		return true;	
	}
	
	inited = false;
	return true;
}

bool
AosJqlStmtStat::isStatKeyField(
		const vector<string> &stat_keys,
		const string &fname,
		int &idx)
{
	idx = -1;	
	for(u32 i=0; i<stat_keys.size(); i++)
	{
		if(stat_keys[i] == fname)
		{
			idx = i;
			return true;
		}
	}
	return false;
}


bool
AosJqlStmtStat::isMeasureField(
		const AosXmlTagPtr &all_measures,
		const OmnString &fname,
		int &idx)
{
	idx = -1;

	AosXmlTagPtr measure_def = all_measures->getFirstChild(true);
	OmnString output_fname;
	u32 i = 0;
	while(measure_def)
	{
		output_fname = AosStatMeasure::parseOutputMeasureField(measure_def);
	
		if(output_fname == fname)
		{
			idx = i; 
			return true;
		}

		i++;
		measure_def = all_measures->getNextChild();
	}
	return false;
}


bool
AosJqlStmtStat::createDataScanner(
		const vector<string> &field_names,
		const AosXmlTagPtr &measures,
		const OmnString &vector2d_str,
		boost::shared_ptr<DataScanner> &data_scanner)
{
	data_scanner->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");  
	
	OmnString name = mStatDocName;
	name << "_level1_task001_datascanner_all";
	data_scanner->setAttribute("zky_name", name); 

	OmnString iilname = "_zt44_";
	iilname << mStatDocName << "_statinternalid";
	boost::shared_ptr<DataConnectorStatQuery> data_conn = boost::make_shared<DataConnectorStatQuery>();     
	OmnString cond_str = "<conds><cond type=\"AND\">";
	cond_str << "<term type=\"jimo\" reverse=\"false\" order=\"false\" classname=\"AosTermArithNew\" "
			 << "iilname=\"" << iilname << "\" zky_opr=\"eq\">"
			 << "<value1 var_type=\"literal\"><![CDATA[" << mPreLevel << "]]></value1>"
			 << "</term>"
			 << "</cond></conds>";
	data_conn->addConds(cond_str);
	data_conn->addGlobalVt2dConf(vector2d_str);
	
	// Ketty 2014/06/19
	data_conn->setIsInternalStat(true);
	data_conn->setIsNeedKeyGroupby(false);
	//for(u32 i=0; i<field_names.size(); i++)
	//{
	//	data_conn->addStatKey(field_names[i]);
	//	data_conn->addGroupByKeyField(field_names[i]);
	//}

	AosXmlTagPtr measure = measures->getFirstChild("measure");
	if (!measure)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString time_unit = measure->getAttrStr("time_unit", "");
	if (!time_unit != "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	while(measure)
	{
		data_conn->addMeasure(measure->toString());
		measure = measures->getNextChild();
	}
	data_conn->addGroupByTimeUnit(time_unit);
	data_scanner->setConnector(data_conn);
	return true;	
}

bool
AosJqlStmtStat::checkKeyFields(
		const AosXmlTagPtr &tag,
		const AosRundataPtr &rdata)
{
	AosValueRslt vv;
	OmnString v;
	AosXmlTagPtr field_tag;
	for(size_t i=0; i<mKeyFields->size(); i++)
	{
		bool rslt = (*mKeyFields)[i]->getValue(rdata.getPtr(), 0, vv);	
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		v = vv.getStr();
		if (!v != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		AosXmlTagPtr field_tag = tag->getChildByAttr("field_name", v);
		if (!field_tag) return false;
	}

	return true;
}

AosJqlStatement *
AosJqlStmtStat::clone()
{
	return OmnNew AosJqlStmtStat(*this);
}


void 
AosJqlStmtStat::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtStat::setDataSetName(OmnString name)
{
	mDataSetName = name;
}


void 
AosJqlStmtStat::setStatDocName(OmnString name)
{
	mStatDocName = name;
}

void
AosJqlStmtStat::setKeyFields(AosExprList* key_fields)
{
	mKeyFields = key_fields;
}
#endif
