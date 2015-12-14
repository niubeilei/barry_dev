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
#include "JimoProg/JimoProgLoadData2.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"
#include "JSON/JSON.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "JimoLogic/JimoLogicDoc2.h"
#include "JimoLogic/JimoLogicIndex2.h"
#include "JimoLogic/JimoLogicMap2.h"
#include "JimoLogic/JimoLogicVirtualField.h"
#include "JimoLogic/JimoLogicStatistic2.h"
#include "AosConf/DataEngineScanSingle.h"
#include "JQLStatement/JqlStatement.h"
#include <boost/make_shared.hpp>

#include "JimoAPI/JimoParserAPI.h"

using boost::make_shared;
using namespace AosConf;


extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJPLoadData2_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJPLoadData2(version);
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


AosJPLoadData2::AosJPLoadData2(const int version)
:
AosGenericObj(version),
mTableName(""),
mSwitchCaseProcName("")
{
	mJimoVersion = version;
}


AosJPLoadData2::~AosJPLoadData2()
{
}


AosJimoPtr
AosJPLoadData2::cloneJimo() const
{
	try
	{
		return OmnNew AosJPLoadData2(*this);
	}
	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return NULL;
	}

	return NULL;
}


bool
AosJPLoadData2::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	JSONValue jsonLoadData;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, jsonLoadData);
	aos_assert_r(rslt, false);

    mJobName = obj_name;
	mTableName = jsonLoadData["tablename"].asString();
	OmnString inputname = jsonLoadData["input"].asString();

	rslt = collectTableInfo(rdata, mTableName, inputname);
	aos_assert_r(rslt, false);

	JSONValue table_json = jsonLoadData["subtablenames"];
	if (table_json.size() > 0)
	{
		mTableNames.clear();
		map<int, vector<OmnString> > tableLenNameMap;
		map<int, vector<OmnString> >::iterator itr;
		map<int, vector<OmnString> >::reverse_iterator r_itr;
		vector<OmnString> v;

		int len = mTableName.length();
		v.push_back(mTableName);
		tableLenNameMap.insert(make_pair(len, v));

		for (size_t i = 0; i < table_json.size(); i++)
		{
			OmnString stname = table_json[i].asString();
			v.clear();
			len = stname.length();
			itr = tableLenNameMap.find(len);
			if (itr != tableLenNameMap.end())
			{
				itr->second.push_back(stname);
			}
			else
			{
				v.push_back(stname);
				tableLenNameMap.insert(make_pair(len, v));
			}

			rslt = collectTableInfo(rdata, stname, inputname, false);
			aos_assert_r(rslt, false);
		}

		for (r_itr = tableLenNameMap.rbegin(); r_itr != tableLenNameMap.rend(); ++r_itr)
		{
			mTableNames.insert(mTableNames.end(), 
					r_itr->second.begin(), r_itr->second.end());
		}
//		for (r_itr = tableLenNameMap.rbegin(); r_itr != tableLenNameMap.rend(); ++r_itr)
//		{
//			mTableNames.insert(r_itr->second.begin(), r_itr->second.end());
//		}

		mSwitchCaseProcName =  (string)mJobName + "_ld_dp_switch_case_";
		mSwitchCaseProcName += mTableName;
	}

	composeConf(rdata, obj_name, inputname, prog);
	return true;
}


bool
AosJPLoadData2::collectTableInfo(
	 	AosRundata *rdata,
		const OmnString &tablename,
		const OmnString &inputname,
		bool prime_table)
{
	// 1. get table doc
	AosXmlTagPtr tabledoc = AosJqlStatement::getDoc(rdata, JQLTypes::eTableDoc, tablename);
	aos_assert_r(tabledoc, false);

    OmnString schemaName = tabledoc->getAttrStr("zky_use_schema").substr(13);
    aos_assert_r(schemaName != "", false);

	if (prime_table)
	{
		mParmName["schema_name"] = schemaName;
	}
 	AosXmlTagPtr columns = tabledoc->getFirstChild("columns");
	//map<OmnString, AosXmlTagPtr> fields_map;
	//list<string> doc_fields;
	//if (columns)
	//{
	//	AosXmlTagPtr column = columns->getFirstChild();
	//	OmnString column_type;
	//	while(column)
	//	{
	//		column_type = column->getAttrStr("type");
	//
	//		if (column_type == "virtual" || column_type == "expr")
	//		{
	//			AosJimoLogicObjPtr logic = OmnNew AosJLVirtualField(mInputds, column);
	//			mJimoLogic.push_back(logic);
	//		}
	//		OmnString fieldname = column->getAttrStr("name");
	//		if (column_type != "expr")
	//		{
	//			doc_fields.push_back(fieldname);
	//		}
	//		fields_map.insert(make_pair(fieldname, column->clone(AosMemoryCheckerArgsBegin)));
	//		column = columns->getNextChild();

	//	}
	//}

	//if (fields.empty())
	//{
	//	AosJimoLogicObjPtr logic = OmnNew AosJLDoc(mInputds, doc_fields, tablename, fields_map, mOp);
	//	mJimoLogic.push_back(logic);
	//}
	//else
	//{
	//	AosJimoLogicObjPtr logic = OmnNew AosJLDoc(mInputds, fields, tablename, fields_map, mOp);
	//	mJimoLogic.push_back(logic);
	//}

	// JIMODB-623
	//if (prime_table)
	//{
		AosJimoLogicObjPtr logic = OmnNew AosJLDoc2(
				rdata, inputname, tabledoc, tablename, this, prime_table);
		mJimoLogic.push_back(logic);
	//}


	AosXmlTagPtr indexes = tabledoc->getFirstChild("indexes");
	if (indexes)
	{
		AosXmlTagPtr index = indexes->getFirstChild();
		if (index)
		{
			//OmnString indexobjid = index->getAttrStr("zky_index_objid");
			//aos_assert_r(indexobjid != "", false);
			//AosXmlTagPtr real_index_doc = AosJqlStatement::getDocByObjid(rdata, indexobjid);
			//aos_assert_r(real_index_doc, false);
			logic = OmnNew AosJLIndex2(rdata,
					//inputname, real_index_doc, tablename, this, tabledoc, prime_table);
					inputname, tablename, this, tabledoc, prime_table);
			mJimoLogic.push_back(logic);
			//index = indexes->getNextChild();
		}
	}

	//AosXmlTagPtr martixNode = tabledoc->getFirstChild("matrix");
	//while (martixNode)
	//{
	//	AosJimoLogicObjPtr logic =  OmnNew AosJLPattern001(mInputds, martixNode, fields_map);
	//	mJimoLogic.push_back(logic);
	//	martixNode = tabledoc->getNextChild("matrix");
	//}

	AosXmlTagPtr maps = tabledoc->getFirstChild("maps");
	if (maps)
	{
		AosXmlTagPtr map = maps->getFirstChild();
		//while(map)
		if(map)
		{
			logic = OmnNew AosJLMap2(
					//rdata, this, inputname, map, tablename, prime_table);
					rdata, this, inputname, tablename, prime_table);
			mJimoLogic.push_back(logic);
			//map = maps->getNextChild();
		}
	}

	AosXmlTagPtr statistics = tabledoc->getFirstChild("statistic");
	
	if(!statistics)	return true;
	logic = OmnNew AosJLStatistic2(
			rdata, inputname, tablename, this);
	mJimoLogic.push_back(logic);
	
/*
	OmnString sobjid = statistics->getAttrStr("stat_doc_objid", "");
	aos_assert_r(sobjid != "", false);

	AosXmlTagPtr statistic_def = AosJqlStatement::getDocByObjid(rdata, sobjid);
	aos_assert_r(statistic_def, false);

	AosXmlTagPtr inte_stats_conf = statistic_def->getFirstChild("internal_statistics");
	if(!inte_stats_conf)	return false;

	AosXmlTagPtr def_confs = statistic_def->getFirstChild("statistic_defs");
	if(!def_confs)	return false;

	AosXmlTagPtr stat = inte_stats_conf->getFirstChild();
	AosXmlTagPtr def_conf = def_confs->getFirstChild();
	OmnString objid;
	AosXmlTagPtr stat_doc, run_stat_doc;
	while(stat)
	{
		objid = stat->getAttrStr("zky_stat_conf_objid", "");
		aos_assert_r(objid != "", false);
		stat_doc = AosJqlStatement::getDocByObjid(rdata, objid);
		aos_assert_r(stat_doc, false);

		run_stat_doc = createRunStatDoc(rdata, statistic_def,def_conf, stat_doc);
		aos_assert_r(run_stat_doc, false);

		OmnString db_tablename;
		OmnString dbname = tabledoc->getAttrStr("zky_database", "");
		db_tablename << dbname << "_" << tablename;
		AosJimoLogicObjPtr logic = OmnNew AosJLStatistic2(
				rdata, inputname, run_stat_doc, tablename, this, tabledoc);
		mJimoLogic.push_back(logic);

		stat = inte_stats_conf->getNextChild();
		def_conf = def_confs->getNextChild();
	}
*/

	return true;
}


bool 
AosJPLoadData2::replaceFirstTask(
		vector<JSONValue> &repaired_tasks, 
		const AosRundataPtr rdata)
{
	aos_assert_r(repaired_tasks.size() > 0, false);
	aos_assert_r(repaired_tasks[0], false);

	int pos;
	size_t j = 0;
	OmnString dpName = "";
	mSubTableNameProcsMap.clear();
	mDefaultProcs.clear();
	map<OmnString, vector<OmnString> >::iterator itr;
	vector<OmnString> procs, dpOutputs;

	JSONValue dataprocs = repaired_tasks[0]["dataprocs"];
	JSONValue inputs = dataprocs[j]["inputs"];
	vector<OmnString>::iterator itr_t;
	for (size_t i = 0; i < dataprocs.size(); ++i)
	{
		dpName = dataprocs[i]["dataproc"].asString();
		for (itr_t = mTableNames.begin(); itr_t != mTableNames.end(); ++itr_t)
		{
			pos = dpName.findSubString(*itr_t, 0, false);
			if (pos != -1) break;	//find subtablename
		}
		aos_assert_r(j < mTableNames.size(), false);

		if (*itr_t == mTableName)
		{
			mDefaultProcs.push_back(dpName);
		}
		else
		{
			itr = mSubTableNameProcsMap.find(*itr_t);
			if (itr != mSubTableNameProcsMap.end())
			{
				itr->second.push_back(dpName);
			}
			else
			{
				procs.clear();
				procs.push_back(dpName);
				mSubTableNameProcsMap.insert(make_pair(*itr_t, procs));
			}
		}

		JSONValue outputs = dataprocs[i]["outputs"];
		for (size_t k = 0 ; k < outputs.size(); ++k)
		{
			dpOutputs.push_back(outputs[k].asString());
		}
	}

	//{
	//    "dataproc": "ld_dp_switch_case_tablename",
	//    "inputs": ["xxxxx"],
	//    "outputs": ["dp1_output", "dp2_output", "dp3_output"]
	//}
	aos_assert_r(mSwitchCaseProcName != "", false);
	string outputs_name;
	string switchCaseProc  = "[ { \"dataproc\":";
	switchCaseProc += "\"" + mSwitchCaseProcName + "\"";
	if (dpOutputs.size() > 0)
	{
		switchCaseProc += ", \"outputs\": [";
		for (size_t i = 0; i < dpOutputs.size(); ++i)
		{
			outputs_name = dpOutputs[i];
			switchCaseProc += "\"" + outputs_name + "\"";
			if (i != dpOutputs.size() - 1) switchCaseProc += ", ";
		}
		switchCaseProc += "] ";
	}

	if (inputs.size() > 0)
	{
		switchCaseProc += ", \"inputs\": [";
		for (size_t i = 0; i < inputs.size(); ++i)
		{
			switchCaseProc += "\"" + inputs[i].asString() + "\"";
			if (i != inputs.size() - 1) switchCaseProc += ", ";
		}
		switchCaseProc += "] ";
	}
	switchCaseProc += "} ]";

OmnScreen << "newProc:" << endl;
OmnCout << switchCaseProc << endl;

	JSONReader reader;
	JSONValue newProcsJSON;
	bool rslt = reader.parse(switchCaseProc, newProcsJSON);
	aos_assert_r(rslt, false);                       

	repaired_tasks[0]["dataprocs"] = newProcsJSON;

	return true;
}


bool
AosJPLoadData2::composeConf(
		const AosRundataPtr &rdata,
		const string obj_name,
		const string input_opt,
		const AosJimoProgObjPtr &prog)
{
	vector<JSONValue> composedTasks = composeTasks(rdata, mTasks);

	// Now, compose create job json config
	vector<JSONValue> repaired_tasks;
	map<string, string> outputs;
	outputs[input_opt] = input_opt;
	map<string, string> new_output_map;
	while (composedTasks.size())
	{
		outputs = useOutputfilterInput(rdata,
				outputs, composedTasks, repaired_tasks);
		if (!outputs.size()) break;
	}

	AosJimoParserObjPtr jimoParser = Jimo::jimoCreateJimoParser(rdata.getPtr());
	aos_assert_rr(jimoParser, rdata.getPtr(), 0);
	vector<AosJqlStatementPtr> statements;

	//replace the firsttask while 'subTableNames' is not empty
	if (!mTableNames.empty())
	{
		bool rslt = replaceFirstTask(repaired_tasks, rdata);
		aos_assert_r(rslt, false);

		//create switchCase dataproc
		string switchCaseJQL = createSwitchCaseProc();
		aos_assert_r(switchCaseJQL != "", false);

		jimoParser->parse(rdata.getPtr(), prog.getPtr(), switchCaseJQL, statements);
		for (u32 i=0; i<statements.size(); i++)
		{
			statements[i]->setContentFormat("xml");
			statements[i]->run(rdata, prog);
		}
          
	}
	
	JSONValue root;
	JSONFastWriter writer;
	JSONValue tasksObj;
	for (size_t i=0; i<repaired_tasks.size(); i++)
	{
		tasksObj.append(repaired_tasks[i]);
	}
	root["tasks"] = tasksObj;
	string jsontasks_str = writer.write(root);

	string conf = "create job " + obj_name + " ";
	conf += jsontasks_str + ";";

OmnScreen << "7777777777777777777" << endl;
OmnCout << conf << endl;

	//AosJimoParserObjPtr jimoParser = Jimo::jimoCreateJimoParser(rdata.getPtr());
	//aos_assert_rr(jimoParser, rdata.getPtr(), 0);
	//vector<AosJqlStatementPtr> statements;
	statements.clear();
	jimoParser->parse(rdata.getPtr(), prog.getPtr(), conf, statements);
	for (u32 i=0; i<statements.size(); i++)
	{
		    statements[i]->setContentFormat("xml");
			statements[i]->run(rdata, prog);
			//statements[i]->run(rdata, 0);
	}

	return true;
}


string
AosJPLoadData2::createSwitchCaseProc()
{
//create dataprocswitchcase ld_dp_switch_case_unicom_v_tb
//{
//  "type": "switch_case", 
//  "switch_value":"tablename",
//  "switch_cases": 
//                [
//                     {
//                       "case":["unicom_tb1"],
//                       "dataprocs":["dp1", "dp2"]
//                     },
//                     {
//                       "case":["unicom_tb2"],
//                       "dataprocs":["dp1", "dp3"]
//                     }
//                  ]
//};
	JSONValue dataproc;
	dataproc["type"] = "switch_case";
	dataproc["switch_value"] = "tablename";
	JSONValue switchCaseJV;

	string tablename = "";
	vector<OmnString> procs;
	string procName;
	map<OmnString, vector<OmnString> >::iterator itr, itr_end;
	itr = mSubTableNameProcsMap.begin();
	itr_end = mSubTableNameProcsMap.end();
	if (mSubTableNameProcsMap.size() > 0)
	{
		--itr_end;
		for (; itr != mSubTableNameProcsMap.end(); ++itr)
		{
			tablename = itr->first;
			procs = itr->second;

			JSONValue onecaseJV;
			JSONValue dataprocsJV;
			for (size_t i = 0; i < procs.size(); ++i)
			{
				procName = procs[i];
				dataprocsJV.append(procName);
			}
			for (size_t i = 0; i < mDefaultProcs.size(); ++i)
			{
				procName = mDefaultProcs[i];
				dataprocsJV.append(procName);
			}
			onecaseJV["case"].append(tablename);
			onecaseJV["dataprocs"] = dataprocsJV;
			switchCaseJV.append(onecaseJV);
		}
	}
	else if (mSubTableNameProcsMap.size() == 0 && mDefaultProcs.size() != 0)
	{
		//tablename = mTableName;
		//mTableNames.erase(tablename);
		//set<OmnString>::iterator itr;

		for (size_t i = 0; i < mTableNames.size(); i++)
		{
			JSONValue onecaseJV;
			tablename = mTableNames[i];
			if (tablename == mTableName) continue;

			onecaseJV["case"].append(tablename);
			
			for (size_t j = 0; j < mDefaultProcs.size(); ++j)
			{
				procName = mDefaultProcs[j];
				onecaseJV["dataprocs"].append(procName);
			}
			switchCaseJV.append(onecaseJV);
		}
	}

	dataproc["switch_cases"] = switchCaseJV;
	string conf = dataproc.toStyledString();
OmnScreen << "3333333333333333333" << conf << endl;
	
	string jql = "create dataprocswitchcase " + mSwitchCaseProcName + " ";
	jql += conf + ";";

	return jql;
}


map<string, string>
AosJPLoadData2::useOutputfilterInput(
		const AosRundataPtr &rdata,
		map<string, string> &outputs,
		vector<JSONValue> &tasks,
		vector<JSONValue> &filtered_tasks)
{
	map<string, string> collect_outputs;
	for (size_t i=0; i<tasks.size(); i++)
	{
		vector<string> new_outputs;
	   	if (getOutputsWithInputFromTask(rdata, tasks[i], outputs, new_outputs))
		{
			filtered_tasks.push_back(tasks[i]);
			tasks[i] = tasks.back();
			i--;
			tasks.pop_back();

			//if ( !new_outputs.size() ) break;
			for (size_t j=0; j<new_outputs.size(); j++)
			{
				collect_outputs[new_outputs[j]] = new_outputs[j];
			}
		}
	}

	return collect_outputs;
}


bool
AosJPLoadData2::runByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosJPLoadData2::showByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosJPLoadData2::dropByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr)
{
	OmnNotImplementedYet;
	return false;
}


AosXmlTagPtr
AosJPLoadData2::createConfig()
{
	OmnNotImplementedYet;
	return NULL;
}


bool
AosJPLoadData2::getOutputsWithInputFromTask(
		const AosRundataPtr &rdata,
		const JSONValue &task,
		map<string, string> &key_inputs,
		vector<string> &value_outputs)
{
	bool flag = false;
	JSONValue dataprocs = task["dataprocs"];
	for (size_t i=0; i<dataprocs.size(); i++)
	{
		JSONValue inputs = dataprocs[i]["inputs"];
		for (size_t j=0; j<inputs.size(); j++)
		{
			if ( key_inputs.count(inputs[j].asString()) )
			{
				flag = true;
				for (size_t k=0; k<dataprocs.size(); k++)
				{
					if (!dataprocs[k].isMember("outputs")) continue;
					JSONValue outputs = dataprocs[k]["outputs"];
					for (size_t kk=0; kk<outputs.size(); kk++)
					{
						value_outputs.push_back(outputs[kk].asString());
					}
				}

			}
		}

	}

	return flag;
}


AosXmlTagPtr
AosJPLoadData2::createRunStatDoc(
		AosRundata *rdata,
		const AosXmlTagPtr &statistic_def,
		const AosXmlTagPtr &def_doc,
		const AosXmlTagPtr &stat_doc)
{
	aos_assert_r(statistic_def && stat_doc, 0);

	AosXmlTagPtr key_fields_conf = stat_doc->getFirstChild("key_fields");
	aos_assert_r(key_fields_conf, 0);

	// maybe has no time field.
	AosXmlTagPtr time_field_conf = stat_doc->getFirstChild("time_field");

	//added by yang
	AosXmlTagPtr stats_def_doc = statistic_def->getFirstChild("statistic_defs");
	OmnString objid = stats_def_doc->getFirstChild()->getAttrStr("zky_stat_conf_objid", "");
	aos_assert_r(objid != "", NULL);
	AosXmlTagPtr stat_def_doc = AosJqlStatement::getDocByObjid(rdata, objid);
	aos_assert_r(stat_def_doc, NULL);

	//OmnString objid = def_doc->getAttrStr("zky_stat_conf_objid", "");
	//aos_assert_r(objid != "", false);
	//AosXmlTagPtr stat_def_doc = AosJqlStatement::getDocByObjid(rdata, objid);
	//aos_assert_r(stat_def_doc, 0);

	AosXmlTagPtr time_tag = stat_def_doc->getFirstChild("time_field");
	if(time_tag)
		time_field_conf->setAttr("grpby_time_unit",time_tag->getAttrStr("grpby_time_unit",""));

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
	saveNewRunStatDoc(rdata, statistic_def, stat_name,
			new_run_stat_doc);
	return new_run_stat_doc;
}


bool
AosJPLoadData2::saveNewRunStatDoc(
		AosRundata *rdata,
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
	AosJqlStatement::createDocByObjid(rdata, new_run_stat_doc, run_stat_doc_objid);

	run_stat_confs->addNode(new_run_stat_doc);
	AosJqlStatement::modifyDoc(rdata, statistic_def);

	return true;
}


vector<JSONValue>
AosJPLoadData2::composeTasks(
		const AosRundataPtr &rdata,
		vector<JSONValue> &tasks)
{
	vector<JSONValue> composedV;
	if ( !tasks.size() ) return composedV;

	map<string, vector<JSONValue> > sameTypeTasks = getSameTypeTasks(rdata, tasks);
	for (map<string, vector<JSONValue> >::iterator itr = sameTypeTasks.begin();
			itr != sameTypeTasks.end(); itr++)
	{
		vector<JSONValue> tmpV = divideTasksWithInputs(rdata, itr->second);
		composedV.insert(composedV.begin(), tmpV.begin(), tmpV.end());
	}

	return composedV;
}


map<string, string>
AosJPLoadData2::getInputsFromTask(
		const AosRundataPtr &rdata,
		const JSONValue &task)
{
	map<string, string> allInputs;	// left: inputs, right: engine_type
	string type = task["dataengine_type"].asString();
	JSONValue dataprocs = task["dataprocs"];
	for (size_t i=0; i<dataprocs.size(); i++)
	{
		JSONValue inputs = dataprocs[i]["inputs"];
		allInputs[inputs.toStyledString()] = type;
	}

	return allInputs;
}


map<string, vector<JSONValue> >
AosJPLoadData2::getSameTypeTasks(
		const AosRundataPtr &rdata,
		const vector<JSONValue> &tasks)
{
	// This function puts all the tasks with the same data engine
	// type into one group. 
	map<string, vector<JSONValue> > sameTypeTasks;

	for (size_t i=0; i<tasks.size(); i++)
	{
		string type = tasks[i]["dataengine_type"].asString();
		if (sameTypeTasks.count(type))
		{
			sameTypeTasks[type].push_back(tasks[i]);
		}
		else
		{
			vector<JSONValue> v;
			v.push_back(tasks[i]);
			sameTypeTasks[type] = v;
		}
	}

	return sameTypeTasks;
}


vector<JSONValue>
AosJPLoadData2::divideTasksWithInputs(
		const AosRundataPtr &rdata,
		const vector<JSONValue> &tasks)
{
	vector<JSONValue> composedV;
	if (!tasks.size()) return composedV;
	composedV.push_back(tasks[0]);
	for (size_t i=1; i<tasks.size(); i++)
	{
		bool is_found = false;
		map<string, string> inputs = getInputsFromTask(rdata, tasks[i]);
		for (size_t j=0; j<composedV.size(); j++)
		{
			map<string, string> inputs2 = getInputsFromTask(rdata, composedV[j]);
			if (inputs.size() == inputs2.size())
			{
				for (map<string, string>::iterator itr = inputs2.begin();
					itr != inputs2.end(); itr++)
				{
					if (inputs.count(itr->first))
					{
						is_found = true;
						composeDataprocs(rdata, composedV[j], tasks[i]);
						break;
					}
				}
			}
		}

		if (!is_found)
		{
			composedV.push_back(tasks[i]);
		}
	}
	return composedV;
}


bool
AosJPLoadData2::composeDataprocs(
		const AosRundataPtr &rdata,
		JSONValue &task1,
		const JSONValue &task2)
{
	JSONValue dataprocs1 = task1["dataprocs"];
	JSONValue dataprocs2 = task2["dataprocs"];
	for (size_t i=0; i<dataprocs2.size(); i++)
	{
		dataprocs1.append(dataprocs2[i]);
	}

	task1["dataprocs"] = dataprocs1;
	return true;
}

bool        
AosJPLoadData2::setConf(OmnString& confList)
{
	OmnNotImplementedYet;
	return true;
}

vector<OmnString>
AosJPLoadData2::getConfList()
{
	OmnNotImplementedYet;
	return mConfList;
}

bool        
AosJPLoadData2::setDataProcName(OmnString& name)
{
	OmnNotImplementedYet;
	return true;
}

vector<OmnString>
AosJPLoadData2::getDataProcNameList()
{
	return mDataProcNameList;
}


