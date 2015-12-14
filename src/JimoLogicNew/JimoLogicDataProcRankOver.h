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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcRankOver_h
#define AOS_JimoLogicNew_JimoLogicDataProcRankOver_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcRankOver : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString			mRankOver;
	OmnString			mName;
	OmnString			mRecordType;
	vector<OmnString>   mOrderBy;
	vector<OmnString>   mPartitionBy;           
	OmnString			mDataProcName;

public:

	AosJimoLogicDataProcRankOver(const int version);
	~AosJimoLogicDataProcRankOver();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);
	
	bool compileJQL(AosRundata *rdata, const AosJimoProgObjPtr &prog);

	//AosJqlStatement interface
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool isparse);

	bool compileJQL(AosRundata *rdata, AosJimoProgObj *prog);

private:
	bool createRankOverDataProc(
					AosRundata *rdata,
					AosJimoProgObj *prog, 
					OmnString &statements_str);
	
	bool addRankOverTask(AosRundata *rdata, AosJimoProgObj *prog);
};
#endif

