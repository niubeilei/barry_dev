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
// A script is similar to Store Procedure. It is made of a sequence 
// of statements. Among them:
// 	1. User Defined Variable statement will add user defined variables;
// 	2. Modify user defined variables
// 	3. Named objects, such as data procs, etc. These named objects 
// 	   are managed by the script.
// 	4. BEGIN JOB ... END JOB: this block creates a job. When encounters
// 	   a BEGIN JOB <jobname> statement, it pushes a AosJimoProgJob to 
// 	   its JimoProgStack and sets it as the current jimo prog.
// 	5. When encounter END JOB, it finishes creating the job. This is a
// 	   named object. 
// 	6. When run a job, in the current implementations, it retrieves
// 	   the job definition from the database (an XML doc). We should 
// 	   let it check the jimo prog first.
//
// 2015/04/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoProg/JimoProgScript.h"

#include "API/AosApi.h"
#include "JQLStatement/JqlStatement.h"
#include "JimoProg/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoProgScript_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosJimoProgScript(version);
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



AosJimoProgScript::AosJimoProgScript(const int version)
{
	mJimoType = AosJimoType::eJimoProg;
	mJimoVersion = version;
}


AosJimoProgScript::~AosJimoProgScript()
{
}


AosJimoPtr 
AosJimoProgScript::cloneJimo() const
{
	return OmnNew AosJimoProgScript(*this);
}


OmnString
AosJimoProgScript::generateCode(AosRundata *rdata)
{
	OmnScreen << "Generated code:" << endl << mCode << endl;
	return mCode;
}


/*
bool
AosJimoProgScript::generateCode(
		AosRundata *rdata, 
		const OmnString &section_name)
{
	itr_t itr = mSections.find(section_name);
	if (itr == mSections.end()) return true;

	for (u32 i=0; i<itr->second.size(); i++)
	{
		mCode << "\n" << itr->second[i];
	}

	return true;
}
*/


bool 
AosJimoProgScript::appendStatement(
		AosRundata *rdata, 
		const OmnString &section_name,
		const AosJqlStatementPtr &stmt)
{
	itr_t itr = mSections.find(section_name);
	if (itr == mSections.end())
	{
		vector<AosJqlStatementPtr> stmts;
		stmts.push_back(stmt);
		mSections[section_name] = stmts;
	}
	else
	{
		itr->second.push_back(stmt);
	}
	return true;
}

