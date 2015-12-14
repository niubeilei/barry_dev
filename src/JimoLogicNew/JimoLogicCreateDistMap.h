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
#ifndef AOS_JimoLogicNew_JimoLogicCreateDistMap_h
#define AOS_JimoLogicNew_JimoLogicCreateDistMap_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicCreateDistMap : virtual public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:
	enum E
	{
		eDftSampleSize = 1 * 1000 * 1000
	};

	OmnString				mDistmapName;
	AosExprObjPtr       	mInputs;
	AosExprObjPtr			mKeyFieldExpr;
	int64_t					mSampleSize;
	int						mSplit;

	OmnString           	mTableName;
    OmnString           	mDataprocDistName;
	vector<AosExprObjPtr> 	mNameValueList;

public:
	AosJimoLogicCreateDistMap(const int version);
	~AosJimoLogicCreateDistMap();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata,
					AosJimoParserObj *jimo_parser,
					AosJimoProgObj *prog,
					bool &parsed,
					bool dft = false);

	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool isparse);

private:
    bool createDistributionmapDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);
    bool createDistributionmap(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);
    bool addDistmapTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
};
#endif
