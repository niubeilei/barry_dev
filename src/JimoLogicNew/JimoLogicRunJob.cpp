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
// A Job is defined as:
// 	Create JimoJob job_name
// 	(
// 		parm_name,
// 		parm_name,
// 		....
//  );
//
// It will create the following:
//
// Modification History:
// 2015/07/24 Worked on by Gavin
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicRunJob.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicRunJob_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicRunJob(version);
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


AosJimoLogicRunJob::AosJimoLogicRunJob(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicNew(version),
mLock(OmnNew OmnMutex())
{
}


AosJimoLogicRunJob::~AosJimoLogicRunJob()
{
}


bool 
AosJimoLogicRunJob::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	parsed = false;

	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "run", rdata, false);
	aos_assert_rr(mKeywords[1] == "job", rdata, false);
	mErrmsg = "run job ";
	OmnString job_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,job_name,mErrmsg))
	{
		return false;
	}

	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if(!rslt)
		return false;

    int threadNum  = jimo_parser->getParmInt(rdata, "threads", name_value_list);
	int procNum	   = jimo_parser->getParmInt(rdata, "procs", name_value_list);	
	
	rslt = generateNewProcedureDoc(rdata, job_name);
	aos_assert_r(rslt, false);
	
	mJobName = job_name;
	mThreadsNum = threadNum;
	mProcs = procNum;

	parsed = true;
	return true;
}


bool 
AosJimoLogicRunJob::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser) 
{
	OmnString str;
	str << "run job " << mJobName << " {\"threads\" : " << mThreadsNum;
	if(mProcs > 0)
		str << ", \"procs\" : " << mProcs;
	str << "};";
	
    bool rslt = parseRun(rdata, str, prog);
	aos_assert_r(rslt, false);

	OmnString jobObjid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, mJobName); 
	if(jobObjid == "") return false;
	
	AosXmlTagPtr job_conf = AosGetDocByObjid(jobObjid, rdata);
	aos_assert_r(job_conf, NULL);
	
	//OmnString ctime = OmnGetTime(AosLocale::eChina);
	//job_conf->setAttr("zky_job_finished_time", ctime);
	
	rslt = AosModifyDoc(job_conf, rdata);
	aos_assert_r(rslt, false);
	return true;
}


AosJimoPtr 
AosJimoLogicRunJob::cloneJimo() const
{
	return OmnNew AosJimoLogicRunJob(*this);
}

bool
AosJimoLogicRunJob::parseRun(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata, jimo_prog, stmt, statements);
	aos_assert_rr(rslt, rdata, false);
	if (statements.size() <= 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< AosFN("Statement") << stmt << enderr;
		return false;
	}

	OmnString ss;
	for(size_t i = 0;i < statements.size();i++)
	{
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, ss, false);
	}
//	if (!statements[0]->run(rdata, jimo_prog))
//	{
//		AosLogError(rdata, true, "failed_run_statement") << enderr;
//		return false;
//	}
	return true;
}


bool
AosJimoLogicRunJob::generateNewProcedureDoc(
		AosRundata* rdata,
		OmnString  &jobName)
{
	bool rslt;
	OmnString jobObjid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, jobName); 
	if(jobObjid == "") return false;
	
	mLock->lock();
	AosXmlTagPtr job_conf = AosGetDocByObjid(jobObjid, rdata);
	//arvin 2015.08.28
	//if return,please unlock
	//aos_assert_r(job_conf, NULL);
	if(!job_conf)
	{
		OmnString msg = "[ERR] : ";
		msg << "job \"" << jobName << "\" doesn't exist!";
		rdata->setJqlMsg(msg);
		mLock->unlock();
		return false;
	}
	AosXmlTagPtr jobIdConf = AosGetDocByObjid(AOSTAG_JOBOBJID, rdata);
	if(!jobIdConf)
	{
		mLock->unlock();
		return false;
	}
	u64 jobId = jobIdConf->getAttrU64("zky_job_id",0);
	if(jobId < 1)
	{
		mLock->unlock(); 
		return  false;
	}
	jobIdConf->setAttr("zky_job_id", jobId + 1);
	rslt = AosModifyDoc(jobIdConf, rdata);
	if(!rslt)
	{
		mLock->unlock(); 
		return  false;
	}

	if(job_conf->getAttrBool("zky_public_origin_doc") == false)
	{
		job_conf->setAttr("zky_public_origin_doc", "true");
		job_conf->setAttr("zky_job_origin_job_name", jobName);
		job_conf->setAttr("zky_instance_id", jobId);
		//job_conf->setAttr("zky_job_id", 0); //id '0' for the most of origin job 
		rslt = AosModifyDoc(job_conf, rdata);
		aos_assert_r(rslt, false);
		mLock->unlock();
		return true;	
	}
	//AosXmlTagPtr temp_conf = job_conf;
	//u64 job_id = job_conf->getAttrU64("zky_instance_id", 0);
	//job_id++;
	OmnString tempJobName = jobName;
	jobName << "_" << jobId;
	//jobName = newJobName;

	//job_id++;
	//job_conf->setAttr("zky_instance_id", jobId);
	//modify and save this doc to database
	//rslt = AosModifyDoc(job_conf, rdata);
	//aos_assert_r(rslt, false);

	//modify starttime 
	OmnString ctime = OmnGetTime(AosLocale::eChina);
	job_conf->setAttr("zky_job_started_time", ctime);
	
	//change the jobname and objid
	job_conf->setAttr("zky_job_name", jobName);
	job_conf->setAttr("zky_job_origin_job_name", tempJobName);
	job_conf->setAttr("zky_instance_id", jobId); 
	job_conf->setAttr("zky_public_origin_doc", false);
	
	OmnString new_objid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, jobName);
	job_conf->setAttr("zky_objid", new_objid);
	job_conf->setAttr("zky_public_ctnr", "true");
	job_conf->setAttr("zky_public_doc", "true");
	//remove the attr docid,status, job_snapshots__n, finishedTime
	job_conf->removeAttr("zky_job_progress");
	job_conf->removeAttr("zky_job_take_time");
	job_conf->removeAttr("zky_job_finished_time");
	job_conf->removeAttr("zky_docid");
	job_conf->removeAttr("zky_status");
	job_conf->removeNode("job_snapshots__n", true, true);
	job_conf->removeNode("script", true, true);

//save this doc to DataBase	
	OmnString strDoc = job_conf->toString();
	AosXmlTagPtr new_doc = AosCreateDoc(strDoc, true, rdata);
	mLock->unlock();
	aos_assert_r(new_doc, NULL);
	
//	OmnString msg = "";
//	msg << "Run Procedure " << jobName << "successful";
//	rdata->setJqlMsg(msg);
	return true;
}
