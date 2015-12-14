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
// 10/26/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_QueryTester_h
#define AosSengTester_QueryTester_h

#include "SengTorturer/SengTester.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosQueryTester : virtual public AosSengTester
{
public:
	enum QueryType
	{
		eKeywordQuery,
		eTagQuery,
		eAttrQuery
	};

	enum Operation
	{
		eSimpleQuery
	};

private:

public:
	AosQueryTester(const bool regflag);
	~AosQueryTester();

	virtual bool test();

private:
	Operation	pickOperation();
	QueryType	pickQueryType();
};
#endif

