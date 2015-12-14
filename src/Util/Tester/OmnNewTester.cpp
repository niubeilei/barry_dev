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
// 2013/01/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/OmnNewTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


struct testtt
{
	char data[10000];
};

bool 
AosNewTester::start()
{
	// testtt *data = OmnNew testtt[100];
	testtt *data = new testtt[100];
	// ::operator delete((void*)data, __FILE__, __LINE__);
	OmnDelete data;
OmnScreen << "OMnString: " << sizeof(OmnString) << endl;
	return true;
}

