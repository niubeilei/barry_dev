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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcSelectR1R2_h
#define AOS_JimoLogicNew_JimoLogicDataProcSelectR1R2_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"

#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>

class AosJimoLogicDataProcSelectR1R2 : virtual public AosJimoLogicDataProc 
{
	OmnDefineRCObject;

protected:
    OmnString           		mDataProcName;
	vector<AosExprObjPtr> 		mNameValueList;
    OmnString					mInput;

	OmnString  					mDataprocSelectName;
	OmnString					mOutputName;
	OmnString					mInputName;

public:
	AosJimoLogicDataProcSelectR1R2(const int version);
	~AosJimoLogicDataProcSelectR1R2();
	
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
	
	bool getInputV(vector<OmnString> &inputs);

private:
	bool createSelectR1R2Dataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str);
    bool addSelectR1R2Task(AosRundata *rdata, AosJimoProgObj *jimo_prog);

public:
	OmnString getNewDataProcName() const;

};
#endif

