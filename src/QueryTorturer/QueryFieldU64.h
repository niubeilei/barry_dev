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
//
// Modification History:
// 2013/09/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryTorturer_QueryFieldU64_h
#define Aos_QueryTorturer_QueryFieldU64_h

#include "QueryTorturer/QueryField.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include <vector>
using namespace std;


class AosIILTester;

class AosQueryFieldU64 : public AosQueryField
{
	OmnDefineRCObject;

private:
	vector<u64>		mSeeds;
	u64				mStartDocid;
	u64				mNumDocsToCreate;

public:
	AosQueryFieldU64(const int field_idx);
	~AosQueryFieldU64();

	virtual OmnString getFieldIILName();
	virtual bool setMinMax(const u64 docid);
	virtual bool createCond(AosQueryTesterUtil::CondDef &cond);
	virtual bool isInRange(const u64 docid);
	virtual bool createAnyCond(AosQueryTesterUtil::CondDef &cond);
	virtual bool createFieldContents(const AosRundataPtr &rdata);
	virtual AosQueryFieldPtr clone();
	virtual bool initField(const FieldInfo &info);

private:
	bool createFields();
	u64 getStrValue(const u64 docid);
};
#endif


