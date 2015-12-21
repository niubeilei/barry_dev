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
// 01/24/2010:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/Tester/Base64Tester.h"

#include "Base64/Base64.h"
#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


bool AosBase64Tester::start()
{
	// 
	// Test default constructor
	//
	cout << "    Start AosBase64 Tester...";
	torturer();
	return true;
}


bool 
AosBase64Tester::torturer()
{
	OmnBeginTest << "Test";

	const int tries = 100000;
	for (int i=0; i<tries; i++)
	{
		u64 v1 = (u64)rand();
		u64 v2 = (u64)rand();
		u64 v3 = (v1 << 32) + v2;
		u64 v5 = v3;
		char buff[100];
		int len = EncodeBase64((u8*)&v3, (unsigned char *)buff, 8, 100);
		u64 v4;
		DecodeBase64((unsigned char*)buff, (u8*)&v4, len);
		OmnCreateTc << (v5 == v4) << endtc;
	}

	return true;
}

#endif
