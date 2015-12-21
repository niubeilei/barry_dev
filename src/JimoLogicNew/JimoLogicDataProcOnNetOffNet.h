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
// 2015/12/13 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicDataProcOnNetOffNet_h
#define AOS_JimoLogicNew_JimoLogicDataProcOnNetOffNet_h

#include "JimoLogicNew/JimoLogicDataProc.h"


class AosJimoLogicDataProcOnNetOffNet : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString										mInput;
	OmnString										mInputName;
	OmnString										mUserIdField;
	OmnString										mTimeField;
	OmnString										mTimeUnit;
	i64 											mShreshold;
	OmnString										mStartDay;
	OmnString										mEndDay;
	OmnString										mNewDataProcName;

public:
	AosJimoLogicDataProcOnNetOffNet(const int version);
	~AosJimoLogicDataProcOnNetOffNet();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	//JimoLogicObjNew interface
	bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog);

	virtual AosJimoLogicType::E getJimoLogicType()const {return AosJimoLogicType::eDataProcOnNetOffNet;}
	
	// AosJqlStatement interface
	virtual bool run(
			AosRundata* rdata,
			AosJimoProgObj *prog,
			OmnString &statement_str,
			bool isparse);

private:
    bool createDataproc(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog,
			OmnString &statement_str);

	bool addTask(
			AosRundata *rdata, 
			AosJimoProgObj *prog);
};
#endif
