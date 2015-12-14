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
// 2013/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataPickerEngine_Testers_DataPickerTester_h
#define Aos_DataPickerEngine_Testers_DataPickerTester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosIILTester : public OmnTestPkg, 
					 public AosDataPickerProc
{

private:

public:
	AosIILTester();
	~AosIILTester() {}

	virtual bool		start();

	virtual bool procDatalet(const vector<AosDataletPtr> &datalets);

private:
	bool	basicTest();
};
#endif
