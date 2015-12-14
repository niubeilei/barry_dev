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
#include "Util/Tester/BuffTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

OmnBuffTester::OmnBuffTester()
{
	mName = "OmnBufflTester";
	mTries = 100;
}


bool OmnBuffTester::start()
{
	torturer();
	return true;
}


bool
OmnBuffTester::torturer()
{
	int tries = 10000;
	char bb[10000];
	AosBuff buff(bb, 10000, 10000 AosMemoryCheckerArgs);

	const int eMaxValues = 100;
	int intIdx, u32Idx, u64Idx, charIdx, strIdx;
	int intValues[eMaxValues];
	u32 u32Values[eMaxValues];
	u64 u64Values[eMaxValues];
	char charValues[eMaxValues];
	int flags[eMaxValues];
	OmnString strValues[eMaxValues]; 

	for (int t=0; t<tries; t++)
	{
		buff.reset();
		charIdx = 0;
		intIdx = 0;
		u32Idx = 0;
		u64Idx = 0;
		strIdx = 0;
		int numEntries = rand() % 50;

		for (int i=0; i<numEntries; i++)
		{
			int flag = rand() % 100;

			if (flag < 20)
			{
				// Add an integer
				intValues[intIdx] = rand();
				buff.setInt(intValues[intIdx++]);
				flags[i] = 0;
				continue;
			}
	
			if (flag < 40)
			{
				// Add u32
				u32Values[u32Idx] = rand();
				buff.setU32(u32Values[u32Idx++]);
				flags[i] = 1;
				continue;
			}

			if (flag < 60)
			{
				// Add u64
				u64Values[u64Idx] = rand();
				buff.setU64(u64Values[u64Idx++]);
				flags[i] = 2;
				continue;
			}

			if (flag < 80)
			{
				// Add char
				charValues[charIdx] = rand() % 124 + 1;
				buff.setChar(charValues[charIdx++]);
				flags[i] = 4;
				continue;
			}

			// Add string
			strValues[strIdx] = getRandStr();
			buff.setOmnStr(strValues[strIdx++]);
			flags[i] = 3;
			continue;
		}

		buff.reset();
		charIdx = 0;
		intIdx = 0;
		u32Idx = 0;
		u64Idx = 0;
		strIdx = 0;

		for (int i=0; i<numEntries; i++)
		{
			switch (flags[i])
			{
			case 0:
				 // It is an integer
				 OmnCreateTc << (intValues[intIdx++] == buff.getInt(-1)) << endtc;
				 break;

			case 1:
				 // It is u32
				 OmnCreateTc << (u32Values[u32Idx++] == buff.getU32(0)) << endtc;
				 break;

			case 2:
				 // It is u64
				 OmnCreateTc << (u64Values[u64Idx++] == buff.getU64(0)) << endtc;
				 break;

			case 3:
				 // It is string
				 OmnCreateTc << (strValues[strIdx++] == buff.getOmnStr("")) << endtc;
				 break;

			case 4:
				 // It is char 
				 OmnCreateTc << (charValues[charIdx++] == buff.getChar(' ')) << endtc;
				 break;
			}
		}
	}
	return true;
}


OmnString
OmnBuffTester::getRandStr()
{

	int len = rand() % 100;
	OmnString str(len+1, ' ', true);
	char *data = (char *)str.data();
	for (int i=0; i<len; i++)
	{
		data[i] = rand() % 124 + 1;
	}
	return str;
}


