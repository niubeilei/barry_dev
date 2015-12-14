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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcUpdateIndex_h
#define AOS_JimoLogicNew_JimoLogicDataProcUpdateIndex_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcUpdateIndex : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	
    OmnString           		mDataProcName;
	OmnString 					mType;
	vector<OmnString>			mInputs;
	OmnString					mNewValue;
	OmnString					mOldValue;
	int							mMaxKeyLen;
	OmnString 					mShuffleType;
	OmnString 					mShuffleField;
	OmnString					mRecordType;
	OmnString 					mDocid;
	AosExprObjPtr 				mTable;
	OmnString					mDataProcUpdateIndexName;
	OmnString					mDataProcIILAddName;
	OmnString					mDataProcIILDelName;
public:

	AosJimoLogicDataProcUpdateIndex(const int version);
	~AosJimoLogicDataProcUpdateIndex();
	
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
					AosJimoProgObj* proc,
					OmnString &statement_str,
					bool inparser);

private:
    bool createDataProcUpdateIndex(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog,
			OmnString &statement_tr);

    bool createDataProcIILBatchopr(
			AosRundata *rdata, 
			AosJimoProgObj *jimo_prog,
			OmnString &statement_str);

 
	bool addUpdateIndexTask(
			AosRundata *rdata, 
			AosJimoProgObj *jimo_prog);
    
	bool addIILTask(
			AosRundata *rdata, 
			AosJimoProgObj *jimo_prog);
};
#endif

