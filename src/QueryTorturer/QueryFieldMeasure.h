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
#ifndef Aos_QueryTorturer_QueryFieldMeasure_h
#define Aos_QueryTorturer_QueryFieldMeasure_h

#include "QueryTorturer/QueryField.h"
#include "QueryTorturer/QueryFieldNum.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include <vector>
using namespace std;

class AosQueryFieldMeasure: public AosQueryFieldNum
{
private:
	//max, min, sum, count .....
public:
	OmnString	 mFieldName;
	//OmnString 	 mAggrFunc;

public:
	//AosQueryFieldMeasure(const int field_idx, OmnString fieldName, OmnString aggrFunc);
	AosQueryFieldMeasure(const int field_idx, const bool is_time_field);
	~AosQueryFieldMeasure();
	
	OmnString getAggrFunc() { return mAggrFunc; }
	virtual	void setFieldName(OmnString name) {mFieldName = name;}
	virtual OmnString getFieldName() const { return mFieldName;}
};
#endif

