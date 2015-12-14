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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcStr2NumMap_h
#define AOS_JimoLogicNew_JimoLogicDataProcStr2NumMap_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcStr2NumMap : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString			mNewDataProcName;
	AosExprObjPtr		mTable;

	vector<OmnString>	mKeyFields;
	OmnString			mValueField;
	int					mMaxKeyLen;
	AosExprObjPtr		mCond;
	OmnString			mAggrOpr;
	OmnString			mDataType;

	OmnString			mDataProcIILName;

public:

	AosJimoLogicDataProcStr2NumMap(const int version);
	~AosJimoLogicDataProcStr2NumMap();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;


	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	//AosJqlStatement interface
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool isparse);
	
	bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog);

private:

	bool createStr2NumMapDataProc(AosRundata *rdata,
					AosJimoProgObj *prog,
					OmnString &statement_str);
	
	bool createIILBatchoprDataproc(AosRundata *rdata,
					AosJimoProgObj *jimo_prog,
					OmnString &statement_str);

	bool addStr2NumMapTask(AosRundata *rdata, 
					AosJimoProgObj *prog);

	bool addIILTask(AosRundata *rdata,
					AosJimoProgObj *jimo_prog);

};
#endif

