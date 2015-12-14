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
#include "UtilTime/Testers/TFEpochDayTester.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/ThreadPool.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"
#include "UtilTime/TimeFormat.h"
#include "XmlUtil/XmlTag.h"



AosTFEpochDayTester::AosTFEpochDayTester()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
}


AosTFEpochDayTester::~AosTFEpochDayTester()
{
}


bool 
AosTFEpochDayTester::start()
{
	int tries = 100000000;
	u64 time1 = OmnGetTimestamp();
	OmnString vv, vv1;
	char data[20];

	for (int i=0; i<tries; i++)
	{
		int year = OmnRandom::nextInt(1950, 2020);
		int month = OmnRandom::nextInt(1, 12);
		int day = OmnRandom::nextInt(1, 31);
		switch (month)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			 break;

		case 2:
			 if (day > 28) day = 28;
			 break;

		case 4:
		case 6:
		case 9:
		case 11:
			 if (day > 30) day = 30;
			 break;
		}

		sprintf(data, "%04d%02d%02d", year, month, day);
		int len = 8;
		AosValueRslt value;
		AosTimeFormat::convert(data, len, AosTimeFormat::eYYYYMMDD,
			AosTimeFormat::eEpochDay, value, mRundata);
		vv = "";
		vv << value.getU32Value(mRundata);

		AosTimeFormat::convert(vv.getBuffer(), len, AosTimeFormat::eEpochDay,
			AosTimeFormat::eYYYYMMDD, value, mRundata);
		vv1 = value.getValueStr1();

		if (i % 100000 == 0)
		{
			u64 time2 = OmnGetTimestamp();
			cout << "Processed: " << i << ". Time: " << time2 - time1 << 
				". Date: " << data << ". EpochDay Value: " << vv 
				<< " Date Value: " << vv1 << endl;
			time1 = time2;
		}
	}
	return true;
}

