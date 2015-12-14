////////////////////////////////////////////////////////////////////////////
//
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: ApplicationProxyTester.cpp
//// Description:
////   
////
//// Modification History:
//// 11/27/2006      Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#include "Random/Tester/RandomLenStrTorturer.h"

#include "Util/OmnNew.h"
#include "Random/Ptrs.h"
#include "Random/RandomLenStr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"

static OmnString example1 =
		"<Random> "
            "<Name>test</Name>"
            "<Type>LenStringSelector</Type>"
            "<Selector>"
	            "<Type>RandomLenStr</Type>"
				"<StrType>LowcaseStr</StrType>"
	            "<Elements>"
	                "<Element>1, 10</Element>"
	                "<Element>[2, 10], 15</Element>"
	                "<Element>[11, 50], 20</Element>"
	                "<Element>51, 55</Element>"
		        "</Elements>"
		    "</Selector>"
		"</Random>";

AosRandomLenStrTorturer::AosRandomLenStrTorturer()
{
}

static int counts[4];

bool AosRandomLenStrTorturer::start()
{
    OmnBeginTest << "Begin the Random LenStr Torturer Testing";
    mTcNameRoot = "RandomLenStr_Torturer";

	OmnString name = "test";
	OmnXmlItemPtr configXmlPtr = OmnNew OmnXmlItem(example1);
	AosRandomObjPtr randomIntegerPtr = AosRandomObj::createRandomObj(example1);

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(randomIntegerPtr)) << endtc;
	if (!randomIntegerPtr) return false;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(randomIntegerPtr->isGood())) << endtc;
	OmnString str;
	int value = 0;

	counts[0] = 0;
	counts[1] = 0;
	counts[2] = 0;
	counts[3] = 0;

	int total = 1000000;
	for (int i=0; i<total; i++) 
	{
		str = randomIntegerPtr->nextStr();
		value = strlen(str.getBuffer());
		if (value == 1)
		{
			counts[0]++;
		}
		else if (value >= 2 && value <= 10)
		{
			counts[1]++;
		}
		else if (value >= 11 && value <= 50)
		{
			counts[2]++;
		}
		else if (value == 51)
		{
			counts[3]++;
		}
		else
		{
			//OmnTC(OmnExpected<bool>(true), OmnActual<bool>(false)) << "Value = " << value << endtc;
		}
	}

	float p1 = counts[0] * 100.0 / total;
	float p2 = counts[1] * 100.0 / total;
	float p3 = counts[2] * 100.0 / total;
	float p4 = counts[3] * 100.0 / total;

	// cout << p1 << ", " << p2 << ", " << p3 << ", " << p4 << endl;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(p1 >= 9.0 && p1 <= 11.0))  << "p1 = " << p1 << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(p2 >= 14.0 && p2 <= 16.0)) << "p2 = " << p2 << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(p3 >= 19.0 && p3 <= 21.0)) << "p3 = " << p3 << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(p4 >= 54.0 && p4 <= 56.0)) << "p4 = " << p4 << endtc;

    return true;
}

