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
#include "JimoLogicNew/JimoLogicRunProcedure.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicRunProcedure_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicRunProcedure(version);
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


AosJimoLogicRunProcedure::AosJimoLogicRunProcedure(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicNew(version),
mLock(OmnNew OmnMutex())
{
}


AosJimoLogicRunProcedure::~AosJimoLogicRunProcedure()
{
}


bool 
AosJimoLogicRunProcedure::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	parsed = false;

	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "run", rdata, false);
	aos_assert_rr(mKeywords[1] == "procedure", rdata, false);
	mErrmsg = "run procedure ";
	OmnString job_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,job_name,mErrmsg))
	{
		return false;
	}

	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if(!rslt)
		return false;

	
	rslt = generateNewProcedureDoc(rdata, job_name);
	aos_assert_r(rslt, false);

	mJobName = job_name;
	
	parsed = true;
	return true;
}


bool 
AosJimoLogicRunProcedure::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser) 
{
	OmnString str;
	str << "run job " << mJobName << " {\"threads\" : " << 1 << "};";
	
    bool rslt = parseRun(rdata, str, prog);
	aos_assert_r(rslt, false);

	return true;
}


AosJimoPtr 
AosJimoLogicRunProcedure::cloneJimo() const
{
	return OmnNew AosJimoLogicRunProcedure(*this);
}

bool
AosJimoLogicRunProcedure::parseRun(
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
		statements[i]->run(rdata, jimo_prog, ss, true);
	}
	return true;
}


bool
AosJimoLogicRunProcedure::generateNewProcedureDoc(
		AosRundata* rdata,
		OmnString  &jobName)
{
	bool rslt;
	OmnString jobObjid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, jobName); 
	if(jobObjid == "") return false;
	
	mLock->lock();
	AosXmlTagPtr job_conf = AosGetDocByObjid(jobObjid, rdata);
	aos_assert_r(job_conf, NULL);

	if(job_conf->getAttrBool("zky_public_origin_doc") == false)
		job_conf->setAttr("zky_public_origin_doc", "true");
	AosXmlTagPtr temp_conf = job_conf;
	u64 job_id = job_conf->getAttrU64("zky_instance_id", 0);
	//the first time to run this job 
	/*if(job_id == 0)
	{
		job_conf->setAttr("zky_instance_id", 1);
		//modify and save this doc to database
		rslt = AosModifyDoc(job_conf, rdata);
		aos_assert_r(rslt, false);
		mLock->unlock();
		return true;	
	}*/
	OmnString newJobName = jobName;
	newJobName << "_" << job_id;
	jobName = newJobName;

	job_id++;
	job_conf->setAttr("zky_instance_id", job_id);
	//modify and save this doc to database
	rslt = AosModifyDoc(job_conf, rdata);
	aos_assert_r(rslt, false);
	mLock->unlock();

	//change the jobname and objid
	job_conf->setAttr("zky_job_name", jobName);
	OmnString new_objid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, jobName);
	job_conf->setAttr("zky_objid", new_objid);
	job_conf->setAttr("zky_public_ctnr", "true");
	job_conf->setAttr("zky_public_doc", "true");
	//remove the attr docid,status, job_snapshots__n
	job_conf->removeAttr("zky_docid");
	job_conf->removeAttr("zky_public_origin_doc");
	job_conf->removeAttr("zky_status");
	job_conf->removeNode("job_snapshots__n", true, true);

//save this doc to DataBase	
	OmnString strDoc = job_conf->toString();
	AosXmlTagPtr new_doc = AosCreateDoc(strDoc, true, rdata);
	aos_assert_r(new_doc, NULL);
	OmnString msg = "";
	msg << "Run Procedure " << jobName << "successful";
	rdata->setJqlMsg(msg);
	return true;
}
