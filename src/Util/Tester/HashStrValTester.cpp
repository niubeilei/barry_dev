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
// 2009/12/05	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/HashStrValTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/HashStrVal.h"
#include "Util/OmnNew.h"
#include "Util/HashStrVal.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

OmnHashStrValTester::OmnHashStrValTester()
{
	mName = "OmnHashStrValTester";
	mTries = 100;
}


bool OmnHashStrValTester::start()
{
	torturer();
	return true;
}


bool
OmnHashStrValTester::torturer()
{
	OmnHashStrVal<OmnString, u64, 10000> htable;
	char data[100];
	u32 tries = 1000000;
	for (u32 i=0; i<tries; i++)
	{
		if (i % 1000 == 0) 
		{
			cout << "Try: " << i << ", Max Bucket Len: " 
				<< htable.getMaxBktLen() << endl;
		}
		sprintf(data, "abc%defg", i);
		OmnCreateTc << (htable.add(data, i+1000)) << endtc;

		for (int m=0; m<10; m++)
		{
			int len = rand() % 30;
			for (int k=0; k<len; k++)
			{
				data[k] = rand() % 120 + 1;
			}
			data[len] = 0;
			if (data[0] == 'a') data[0] = 'b';
			OmnCreateTc << (!htable.exist(data)) << endtc;
		}

		if (i>0)
		{
			u32 startidx = rand() % i;
			u32 endidx = startidx + 20;
			if (endidx > i) endidx = i;
			for (u32 m=startidx; m<endidx; m++)
			{
				sprintf(data, "abc%defg", m);
				OmnCreateTc << (htable.exist(data)) << endtc;
				u64 vv = htable.get(data, 0);
				OmnCreateTc << (vv == m + 1000) << endtc;
			}
		}
	}
	return true;
}
