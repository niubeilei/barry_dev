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
#ifndef Aos_QueryTorturer_QueryFieldStr_h
#define Aos_QueryTorturer_QueryFieldStr_h

#include "QueryTorturer/QueryField.h"
//#include "QueryTorturer/QueryFieldKey.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include <vector>
using namespace std;


class AosIILTester;

class AosQueryFieldStr : public AosQueryField
{
	OmnDefineRCObject;

private:

public:
	AosQueryFieldStr(const int field_idx, const bool is_time_field);
	~AosQueryFieldStr();

	virtual OmnString getFieldIILName();
	virtual bool setMinMax(const u64 docid);
	virtual bool setRandomMinMax();
	virtual bool createCond(AosQueryTesterUtil::CondDef &cond);
	virtual bool isInRange(const u64 docid, u64 &value);
	virtual bool createAnyCond(AosQueryTesterUtil::CondDef &cond);
	// virtual bool createFieldContents(const AosRundataPtr &rdata);
	virtual AosQueryFieldPtr clone();
	virtual bool initField(const FieldInfo &info);
	virtual FieldInfo getFieldInfo();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual bool serializeToBuff(const AosBuffPtr &buff);
	virtual OmnString getMinValue() const;
	virtual OmnString getMaxValue() const;
	bool init();

private:
	bool createFields();
	bool determineNumAlpha();
	int compareTwoNumbers(u64 value1, u64 value2);
	virtual void setFieldName(OmnString name) {mFieldName = name;}
	virtual OmnString getFieldname() const;
};
#endif

