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
// 2013/07/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoJob/JimoJob.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "JimoJob/JimoTask.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "JimoProg/JimoProgLoadData2.h"
#include "JimoProg/Ptrs.h"
#include "SEInterfaces/JobMgrObj.h"
#include "API/AosApi.h"
#include <string>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoJob_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoJob(version);
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


AosJimoJob::AosJimoJob(const int version)
{
	mJimoVersion = version;
}


AosJimoJob::AosJimoJob(const bool regflag)
{
	mJimoVersion = 1;
}


// Chen Ding, 2015/05/26
// AosJimoJob::AosJimoJob(const AosJimoJob &job)
// {
// 	mJimoVersion = job.mJimoVersion;
// }

AosJimoJob::AosJimoJob(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	mJimoVersion = 1;
	config(def, rdata);
}

AosJimoJob::~AosJimoJob()
{
}


bool 
AosJimoJob::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	return false;
}


bool 
AosJimoJob::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	return false;
}


bool 
AosJimoJob::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	return false;
}


AosJimoJobPtr
AosJimoJob::clone()
{
	return NULL;
}


AosJimoJobPtr 
AosJimoJob::clone( 
		const AosXmlTagPtr &def,
		const AosRundataPtr &)
{
	return NULL;
}


bool 
AosJimoJob::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	AosJimoScheduler scheduler;

	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, obj_name);
	OmnString conf;
	conf << "<" << AOSTAG_JOB << " "
			<< AOSTAG_JOB_VERSION << "=\"1\" "
			<< AOSTAG_OTYPE << "=\"job\" "
			<< AOSTAG_PUB_CONTAINER << "=\"jobctnr\" " 
			<< AOSTAG_JOB_NAME << "=\"" << obj_name << "\" "
			<< AOSTAG_CTNR_PUBLIC << "=\"true\" "
			<< AOSTAG_PUBLIC_DOC << "=\"true\" "
			<< AOSTAG_OBJID << "=\"" << objid << "\" "
			<< AOSTAG_CONTAINER_OBJID << "=\"task_ctnr\">";

	conf << scheduler.getConfig();
	conf << "<" << AOSTAG_TASKS_TAG << ">"; 

	JSONValue jsonObj;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, jsonObj);
	aos_assert_r(rslt, false);

	// Add by Young, 2015/04/27
	if (jsonObj["type"].asString() == "loaddata")
	{
		AosJPLoadData2Ptr loaddata = OmnNew AosJPLoadData2(0);
		loaddata->createByJql(rdata, obj_name, jsonstr, loaddata);
		return true;
	}

	JSONValue task_json = jsonObj["tasks"];
	aos_assert_r(!task_json.empty(), false);
	for (u32 i = 0; i < task_json.size(); i++)
	{
		AosJimoTask task(task_json[i], this);
		conf << task.getConfig(rdata, prog);
	}

	conf << "</" << AOSTAG_TASKS_TAG << ">"; 
	conf << "</" << AOSTAG_JOB << ">";
	AosXmlTagPtr new_doc = AosCreateDoc(conf, true, rdata);
	if(!new_doc)
	{
		OmnString errmsg = "";
		errmsg << "[ERR] : Job " << obj_name << " exist";
		rdata->setJqlMsg(errmsg);
		aos_assert_r(new_doc, false);
		return false;
	}
	
OmnCout << "66666666666666666666" << conf << endl;
	return true;
}


bool 
AosJimoJob::runByJql(
		AosRundata *rdata,
		const OmnString &jobname, 
		const OmnString &jsonstr)
{
	JSONValue jsonObj;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, jsonObj);
	aos_assert_r(rslt, false);

	u32 threadsNum = jsonObj["threads"].asUInt();
	if (threadsNum <= 0) threadsNum = 1;

	// 1. get original job doc
	OmnString jobObjid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, jobname);
	AosXmlTagPtr job_conf = AosGetDocByObjid(jobObjid, rdata);
	if(!job_conf)
	{
		OmnString errMsg = "";
		errMsg << "job " << "\"" << jobname << "\"" << " maybe doesn't exist!";
		rdata->setJqlMsg(errMsg);
		aos_assert_r(job_conf, false);
		return false;
	}

	// 2. get current job doc
	//int crt_job_idx = job_conf->getAttrInt("zky_crt_job_idx", 0);
	//aos_assert_r(crt_job_idx >= 0, false);
	//crt_job_idx++;
	//job_conf->setAttr("zky_crt_job_idx", crt_job_idx);

	//OmnString crt_job_name = "";
	//crt_job_name << jobname << "_ddd_" << crt_job_idx;
	//AosXmlTagPtr crt_job_doc = job_conf->clone(AosMemoryCheckerArgsBegin);
	//aos_assert_r(crt_job_doc, false);
	//crt_job_doc->removeMetadata();

	//Jozhi 2015-04-10 start max task.exe process
	u32 numslots = jsonObj["procs"].asUInt();
	if (numslots <=0 ) numslots = 1;

	//AosXmlTagPtr schedule_conf = crt_job_doc->getFirstChild("scheduler");
	AosXmlTagPtr schedule_conf = job_conf->getFirstChild("scheduler");

	if (schedule_conf)
	{
		schedule_conf->setAttr("zky_numslots", numslots);
	}

	//AosXmlTagPtr tasks_conf = crt_job_doc->getFirstChild("tasks");
	AosXmlTagPtr tasks_conf = job_conf->getFirstChild("tasks");

	if(!tasks_conf)
	{
		aos_assert_r(tasks_conf, false);
		return false;
	}
	AosXmlTagPtr task = tasks_conf->getFirstChild(true);
	while (task)
	{
		task->setAttr("zky_max_num_thread", threadsNum);
		task = tasks_conf->getNextChild();
	}

	OmnString args = "";  
	args << "objid=" << jobname << ","<< "cmdid=" << AOSCOMMAND_STARTJOB;
	AosJobMgrObjPtr jobmgr = AosJobMgrObj::getJobMgr();
	aos_assert_r(jobmgr, false);

	return jobmgr->startJob(args, jobObjid, job_conf, rdata);

	// 3. save original & current job doc
	//OmnString crt_job_objid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, crt_job_name);
	//crt_job_doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//crt_job_doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//crt_job_doc->setAttr(AOSTAG_PUB_CONTAINER, "jobctnr");
	//crt_job_doc->setAttr(AOSTAG_OBJID, crt_job_objid);
	//crt_job_doc->setAttr("zky_job_name", crt_job_name);

	//crt_job_doc = AosCreateDoc(crt_job_doc->toString(), true, rdata);
	//aos_assert_r(crt_job_doc, false);
	//if (AosModifyDoc(job_conf, rdata))
	//{
	//	if (jobmgr->startJob(args, crt_job_objid, crt_job_doc, rdata))
	//	{
	//		return true;
	//	}
	//}
	//return false;
}


bool
AosJimoJob::showByJql(
		AosRundata *rdata,
		const OmnString &obj_name, 
		const OmnString &jsonstr)
{
	return false;
}


bool 
AosJimoJob::dropByJql(
		AosRundata *rdata,
		const OmnString &obj_name, 
		const OmnString &jsonstr)
{
	return false;
}


AosJimoPtr 
AosJimoJob::cloneJimo() const
{
	return new AosJimoJob(1);
}

