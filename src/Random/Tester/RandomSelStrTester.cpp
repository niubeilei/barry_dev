////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.cpp
// Description:
//   
//
// Modification History:
// 11/27/2006      Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Random/Tester/RandomSelStrTester.h"

#include "Util/OmnNew.h"
#include "Random/Ptrs.h"
#include "Random/RandomSelStr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


AosRandomSelStrTester::AosRandomSelStrTester()
{
}


bool AosRandomSelStrTester::start()
{
	u32 tries = 100;
	basicTest1(tries);
//	basicTest2(tries);
	return true;
}


bool
AosRandomSelStrTester::basicTest1(const u32 tries)
{
	OmnString example1 =
		"<Random> "
            "<Name>test</Name>"
            "<Type>StringSelector</Type>"
            "<Selector>"
	            "<Type>StringSelector</Type>"
				"<StrType>LowcaseStr</StrType>"
	            "<Elements>"
	                "<Element>value1, 10</Element>"
	                "<Element>value2, 15</Element>"
	                "<Element>value3, 20</Element>"
	                "<Element>value4, 55</Element>"
		        "</Elements>"
				"<SeparatorDef>"
					"<Separator>|</Separator>"
					"<MinSeg>1</MinSeg>"
					"<MaxSeg>4</MaxSeg>"
					"<Unique>true</Unique>"
					"<Ordered>true</Ordered>"
				"</SeparatorDef>"
		    "</Selector>"
		"</Random>";
    OmnBeginTest << "Begin the Random LenStr Torturer Testing";
    mTcNameRoot = "RandomLenStr_Torturer";

	OmnString name = "test";
	OmnXmlItemPtr configXmlPtr = OmnNew OmnXmlItem(example1);
	AosRandomObjPtr obj = AosRandomObj::createRandomObj(example1);

	OmnTCTrue(OmnActual<bool>(obj)) << endtc;
	if (!obj) return false;

	OmnTCTrue(OmnActual<bool>(obj->isGood())) << endtc;
	OmnString str;

	OmnNotImplementedYet;
	/* Chen Ding, 2013/05/13
	for (u32 i=0; i<tries; i++) 
	{
		str = obj->nextStr();

		OmnTCTrue(str != "") << endtc;
		OmnStrParser1 parser(str);
		parser.reset();
		while (parser.hasMore())
		{
			OmnString word = parser.nextWord("", "|");
			// OmnTrace << "Word: " << word << endl;
			if (parser.hasMore())
			{
				OmnTCTrue(parser.expectNext('|', true)) << str << endtc;
			}

			OmnTCTrue(word != "") << endtc;

			bool actual = (word == "value1" ||
						   word == "value2" ||
						   word == "value3" ||
						   word == "value4");
			OmnTCTrue(actual) << endtc;
		}
	}

OmnTrace << "To generate incorrect value" << endl;

	AosParmReturnCode rcode; 
	OmnString errmsg;
	for (u32 i=0; i<tries; i++) 
	{
		OmnString str = obj->nextIncorrectStr(rcode, errmsg);
if (rcode == eAosParmRc_Success)
cout << "Success: " << str << endl;
else
cout << "Failed:  " << str << ". Error: " << errmsg << endl;

//		OmnStrParser parser(str);
//		parser.reset();
//		while (parser.hasMore())
//		{
//			OmnString word = parser.nextWord("", "|");
//			if (parser.hasMore())
//			{
//				OmnTCTrue(parser.expectNext('|', true)) << str << endtc;
//			}
//
//			OmnTCTrue(word != "") << endtc;
//
//			bool actual = !(word == "value1" ||
//						   word == "value2" ||
//						   word == "value3" ||
//						   word == "value4");
//			OmnTCTrue(actual) << word << endtc;
//			
//		}
	}

	*/
    return true;
}


bool
AosRandomSelStrTester::basicTest2(const u32 tries)
{
	int counts[4];
	memset(counts, 0, sizeof(int)*4);
    OmnBeginTest << "Begin the Random LenStr Torturer Testing";
    mTcNameRoot = "RandomLenStr_Torturer";
	OmnString values;
	values << "[value1, 10], [value2, 20], [value3, 30], [value4, 40]";

	AosRandomSelStr obj(values);

	OmnTCTrue(OmnActual<bool>(obj.isGood())) << endtc;
	OmnString str;

	for (u32 i=0; i<tries; i++) 
	{
		str = obj.nextStr();
		bool actual = (str == "value1") ||
					  (str == "value2") ||
					  (str == "value3") ||
					  (str == "value4");
		OmnTCTrue(actual) << str << endtc;
 // cout << str << endl;
		if (str == "value1")
		{
			counts[0]++;
		}
		else if (str == "value2")
		{
			counts[1]++;
		}
		else if (str == "value3")
		{
			counts[2]++;
		}
		else if (str == "value4")
		{
			counts[3]++;
		}
		else
		{
			OmnTCBool(false, true) << "Unrecognized value: " << str << endtc;
		}
	}

	float p1 = counts[0] * 100.0 / tries;
	float p2 = counts[1] * 100.0 / tries;
	float p3 = counts[2] * 100.0 / tries;
	float p4 = counts[3] * 100.0 / tries;

	cout << p1 << ", " << p2 << ", " << p3 << ", " << p4 << endl;

	OmnTCTrue(OmnActual<bool>(p1 >= 9.0 && p1 <= 11.0)) << "p1 = " << p1 << endtc;
	OmnTCTrue(OmnActual<bool>(p2 >= 19.0 && p2 <= 21.0)) << "p2 = " << p2 << endtc;
	OmnTCTrue(OmnActual<bool>(p3 >= 29.0 && p3 <= 31.0)) << "p3 = " << p3 << endtc;
	OmnTCTrue(OmnActual<bool>(p4 >= 39.0 && p4 <= 41.0)) << "p4 = " << p4 << endtc;

    return true;
}


