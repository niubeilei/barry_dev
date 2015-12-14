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
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicJob_h
#define AOS_JimoLogicNew_JimoLogicJob_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include "SEInterfaces/JPSyntaxCheckerObj.h"

class AosJimoLogicCreateJob : public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:
	OmnString					mJobName;
	AosJimoProgObjPtr			mJimoProgJob;
	vector<OmnString>			mParms;
	vector<AosJqlStatementPtr>  mStatements;

	static vector<AosJPSyntaxCheckerObjPtr> sgCheckers;
public:
	AosJimoLogicCreateJob(AosRundata *rdata,const int version);
	~AosJimoLogicCreateJob();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosJimoLogicObjNew interface
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool isparse);

	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

private:

	bool appendStatement(AosRundata *rdata,const AosJqlStatementPtr &stmt);
	
	bool findSubString(
		AosRundata *rdata,
		OmnString str,
		vector<OmnString> &replace_parms);

	bool syntaxCheck(AosRundata *rdata);//const AosJimoLogicCreateJobPtr &stmt);

	bool init(AosRundata *rdata);
};
#endif

