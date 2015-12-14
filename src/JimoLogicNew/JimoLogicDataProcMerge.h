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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcMerge_h
#define AOS_JimoLogicNew_JimoLogicDataProcMerge_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcMerge : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString 			mDataProcName;
	OmnString			mNewDataProcName;
	vector<OmnString>	mInputs;
public:

	AosJimoLogicDataProcMerge(const int version);
	~AosJimoLogicDataProcMerge();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	bool compileJQL(AosRundata *rdata, AosJimoProgObj *prog);

	// AosJqlStatementObj interface
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool isparse);

private:
	bool createDataProcMerge(
					AosRundata *rdata,
					AosJimoProgObj *prog,
					OmnString &statement_str);
	
	bool addMergeTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog);

};
#endif

