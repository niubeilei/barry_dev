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
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicShowTorturer_h
#define AOS_JimoLogicNew_JimoLogicShowTorturer_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicShowTorturer : virtual public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:

	enum torturerType
	{
		eIILTorturer,
		eSortTorturer,
		eMultiCondTorturer,
		eBitmapTorturer,
		
		eErrTorturer
	};
	torturerType		mTorturerType;
	OmnString           mTorturerName;
	i64					mNumRound;
	OmnString			mShowMsg;
	bool				mShowOthers;
	AosExprObjPtr		mInput;
	vector<OmnString>	mKeyFields;
	OmnString 			mIndexName;
	OmnString			mCond;
	OmnString           mTableName;
	OmnString  			mDataprocIndex;
//	OmnString			mDataprocU64Index;
	OmnString			mIndexIILName;
	OmnString			mType;
	vector<OmnString>	mTasks;
	vector<AosExprObjPtr> mNameValueList;
    OmnString           mStmt;

public:
	AosJimoLogicShowTorturer(const int version);
	~AosJimoLogicShowTorturer();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosJimoLogicObjNew interface
	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata,
					AosJimoParserObj *jimo_parser,
					AosJimoProgObj *prog,
					bool &parsed,
					bool dft = false);

    virtual bool parseRun(AosRundata *rdata ,OmnString &, AosJimoProgObj *);

	virtual bool run(
			       	AosRundata *rdata,
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool isparse);

private:
	bool addIILTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
	bool addIndexTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
	bool createIILDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);
	bool createIndexDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);
	bool createIndexU64(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);
};
#endif

