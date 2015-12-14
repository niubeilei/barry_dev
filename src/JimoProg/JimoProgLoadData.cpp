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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoProg/JimoProgLoadData.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"
#include "JimoLogic/JimoLogicDoc.h"
#include "JimoLogic/JimoLogicIndex.h"
#include "JimoLogic/JimoLogicMap.h"
#include "JimoLogic/JimoLogicVirtualField.h"
#include "JimoLogic/JimoLogicStatistic.h"
#include "AosConf/DataEngineScanSingle.h"
#include "JQLStatement/JqlStatement.h"
#include <boost/make_shared.hpp>

using boost::make_shared;
using namespace AosConf;


AosJPLoadData::AosJPLoadData(
		AosRundata *rdata,
		const AosXmlTagPtr &inputds, 
		const AosXmlTagPtr &tabledoc,
		list<string> &fields,
		JQLTypes::OpType &op)
{
	aos_assert(inputds);
	aos_assert(tabledoc);
	mTableName = tabledoc->getAttrStr("zky_tb_r_name");
	aos_assert(mTableName != "");
	mInputds = boost::make_shared<DataSet>(inputds);
	mOp = op;
	
	AosXmlTagPtr columns = tabledoc->getFirstChild("columns");

	map<OmnString, AosXmlTagPtr> fields_map;

	list<string> doc_fields;
	if (columns) 
	{
		AosXmlTagPtr column = columns->getFirstChild();
		OmnString column_type;
		while(column) 
		{
			column_type = column->getAttrStr("type");
			
			if (column_type == "virtual" || column_type == "expr")
			{
				AosJimoLogicObjPtr logic = OmnNew AosJLVirtualField(mInputds, column);
				mJimoLogic.push_back(logic);
			}
			OmnString fieldname = column->getAttrStr("name");
			if (column_type != "expr")
			{
				doc_fields.push_back(fieldname);
			}
			fields_map.insert(make_pair(fieldname, column->clone(AosMemoryCheckerArgsBegin)));
			column = columns->getNextChild();

		}
	}

	if (fields.empty())
	{
		AosJimoLogicObjPtr logic = OmnNew AosJLDoc(mInputds, doc_fields, mTableName, fields_map, mOp);
		mJimoLogic.push_back(logic);
	}
	else
	{
		AosJimoLogicObjPtr logic = OmnNew AosJLDoc(mInputds, fields, mTableName, fields_map, mOp);
		mJimoLogic.push_back(logic);
	}
	
	AosXmlTagPtr indexes =  tabledoc->getFirstChild("indexes");
	if (indexes) 
	{
		AosXmlTagPtr index = indexes->getFirstChild();
		while(index) 
		{
			AosJimoLogicObjPtr logic = OmnNew AosJLIndex(rdata, 
					mInputds, index, mTableName, fields_map, mOp);
			mJimoLogic.push_back(logic);
			index = indexes->getNextChild();
		}

	}

	AosXmlTagPtr martixNode = tabledoc->getFirstChild("matrix");
	//felicia, 2014/10/17, for overspeed
	//if (martixNode)
	while (martixNode)
	{
		AosJimoLogicObjPtr logic =  OmnNew AosJLPattern001(mInputds, martixNode, fields_map);
		mJimoLogic.push_back(logic);
		martixNode = tabledoc->getNextChild("matrix");
	}

	AosXmlTagPtr maps =  tabledoc->getFirstChild("maps");
	if (maps) 
	{
		AosXmlTagPtr map = maps->getFirstChild();
		while(map) 
		{
			AosJimoLogicObjPtr logic = OmnNew AosJLMap(mInputds, map, mTableName, fields_map);
			mJimoLogic.push_back(logic);
			map = maps->getNextChild();
		}
	}
	// Ketty 2014/10/11
	/*
	AosXmlTagPtr statistics = tabledoc->getFirstChild("statistic");
	if(!statistics)	return;
	
	OmnString sobjid = statistics->getAttrStr("stat_doc_objid", "");
	aos_assert(sobjid != "");

	AosXmlTagPtr statistic_def = AosJqlStatement::getDocByObjid(sobjid);
	aos_assert(statistic_def);

	AosXmlTagPtr run_stat_doc = createRunStatDoc(statistic_def);
	aos_assert(run_stat_doc);
		
	AosJimoLogicObjPtr logic = OmnNew AosJLStatistic(mInputds, run_stat_doc, fields_map);
	mJimoLogic.push_back(logic);
	*/

	AosXmlTagPtr statistics = tabledoc->getFirstChild("statistic");
	if(!statistics)	return;
	
	OmnString sobjid = statistics->getAttrStr("stat_doc_objid", "");
	aos_assert(sobjid != "");

	AosXmlTagPtr statistic_def = AosJqlStatement::getDocByObjid(0, sobjid);
	aos_assert(statistic_def);

	AosXmlTagPtr inte_stats_conf = statistic_def->getFirstChild("internal_statistics");
	if(!inte_stats_conf)	return;
	
	AosXmlTagPtr stat = inte_stats_conf->getFirstChild();
	OmnString objid;
	AosXmlTagPtr stat_doc, run_stat_doc;
	while(stat)
	{
		objid = stat->getAttrStr("zky_stat_conf_objid", "");
		aos_assert(objid != "");
		stat_doc = AosJqlStatement::getDocByObjid(0, objid);
		aos_assert(stat_doc);
		
		run_stat_doc = createRunStatDoc(statistic_def, stat_doc);
		aos_assert(run_stat_doc);
		
		AosJimoLogicObjPtr logic = OmnNew AosJLStatistic(mInputds, run_stat_doc, fields_map, mOp);
		mJimoLogic.push_back(logic);

		stat = inte_stats_conf->getNextChild();
	}

	//felicia, 2014/09/15
	/*
	AosXmlTagPtr statistics = tabledoc->getFirstChild("statistic");
	if(statistics)
	{
		OmnString sobjid = statistics->getAttrStr("stat_doc_objid", "");
		aos_assert(sobjid != "");

		AosXmlTagPtr statistic_doc = AosJqlStatement::getDocByObjid(sobjid);
		aos_assert(statistic_doc);

		AosXmlTagPtr statistic_defs = statistic_doc->getFirstChild("statistic_defs");
		if(statistic_defs)
		{
			AosXmlTagPtr stat = statistic_defs->getFirstChild();
			OmnString objid;
			AosXmlTagPtr stat_doc;
			while(stat)
			{
				objid = stat->getAttrStr("zky_stat_conf_objid", "");
				aos_assert(objid != "");
				stat_doc = AosJqlStatement::getDocByObjid(objid);
				aos_assert(stat_doc);

				stat_doc->setAttr("stat_doc_objid", sobjid);
				stat_doc->setAttr("zky_table_name", mTableName);
				AosJimoLogicObjPtr logic = OmnNew AosJLStatistic(mInputds, stat_doc, fields_map);
				mJimoLogic.push_back(logic);

				stat = statistic_defs->getNextChild();
			}
		}
	}
	*/

}


AosJPLoadData::~AosJPLoadData()
{
}


AosXmlTagPtr 
AosJPLoadData::createConfig() 
{
	boost::shared_ptr<Job> jobObj = boost::make_shared<Job>();  

	jobObj->setAttribute("zky_otype", "job");
	jobObj->setAttribute("zky_pctrs", "jobctnr");
	jobObj->setAttribute("zky_job_version", "1");

	boost::shared_ptr<JobTask> jobTaskObj = boost::make_shared<JobTask>();
	OmnString taskname  =  "loaddata_";
	taskname <<  mTableName;
	jobTaskObj->setAttribute("zky_name", taskname);
	jobTaskObj->setAttribute("zky_otype", "task");
	jobTaskObj->setAttribute("zky_max_num_thread", 6);
	jobTaskObj->setAttribute("type", "ver1");


	vector<boost::shared_ptr<AosConf::DataSet> > inputdss = collectInputDataSets();
	for (size_t i=0; i<inputdss.size(); i++)
	{
		jobTaskObj->setInputDataset(inputdss[i]);
	}

	vector<boost::shared_ptr<AosConf::DataSet> > outputdss = collectOutputDataSets();
	for (size_t i=0; i<outputdss.size(); i++)
	{
		jobTaskObj->setOutputDataset(outputdss[i]);
	}

	boost::shared_ptr<AosConf::MapTask> maptask = collectMapTask();
	jobTaskObj->setMapTask(maptask);

	boost::shared_ptr<AosConf::ReduceTask> reducetask = collectReduceTask();
	jobTaskObj->setReduceTask(reducetask);

	jobObj->setJobTask(jobTaskObj);

	vector<boost::shared_ptr<AosConf::JobTask> > tasks = collectTasks();
	for(size_t i=0; i<tasks.size(); i++)
	{
		jobObj->setJobTask(tasks[i]);
	}

	OmnString s = jobObj->getConfig();
	AosXmlParser parser;
	AosXmlTagPtr jobconfig = parser.parse(s , "" AosMemoryCheckerArgs);
	return jobconfig;
}


vector<boost::shared_ptr<AosConf::DataSet> >
AosJPLoadData::collectInputDataSets() 
{
	vector<boost::shared_ptr<AosConf::DataSet> > v;

	boost::shared_ptr<DataSet> dataset = mInputds->clone();
	boost::shared_ptr<DataRecord> datarecord = dataset->getSchema()->getRecord();
	
	for (size_t i=0; i<mJimoLogic.size(); i++) {
		vector<boost::shared_ptr<DataField> > &vv = mJimoLogic[i]->getInputFields();
		for (size_t i=0; i<vv.size(); i++) {
			datarecord->setField(vv[i]);
		}
	}

	v.push_back(dataset);
	return v;
}


vector<boost::shared_ptr<AosConf::DataSet> >
AosJPLoadData::collectOutputDataSets() 
{
	vector<boost::shared_ptr<AosConf::DataSet> > v;

	for (size_t i=0; i<mJimoLogic.size(); i++) {
		vector<boost::shared_ptr<DataSet> > dsV = mJimoLogic[i]->getOutputDataset();
		v.insert(v.end(), dsV.begin(), dsV.end());
		//v.push_back(ds);
	}
	return v;
}

vector<boost::shared_ptr<AosConf::JobTask> >
AosJPLoadData::collectTasks() 
{
	vector<boost::shared_ptr<AosConf::JobTask> > v;

	for (size_t i=0; i<mJimoLogic.size(); i++) {
		vector<boost::shared_ptr<JobTask> > dsV = mJimoLogic[i]->getTasks();
		v.insert(v.end(), dsV.begin(), dsV.end());
		//v.push_back(ds);
	}
	return v;
}


boost::shared_ptr<AosConf::MapTask>
AosJPLoadData::collectMapTask() 
{
	boost::shared_ptr<MapTask> maptask = boost::make_shared<MapTask>();  
	boost::shared_ptr<DataEngineScanSingle> engine = boost::make_shared<DataEngineScanSingle>();

	for (size_t i=0; i<mJimoLogic.size(); i++) {
		vector<boost::shared_ptr<DataProc> > &v = mJimoLogic[i]->getMapTaskDataProc();
		for (size_t i=0; i<v.size(); i++) {
			engine->setDataProc(v[i]);
		}
	}
	maptask->setDataEngine(engine);
	return maptask;
}


boost::shared_ptr<AosConf::ReduceTask>
AosJPLoadData::collectReduceTask() 
{
	boost::shared_ptr<ReduceTask> reducetask = boost::make_shared<ReduceTask>();  

	vector<boost::shared_ptr<DataField> > vv;
	for (size_t i=0; i<mJimoLogic.size(); i++)
	{
		vector<boost::shared_ptr<DataField> > tmpV = mJimoLogic[i]->getOutputFields();	
		if (!tmpV.empty()) 
		{
			vv.insert(vv.end(), tmpV.begin(), tmpV.end());
		}
	}

	for (size_t i=0; i<mJimoLogic.size(); i++) {
		vector<boost::shared_ptr<ReduceTaskDataProc> > &v = mJimoLogic[i]->getReduceTaskDataProc();
		for (size_t j=0; j<v.size(); j++) 
		{
			OmnString type = v[j]->getAttribute("type");
			boost::shared_ptr<DataRecord> outputdr = v[j]->getRecord();
			if ((type == "importdoc_csv") || type == "importdoc_fixbin")
			{
				//vector<boost::shared_ptr<DataField> > &vv = mJimoLogic[i]->getOutputFields();
				for (size_t k=0; k<vv.size(); k++) 
				{
					outputdr->setField(vv[k]);
				}
			}
			v[j]->setRecord(outputdr);
			reducetask->setDataProc(v[j]);
		}
	}
	return reducetask;
}


/*
AosXmlTagPtr
AosJPLoadData::createRunStatDoc(const AosXmlTagPtr &statistic_def)
{
	aos_assert_r(statistic_def, 0);
	bool rslt;

	AosXmlTagPtr inte_stats_conf = statistic_def->getFirstChild("internal_statistics");
	aos_assert_r(inte_stats_conf, 0);
	
	OmnString str;
	str << "<runstat>";
	bool set_time_field = false;	

	AosXmlTagPtr stat = inte_stats_conf->getFirstChild();
	OmnString objid;
	AosXmlTagPtr stat_doc;
	while(stat)
	{
		objid = stat->getAttrStr("zky_stat_conf_objid", "");
		aos_assert_r(objid != "", 0);
		stat_doc = AosJqlStatement::getDocByObjid(objid);
		aos_assert_r(stat_doc, 0);
	
		if(!set_time_field)
		{
			AosXmlTagPtr time_field_conf = stat_doc->getFirstChild("time_field"); 
			if(time_field_conf)	str << time_field_conf->toString();
			set_time_field = true;
		}
	
		rslt = createRunStatDocEntry(stat_doc, str);
		aos_assert_r(rslt, 0);
		
		stat = inte_stats_conf->getNextChild();
	}

	str << "</runstat>";
	
	AosXmlTagPtr new_run_stat_doc = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(new_run_stat_doc, 0);
	
	saveNewRunStatDoc(statistic_def, new_run_stat_doc);
	return new_run_stat_doc;

}


bool
AosJPLoadData::createRunStatDocEntry(
		const AosXmlTagPtr &stat_doc, 
		OmnString &str)
{
	aos_assert_r(stat_doc, false);

	AosXmlTagPtr key_fields_conf = stat_doc->getFirstChild("key_fields"); 
	aos_assert_r(key_fields_conf, false); 
	
	OmnString measures_str = "<measures>";
	AosXmlTagPtr vt2ds_conf = stat_doc->getFirstChild("vector2ds");
	aos_assert_r(vt2ds_conf, false);
	
	AosXmlTagPtr each_conf = vt2ds_conf->getFirstChild("vt2d", true);
	AosXmlTagPtr vt2d_info_conf, measures_conf, each_measure_conf;
	OmnString unit_str;
	while(each_conf)
	{
		vt2d_info_conf = each_conf->getFirstChild("vt2d_info");
		aos_assert_r(vt2d_info_conf, false);

		unit_str = vt2d_info_conf->getAttrStr("grpby_time_unit", "");
		aos_assert_r(unit_str != "", false);
		
		measures_conf = vt2d_info_conf->getFirstChild("measures", true);
		aos_assert_r(measures_conf, false);
		
		each_measure_conf = measures_conf->getFirstChild(true);
		while(each_measure_conf)
		{
			each_measure_conf->setAttr("grpby_time_unit", unit_str);
			
			measures_str << each_measure_conf->toString();
			each_measure_conf = measures_conf->getNextChild();
		}
		
		each_conf = vt2ds_conf->getNextChild("vt2d");
	}
	measures_str << "</measures>";
	
	str << "<stat>"
		<< key_fields_conf->toString()
		<< measures_str
		<< "</stat>";
	
	return true;
}


bool
AosJPLoadData::saveNewRunStatDoc(
		const AosXmlTagPtr &statistic_def,
		AosXmlTagPtr &new_run_stat_doc)
{
	OmnString stat_name = statistic_def->getAttrStr("zky_stat_name");
	aos_assert_r(stat_name != "", false);

	AosXmlTagPtr run_stat_confs = statistic_def->getFirstChild("run_stat_docs");
	if(!run_stat_confs)
	{
		OmnString run_confs_str = "<run_stat_docs></run_stat_docs>";
		run_stat_confs = AosXmlParser::parse(run_confs_str AosMemoryCheckerArgs);
		run_stat_confs = statistic_def->addNode(run_stat_confs);	
	}
	u32 crt_runstat_doc_num = run_stat_confs->getNumSubtags();

	OmnString run_stat_doc_objid = stat_name; 
	run_stat_doc_objid << "_rundoc" << crt_runstat_doc_num;
	
	new_run_stat_doc->setAttr("zky_stat_name", stat_name);
	new_run_stat_doc->setAttr(AOSTAG_OBJID, run_stat_doc_objid);
	new_run_stat_doc->setAttr(AOSTAG_PUBLIC_DOC, true);
	new_run_stat_doc->setAttr(AOSTAG_CTNR_PUBLIC, true);
	new_run_stat_doc->setAttr(AOSTAG_PUB_CONTAINER, "_zt4g_stat_ctnr");
	AosJqlStatement::createDoc(new_run_stat_doc);
	
	run_stat_confs->addNode(new_run_stat_doc);
	AosJqlStatement::modifyDoc(statistic_def);
	
	return true;
}
*/


AosXmlTagPtr
AosJPLoadData::createRunStatDoc(
		const AosXmlTagPtr &statistic_def,
		const AosXmlTagPtr &stat_doc)
{
	aos_assert_r(statistic_def && stat_doc, 0);
	
	AosXmlTagPtr key_fields_conf = stat_doc->getFirstChild("key_fields"); 
	aos_assert_r(key_fields_conf, 0); 

	// maybe has no time field.
	AosXmlTagPtr time_field_conf = stat_doc->getFirstChild("time_field"); 
	//aos_assert_r(time_field_conf, 0); 

	AosXmlTagPtr cond_conf = stat_doc->getFirstChild("cond");

	AosXmlTagPtr vt2ds_conf = stat_doc->getFirstChild("vector2ds");
	aos_assert_r(vt2ds_conf, 0);
	
	OmnString measures_str = "<measures>";
	
	AosXmlTagPtr each_conf = vt2ds_conf->getFirstChild("vt2d", true);
	AosXmlTagPtr vt2d_info_conf, measures_conf, each_measure_conf;
	OmnString unit_str;
	while(each_conf)
	{
		vt2d_info_conf = each_conf->getFirstChild("vt2d_info");
		aos_assert_r(vt2d_info_conf, 0);

		// maybe has no time_field.
		unit_str = vt2d_info_conf->getAttrStr("grpby_time_unit", "");
		//aos_assert_r(unit_str != "", 0);
		if(unit_str == "")	aos_assert_r(!time_field_conf, 0);

		measures_conf = vt2d_info_conf->getFirstChild("measures", true);
		aos_assert_r(measures_conf, 0);
		
		each_measure_conf = measures_conf->getFirstChild(true);
		while(each_measure_conf)
		{
			if(unit_str != "") each_measure_conf->setAttr("grpby_time_unit", unit_str);
			
			measures_str << each_measure_conf->toString();
			each_measure_conf = measures_conf->getNextChild();
		}
		
		each_conf = vt2ds_conf->getNextChild("vt2d");
	}
	measures_str << "</measures>";
	
	OmnString str;
	str << "<runstat>"
		<< key_fields_conf->toString();
	
	if(time_field_conf) str << time_field_conf->toString();
	if(cond_conf) str << cond_conf->toString();

	str << measures_str
		<< "</runstat>";
	
	AosXmlTagPtr new_run_stat_doc = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(new_run_stat_doc, 0);
	
	//OmnString stat_name = stat_doc->getAttrStr("zky_stat_name");
	OmnString stat_name = stat_doc->getAttrStr(AOSTAG_OBJID);
	saveNewRunStatDoc(statistic_def, stat_name, 
			new_run_stat_doc);
	return new_run_stat_doc;
}


bool
AosJPLoadData::saveNewRunStatDoc(
		const AosXmlTagPtr &statistic_def,
		const OmnString &stat_name,
		AosXmlTagPtr &new_run_stat_doc)
{
	aos_assert_r(stat_name != "", 0);

	AosXmlTagPtr run_stat_confs = statistic_def->getFirstChild("run_stat_docs");
	if(!run_stat_confs)
	{
		OmnString run_confs_str = "<run_stat_docs></run_stat_docs>";
		run_stat_confs = AosXmlParser::parse(run_confs_str AosMemoryCheckerArgs);
		run_stat_confs = statistic_def->addNode(run_stat_confs);	
	}
	u32 crt_runstat_doc_num = run_stat_confs->getNumSubtags();

	OmnString run_stat_doc_objid = stat_name; 
	run_stat_doc_objid << "_rundoc" << crt_runstat_doc_num;
	
	new_run_stat_doc->setAttr("zky_stat_name", stat_name);
	new_run_stat_doc->setAttr(AOSTAG_OBJID, run_stat_doc_objid);
	new_run_stat_doc->setAttr(AOSTAG_PUBLIC_DOC, true);
	new_run_stat_doc->setAttr(AOSTAG_CTNR_PUBLIC, true);
	new_run_stat_doc->setAttr(AOSTAG_PUB_CONTAINER, "_zt4g_stat_ctnr");
	AosJqlStatement::createDocByObjid(0, new_run_stat_doc, run_stat_doc_objid);
	
	run_stat_confs->addNode(new_run_stat_doc);
	AosJqlStatement::modifyDoc(0, statistic_def);
	
	return true;
}
