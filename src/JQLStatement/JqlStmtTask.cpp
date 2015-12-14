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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtTask.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "AosConf/JobTask.h"
#include "AosConf/DataEngineScanSingle.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
using boost::shared_ptr;
using namespace AosConf;

AosJqlStmtTask::AosJqlStmtTask()
{
	mTaskName = "";
	mInputDataSetNames = 0;
	mOutputDataSetNames = 0;
	mDataProcsNames = 0;
	mActionNames = 0;
	mMapTaskUseSingleFlag = false;
	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtTask::~AosJqlStmtTask()
{
	OmnDelete(mInputDataSetNames);
	OmnDelete(mOutputDataSetNames);
	OmnDelete(mDataProcsNames);
	OmnDelete(mActionNames);
}

bool
AosJqlStmtTask::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createTask(rdata);
	if (mOp == JQLTypes::eShow) return showTasks(rdata);
	if (mOp == JQLTypes::eDrop) return dropTask(rdata);
	if (mOp == JQLTypes::eDescribe) return describeTask(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Task", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtTask::createTask(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString objid = getObjid(rdata, JQLTypes::eTaskDoc, mTaskName);
	//generate the xml data firstly
	AosXmlTagPtr doc = convertToXml(rdata);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	if ( !createDoc(rdata, JQLTypes::eTaskDoc, mTaskName, doc) )
	{
		OmnString error;
		error << "Failed to create Task doc";
		rdata->setJqlMsg(error);

		OmnCout << "Failed to create Task doc." << endl;
		return false;
	}

	OmnString msg;
	msg << "Task '" << mTaskName << "'created successfully.";
	rdata->setJqlMsg(msg);

	return true;
}


AosXmlTagPtr
AosJqlStmtTask::convertToXml(const AosRundataPtr &rdata)
{
	boost::shared_ptr<JobTask> task = boost::make_shared<JobTask>();
	if (mConfParms)
	{
		return getConf(rdata, task, mConfParms);
	}

	task->setAttribute("zky_name", AosObjid::getObjidByJQLDocName(JQLTypes::eTaskDoc, mTaskName));
	task->setAttribute(AOSTAG_PUBLIC_DOC, "true");   
	task->setAttribute(AOSTAG_CTNR_PUBLIC, "true");  
	task->setAttribute(AOSTAG_PUB_CONTAINER, AOSCTNR_TASK_CONTAINER);   
	task->setAttribute("zky_name", mTaskName);   
	task->setAttribute("zky_otype", "task");   

	for (u32 i = 0; i < mInputDataSetNames->size(); i++) 
	{
		OmnString dsname = (*mInputDataSetNames)[i]->getValue(rdata.getPtrNoLock());
		AosXmlTagPtr dsconf = getDoc(rdata, JQLTypes::eDatasetDoc, dsname);
		if (!dsconf)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return NULL;
		}

		task->setInputDataset(dsconf->toString());
	}
	
	for (u32 i = 0; i < mOutputDataSetNames->size(); i++) 
	{                                                                     
		OmnString dsname = (*mOutputDataSetNames)[i]->getValue(rdata.getPtrNoLock());
		//AosXmlTagPtr dsconf = getDoc(JQLTypes::eDatasetDoc, dsname);
		//if (!dsconf)
		//{
		//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//	OmnAlarm << __func__ << enderr;
		//	return NULL;
		//}

		boost::shared_ptr<DataSet> dsObj = boost::make_shared<DataSet>();
		dsObj->setAttribute("zky_name", dsname);
		task->setOutputDataset(dsObj);
	}

	//if (mMapTaskUseSingleFlag);
	boost::shared_ptr<MapTask> mapTaskObj = boost::make_shared<MapTask>();
	boost::shared_ptr<DataEngineScanSingle> dataEngineObj = boost::make_shared<DataEngineScanSingle>();
	for (u32 i = 0; i < mDataProcsNames->size(); i++)
	{                                                                     
		OmnString dpname = (*mDataProcsNames)[i]->getValue(rdata.getPtrNoLock());
		AosXmlTagPtr dpconf = getDoc(rdata, JQLTypes::eDataProcDoc, dpname);
		if (!dpconf)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return NULL;
		}
		dataEngineObj->setDataProc(dpconf->toString());
	}
	mapTaskObj->setDataEngine(dataEngineObj);
	task->setMapTask(mapTaskObj);

	boost::shared_ptr<ReduceTask> reduceTaskObj = boost::make_shared<ReduceTask>();
	for (u32 i = 0; i < mActionNames->size(); i++)
	{                                                                     
		OmnString dsname = (*mActionNames)[i]->getValue(rdata.getPtrNoLock());
		AosXmlTagPtr dsconf = getDoc(rdata, JQLTypes::eDataProcDoc, dsname);
		if (!dsconf)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return NULL;
		}

		reduceTaskObj->setDataProc(dsconf->toString());
	}
	task->setReduceTask(reduceTaskObj);

	return AosXmlParser::parse(task->getConfig() AosMemoryCheckerArgs); 
}

bool
AosJqlStmtTask::showTasks(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> tasks;
	if ( !getDocs(rdata, JQLTypes::eTaskDoc, tasks) )
	{
		msg << "No Task Found. ";
		rdata->setJqlMsg(msg);

		OmnCout << "No Task Found. " << endl;
		return true;
	}
	OmnString content = "<content>";
	for(u32 i=0; i<tasks.size(); i++)
	{
		OmnString vv = tasks[i]->getAttrStr("zky_name", "");
		if (vv != "") {
			content << "<record taskname=\"" << vv << "\"/>";
		}
	}

	content << "</content>";
	rdata->setResults(content);

	vector<OmnString> fields;
	fields.push_back("taskname");

	map<OmnString, OmnString> alias_name;
	alias_name["taskname"] = "TaskName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	if (mContentFormat == "xml")
	{
		rdata->setResults(doc->toString());
		return true;
	}
	setPrintData(rdata, msg);
	return true;
}

bool 
AosJqlStmtTask::dropTask(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eTaskDoc, mTaskName);
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                            
		msg << "Failed to delete data record doc."; 
		rdata->setJqlMsg(msg);
		OmnCout << "Failed to delete data record doc." << endl; 
		return false;   
	}                                                        
	msg << "Task '" << mTaskName << "'dropped successfully.";
	rdata->setJqlMsg(msg);
	OmnCout << "Task " << mTaskName<< " run successfully." << endl; 
	return true;
}


bool
AosJqlStmtTask::describeTask(const AosRundataPtr &rdata)
{
	if (mTaskName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eTaskDoc, mTaskName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display task " << mTaskName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;

	int idx = 0;
	fields.push_back("zky_name");
	composeXml(rdata, "zky_dataset_name", "input_datasets", "DataSet_", doc, fields, alias_name, &idx);
	composeXml(rdata, "zky_dataset_name", "output_datasets", "DataSet_", doc, fields, alias_name, &idx);
	idx = 0;
	composeXml(rdata, "zky_dataproc_name", "map_task", "DataProc_", doc, fields, alias_name, &idx);
	composeXml(rdata, "zky_dataproc_name", "reduce_task", "DataProc_", doc, fields, alias_name, &idx);
	alias_name["zky_name"] = "Name";

	OmnString content = "<content>";
	content << doc->toString() << "</content>";
	AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, def);
	if (mContentFormat == "xml")
	{
		rdata->setResults(def->toString());
		return true;
	}
	setPrintData(rdata, msg);
	return true;
}


bool
AosJqlStmtTask::composeXml(
		const AosRundataPtr &rdata,
		const OmnString &tagname, 
		const OmnString &child_tagname, 
		const OmnString &attr_name, 
		const AosXmlTagPtr &doc,
		vector<OmnString> &fields,      
		map<OmnString, OmnString> &alias_name,
		int* idx)
{
	if (!idx) *idx = 0;
	AosXmlTagPtr node = doc->getFirstChild(child_tagname);
	if (!node)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (child_tagname == "map_task")
	{
		node = node->getFirstChild(true);
		if (!node)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		node = node->getFirstChild(true);
		if (!node)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}

	if (child_tagname == "reduce_task")
	{
		node = node->getFirstChild(true);
		if (!node)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}

	if (!node)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr child_node = node->getFirstChild(true);
	OmnString field_name;
	OmnString tmp_name;
	while (child_node)
	{
		field_name = child_node->getAttrStr(tagname);
		tmp_name << tagname << (*idx);
		fields.push_back(tmp_name);
		OmnString vv ;
		vv << attr_name << (*idx);
		alias_name[tmp_name] = vv;
		doc->setAttr(tmp_name, field_name);
		child_node = node->getNextChild();
		tmp_name = "";
		(*idx)++;
	}
	return true;
}


AosJqlStatement *
AosJqlStmtTask::clone()
{
	return OmnNew AosJqlStmtTask(*this);
}


void 
AosJqlStmtTask::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtTask::setTaskName(OmnString name)
{
	mTaskName = name;
}


void
AosJqlStmtTask::setInputDataSetNames(AosExprList *input_dataset_names)
{
	mInputDataSetNames = input_dataset_names;
}


void
AosJqlStmtTask::setOutputDataSetNames(AosExprList *output_dataset_names)
{
	mOutputDataSetNames = output_dataset_names;
}


void
AosJqlStmtTask::setDataProcsNames(AosExprList *data_procs_names)
{
	mDataProcsNames = data_procs_names;
}


void
AosJqlStmtTask::setActionNames(AosExprList *action_names)
{
	mActionNames = action_names;
}

