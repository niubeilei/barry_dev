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
// 2013/05/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilTime_Tester_TFEpochDayTester_h
#define Omn_UtilTime_Tester_TFEpochDayTester_h

#include "Debug/Debug.h"
#include "UtilTime/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosTFEpochDayTester : public OmnTestPkg
{
private:
	AosRundataPtr 	mRundata;

public:
	AosTFEpochDayTester();
	~AosTFEpochDayTester();

	virtual bool		start();

private:
};
#endif

