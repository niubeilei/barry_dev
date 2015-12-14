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
//	12/17/2009	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/QueueTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Queue.h"
#include "Util/OmnNew.h"



bool AosQueueTester::start()
{
	torturer();
	return true;
}


bool
AosQueueTester::torturer()
{
	const int lsMaxValues = 10000;
	OmnQueue<int, 10, 10, lsMaxValues> qq;

	int tries = mTestMgr->getTries();
	if (tries <= 0) tries = 100;
	int totalValues = 0;
	int localQueue[10100];
	int head = 0;
	int tail = 0;
	for (int i=0; i<tries; i++)
	{
		int opr = rand() % 100;
		if (opr < 80)
		{
			// Add a new value
			int value = rand();
			if (totalValues >= lsMaxValues)
			{
				// Adding it will fail. 
				OmnCreateTc << (!qq.push(value)) << endtc;
			}
			else
			{
				OmnCreateTc << (qq.push(value)) << endtc;
				localQueue[head++] = value;
				if (head == 10100) head = 0;
				totalValues++;
			}
		}
		else
		{
			// Pop a value
			if (totalValues > 0)
			{
				int value1 = qq.pop();
				int value2 = localQueue[tail++];
				if (tail == 10100) tail = 0;
				totalValues--;
				OmnCreateTc << (value1 == value2) << endtc;
			}
		}
	}
	return true;
}

