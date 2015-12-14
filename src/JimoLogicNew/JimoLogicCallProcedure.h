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
// 2015/05/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicCallProcedure_h
#define AOS_JimoLogicNew_JimoLogicCallProcedure_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "JimoAPI/JimoParserAPI.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicCallProcedure : public AosJimoLogicNew
{
	OmnDefineRCObject;

private:
	OmnString	      	mJimoName;
	vector<OmnString> 	mParms;
	vector<AosJqlStatementPtr>  mStatements;

public:
	AosJimoLogicCallProcedure(const int version);
	~AosJimoLogicCallProcedure();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					AosJqlStatementPtr &statement);

	// AosJqlStatement interface
	virtual bool run(
			        AosRundata *rdata,
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool isparse);

	virtual bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog); 

private:
	bool findSubString(AosRundata *rdata,
						OmnString str,
						vector<OmnString> &replace_parms);

};
#endif

