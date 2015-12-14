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
// This query proc assumes:
// This is a query that has a simple join, or it has only one join, 
// which can be one of the following:
// 	1. Inner join
// 	2. Outter join
// 	3. Left Outer Join
// 	4. Right Outer Join
// 	5. Left Exclusive Join
// 	6. Right Exclusive Join
// 	7. XO Join
//
// Modification History:
// 2015/01/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryProcJimos/QueryProcSimpleJoin.h"

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosQueryProcSimpleJoin_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosQueryProcSimpleJoin(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosQueryProcSimpleJoin::AosQueryProcSimpleJoin(const int version)
:
AosQueryProcJimo(version, 1)
{
}


AosQueryProcSimpleJoin::~AosQueryProcSimpleJoin()
{
}


AosJimoPtr
AosQueryProcSimpleJoin::cloneJimo() const
{
	try
	{
		return OmnNew AosQueryProcSimpleJoin(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosQueryProcSimpleJoin::procJoin(
		AosRundata *rdata, 
		AosQueryReqStmt *query, 
		AosJoinStmt *join)
{
	return true;
}

