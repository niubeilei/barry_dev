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
#include "Random/Tester/RandomIntegerTorturer.h"

#include "Util/OmnNew.h"
#include "Random/Ptrs.h"
#include "Random/RandomInteger.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"

const int sgTries = 1000000;

AosRandomIntegerTorturer::AosRandomIntegerTorturer()
{
}


bool 
AosRandomIntegerTorturer::start()
{
	
	test1();
	test2();
	return true;
}


bool
AosRandomIntegerTorturer::test1()
{
	static OmnString example =
		"<Random> "
            "<Name>test</Name>"
            "<Type>Selector</Type>"
            "<Selector>"
	            "<Type>IntegerRandom</Type>"
				"<Base>100</Base>"
	            "<Elements>"
	                "<Element>1, 10</Element>"
	                "<Element>[2, 10], 15</Element>"
	                "<Element>[11, 50], 20</Element>"
	                "<Element>51, 55</Element>"
		        "</Elements>"
		    "</Selector>"
		"</Random>";

	int low[4];
	int high[4];
	u16 weights[4];

	weights[0] = 10;
	weights[1] = 15;
	weights[2] = 20;
	weights[3] = 55;

	low[0] = 1; high[0] = 1;
	low[1] = 2; high[1] = 10;
	low[2] = 11; high[2] = 50;
	low[3] = 51; high[3] = 51;

	test(example, sgTries, low, high, weights, 4);
	return true;
}


bool
AosRandomIntegerTorturer::test2()
{
	static OmnString example =
		"<Random> "
            "<Name>test2</Name>"
            "<Type>Selector</Type>"
            "<Selector>"
	            "<Type>IntegerRandom</Type>"
				"<Base>0</Base>"
	            "<Elements>"
	                "<Element>1, 10</Element>"
	                "<Element>[2, 10], 20</Element>"
	                "<Element>[11, 50], 30</Element>"
	                "<Element>51, 40</Element>"
	                "<Element>52, 50</Element>"
		        "</Elements>"
		    "</Selector>"
		"</Random>";

	int low[5];
	int high[5];
	u16 weights[5];

	weights[0] = 10;
	weights[1] = 20;
	weights[2] = 30;
	weights[3] = 40;
	weights[4] = 50;

	low[0] = 1; high[0] = 1;
	low[1] = 2; high[1] = 10;
	low[2] = 11; high[2] = 50;
	low[3] = 51; high[3] = 51;
	low[4] = 52; high[4] = 52;

	test(example, sgTries, low, high, weights, 5);

	return true;
}


bool AosRandomIntegerTorturer::test(
		const OmnString &def, 
		int tries, 
		int *low, 
		int *high, 
		u16 *weight, 
		int numElems)
{
    OmnBeginTest << "Begin the Random Integer Torturer Testing";
    mTcNameRoot = "RandomInteger_Torturer";

	OmnString name = "test";
	OmnXmlItemPtr configXmlPtr = OmnNew OmnXmlItem(def);
	AosRandomObjPtr randomIntegerPtr = 
		AosRandomObj::createRandomObj(configXmlPtr);

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(randomIntegerPtr)) 
		<< endtc;

	if (!randomIntegerPtr) return false;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(randomIntegerPtr->isGood())) << endtc;
	int value;

	int *counts = OmnNew int[numElems];

	for (int i=0; i<numElems; i++)
	{
		counts[i] = 0;
	}

	int totalCount = 0;
	int totalMissed = 0;
	for (int i=0; i<tries; i++) 
	{
		value = randomIntegerPtr->nextInt();
		bool found = false;
		for (int k=0; k<numElems; k++)
		{
			if (value >= low[k] && value <= high[k])
			{
				counts[k]++;
				found = true;
				totalCount++;
				break;
			}
		}

		if (!found)
		{
			totalMissed++;
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(false)) 
				<< "Value = " << value << endtc;
		}
	}

	int totalWeight = 0;
	for (int i=0; i<numElems; i++)
	{
		totalWeight += weight[i];
	}

	for (int i=0; i<numElems; i++)
	{
		float c = counts[i];
		float t = totalCount + totalMissed;
		float actual = c / t;
		float expected = (float)weight[i]/(float)totalWeight;
		bool rslt = actual <= expected + 0.01 && actual >= expected - 0.01;
		cout << "Actual: " << actual 
			<< ". Expected: " << expected << endl;
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) 
			<< "Actual = " << actual
		   	<< ". Expected: " << expected << endtc;
	}

    return true;
}

