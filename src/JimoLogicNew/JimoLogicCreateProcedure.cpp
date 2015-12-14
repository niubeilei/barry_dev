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
#include "JimoLogicNew/JimoLogicCreateProcedure.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"


extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicCreateProcedure_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicCreateProcedure(version);
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


AosJimoLogicCreateProcedure::AosJimoLogicCreateProcedure(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicNew(version)
{
}


AosJimoLogicCreateProcedure::~AosJimoLogicCreateProcedure()
{
}


bool 
AosJimoLogicCreateProcedure::parseJQL(
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
	
	parsed = false;

	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "create", rdata, false);
	aos_assert_rr(mKeywords[1] == "procedure", rdata, false);
	mErrmsg = "create procedure ";
	OmnString job_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,job_name,mErrmsg))
	{
		return false;
	}
	mErrmsg << job_name;
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
		bool rslt = jimo_parser->nextStatement(rdata, prog, expr, ss);
		if(!rslt)
		{
		//	AosLogError(rdata, true, "failed_parsing_statement") << enderr;
			return false;
		}
		if (!ss && rslt)
		{
			parsed = true;
			return true;			
		}
		// Found a statement for the job
		appendStatement(rdata, ss);
	}

	OmnShouldNeverComeHere;
	return false;
}


bool 
AosJimoLogicCreateProcedure::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser) 
{
	AosJimoProgObjPtr jimo_job = Jimo::jimoCreateJimoProg(rdata, "AosJimoProgJob", 1);
	AosJimoProgObj *jimo_job_raw = jimo_job.getPtrNoLock();
	aos_assert_rr(jimo_job_raw, rdata, false);
	jimo_job_raw->setJobName(mJobName);	

	bool rslt;
	for (u32 i=0; i<mStatements.size(); i++)
	{
		// AosJimoLogicObjNew *jimo_logic = dynamic_cast<AosJimoLogicObjNew *>(mStatements[i].getPtr());
		// aos_assert_rr(jimo_logic, rdata, false);
		rslt = mStatements[i]->compileJQL(rdata, jimo_job_raw);
		aos_assert_r(rslt, false);
	}

	for (u32 i=0; i<mStatements.size(); i++)
	{
		rslt = mStatements[i]->run(rdata, jimo_job_raw, statements_str, true);
		aos_assert_r(rslt, false);
	}

	OmnScreen << "Statements to run for job: " << mJobName << endl;
	cout << statements_str << endl;

	rslt = jimo_job_raw->run(rdata);
	aos_assert_rr(rslt, rdata, false);
	OmnString job_msg = "job "; 
	job_msg << "'" <<  mJobName << "' created successfully.";
	rdata->setJqlMsg(job_msg);
	return true;
}

bool 
AosJimoLogicCreateProcedure::appendStatement(AosRundata *rdata,const AosJqlStatementPtr &stmt)
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
AosJimoLogicCreateProcedure::findSubString(
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
AosJimoLogicCreateProcedure::cloneJimo() const
{
	return OmnNew AosJimoLogicCreateProcedure(*this);
}

