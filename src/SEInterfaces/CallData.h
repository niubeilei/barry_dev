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
// 2015/02/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_CallData_h
#define Aos_SEInterfaces_CallData_h

#include "SEInterfaces/JimoCaller.h"


#include <vector>
using namespace std;

class AosCallData
{
private:
	AosJimoCallerPtr	mCaller;
	vector<i64>			mI64Values;

public:
	AosCallData();
	AosCallData(const AosJimoCallerPtr &caller);
	~AosCallData();

	void appendInt(const i64 v) {mI64Values.push_back(v);}
	i64 getInt(const int idx) const 
	{
		aos_assert_r(idx >= 0 && (u32)idx < mI64Values.size(), -1);
		return mI64Values[idx];
	}

	void reset()
	{
		mCaller = 0;
		mI64Values.clear();
	}
	void setSuccess();
};
#endif

