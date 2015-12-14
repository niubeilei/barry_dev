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
#ifndef Aos_QueryTorturer_QueryFieldTime_h
#define Aos_QueryTorturer_QueryFieldTime_h

#include "QueryTorturer/QueryField.h"
#include "QueryTorturer/QueryFieldNum.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include <vector>
using namespace std;

class AosQueryFieldTime: public AosQueryFieldNum
{
private:
	//max, min, sum, count .....
	OmnString	mTimeUnit;
	OmnString	mTimeField;

public:
	AosQueryFieldTime(const int field_idx,    
						const bool is_time_field);

	~AosQueryFieldTime();
	
	OmnString getTimeUnit() { return mTimeUnit; }
	virtual	void setFieldName(OmnString name) {mTimeField = name;}
	virtual OmnString getFieldName() const { return mTimeField; }

private:

};
#endif

