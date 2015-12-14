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
:
AosGenericObj(version)
{
}


AosJimoJob::AosJimoJob(const bool regflag)
:
AosGenericObj(0)
{
}

AosJimoJob::AosJimoJob(const AosJimoJob &job)
:
AosGenericObj(0)
{
}

AosJimoJob::AosJimoJob(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosGenericObj(0)
{
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
		const OmnString &jsonstr)
{
	AosJimoScheduler scheduler;

	OmnString conf;
	conf << "<" << AOSTAG_JOB << " "
			<< AOSTAG_JOB_VERSION << "=\"1\" "
			<< AOSTAG_OTYPE << "=\"job\" "
			<< AOSTAG_PUB_CONTAINER << "=\"jobctnr\" " 
			<< AOSTAG_JOB_NAME << "=\"" << obj_name << "\" "
			<< AOSTAG_CTNR_PUBLIC << "=\"true\" "
			<< AOSTAG_PUBLIC_DOC << "=\"true\" "
			<< AOSTAG_OBJID << "=\"" << getObjid("job", obj_name) << "\" "
			<< AOSTAG_CONTAINER_OBJID << "=\"task_ctnr\">";

	conf << scheduler.getConfig();
	conf << "<" << AOSTAG_TASKS_TAG << ">"; 

	JSONValue jsonObj;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, jsonObj);
	aos_assert_r(rslt, false);

	JSONValue task_json = jsonObj["tasks"];
	aos_assert_r(!task_json.empty(), false);
	for (u32 i = 0; i < task_json.size(); i++)
	{
		AosJimoTask task(task_json[i], this);
		conf << task.getConfig(rdata);
	}

	conf << "</" << AOSTAG_TASKS_TAG << ">"; 
	conf << "</" << AOSTAG_JOB << ">";
	AosXmlTagPtr new_doc = AosCreateDoc(conf, true, rdata);
	aos_assert_r(new_doc, false);
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

	OmnString jobObjid = getObjid("job", jobname);
	AosXmlTagPtr job_conf = AosGetDocByObjid(jobObjid, rdata);
	aos_assert_r(job_conf, false);

	AosXmlTagPtr tasks_conf = job_conf->getFirstChild("tasks");
	aos_assert_r(tasks_conf, false);
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


bool
AosJimoJob::convertToInput(const OmnString &name, const AosXmlTagPtr &dataset)
{
	OmnString type = dataset->getAttrStr(AOSTAG_ZKY_TYPE);
	if (type == "datacol")
	{
		AosXmlTagPtr tag =  dataset->getFirstChild("datacollector");
		aos_assert_r(tag, false);
		tag = tag->getFirstChild("asm");
		aos_assert_r(tag, false);
		tag = tag->getFirstChild("datarecord");
		aos_assert_r(tag, false);
		tag = tag->getFirstChild("datarecord");

		OmnString str = "<dataset jimo_objid=\"dataset_bydatascanner_jimodoc_v0\" trigger_type=\"datacol\" zky_name=\"";
		str << name <<"\">";
		str << "<datascanner jimo_objid=\"datascanner_parallel_jimodoc_v0\" zky_name=\""
			<< name << "\">";
		str << "<dataconnector jimo_objid=\"dataconnector_datacol_jimodoc_v0\" zky_datacolid=\"" << 
			name << "\"><split jimo_objid=\"dataspliter_datacolbycube_jimodoc_v0\"/></dataconnector></datascanner>";
		str << "<dataschema jimo_objid=\"dataschema_record_jimodoc_v0\" zky_dataschema_type=\"static\" zky_name=\"" << name << "\">";
		str  << tag->toString() << "</dataschema></dataset>";

		mDatasets[name]= str;
	}

	return true;
}
