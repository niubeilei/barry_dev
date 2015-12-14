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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcPipe_h
#define AOS_JimoLogicNew_JimoLogicDataProcPipe_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcPipe : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString 						mDataProcName;
	OmnString 						mParentProcName;
	OmnString 						mNewParentProcName;
	OmnString						mDataProcPipeName;

	vector<vector<OmnString> > 		mChildProcs;
	vector<vector<OmnString> > 		mNewChildProcs;
	vector<OmnString>				mInputs;
	vector<OmnString>				mInputNames;
	vector<OmnString>				mOutputs;

public:

	AosJimoLogicDataProcPipe(const int version);
	~AosJimoLogicDataProcPipe();
	
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
					AosJimoProgObj* prog,
					OmnString &statement_str,
					bool isparse);

private:
	bool getChildDataProcs(AosRundata *rdata,
				AosJimoParserObj *jimo_parser, 
				const vector<AosExprObjPtr> &procLists);

	bool createDataProcPipe(
					AosRundata *rdata,
					AosJimoProgObj *prog,
					OmnString &statement_str);
	
	bool addPipeTask(
				AosRundata *rdata, 
				AosJimoProgObj *prog);
	void setDeleteTask(
				AosRundata *rdata,
				AosJimoProgObj *prog);

	bool changeProcName(
			AosRundata *rdata, 
			AosJimoProgObj *prog);
};
#endif

