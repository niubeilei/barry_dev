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
#ifndef AOS_JimoLogicNew_JimoLogicJob_h
#define AOS_JimoLogicNew_JimoLogicJob_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicRunJob : public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:
	OmnString					mJobName;
	int							mThreadsNum;
	int 						mProcs;
	OmnMutexPtr					mLock;

	

public:
	AosJimoLogicRunJob(const int version);
	~AosJimoLogicRunJob();

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

