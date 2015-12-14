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
// 01/13/2010:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/StrHashTester.h"

#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/StrHash.h"


bool AosStrHashTester::start()
{
	// 
	// Test default constructor
	//
	cout << "    Start AosStrHash Tester...";
	// torturer();
	torturer1();
	return true;
}


bool 
AosStrHashTester::torturer()
{
	OmnBeginTest << "Test";

	OmnStrHash<OmnString, 0x3f> hash;
	OmnString key, value;

	const int tries = 10000;
	for (int i=0; i<tries; i++)
	{
		key = "key";
		key << i;
		value = "value";
		value << i;
		hash.add(key, value);

		for (int k=0; k<100; k++)
		{
			int vv = rand() % (i+1) + 100;
			key = "key";
			key << vv;
			if (vv > i) 
			{
				OmnCreateTc << (!hash.get(key, value, false)) << endtc;
			}
			else
			{
				OmnCreateTc << (hash.get(key, value, false)) << endtc;
			}
		}
	}

	for (int i=0; i<tries; i++)
	{
		key = "key";
		key << i;
		OmnCreateTc << (hash.get(key, value, false)) << endtc;
		OmnCreateTc << (hash.get(key, value, true)) << endtc;

		for (int k=0; k<tries; k++)
		{
			key = "key";
			key << k;
			if (k <= i)
			{
				OmnCreateTc << (!hash.get(key, value, false)) << endtc;
			}
			else
			{
				if (!hash.get(key, value, false))
				{
					hash.get(key, value, false);
				}
				OmnCreateTc << (hash.get(key, value, false)) << endtc;
			}
		}
	}

	return true;
}


bool 
AosStrHashTester::torturer1()
{
	OmnBeginTest << "Test";

	OmnStrHash<int, 0x3f> hash;
	OmnString key;
	int value;

	const int tries = 10000;
	for (int i=0; i<tries; i++)
	{
		key = "key";
		key << i;
		hash.add(key, i);

		for (int k=0; k<100; k++)
		{
			int vv = rand() % (i+1) + 100;
			key = "key";
			key << vv;
			if (vv > i) 
			{
				OmnCreateTc << (!hash.get(key, value, false)) << endtc;
			}
			else
			{
				OmnCreateTc << (hash.get(key, value, false)) << endtc;
			}
		}
	}

	for (int i=0; i<tries; i++)
	{
		key = "key";
		key << i;
		OmnCreateTc << (hash.get(key, value, false)) << endtc;
		OmnCreateTc << (hash.get(key, value, true)) << endtc;
if (hash.get(key, value, false))
{
	hash.get(key, value, true);
	OmnMark;
}
		OmnCreateTc << (!hash.get(key, value, false)) << endtc;

		for (int k=0; k<tries; k++)
		{
			key = "key";
			key << k;
			if (k <= i)
			{
if (hash.get(key, value, false))
{
	OmnMark;
}
				OmnCreateTc << (!hash.get(key, value, false)) << endtc;
			}
			else
			{
				if (!hash.get(key, value, false))
				{
					hash.get(key, value, false);
				}
				OmnCreateTc << (hash.get(key, value, false)) << endtc;
			}
		}
	}

	return true;
}



