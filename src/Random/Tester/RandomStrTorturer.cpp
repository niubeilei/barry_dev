////////////////////////////////////////////////////////////////////////////
//
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: RandomStrTorturer.cpp
//// Description:
////   
////
//// Modification History:
//// 11/27/2006      Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#if 0
#include "Random/Tester/RandomStrTorturer.h"

#include "Util/OmnNew.h"
#include "Random/Ptrs.h"
#include "Random/RandomStr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"

static OmnString example1 =
		"<Random> "
            "<Name>test</Name>"
            "<Type>StringSelector</Type>"
            "<Selector>"
	            "<Type>StringSelectRandom</Type>"
	            "<Elements>"
	                "<Element>string1, 10</Element>"
	                "<Element>string2, 15</Element>"
	                "<Element>string3, 20</Element>"
	                "<Element>string4, 55</Element>"
		        "</Elements>"
		    "</Selector>"
		"</Random>";

static OmnString example2 = 
"<Random>"
	"<Name>RanStr1Test</Name>"
	"<Type>RandomString</Type>"
	"<MinLen>2</MinLen>"
	"<MaxLen>16</MaxLen>"
	"<StrType>NoSpaceStr</StrType>"
"</Random>";

AosRandomStrTorturer::AosRandomStrTorturer()
{
}

static int counts[4];

bool AosRandomStrTorturer::start()
{
    OmnBeginTest << "Begin the Random String Torturer Testing";
    mTcNameRoot = "RandomStr_Torturer";

	u32 tries = 10000000;
	testRandomStr(tries);
	return true;
}


bool
AosRandomStrTorturer::testRandomStr(const u32 tries)
{
	OmnXmlItemPtr configXmlPtr = OmnNew OmnXmlItem(example2);
	AosRandomObjPtr gen = OmnNew AosRandomStr(configXmlPtr);
	OmnTCTrue(gen->isGood()) << endtc;

	u32 count = 0;
	for (u32 i=0; i<tries; i++)
	{
		if (count > 10000)
		{
			cout << "Tried: " << i << endl;
			count = 0;
		}
		count++;

		OmnString value = gen->nextStr();
		OmnTCTrue(value.length() >= 2) << value << endtc;
		OmnTCTrue(value.length() <= 16) << value << endtc;
		const char *data = value.data();
		for (int k=0; k<value.length(); k++)
		{
			if (!(data[k] == 33 || data[k] >= 35 && data[k] <= 126))
			{
				OmnTCTrue(false) << value.toHex() << endtc;
				break;
			}
		}
	}

	return true;
}

/*
	OmnString name = "test";
	OmnXmlItemPtr configXmlPtr = OmnNew OmnXmlItem(example1);
	AosRandomObjPtr RandomStrPtr = AosRandomObj::createRandomObj(configXmlPtr);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(RandomStrPtr)) << endtc;
	if (!RandomStrPtr) return false;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(RandomStrPtr->isGood())) << endtc;
	OmnString value;

	counts[0] = 0;
	counts[1] = 0;
	counts[2] = 0;
	counts[3] = 0;

	int total = 1;
	for (int i=0; i<total; i++) 
	{
		value = RandomStrPtr->nextStr();
		if (value == "string1")
		{
			counts[0]++;
		}
		else if (value == "string2")
		{
			counts[1]++;
		}
		else if (value == "string3")
		{
			counts[2]++;
		}
		else if (value == "string4")
		{
			counts[3]++;
		}
		else
		{
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(false)) << "Value = " << value << endtc;
		}
	}

	float p1 = counts[0] * 100.0 / total;
	float p2 = counts[1] * 100.0 / total;
	float p3 = counts[2] * 100.0 / total;
	float p4 = counts[3] * 100.0 / total;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(p1 >= 9.0 && p1 <= 11.0))  << "p1 = " << p1 << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(p2 >= 14.0 && p2 <= 16.0)) << "p2 = " << p2 << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(p3 >= 19.0 && p3 <= 21.0)) << "p3 = " << p3 << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(p4 >= 54.0 && p4 <= 56.0)) << "p4 = " << p4 << endtc;

    return true;
}

*/
#endif
