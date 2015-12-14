////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CharPtreeTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelUtilTester/CharPtreeTester.h"


#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool AosCharPtreeTester::start()
{
	// 
	// Test default constructor
	//
	normalTest();
	return true;
}


bool AosCharPtreeTester::normalTest()
{
	OmnBeginTest << "Test CharPtree";
	mTcNameRoot = "Test_Normal";
	int ret;

	aosCharPtree tree;
	ret = aosCharPtree_init(&tree);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	// Insert key1
	OmnString key1 = "key1";
	ret = aosCharPtree_insert(&tree, key1.data(), key1.length(), (void *)10, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	// Insert key2
	OmnString key2 = "key11";
	ret = aosCharPtree_insert(&tree, key2.data(), key2.length(), (void *)20, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	int foundIndex; 
	void *foundNode;

	// Retrieve key1
	ret = aosCharPtree_get(&tree, key1.data(), &foundIndex, &foundNode);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(10), OmnActual<int>((int)foundNode)) << endtc;

	// Retrieve key2
	ret = aosCharPtree_get(&tree, key2.data(), &foundIndex, &foundNode);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(20), OmnActual<int>((int)foundNode)) << endtc;

 /*   ret = aosCharPtree_insert1(&tree, "AppProxyAdd", 		(void *)41, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxyDel", 		(void *)42, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxyShow", 		(void *)43, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxyClearAll", 	(void *)44, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxySetStatus", 	(void *)45, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxyAaaAdd", 	(void *)46, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxyAaaDel", 	(void *)47, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxyAaaClear", 	(void *)48, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxySetSvrGrp", 	(void *)49, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxyResetGroup",	(void *)50, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxyShowConn", 	(void *)51, 0);
    ret = aosCharPtree_insert1(&tree, "AppProxyConnDurSet",	(void *)52, 0);
*/
	// Retrieve "AppProxyShowConn"
	ret = aosCharPtree_get(&tree, "AppProxyShowConn", &foundIndex, &foundNode);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(51), OmnActual<int>((int)foundNode)) << endtc;

	return true;
}

	
