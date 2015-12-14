////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashIntTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelUtilTester/HashIntTester.h"


#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "KernelUtil/HashInt.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "aos/aosKernelAlarm.h"



bool AosHashIntTester::start()
{
	// 
	// Test default constructor
	//
	normalTest();
	return true;
}


bool AosHashIntTester::normalTest()
{
    OmnBeginTest << "Test HashInt";
	mTcNameRoot = "Test_Normal";
	int ret;
	void *value;
//	OmnTC(OmnExpected<int>(51), OmnActual<int>(0)) << endtc;

    struct AosHashInt *theTable = 0;
   //
   //  test Constructor
   //
	ret = AosHashInt_constructor(&theTable, 2047, 500000);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) 
		<< "This is a false error. Actual: " 
		<< ret << endtc;    

   //
   //  test Add an Entry
   //
    ret = AosHashInt_add(theTable, 1, 0, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	ret = AosHashInt_get(theTable, 1, &value, 0);
    OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
  
    ret = AosHashInt_add(theTable, 1, 0, 0);
    OmnTC(OmnExpected<int>(eAosRc_EntryAlreadyInList), OmnActual<int>(ret)) << endtc;
 
     
 



	return true;
}

	
