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
// 2013/02/28	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Datalet_Testers_DataletBuffTester_h
#define Aos_Datalet_Testers_DataletBuffTester_h

#include "Value/Testers/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosDataletBuffTester : public OmnTestPkg
{
private:
	int mTries;

public:
	AosDataletBuffTester();
	~AosDataletBuffTester() {}

	virtual bool		start();

private:
	bool	init();
	bool	basicTest();
	bool	config();
};
#endif
