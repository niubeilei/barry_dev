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
#include "Util/Tester/HashStrTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/HashStr.h"
#include "Util/OmnNew.h"
#include "Util/HashStr.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

OmnHashStrTester::OmnHashStrTester()
{
	mName = "OmnHashStrTester";
	mTries = 100;
}


bool OmnHashStrTester::start()
{
	torturer();
	return true;
}


bool
OmnHashStrTester::torturer()
{
	OmnHashStr<OmnString, 10000> htable;
	char data[100];
	int tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		if (i % 1000 == 0) 
		{
			cout << "Try: " << i << ", Max Bucket Len: " 
				<< htable.getMaxBktLen() << endl;
		}
		sprintf(data, "abc%defg", i);
		OmnCreateTc << (htable.add(data)) << endtc;

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
			int startidx = rand() % i;
			int endidx = startidx + 20;
			if (endidx > i) endidx = i;
			for (int m=startidx; m<endidx; m++)
			{
				sprintf(data, "abc%defg", m);
				OmnCreateTc << (htable.exist(data)) << endtc;
			}
		}
	}
	return true;
}
