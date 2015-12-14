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
// 2015/03/22 Created by Chen Ding
// 2015/05/20 Worked on by Arvin
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicCreateJob.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"

vector<AosJPSyntaxCheckerObjPtr> AosJimoLogicCreateJob::sgCheckers;

static bool sgInit = false;

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicCreateJob_1(AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicCreateJob(rdata.getPtr(),version);
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


AosJimoLogicCreateJob::AosJimoLogicCreateJob(AosRundata *rdata,const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicNew(version)
{
	//init(rdata);
}


AosJimoLogicCreateJob::~AosJimoLogicCreateJob()
{
}

bool
AosJimoLogicCreateJob::init(AosRundata *rdata)
{
	if(sgInit) return true;
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_rr(conf, rdata, false);
	AosXmlTagPtr syntax_check = conf->getFirstChild("syntax_check");
	aos_assert_rr(syntax_check, rdata, false);
	AosXmlTagPtr jimo_prog_syntax_check = syntax_check->getFirstChild("jimo_prog_syntax_check");
	aos_assert_rr(jimo_prog_syntax_check, rdata, false);
	AosXmlTagPtr checkers = jimo_prog_syntax_check->getFirstChild("checkers");
	aos_assert_rr(checkers, rdata, false);
	AosXmlTagPtr checker = checkers->getFirstChild();
	while(checker)
	{
		OmnString jimo_name = checker->getAttrStr("jimo_name","");
		int version = checker->getAttrInt("version",1);
		AosJimoPtr jimo = AosCreateJimoByClassname(rdata, jimo_name, version);
		if(!jimo) return true;
		AosJPSyntaxCheckerObj  *jpsc = dynamic_cast<AosJPSyntaxCheckerObj*>(jimo.getPtr()); 		
		sgCheckers.push_back(jpsc);
		checker = checkers->getNextChild();
	}
	sgInit = true;
	return true;
}


bool 
AosJimoLogicCreateJob::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	Create Job job_name(parm, parm, ...)
	// 	BEGIN
	// 		statement;
	// 		statement;
	// 		...
	// 		statement;
	// 	END;
	
	mJimoProgJob = Jimo::jimoCreateJimoProg(rdata, "AosJimoProgJob", 1);

	parsed = false;

	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "create", rdata, false);
	aos_assert_rr(mKeywords[1] == "job", rdata, false);
	mErrmsg = "create job ";
	OmnString job_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,job_name,mErrmsg))
	{
		return false;
	}
	mErrmsg << job_name;
	//arvin 2015.08.06
	//JIMODB-297
	OmnString name = job_name;
	if(name.toLower() == "jobs")
	{
		setErrMsg(rdata,eIILegalName,job_name,mErrmsg);
		return false;
	}
	// Parser the parameters. 
	if (!jimo_parser->peekNextChar('('))
	{
		// The next character is not '('. 
		return true;
	}

	// From now on, we assume this is the job statement. 
	jimo_parser->parseProcedureParms(rdata, mParms);
	
	// The next character should not be ';'.
	if (jimo_parser->peekNextChar(';'))
	{
		AosLogError(rdata, true, "expecting_statement_but_found_semi_column") << enderr;
		return false;
	}
	// The next keyword must be "begin"
	OmnString begin = jimo_parser->nextKeyword(rdata);
	if(begin.toLower() != "begin")
	{
		mErrmsg << " : miss keyword \"begin\"";
		rdata->setJqlMsg(mErrmsg);
		return false;
	}
	
	mJobName = job_name;
	mOriginJob = jimo_parser->getOrigStmt();
	//replace
	if(mParms.size() > 0)
	{
		OmnString stmt_str = jimo_parser->getOrigStmt();
		vector<OmnString> replace_parms;
		findSubString(rdata, stmt_str, replace_parms);
		if(replace_parms.size() > 0)
		{
			aos_assert_rr(replace_parms.size()==mParms.size(),rdata,false);

			for(size_t i =0; i<mParms.size();i++)
			{
				stmt_str.replace(replace_parms[i],mParms[i],false);	
			}
			jimo_parser->setOrigStmt(stmt_str);
		}
	}

	// Parse the statements and adds them to mStatements until
	// it hits "END job".
	int guard = 1000000;
	OmnString expr;
	while (guard--)
	{
		AosJqlStatementPtr ss;
		bool rslt = jimo_parser->nextStatement(rdata, mJimoProgJob.getPtr(), expr, ss, false);
		if(!rslt)
		{
			return false;
		}
		if (!ss && rslt)
		{
			parsed = true;
			//if (!stmt->syntaxCheck(rdata,stmt)) return false;
			if (!syntaxCheck(rdata)) return false;
			return true;			
		}
		// Found a statement for the job
		appendStatement(rdata, ss);
	}

	OmnShouldNeverComeHere;
	return false;
}

// Arvin, 2015.08.20
// JIMODB417
bool
AosJimoLogicCreateJob::syntaxCheck(AosRundata *rdata)//, const AosJimoLogicCreateJobPtr &statement)
{
	//AosJimoPtr jimo = statement->cloneJimo();
	//AosJimoPtr jimo = this->cloneJimo();
	//AosJimoLogicCreateJobPtr stmt = dynamic_cast<AosJimoLogicCreateJob*>(jimo.getPtr());
	AosJimoProgObj *jimo_job_raw = mJimoProgJob.getPtrNoLock();
	aos_assert_rr(jimo_job_raw, rdata, false);
	jimo_job_raw->setJobName(mJobName);	


	bool rslt;
	for (u32 i=0; i< mStatements.size(); i++)
	{
		rslt = mStatements[i]->compileJQL(rdata, jimo_job_raw);
		aos_assert_r(rslt, false);
	}
	for (u32 i = 0; i < sgCheckers.size(); i++)
	{
		if (!sgCheckers[i]->checkSyntax(rdata, jimo_job_raw)) return false;
	}
	// semantics check
	for (u32 i=0; i< mStatements.size(); i++)
	{
		rslt = mStatements[i]->semanticsCheck(rdata, jimo_job_raw);
		aos_assert_r(rslt, false);
	}


	return true;
}



bool 
AosJimoLogicCreateJob::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser) 
{
//	AosJimoProgObjPtr jimo_job = Jimo::jimoCreateJimoProg(rdata, "AosJimoProgJob", 1);
	AosJimoProgObj *jimo_job_raw = mJimoProgJob.getPtrNoLock();
//	aos_assert_rr(jimo_job_raw, rdata, false);
//	jimo_job_raw->setJobName(mJobName);	

	bool rslt;
/*
	for (u32 i=0; i<mStatements.size(); i++)
	{
		rslt = mStatements[i]->compileJQL(rdata, jimo_job_raw);
		aos_assert_r(rslt, false);
	}
*/
	
	for (u32 i=0; i<mStatements.size(); i++)
	{
		rslt = mStatements[i]->run(rdata, jimo_job_raw, statements_str, true);
		aos_assert_r(rslt, false);
	}

	OmnScreen << "Statements to run for job: " << mJobName << endl;
	cout << statements_str << endl;

	rslt = jimo_job_raw->run(rdata);
	
	//set OriginJobCode to JobDoc
	OmnString jobObjid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, mJobName); 
	if(jobObjid == "") return false;
	
	AosXmlTagPtr job_conf = AosGetDocByObjid(jobObjid, rdata);
	aos_assert_r(job_conf, NULL);
	
	job_conf->setNodeText("script", mOriginJob, true);
	rslt = AosModifyDoc(job_conf, rdata);
	aos_assert_r(rslt, false);

	return true;
}

bool 
AosJimoLogicCreateJob::appendStatement(AosRundata *rdata,const AosJqlStatementPtr &stmt)
{
	if (!stmt)
	{
		AosLogError(rdata, true, "Missing_statement") << enderr;
		return false;
	}
	mStatements.push_back(stmt);
	return true;
}

bool
AosJimoLogicCreateJob::findSubString(
		AosRundata *rdata,
		OmnString str,
		vector<OmnString> &replace_parms)
{
	const char *data = str.data();
	int start_idx,end_idx;
	for(int i = 0; i < str.length(); i++)
	{

		if(data[i] == '$')
		{
			start_idx = i;
			for(int j = i ;j < str.length();j++)
			{
				if(data[j] == '"')
				{
					end_idx = j;
					i = j;
					break;
				}
			}

			replace_parms.push_back(OmnString(&data[start_idx],end_idx-start_idx));			
		}
	}
	return true;
}



AosJimoPtr 
AosJimoLogicCreateJob::cloneJimo() const
{
	return OmnNew AosJimoLogicCreateJob(*this);
}

