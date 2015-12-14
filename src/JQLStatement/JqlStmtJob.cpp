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
#include "JQLStatement/JqlStmtJob.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "JQLStatement/JqlStmtQuery.h"

#include "AosConf/Job.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using boost::shared_ptr;
using namespace AosConf;

#include "JQLParser/JQLParser.h"                     
#include "Debug/Debug.h"

using namespace std;                                 
extern int AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;

AosJqlStmtJob::AosJqlStmtJob(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mJobName = "";
	mTaskNames = 0;
	mOp = JQLTypes::eOpInvalid;
	mIsJobLog = false;
}


AosJqlStmtJob::AosJqlStmtJob()
{
	mJobName = "";
	mTaskNames = 0;
	mOp = JQLTypes::eOpInvalid;
	mIsJobLog = false;
}

AosJqlStmtJob::~AosJqlStmtJob()
{
	OmnDelete mTaskNames;
}


bool
AosJqlStmtJob::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	// 2015/7/28
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createJob(rdata);
	if (mOp == JQLTypes::eShow) return showJobs(rdata);
	if (mOp == JQLTypes::eShowStatus) return showJobStatus(rdata);
	if (mOp == JQLTypes::eDrop) return dropJob(rdata);
	if (mOp == JQLTypes::eDescribe) return describeJob(rdata);
	if (mOp == JQLTypes::eRun) return runJob(rdata);
	if (mOp == JQLTypes::eWait) return waitJob(rdata);
	if (mOp == JQLTypes::eRestart) return restartJob(rdata);
	if (mOp == JQLTypes::eStop) return stopJob(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Job", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtJob::createJob(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString msg;
	//OmnString objid = getObjid(mJobName);
	AosXmlTagPtr doc = AosJqlStatement::getDoc(rdata, JQLTypes::eJobDoc, mJobName);
	if (doc)
	{
		msg << "Job " << mJobName << " already exists. "
			    << " Use \"drop\" command to delete firstly.";	
		rdata->setJqlMsg(msg);
		return true;
	}

	doc = convertToXml(rdata);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	if ( !createDoc(rdata, JQLTypes::eJobDoc, mJobName, doc) )
	{
		msg << "Failed to create Job doc.";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg << "Job " << mJobName << " created successfully.";
	rdata->setJqlMsg(msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtJob::convertToXml(const AosRundataPtr &rdata)
{
	OmnString task_name;
	boost::shared_ptr<Job> job = boost::make_shared<Job>();
	if (mConfParms)
	{
		return getConf(rdata, job, mConfParms);
	}

	job->setAttribute("zky_sdoc_objid", getObjid(rdata, JQLTypes::eJobDoc, mJobName));
	job->setAttribute("zky_name", mJobName);
	job->setAttribute("zky_otype", "job");
	job->setAttribute("zky_pctrs", "jobctnr");
	job->setAttribute("zky_job_version", "1");
	job->setSchedulerAttr("zky_numslots", "1");
	job->setSchedulerAttr("zky_jobschedulerid", "norm");
	job->setAttribute(AOSTAG_PUBLIC_DOC, "true");   
	job->setAttribute(AOSTAG_CTNR_PUBLIC, "true");  
	job->setAttribute(AOSTAG_PUB_CONTAINER, AOSCTNR_JOB_CONTAINER);   

	for (u32 i = 0; i < mTaskNames->size(); i++)                    
	{                                                                     
		task_name = (*mTaskNames)[i]->getValue(rdata.getPtrNoLock());
	
		AosXmlTagPtr taskDoc = AosJqlStatement::getDoc(rdata, JQLTypes::eTaskDoc, task_name);
		if (!taskDoc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return NULL;
		}
		job->setJobTask(taskDoc->toString());
	}
	return AosXmlParser::parse(job->getConfig() AosMemoryCheckerArgs); 
}


bool
AosJqlStmtJob::showJobs(const AosRundataPtr &rdata)
{
	if (mJobName != "") return showJobStat(rdata);
	OmnString msg;
	vector<AosXmlTagPtr> jobs;
	if ( !getDocs(rdata, JQLTypes::eJobDoc, jobs) )
	{
		msg << "No Job Found. ";
		rdata->setJqlMsg(msg);
		return true;
	}

	int total = jobs.size() ;
	
	//for(size_t i = 0; i < jobs.size(); i++)
	//{
	vector<AosXmlTagPtr>::iterator itr = jobs.begin();
	//vector<AosXmlTagPtr>::iterator tmp_itr;
	while(itr != jobs.end())
	{
		if(!(*itr)->getAttrBool("zky_public_origin_doc"))
		{
	//		tmp_itr = itr;
	//		itr++;
			jobs.erase(itr);
			total--;
		}
		else
		{
			itr++;
		}
	}

	OmnString content;
	content << "<content total=\"" << total << "\">"; 
	for(u32 i=0; i<jobs.size(); i++)
	{
		OmnString vv = jobs[i]->getAttrStr("zky_name", "");
		if (vv == "")
		{
			//Gavin 2015/07/31,JIMODB-145
			vv = jobs[i]->getAttrStr("zky_job_name", "");
		}
		if (vv != "")
		{
			content << "<record jobname=\"" << vv << "\"/>";
		}
	}

	content << "</content>";
	rdata->setResults(content);
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}
	vector<OmnString> fields;
	fields.push_back("jobname");
	map<OmnString, OmnString> alias_name;
	alias_name["jobname"] = "JobName";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);

	return true;
}


bool
AosJqlStmtJob::showJobStatus(const AosRundataPtr &rdata)
{
	// select synatx
    OmnString queryCmd = "";
	i64 page_size = 0;
	i64 black_size = 20;
	if(mLimit)
	{
		page_size = mLimit->getOffset();
		black_size = mLimit->getRowCount();
	}
   	queryCmd << "select zky_instance_id JOBID, zky_job_origin_job_name JOB_NAME, zky_status STATUS, zky_job_progress PROGRESS, zky_job_take_time TAKE_TIME, zky_job_started_time START_TIME, zky_job_finished_time END_TIME from jobctnr order by START_TIME" << " limit " << page_size << "," << black_size << ";";

    int val = AosParseJQL((char *)queryCmd.data(), false);
    if (val != 0) 
    {
        rdata->setJqlMsg("stdin: syntax error");
        return false;
    }
    gAosJQLParser.dump();

    vector<AosJqlStatementPtr> statements = gAosJQLParser.getStatements();
    OmnString errmsg = gAosJQLParser.getErrmsg();
    if (errmsg != "") 
    {
        AosSetEntityError(rdata, "JQLParser_", errmsg, errmsg) << enderr;
        return false;
    }

	for (u32 i=0; i<statements.size(); i++)
	{
		statements[i]->setContentFormat(mContentFormat);
		statements[i]->run(rdata, 0);
	}
	return true;
}

bool
AosJqlStmtJob::showJobStat(const AosRundataPtr &rdata)
{
	if (mIsJobLog)
		return showJobLogStat(rdata);

	// select synatx
    OmnString queryCmd = "";
    queryCmd << "select zky_job_name JOB_NAME, zky_status STATUS, zky_job_progress PROGRESS, zky_job_take_time TAKE_TIME, zky_job_started_time START_TIME, zky_job_finished_time END_TIME, zky_docid DOCID from jobctnr where zky_job_name = \"" << mJobName << "\" limit 100000;";

    int val = AosParseJQL((char *)queryCmd.data(), false);
    if (val != 0) 
    {
        rdata->setJqlMsg("stdin: syntax error");
        return false;
    }
    gAosJQLParser.dump();

    vector<AosJqlStatementPtr> statements = gAosJQLParser.getStatements();
    OmnString errmsg = gAosJQLParser.getErrmsg();
    if (errmsg != "") 
    {
        AosSetEntityError(rdata, "JQLParser_", errmsg, errmsg) << enderr;
        return false;
    }

	for (u32 i=0; i<statements.size(); i++)
	{
		statements[i]->setContentFormat(mContentFormat);
		statements[i]->run(rdata, 0);
	}
	return true;
}


bool
AosJqlStmtJob::showJobLogStat(const AosRundataPtr &rdata)
{
	//get jobid
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eJobDoc, mJobName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display job " << mJobName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}
	OmnString jobid = doc->getAttrStr("zky_docid", "");
	u64 page_size = 0;
	u64 black_size = 20;
	if(mLimit)
	{
		page_size = mLimit->getOffset();
		black_size = mLimit->getRowCount();
	}
	// select synatx
    OmnString queryCmd = "";
    queryCmd << "select zky_jobdocid jobid, zky_taskid, zky_docid docid, zky_task_serverid svrid, zky_status status, zky_starttime, zky_endtime, zky_take_time from task_ctnr where zky_jobdocid= \"" << jobid << "\"" <<" limit "<<page_size<<","<<black_size<<";";

    int val = AosParseJQL((char *)queryCmd.data(), false);
    if (val != 0) 
    {
        rdata->setJqlMsg("stdin: syntax error");
        return false;
    }
    gAosJQLParser.dump();

    vector<AosJqlStatementPtr> statements = gAosJQLParser.getStatements();
    OmnString errmsg = gAosJQLParser.getErrmsg();
    if (errmsg != "") 
    {
        AosSetEntityError(rdata, "JQLParser_", errmsg, errmsg) << enderr;
        return false;
    }

	for (u32 i=0; i<statements.size(); i++)
	{
		statements[i]->setContentFormat(mContentFormat);
		statements[i]->run(rdata, 0);
	}
	return true;
}


bool
AosJqlStmtJob::runJob(const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;

	if (mJobFileName != "")
	{
		return runJobByFileName(rdata);	
	}
	
	OmnString msg = "";
	OmnString objid = getObjid(rdata, JQLTypes::eJobDoc, mJobName);
	if ( !startJob(rdata, objid) )
	{
		msg << "Failed to run Job .";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg = rdata->getJqlMsg();
	if(msg == "") msg << "Job " << mJobName << " started successfully."; 
	rdata->setJqlMsg(msg);
	
	return true;
}


bool
AosJqlStmtJob::runJobByFileName(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString str;
	OmnFile file(mJobFileName, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file.getLength() > 0)
	{
		msg << "Failed to open job file : " << mJobFileName<< " .";
		rdata->setJqlMsg(msg);
		return false;
	}
	bool rslt = file.readToString(str);
	if (!rslt)
	{
		msg << "Failed to read job file : " << mJobFileName<< " .";
		rdata->setJqlMsg(msg);
		return false;
	}

	AosXmlTagPtr job_doc = AosXmlParser::parse(str AosMemoryCheckerArgs);
	if (!job_doc)
	{
		msg << "Failed to parse job doc .";
		rdata->setJqlMsg(msg);
		return false;
	}
	
	job_doc->setAttr("zky_job_name", mJobName);   
	job_doc->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	job_doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");  
	job_doc->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_JOB_CONTAINER);   
	job_doc->setAttr(AOSTAG_CONTAINER_OBJID, "task_ctnr");   

OmnCout << "===================== LoadData XML ==================" << endl;
OmnCout << job_doc->toString() << endl;
OmnCout << "===================== LoadData XML ==================" << endl;

	if ( !createDoc(rdata, JQLTypes::eJobDoc, mJobName, job_doc) )
	{
		msg << "Failed to create Job doc.";
		rdata->setJqlMsg(msg);
		return false;
	}

	OmnString objid = getObjid(rdata, JQLTypes::eJobDoc, mJobName);
	if ( !startJob(rdata, objid) )
	{
		msg << "Failed to run Job .";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg << "Job " << mJobName << " started successfully."; 
	rdata->setJqlMsg(msg);
	return true;
}

bool 
AosJqlStmtJob::dropJob(const AosRundataPtr &rdata)
{
	OmnString msg;
	bool rslt;
	OmnString objid = getObjid(rdata, JQLTypes::eJobDoc, mJobName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	aos_assert_r(doc, NULL);
	if(!doc->getAttrBool("zky_public_origin_doc"))
	{
		/*OmnString originName = doc->getAttrStr("zky_job_origin_job_name");
		if(originName != "")
		{
			OmnString newObjId = getObjid(rdata, JQLTypes::eJobDoc, originName);
			AosXmlTagPtr newDoc = getDocByObjid(rdata, newObjId);
			u64 job_id = newDoc->getAttrU64("zky_instance_id", 0);
			if(job_id == 0) return false;
			job_id--;
			newDoc->setAttr("zky_instance_id", job_id);
			rslt = AosModifyDoc(newDoc, rdata);
			aos_assert_r(rslt, false);
		}*/
		OmnString msg = "";
		msg << "Job " << mJobName << " isn't origin job, should not be described.";
		rdata->setJqlMsg(msg);
		return true;
	}
	rslt = deleteDocByObjid(rdata, objid);    
	if (!rslt)
	{                                                               
		msg << "Failed to delete job doc.";              
		rdata->setJqlMsg(msg);
		return false;                                               
	}                                                               
	msg << "Job " << mJobName << " dropped successfully."; 
	rdata->setJqlMsg(msg);
	return true;
}


bool
AosJqlStmtJob::describeJob(const AosRundataPtr &rdata)
{
	if (mJobName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eJobDoc, mJobName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display job " << mJobName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}
	bool originDoc = doc->getAttrBool("zky_public_origin_doc");
	if(!originDoc)
	{
		OmnString msg = "";
		msg << "The job " << mJobName << " isn't origin job, should not be described."; 
		rdata->setJqlMsg(msg);
		return true;
	}
	OmnString jobCode = doc->getNodeText("script");
	setPrintData(rdata, jobCode);
	return true;
	/*vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;

	fields.push_back("zky_name");
	fields.push_back("zky_type");
	fields.push_back("zky_job_version");
	alias_name["zky_name"] = "JobName";
	alias_name["zky_type"] = "Type";
	alias_name["zky_job_version"] = "Version";

	AosXmlTagPtr task_docs = doc->getFirstChild("tasks");
	if (!task_docs)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr task_doc = task_docs->getFirstChild(true);
	int idx = 1;
	OmnString task_name;
	OmnString tmp_name;
	while (task_doc)
	{
		task_name = task_doc->getAttrStr("zky_name");
		tmp_name << "zky_name_" << idx;
		fields.push_back(tmp_name);
		alias_name[tmp_name] << "TaskName_" << idx;
		doc->setAttr(tmp_name, task_name);
		task_doc = task_docs->getNextChild();
		tmp_name = "";
		idx++;
	}

	OmnString content = "<content>";
	content << doc->toString() << "</content>";
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, def);
	setPrintData(rdata, msg);
	return true;*/
}


AosJqlStatement *
AosJqlStmtJob::clone()
{
	return OmnNew AosJqlStmtJob(*this);
}


void 
AosJqlStmtJob::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtJob::setJobName(OmnString name)
{
	mJobName = name;
}


void 
AosJqlStmtJob::setLimit(AosJqlLimit* limit)
{
	mLimit = limit;
}

	
void
AosJqlStmtJob::setJobFileName(OmnString name)
{
	mJobFileName = name;
}

void
AosJqlStmtJob::setTaskNames(AosExprList *task_names)
{
	mTaskNames = task_names;
}


AosXmlTagPtr 
AosJqlStmtJob::createTaskConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	//task doc format:
	//<task zky_name="import_vc_792_task" zky_otype="task">
	//	<input_datasets>
	//		<dataset zky_name="vc_792_dataset"/>
	//	</input_datasets>
	//	<output_datasets>
	//		<dataset zky_name="idx_msisdn_datset"/>
	//		<dataset zky_name="doc_vc_792_dataset"/>
	//	</output_datasets>
	//	<map_task>
	//		<actions>
	//			<action zky_type="dataengine_scan_single">
	//				<dataproc zky_name="doc_all_proc"/>				
	//				<dataproc zky_name="idx_msisdn_key_proc"/>
	//				<dataproc zky_name="idx_msisdn_value_proc"/>
	//		</action>
	//		</actions>
	//	</map_task>
	//	<reduce_task>
	//		<actions>
	//			<dataproc zky_name="idx_msisdn_proc"/>
	//			<dataproc zky_name="doc_vc_792_proc"/>
	//		</actions>
	//	</reduce_task>
	//</task>
	OmnString otype = "task";
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr task_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eTaskDoc, avalue);
	if (!task_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr result_task_doc = task_doc->clone(AosMemoryCheckerArgsBegin);
	if (!result_task_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	modifyName(def, result_task_doc, aname, avalue);	

	// input_datasets
	OmnString tagname = "input_datasets";
	result_task_doc->removeNode(tagname, false, false); 
	AosXmlTagPtr result = result_task_doc->addNode1(tagname);
	if (!result)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr parm = task_doc->getFirstChild(tagname);	
	if (!parm)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	createDatasetsConf(rdata, parm, result, true);

	// output_datasets
	tagname = "output_datasets";
	result_task_doc->removeNode(tagname, false, false);
	result = result_task_doc->addNode1(tagname);
	if (!result)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	parm = task_doc->getFirstChild(tagname);	
	if (!parm)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	createDatasetsConf(rdata, parm, result, false);

	// map_task
	tagname = "map_task";
	result = result_task_doc->getFirstChild(tagname);
	if (!result)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	parm = task_doc->getFirstChild(tagname);	
	if (!parm)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	createMapTaskConf(rdata, parm, result);
	
	// reduce_task
	tagname = "reduce_task";
	result = result_task_doc->getFirstChild(tagname);
	if (!result)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	parm = task_doc->getFirstChild(tagname);	
	if (!parm)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	createReduceTaskConf(rdata, parm, result);
	return result_task_doc;
}


bool
AosJqlStmtJob::createMapTaskConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def,
		const AosXmlTagPtr &result)
{
	//<map_task>
	//	<actions>
	//		<action zky_type="dataengine_scan_single">
	//			<dataproc zky_name="doc_all_proc"/>				
	//			<dataproc zky_name="idx_msisdn_key_proc"/>
	//			<dataproc zky_name="idx_msisdn_value_proc"/>
	//		</action>
	//	</actions>
	//</map_task>
	OmnString tagname = "actions";
	AosXmlTagPtr actions_tag = def->getFirstChild(tagname);
	if (!actions_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	AosXmlTagPtr actions_rslt_tag = result->getFirstChild(tagname);
	if (!actions_rslt_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	tagname = "action";
	AosXmlTagPtr action_tag = actions_tag->getFirstChild(tagname);
	AosXmlTagPtr action_rslt_tag = actions_rslt_tag->getFirstChild(tagname);
	while(action_tag && action_rslt_tag)
	{
		createMapActionConf(rdata, action_tag, action_rslt_tag);
		action_tag = actions_tag->getNextChild();
		action_rslt_tag = actions_tag->getNextChild();
	}
	return true;
}


bool
AosJqlStmtJob::createMapActionConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def,
		const AosXmlTagPtr &result)
{
	// map action
	//	<action zky_type="dataengine_scan_single">
	//		<dataproc zky_name="doc_all_proc"/>				
	//		<dataproc zky_name="idx_msisdn_key_proc"/>
	//		<dataproc zky_name="idx_msisdn_value_proc"/>
	//	</action>
	
	AosXmlTagPtr tag;
	vector<AosXmlTagPtr> rslt;
	AosXmlTagPtr dataproc = def->getFirstChild();
	while(dataproc)
	{
		result->removeNode("dataproc", false, false);
		tag = createMapDataProcConf(rdata, dataproc);
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		rslt.push_back(tag);
		dataproc = def->getNextChild();
	}

	for (u32 i = 0; i < rslt.size(); i++)
	{
		result->addNode(rslt[i]);
	}
	return true;
}


bool
AosJqlStmtJob::createReduceTaskConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def,
		const AosXmlTagPtr &result)
{
	OmnString tagname = "actions";
	AosXmlTagPtr actions_tag = def->getFirstChild(tagname);
	if (!actions_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	result->removeNode(tagname, false, false);
	AosXmlTagPtr result_actions = result->addNode1(tagname);

	if (!result_actions)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	createReduceActionConf(rdata, actions_tag, result_actions);
	return true;
}


bool
AosJqlStmtJob::createReduceActionConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def,
		const AosXmlTagPtr &result)
{
	//reduce action
	//<actions>
	//	<dataproc zky_name="idx_msisdn_proc"/>
	//	<dataproc zky_name="doc_vc_792_proc"/>
	//</actions>
	AosXmlTagPtr tag;
	AosXmlTagPtr dataproc = def->getFirstChild();
	while(dataproc)
	{
		tag = createReduceDataProcConf(rdata, dataproc);
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		result->addNode(tag);
		dataproc = def->getNextChild();
	}
	return true;
}


AosXmlTagPtr
AosJqlStmtJob::createMapDataProcConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	//<dataproc zky_name="doc_all_proc" zky_otype="dataproc" type="compose">
	//	<input>
	//		<filed zky_input_field_name="vc_792_rcd.all"/>
	//	</input>
	//	<output>
	//		<field zky_output_field_name="doc_vc_792_rcd.all"/>
	//	</output>
	//</dataproc>
	OmnString otype = "dataproc";
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr proc_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataProcDoc, avalue);
	if (!proc_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	modifyName(def, proc_doc, aname, avalue);	
	return proc_doc;
}


AosXmlTagPtr
AosJqlStmtJob::createReduceDataProcConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	//<dataproc zky_name="idx_msisdn_proc" zky_otype="dataproc" type="iilbachopr" 
	//	datasetname="idx_msisdn_datset" iilname="_zt22_msisdn">
	//	<datacollector zky_name="idx_msisdn_datset" zky_type="iil">
	//		<asm type="stradd">
	//			<record zky__name="iil_rcd"/> 
	//		</asm>
	//	</datacollector>
	//</dataproc>

	OmnString otype = "dataproc";
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr proc_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataProcDoc, avalue);
	if (!proc_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr collector_tag = proc_doc->getFirstChild("datacollector");
	if (!collector_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr asm_tag = collector_tag->getFirstChild("asm");
	if (!asm_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	OmnString tagname = "record";
	AosXmlTagPtr record_tag = asm_tag->getFirstChild(tagname);
	if (!record_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr result= createDataRecordConf(rdata, record_tag);
	if (!result)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	asm_tag->removeNode(tagname, false, false);
	asm_tag->addNode(result);

	modifyName(def, proc_doc, aname, avalue);	
	return proc_doc;
}


bool
AosJqlStmtJob::createDatasetsConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def,
		const AosXmlTagPtr &result,
		const bool input_dataset)
{
	AosXmlTagPtr tag;
	AosXmlTagPtr dataset_tag = def->getFirstChild();
	while(dataset_tag)
	{
		if (input_dataset)
		{
			 tag = createInputDatasetConf(rdata, dataset_tag);
		}
		else
		{
			 tag = createOutputDatasetConf(rdata, dataset_tag);
		}
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		result->addNode(tag);
		dataset_tag = def->getNextChild();
	}
	return true;
}


AosXmlTagPtr
AosJqlStmtJob::createOutputDatasetConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// format:
	//<dataset zky_name="idx_msisdn_datset" zky_otype="dataset"/>
	OmnString otype = "dataset";
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr dataset_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDatasetDoc, avalue);
	if (!dataset_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	modifyName(def, dataset_doc, aname, avalue);	
	return dataset_doc;
}


AosXmlTagPtr
AosJqlStmtJob::createInputDatasetConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// format:
	//<dataset zky_name="vc_792_dataset" zky_otype="dataset">
	//	<datascanner zky_name="vc_792_scanner"/>
	//	<dataschema zky_name="vc_792_schema"/>
	//</dataset>

	OmnString otype = "dataset";
	OmnString aname = "zky_name";
	OmnString avalue= def->getAttrStr(aname, "");
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr dataset_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDatasetDoc, avalue);
	if (!dataset_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	OmnString tagname = "datascanner";
	AosXmlTagPtr scanner_tag = dataset_doc->getFirstChild(tagname);
	if (!scanner_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr result = createDataScannerConf(rdata, scanner_tag);
	if (!result)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	OmnString tagname1 = "dataschema";
	AosXmlTagPtr schema_tag = dataset_doc->getFirstChild(tagname1);
	if (!schema_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr result1 = createDataSchemaConf(rdata, schema_tag);
	if (!result1)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	dataset_doc->removeNode(tagname, false, false);
	dataset_doc->removeNode(tagname1, false, false);
	dataset_doc->addNode(result);
	dataset_doc->addNode(result1);
	modifyName(def, dataset_doc, aname, avalue);	
	return dataset_doc;
}


AosXmlTagPtr
AosJqlStmtJob::createDataScannerConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	//format:
	//<datascanner zky_name="vc_792_scanner" zky_otype="datasanner" 
	//	zky_jimo_objid="datascanner_parallel_jimodoc_v1" zky_public_doc="true" zky_public_ctnr="true" zky_objid="test_4">
	//	<dataconnector zky_name="vc_792_file"/>
	//</datascanner>

	OmnString otype = "datascanner";
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr scanner_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataScannerDoc, avalue);
	if (!scanner_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	OmnString tagname = "dataconnector";
	AosXmlTagPtr connector_tag = scanner_doc->getFirstChild(tagname);
	if (!connector_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr result = createConnectorConf(rdata, connector_tag);
	scanner_doc->removeNode(tagname, false, false);
	scanner_doc->addNode(result);

	modifyName(def, scanner_doc, aname, avalue);	
	return scanner_doc;
}


AosXmlTagPtr
AosJqlStmtJob::createConnectorConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	//<dataconnector zky_name="vc_792_file" zky_otype="dataconnector" 
	//	zky_jimo_objid="dataconnector_file_jimodoc_v1" physical_id="0"
	//	file_name="/home/prod/vc_792_file.txt" zky_character="UTF-8">
	//	<split zky_jimo_objid="dataspliter_filesize_jimodoc_v1" group_size="1G"></split>
	//</dataconnector>
	OmnString otype = "dataconnector";
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr conn_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataConnectorDoc, avalue);
	if (!conn_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	modifyName(def, conn_doc, aname, avalue);	
	return conn_doc;
}


AosXmlTagPtr
AosJqlStmtJob::createDataSchemaConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	//format:
	//<dataschema zky_name="dataschemaname" zky_otyp="dataschema" 
	//	zky_jimo_objid="dataschema_unilength_jimodoc_v1">
	//		<record zky_name="vc_792_rcd"/>
	//</dataschema>	
	OmnString otype = "dataschema";	
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr schema_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eSchemaDoc, avalue);
	if (!schema_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr record_tag = schema_doc->getFirstChild("datarecord"); 
	if (!record_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	OmnString tagname = record_tag->getTagname();

	AosXmlTagPtr result = createDataRecordConf(rdata, record_tag);	
	if (!result)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	schema_doc->removeNode(tagname, false, false);
	schema_doc->addNode(result);
	modifyName(def, schema_doc, aname, avalue);	
	return schema_doc;
}


AosXmlTagPtr
AosJqlStmtJob::createDataRecordConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// format:
	//<datarecord zky_name="vc_792" zky_otype="datarecord" type="fixbin" length="792">
	//	<datafields>
	//		<datafield zky_name="system_type"/>
	//		<datafield zky_name="msisdn"/>
	//		<datafield zky_name="other_party"/>
	//		<datafield zky_name="start_datetime"/>
	//		<datafield zky_name="call_duration"/>
	//		<datafield zky_name="all"/>
	//	</datafields>
	//</datarecord>
	OmnString otype = "datarecord";	
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr record_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataRecordDoc, avalue);
	if (!record_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	OmnString record_type = record_doc->getAttrStr(AOSTAG_TYPE);
	if (record_type == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	OmnString tagname = "datafields";
	AosXmlTagPtr fields_tag = record_doc->getFirstChild(tagname);
	if (!fields_tag)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	int next_length = 0;
	vector<AosXmlTagPtr> result;
	AosXmlTagPtr field_tag = fields_tag->getFirstChild();
	while(field_tag)
	{
		AosXmlTagPtr tag = createDataFieldConf(rdata, field_tag);
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}

		OmnString type = tag->getAttrStr(AOSTAG_TYPE);
		if (record_type == AOSRECORDTYPE_FIXBIN && 
				(type == AOSDATAFIELDTYPE_STR || type == AOSDATAFIELDTYPE_U64))
		{
			int off = tag->getAttrInt(AOSTAG_OFFSET, -1);
			if (off == -1)
			{
				tag ->setAttr(AOSTAG_OFFSET, next_length);
				off = next_length;
			}

			int len = tag->getAttrInt(AOSTAG_LENGTH, 0);
			if (type == AOSDATAFIELDTYPE_U64 && len == 0)
			{
				len = sizeof(u64);
				tag->setAttr(AOSTAG_LENGTH, len);
			}
			next_length = off + len;
		}

		result.push_back(tag);
		field_tag = fields_tag->getNextChild();
	}

	int len = record_doc->getAttrInt(AOSTAG_LENGTH, 0);
	if (len != 0)
	{
		if (!(next_length <= len))
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
	}
	if (len == 0 && record_type == AOSRECORDTYPE_FIXBIN)
	{
		record_doc->setAttr(AOSTAG_LENGTH, next_length);
	}
	record_doc->removeNode(tagname, false, false);
	fields_tag = record_doc->addNode1(tagname);

	for(u32 i = 0; i < result.size(); i++)
	{
		fields_tag->addNode(result[i]);
	}

	modifyName(def, record_doc, aname, avalue);	
	return record_doc;
}


AosXmlTagPtr
AosJqlStmtJob::createDataFieldConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// format: <datafield zky_name="system_type" zky_otype="datafiled" type="str" zky_length="2" zky_offset="0"/>
	OmnString otype = "datafield";
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, ""); 
	if (avalue == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr field_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataFieldDoc, avalue);
	if (!field_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	modifyName(def, field_doc, aname, avalue);
	return field_doc;
}


void
AosJqlStmtJob::modifyName(
		const AosXmlTagPtr &def,
		const AosXmlTagPtr &doc,
		const OmnString &aname,
		const OmnString &avalue)
{
	//doc->removeAttr(aname);
	//doc->removeAttr(AOSTAG_NAME);
	doc->removeAttr(aname, 0, true);
	doc->removeAttr(AOSTAG_NAME, 0, true);
	doc->removeAttr(AOSTAG_PUB_CONTAINER, 0, true);
	doc->removeAttr(AOSTAG_TOO_SHORT_POLICY, 0, true);
	doc->removeAttr(AOSTAG_OBJID, 0, true);
	doc->removeAttr(AOSTAG_SITEID, 0, true);
	doc->removeAttr(AOSTAG_CREATOR, 0, true);
	doc->removeAttr(AOSTAG_CTIME, 0, true);
	doc->removeAttr(AOSTAG_MTIME, 0, true);
	doc->removeAttr(AOSTAG_CT_EPOCH, 0, true);
	doc->removeAttr(AOSTAG_MT_EPOCH, 0, true);
	//doc->removeAttr(AOSTAG_DOCID, 0, true);

	OmnString name = def->getAttrStr(AOSTAG_NAME, "");
	if (name == "") name = avalue;

	doc->setAttr(AOSTAG_NAME, name);
}



AosXmlTagPtr
AosJqlStmtJob::getDoc(
		const AosRundataPtr &rdata,
		const OmnString &attrname, 
		const OmnString &avalue,
		const OmnString &otype)
{
	vector<OmnString> key_names;
	key_names.push_back(attrname);
	key_names.push_back(AOSTAG_OTYPE);

  	vector<OmnString> key_values;
	key_values.push_back(avalue);
	key_values.push_back(otype);
	AosXmlTagPtr doc = AosGetDocByKeys(rdata, "", key_names, key_values);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	doc = doc->clone(AosMemoryCheckerArgsBegin);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	doc->removeAttr(AOSTAG_DOCID);
	doc->removeAttr(AOSTAG_OBJID);
	doc->removeAttr(AOSTAG_SITEID);
	doc->removeAttr(AOSTAG_PARENTC);
	doc->removeAttr(AOSTAG_CREATOR);
	doc->removeAttr(AOSTAG_CTIME);
	doc->removeAttr(AOSTAG_MTIME);
	doc->removeAttr(AOSTAG_CT_EPOCH);
	doc->removeAttr(AOSTAG_MT_EPOCH);
	doc->removeAttr(AOSTAG_MODUSER);
	doc->removeAttr(AOSTAG_OTYPE);
	return doc;
}


bool
AosJqlStmtJob::waitJob(const AosRundataPtr &rdata)
{
	OmnString msg;
	// 1. get job doc
	AosXmlTagPtr jobdoc = AosJqlStatement::getDoc(rdata, JQLTypes::eJobDoc, mJobName);
	if (!jobdoc)
	{
		msg << "Job '(" << mJobName << ")' not found";
		rdata->setJqlMsg(msg);
		return true;
	}

    OmnString queryCmd = "";
    queryCmd << "select zky_job_name, zky_status from jobctnr where zky_job_name = \"" << mJobName << "\";";

	OmnString errmsg;
	AosJqlStatementPtr stmtObj = AosParseStatement(
			queryCmd, errmsg, rdata.getPtr());
	if (!stmtObj)
	{
		rdata->setJqlMsg("stdin: syntax error");
		return false;
	}

	AosJqlStmtQuery* queryObj = 
		 dynamic_cast<AosJqlStmtQuery*>(stmtObj.getPtr());
	aos_assert_r(queryObj, false);
	
	AosXmlTagPtr query_conf = queryObj->getQueryConf(rdata);
	aos_assert_r(query_conf, false);
	
	while (1)
	{
		AosXmlTagPtr result = queryObj->getQueryRslt(rdata, query_conf);
		aos_assert_r(result, false);

		OmnScreen << "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYY" << endl;
		OmnScreen << result->toString() << endl;
		OmnScreen << "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYY" << endl;
	
		AosXmlTagPtr recordNode = result->getFirstChild("record");
		if (recordNode)
		{
			AosXmlTagPtr statusNode = recordNode->getFirstChild("zky_status");
			aos_assert_r(statusNode, false);
			if (statusNode->getNodeText() == "finish")
			{
				rdata->setJqlMsg("Job finished");
				return true;
			}
		}
		sleep(3);
		OmnScreen << "Job not finish" << endl;
	}

	return true;
}


void
AosJqlStmtJob::setIsLog(bool is_log)
{
	mIsJobLog = is_log;
}


bool 
AosJqlStmtJob::restartJob(const AosRundataPtr &rdata)
{
	OmnString msg = "";
	OmnString objid = getObjid(rdata, JQLTypes::eJobDoc, mJobName);
	if ( !AosJqlStatement::restartJob(rdata, objid) )
	{
		msg << "Failed to run Job .";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg << "Job " << mJobName << " restart successfully."; 
	rdata->setJqlMsg(msg);
	return true;
}


bool 
AosJqlStmtJob::stopJob(const AosRundataPtr &rdata)
{
	OmnString msg = "";
	OmnString objid = getObjid(rdata, JQLTypes::eJobDoc, mJobName);
	if ( !AosJqlStatement::stopJob(rdata, objid) )
	{
		msg << "Failed to run Job .";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg << "Job " << mJobName << " stop successfully."; 
	rdata->setJqlMsg(msg);
	return true;
}
