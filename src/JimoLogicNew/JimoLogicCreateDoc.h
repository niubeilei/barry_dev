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
#ifndef AOS_JimoLogicNew_JimoLogicCreateDoc_h
#define AOS_JimoLogicNew_JimoLogicCreateDoc_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicCreateDoc : virtual public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:
	AosExprObjPtr			mInput;
	AosExprObjPtr			mFields;
	vector<OmnString>		mKeyFields;
	OmnString				mDocName;
	OmnString				mTableName;
	OmnString				mDataprocDoc;
	vector<OmnString>		mTasks;
	vector<AosExprObjPtr> 	mNameValueList;
	OmnString				mRecordName;

public:
	AosJimoLogicCreateDoc(const int version);
	~AosJimoLogicCreateDoc();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosJimoLogicObjNew interface
	virtual bool parseJQL(
					AosRundata *rdata,
					AosJimoParserObj *jimo_parser,
					AosJimoProgObj *prog,
					bool &parsed,
					bool dft = false);

    virtual bool parseRun(AosRundata *rdata ,OmnString &, AosJimoProgObj*);

	virtual bool run(
			       	AosRundata *rdata,
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool isparse);

private:
	bool addDocTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
	bool createDocDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);
};
#endif

