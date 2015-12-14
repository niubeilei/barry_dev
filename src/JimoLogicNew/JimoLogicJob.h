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

#include "JimoLogicNew/JimoLogicStat.h"
#include "SEInterfaces/JqlStatementObj.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicJob : public AosJimoLogic
{

protected:
	
	vector<OmnString>				mKeywords;
	OmnString						mJobName;
	AosJimoProgObjPtr				mJob;
	std::vector<AosNameValueObjPtr> mNameValueList;


public:
	AosJimoLogicJob(const int version);
	~AosJimoLogicJob();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					AosJqlStatementPtr &statement);

	virtual bool runData(
					AosRundata *rdata,
					AosJimoParserObj *parser,
					AosJimoProg *jimo_prog);
	
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool inparser);

private:
	bool parseCreateJob(AosRundata*, AosJimoParserObj*, bool&, AosJqlStatementPtr&);
	bool parseRemoveJob(AosRundata*, AosJimoParserObj*, bool&, AosJqlStatementPtr&);
	bool parseDescribeJob(AosRundata*, AosJimoParserObj*, bool&, AosJqlStatementPtr&);
	bool syntaxCheck(AosRundata*, AosJimoParserObj*);
	bool checkCreateJob(AosRundata*, AosJimoParserObj*);
	bool checkRemoveJob(AosRundata*);
	bool checkDescribeJob(AosRundata*);
	bool createJob(AosRundata*, AosJimoParserObj*, AosJimoProgObj *);

	virtual bool runJob(AosRundata *rdata, AosJimoProgObj *prog);
	virtual AosJimoProgObjPtr jimoGetJimoProg(AosRundata *rdata, OmnString &job_name); 

};
#endif

