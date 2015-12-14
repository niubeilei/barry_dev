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
// A job has the following sections:
// 1. Schema
// 2. Datasets
// 3. Data Procs
// 4. Job
// 		tasks
//
// 2014/07/26 Created by Chen Ding
// 2015/05/21 Reworked by Arvin
////////////////////////////////////////////////////////////////////////////
#include "JimoProg/JimoProgJob.h"

#include "API/AosApi.h"
#include "JimoProg/Ptrs.h"
#include "JimoAPI/JimoParserAPI.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "SEInterfaces/JimoParserObj.h"
#include "JQLStatement/JqlStatement.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoProgJob_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosJimoProgJob(version);
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



AosJimoProgJob::AosJimoProgJob(const int version)
{
	mJimoVersion = version;
}


AosJimoProgJob::~AosJimoProgJob()
{
}


AosJimoPtr
AosJimoProgJob::cloneJimo() const
{
	return OmnNew AosJimoProgJob(*this);
}


OmnString
AosJimoProgJob::generateCode(AosRundata *rdata)
{
	/*
	// The code is in the following format:
	// 	Schema section
	// 	Dataset section
	// 	Data Proc section
	// 	Job
	mCode = "";
	generateCode(rdata, "schema");
	generateCode(rdata, "dataset");
	generateCode(rdata, "dataproc");
	generateJobCode(rdata);
	OmnScreen << "Generated code:" << endl << mCode << endl;
	return mCode;
	*/
	OmnShouldNeverComeHere;
	return "";
}


bool
AosJimoProgJob::generateJobCode(AosRundata *rdata)
{
	// This is the function to create the atomic "create job" 
	// statement. The statement format is:
	// 	create job <jobname>
	// 	{
	//  	"tasks": [
	//  	{
	//  		"name": "t1_new",
	//  		"dataengine_type": "dataengine_scan2",
	//  		"dataprocs": 
	//  		[
	//  			{
	//  				"dataproc": "dp_t_owner_select_new",
	//  				"inputs": ["t_owner_dataset_new"], 
	//  				"outputs": ["dp_t_owner_select_new_output"]
	//  			},
	//  			...
	//  		]
	//  	}
	//  	]
	//  }
	//

	// Retrieve the task section. The section contains all the atomic
	// tasks, or the tasks that contain only one data proc. These
	// tasks were created by higher level statements.
	itr_t itr = mSections.find("tasks");
	if (itr == mSections.end()) return true;
	
	bool rslt  = getJSONTasks(rdata, itr->second);
	aos_assert_r(rslt,false);

	vector<JSONValue> join_tasks;
	vector<JSONValue> normal_tasks;
	
	rslt = separateJoinTask(join_tasks, normal_tasks);
	aos_assert_r(rslt,false);

	//combine task 
	vector<JSONValue> tasks = combineTasks(rdata, normal_tasks);

	// Put normal tasks and join tasks togethre
	tasks.insert(tasks.begin(),join_tasks.begin(),join_tasks.end());

	// Sort tasks
	map<OmnString, int>	outputs;
	map<OmnString, int>	inputs;
	vector<JSONValue> finish_tasks;		// Hold the sorted tasks
	rslt = collectInputOutput(rdata, tasks, outputs, inputs);
	aos_assert_rr(rslt,rdata,false);
	
	rslt = checkTaskOrder(rdata,outputs,tasks,finish_tasks);
	
	JSONValue root;
	JSONFastWriter writer;
	JSONValue taskObj;
	for(size_t i =0;i < finish_tasks.size();i++)
	{
		taskObj.append(finish_tasks[i]);
	}
	root["tasks"] = taskObj;
	string jsontasks_str = writer.write(root);

	mCode <<  "create job " << mJobName << " "
		  <<  jsontasks_str.data() << ";";
	mCode.replaceStrWithSpace();

	OmnScreen << "To Create Job: ";
	OmnScreen << mCode << endl;
	return true;
}


bool
AosJimoProgJob::checkTaskOrder(
		AosRundata *rdata,
		map<OmnString,int> &outputs_map,
		vector<JSONValue>  &tasks,
		vector<JSONValue>  &finish_tasks)
{
	// This function sorts all the tasks based on their inputs readiness. 
	// 1. First pick all the tasks that do not depend on any outputs.
	//    These are Initial Tasks.
	// 2. Pick the new one as their outputs become available.

	u32 size = tasks.size();

	pickReadyTasks(rdata,outputs_map,tasks,finish_tasks);
	if (finish_tasks.size() <= 0)
	{
		AosLogError(rdata, true, "no_initial_tasks_found") << enderr;
		return false;
	}

	u32 idx = 0;
	while (finish_tasks.size() < size)
	{
		JSONValue task = finish_tasks[idx++];
		updateTaskOutputs(rdata, task, outputs_map);
		pickReadyTasks(rdata, outputs_map, tasks, finish_tasks);
	}
	return true;
}


bool
AosJimoProgJob::pickReadyTasks(
		AosRundata *rdata, 
		map<OmnString,int> &outputs_map,
		vector<JSONValue>  &tasks,
		vector<JSONValue>  &finish_tasks)
{
	vector<JSONValue>::iterator itr;
	for (itr = tasks.begin(); itr!=tasks.end();)
	{				
		// If the task's inputs are all ready, put the task into 'finish_tasks'.
		vector<OmnString> inputs_str;
		bool rslt = getTaskInputsOrOutputs(rdata, "inputs", *itr, inputs_str);
		aos_assert_rr(rslt,rdata,false);

		if (inputsIsReady(rdata, outputs_map, inputs_str, *itr))
		{
			finish_tasks.push_back(*itr);
			itr = tasks.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	return true;
}


bool 
AosJimoProgJob::inputsIsReady(
		AosRundata* rdata,
		map<OmnString,int> &outputs_map,
		vector<OmnString> &inputs_str,
		JSONValue &task)
{
	// This function checks whether all inputs in 'inputs_str' are ready.
	// An input is ready if:
	// 	1. The input is not in outputs_map, or
	// 	2. The output is ready (i.e., its value is 0)

	aos_assert_rr(inputs_str.size() > 0, rdata, false);

	map<OmnString, int>::iterator itr;
	for (size_t i = 0;i < inputs_str.size();i++)
	{
		itr = outputs_map.find(inputs_str[i]);
		if (itr != outputs_map.end() && itr->second > 0) return false;
	}
	return true;
}


bool
AosJimoProgJob::updateTaskOutputs(
		AosRundata* rdata, 
		JSONValue &task, 
		map<OmnString,int> &outputs_map)
{
	vector<OmnString> outputs_str;
	bool rslt = getTaskInputsOrOutputs(rdata, "outputs", task, outputs_str);
	aos_assert_rr(rslt,rdata,false);
	
	for(size_t i = 0; i < outputs_str.size(); i++)
	{
		outputs_map[outputs_str[i]] -= 1;
	}
	return true;
}



bool
AosJimoProgJob::getTaskInputsOrOutputs(
		AosRundata *rdata,
		const OmnString &parm,
		JSONValue &task,
		vector<OmnString> &inputs_str)
{
	// This function retrieves all the inputs for 'task' and 
	// put the inputs to 'inputs_str'.
	JSONValue dataprocs = task["dataprocs"];
	for(size_t i = 0;i < dataprocs.size();i++)
	{
		JSONValue inputs = dataprocs[i][parm];
		for(size_t i = 0; i < inputs.size();i++)
		{
			OmnString input = inputs[i].asString();
			if(input != "") inputs_str.push_back(input);
		}
	}
	return true;
}

/*
bool 
AosJimoProgJob::notInOutputs(
		AosRundata *rdata,
		map<OmnString,int> &outputs,
		map<OmnString,int> &inputs)
{
	map<OmnString,int>::iterator  itr;
	for (itr = inputs.begin(); itr != inputs.end(); itr++)
	{	
		if (!outputs.count(itr->first))
		{	
			itr->second = 0;
		}
	}
	return true;
}
*/

bool
AosJimoProgJob::collectInputOutput(
		AosRundata *rdata,
		vector<JSONValue> &tasks,
		map<OmnString, int> &outputs,
		map<OmnString, int> &inputs)
{
	// This function retrieves all the inputs and outputs from 'tasks'
	// and add them to the 'outputs' and 'inputs' maps. 
	if(tasks.size() <= 0)	return false;
	for (size_t i = 0; i < tasks.size();i++)
	{
		JSONValue dataprocs = tasks[i]["dataprocs"];
		for(size_t j = 0; j < dataprocs.size();j++ )
		{
			JSONValue input_values = dataprocs[j]["inputs"];
			JSONValue output_values = dataprocs[j]["outputs"];

			bool rslt = putIntoMap(input_values, inputs);
			aos_assert_rr(rslt,rdata,false);

			rslt = putIntoMap(output_values, outputs);
			aos_assert_rr(rslt,rdata,false);
		}
	}
	//collect  invalid outputs	
	map<OmnString,int>::iterator itr;
	vector<OmnString> invalid_outputs;
	for(itr = outputs.begin(); itr != outputs.end(); itr++)
	{
		if(!inputs.count(itr->first))
		{
			invalid_outputs.push_back(itr->first);
		}
	}
	//delete the invalid task
/*	if(invalid_outputs.size() >0 )
	{
		bool rslt = deleteInvalidTask(rdata,tasks,invalid_outputs);
		aos_assert_rr(rslt, rdata, false);
	}
*/	
	////////////////////////////////////////////////////////
	//arvin 2015.10.27
	//jimodb-1026:when we find invalid tasks,we will send msg
	/////////////////////////////////////////////////////////
	if(invalid_outputs.size() >0 )
	{
		OmnString errmsg = "[ERR] : there are some invalid outputs,please remove them :";
		for(size_t i = 0;i < invalid_outputs.size();i++)
		{
			if(i > 0)
				errmsg << ",";
			errmsg <<    "\n      " << invalid_outputs[i];
		}
		rdata->setJqlMsg(errmsg);
		return false;
	}
	return true;
}

bool
AosJimoProgJob::deleteInvalidTask(
		AosRundata *rdata,
		vector<JSONValue> &tasks,
		vector<OmnString> &invalid_outputs)
{
	aos_assert_rr(tasks.size()>0, rdata, false);
	vector<JSONValue>::iterator itr;
	for(itr = tasks.begin();itr != tasks.end();)
	{
		vector<OmnString> outputs;
		bool rslt = getTaskInputsOrOutputs(rdata,"outputs",*itr,outputs);
		aos_assert_rr(rslt, rdata, false);
		if(outputs.size() <= 0) 
		{
			itr++;
			continue;
		}
		if(isDelete(rdata,outputs,invalid_outputs))
		{
			itr = tasks.erase(itr);
		}
		else
		{
			itr++;
		}
	}
	return true;
}

bool
AosJimoProgJob::isDelete(
		AosRundata *rdata,
		const vector<OmnString> &outputs,
		const vector<OmnString> &invalid_outputs)
{
	for(size_t i = 0; i < outputs.size(); i++)
	{
		if(!count(invalid_outputs.begin(),invalid_outputs.end(),outputs[i]))
			return false;
	}
	return true;
}

bool
AosJimoProgJob::putIntoMap(
				const JSONValue &values,
				map<OmnString,int> &map)
{
	for(size_t i = 0; i < values.size(); i++)
	{
		OmnString value = values[i].asString();
		if(map.count(value))
		{
			map[value] += 1;
		}
		else
		{
			map[value] = 1;
		}
	}
	return true;
}


bool 
AosJimoProgJob::separateJoinTask(
		vector<JSONValue> &join_tasks,
		vector<JSONValue> &normal_tasks)
{
	// This function separates tasks that use "dataengine_join2" from 
	// others. 
	//
	// IMPORTANT!!!!!!!!!!!!!! 
	// In the current implementations, tasks that use 
	// "dataengine_join2" cannot support mutliple data procs. 
	
	for(size_t i =0; i < mTasks.size();i++)
	{
		OmnString type = mTasks[i]["dataengine_type"].asString();
		if (type == "dataengine_join2")
		{
			join_tasks.push_back(mTasks[i]);
			continue;
		}
		normal_tasks.push_back(mTasks[i]);
	}
	return true;
}


bool 
AosJimoProgJob::getJSONTasks(
		AosRundata* rdata,
		const vector<OmnString> &tasks_str)
{
	// 'tasks_str' is a vector of strings. Each string is a
	// task expressed in JSON form. This function converts
	// these JSON docs to JSON objects saved in mTasks.
	aos_assert_rr(tasks_str.size()>0,rdata,false);
	JSONReader reader;
	JSONValue value;
	bool rslt;
	for(size_t i = 0;i < tasks_str.size();i++)
	{
		rslt = reader.parse(tasks_str[i],value);
		aos_assert_r(rslt,false);

		JSONValue dataprocs = value["dataprocs"];
		OmnString dataproc = dataprocs[0u]["dataproc"].asString();
		size_t j = 0;
		for (; j < mDeleteProcNames.size(); j++)
		{
			if (dataproc == mDeleteProcNames[j])
				break;
		}
		if (j >= mDeleteProcNames.size())
		{
			mTasks.push_back(value);
		}
	}
	return true;
}


void 
AosJimoProgJob::setJobName(const OmnString &job_name)
{
	mJobName = job_name;
}


bool
AosJimoProgJob::handleError(AosRundata *rdata)
{
	AosSetErrorUser3(rdata, "jimoprogjob_syntax_error", rdata->getErrmsg());
	return true;
}



vector<JSONValue>
AosJimoProgJob::combineTasks(
		AosRundata *rdata,
		vector<JSONValue> &taskV)
{
	vector<JSONValue> combinedTasks;
	if(taskV.size() <= 0) return combinedTasks;
	//put the same dataengine_type together
	map<string, vector<JSONValue> > sameTypeTasks;
	bool rslt = getSameTypeTasks(rdata,taskV,sameTypeTasks);

	map<string, vector<JSONValue> >::iterator itr ;
	for(itr = sameTypeTasks.begin() ; itr != sameTypeTasks.end(); itr ++)
	{
		//merge the same inputs
		vector<JSONValue> tmpV;
		rslt = getSameTypeInputTask(rdata,itr->second,tmpV);
		if(!rslt) continue;
		combinedTasks.insert(combinedTasks.begin(),tmpV.begin(),tmpV.end());
	}

	return combinedTasks;
}


bool
AosJimoProgJob::getSameTypeTasks(
		AosRundata *rdata,
		vector<JSONValue> &taskV,
		map<string, vector<JSONValue> > &sameTypeTasks)
{
	// This function combines tasks that have the same data engine
	// into the same group. The results are a hash map that 
	// maps data engine type to tasks.
	for(size_t i=0;i < taskV.size(); i++)
	{
		string type = taskV[i]["dataengine_type"].asString();
		if(sameTypeTasks.count(type))
		{
			sameTypeTasks[type].push_back(taskV[i]);
		}
		else
		{
			vector<JSONValue> v;
			v.push_back(taskV[i]);
			sameTypeTasks[type] = v;
		}
	}
	return true;
}


bool 
AosJimoProgJob::getSameTypeInputTask(
		AosRundata *rdata,
		const vector<JSONValue> &tasks,
		vector<JSONValue> &tmpV)
{
	// This function combines tasks in 'tasks' into groups of 
	// tasks based on inputs. That is, tasks that have the 
	// same inputs are packed into the same group.
	if(tasks.size() <= 0) return false;
	tmpV.push_back(tasks[0]);
	for(size_t i=1; i<tasks.size(); i++)
	{
		bool is_found = false;
		map<string,string> inputs = getInputsFromTask(rdata,tasks[i]);
		for(size_t j=0; j<tmpV.size(); j++)
		{
			map<string,string> inputs2 = getInputsFromTask(rdata,tmpV[j]);
			if(compare(inputs,inputs2))
			{
				is_found = true;
				composeDataProcs(rdata,tmpV[j],tasks[i]);
				break;
			}
		}

		if(!is_found)
		{
			tmpV.push_back(tasks[i]);
		}
	}
	return true;
}

bool
AosJimoProgJob::composeDataProcs(
		AosRundata *rdata,
		JSONValue &task1,
		const JSONValue &task2)
{
	JSONValue dataprocs1 = task1["dataprocs"];
	JSONValue dataprocs2 = task2["dataprocs"];
	for(size_t i =0;i <dataprocs2.size();i++)
	{
		dataprocs1.append(dataprocs2[i]);
	}
	task1["dataprocs"] = dataprocs1;
	return true;
}


map<string,string>
AosJimoProgJob::getInputsFromTask(
		AosRundata* rdata,
		const JSONValue &task)
{
	// left string::inputs, right string: engine_type
	map<string,string> allInputs;
	string type = task["dataengine_type"].asString();
	JSONValue dataprocs = task["dataprocs"];
	for(size_t i = 0; i < dataprocs.size(); i++)
	{
		JSONValue inputs = dataprocs[i]["inputs"];
		allInputs[inputs.toStyledString()] = type;
	}
	return allInputs;
}


bool 
AosJimoProgJob::compare(
		const map<string,string> &lhs,
		const map<string,string> &rhs)
{
	// This function compares the two maps: lhs and rhs. It returns
	// true if they are identical.
	if(lhs.size() != rhs.size()) return false;

	map<string,string>::const_iterator itr;
	for(itr = rhs.begin();itr != rhs.end();itr++)
	{
		if(lhs.count(itr->first) <= 0)return false;
	}
	return true;
}


bool
AosJimoProgJob::appendStatement(
		AosRundata *rdata,
		const OmnString &section_name,
		const OmnString &statement)
{
	itr_t itr = mSections.find(section_name);
	if (itr == mSections.end())
	{
		vector<OmnString> stmts;
		stmts.push_back(statement);
		mSections[section_name] = stmts;
	}
	else
	{
		itr->second.push_back(statement);
	}
	return true;
}


void
AosJimoProgJob::setDeleteProcName(
		AosRundata *rdata,
		const OmnString &proc_name)
{
	mDeleteProcNames.push_back(proc_name);
}


bool
AosJimoProgJob::run(AosRundata *rdata)
{
	bool rslt = generateJobCode(rdata);	
	aos_assert_rr(rslt,rdata,false);
	if(mCode == "")
	{
		AosLogError(rdata, true, "miss_statement") << enderr;
		return false;
	}
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	AosJimoProgObj* prog = NULL;
	rslt = jimo_parser->parse(rdata, prog, mCode, statements);
	aos_assert_rr(rslt, rdata, false);
	if (statements.size() <= 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< AosFN("Statement") << mCode << enderr;
		return false;
	}

	for(size_t i = 0;i < statements.size();i++)
	{
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, this);
	}
	return true;
}


bool 
AosJimoProgJob::setUserVarValue(
		AosRundata *rdata, 
		const OmnString &varname, 
		const AosValueRslt &value)
{
	if (mUserVarMap.count(varname))
	{
		mUserVarMap[varname]->setUserVarValue(rdata, value);
		return true;
	}
	return false;
}


AosValueRslt
AosJimoProgJob::getUserVarValue(
		AosRundata *rdata, 
		const OmnString &varname)
{
	if (mUserVarMap.count(varname))
	{
		return mUserVarMap[varname]->getUserVarValue(rdata);
	}

	return AosValueRslt();
}


bool        
AosJimoProgJob::addUserVar(
		AosRundata *rdata,               
		const OmnString &varname, 
		const AosJimoLogicObjNewPtr &logic)
{
	mUserVarMap[varname] = logic;
	return true;
}
