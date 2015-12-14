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
// 2015/07/24 Created by Gavin
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicRunProcedure_h
#define AOS_JimoLogicNew_JimoLogicRunProcedure_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicRunProcedure : public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:
	OmnString					mProcedureName;
	int							mThreadsNum;	
	OmnMutexPtr					mLock;

public:
	AosJimoLogicRunProcedure(const int version);
	~AosJimoLogicRunProcedure();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosJimoLogicObjNew interface
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool inparser);

    virtual bool parseRun(AosRundata *rdata ,OmnString &, AosJimoProgObj*);

	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

private:
bool generateNewProcedureDoc(
		AosRundata* rdata,
		OmnString  &JobName);


};
#endif

