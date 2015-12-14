////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// RediKeyibution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2013/09/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryTorturer_QueryFieldKey_h
#define Aos_QueryTorturer_QueryFieldKey_h

#include "QueryTorturer/QueryField.h"
#include "QueryTorturer/QueryFieldStr.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include <vector>
using namespace std;

class AosQueryFieldKey : public AosQueryFieldStr
{
public:
	OmnString	mFieldName;

public:
	AosQueryFieldKey(const int field_idx, const bool is_time_field);
	~AosQueryFieldKey();
	
	virtual	void setFieldName(OmnString name) {mFieldName = name;}
	virtual OmnString getFieldName() const { return mFieldName; }

};
#endif

