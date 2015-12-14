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
// 08/19/2015	Created by Andy Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_Testers2_ValueRsltPerformances_h
#define Aos_Util_Testers2_ValueRsltPerformances_h

#include "Util/ValueRslt.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"

using namespace std;
class AosValueRsltPerformances : public OmnTestPkg
{
public:
	AosValueRsltPerformances();
	~AosValueRsltPerformances();

	virtual bool		start();

private:
	u64 getU64Value();
	i64 getI64Value();
	double getDoubleValue();
	OmnString getStringValue();

};

#endif

