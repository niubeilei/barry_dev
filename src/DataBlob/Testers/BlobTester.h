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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataBlob_Testers_BlobTester_h
#define Aos_DataBlob_Testers_BlobTester_h

#include "DataBlob/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include <vector>
using namespace std;

class AosBlobTester : virtual public OmnTestPkg
{
private:
	AosRundataPtr 					mRundata;
public:
	AosBlobTester();
	~AosBlobTester();

	virtual bool start();

private:
	bool			basicTest();
};
#endif
