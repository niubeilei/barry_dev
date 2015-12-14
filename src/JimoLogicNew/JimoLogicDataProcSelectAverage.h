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
// 2015/06/10 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicDataProcSelectAverage_h
#define AOS_JimoLogicNew_JimoLogicDataProcSelectAverage_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"

#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosJimoLogicDataProcSelectAverage : virtual public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
    OmnString           	mDataProcName;

	vector<AosExprObjPtr> 	mNameValueList;
    OmnString				mInput;
	vector<OmnString>		mGroupBys;
	vector<OmnString>		mFields;
	OmnString				mNumName;
	vector<OmnString>		mSumNames;

public:
	AosJimoLogicDataProcSelectAverage(const int version);
	~AosJimoLogicDataProcSelectAverage();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	bool compileJQL(AosRundata*, AosJimoProgObj *);

	// AosJqlStatement interface
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool isparse);

private:
	bool createSelectDataprocAddCount(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &dp_nam, OmnString &statements_str);
	bool createSelectDataprocSumCount(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &dp_nam, OmnString &statements_str);
	bool createSelectDataprocAverage(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &dp_nam, OmnString &statements_str);
	bool parseRun(AosRundata* rdata, OmnString &stmt, AosJimoProgObj *jimo_prog); 

};
#endif

