////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PtrHashTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/PtrHashTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/PtrHash.h"

bool OmnPtrHashTester::start()
{
	// 
	// Test default constructor
	//
	testDefaultConstructor();
	testAddAndDelete();

	return true;
}


bool
OmnPtrHashTester::testDefaultConstructor()
{
	OmnBeginTest << "Test Default Constructor";
	mTcNameRoot = "PtrHash-DFT-CTR";

	OmnPtrHash table;

	OmnCreateTc << (table.getNumPtrsAdded() == 0) << endtc;
	OmnCreateTc << (table.getNumPtrsDeleted() == 0) << endtc;
	OmnCreateTc << (table.getNumIdleElems() == 0) << endtc;
	OmnCreateTc << (table.getNumElemsCreated() == 0) << endtc;
	OmnCreateTc << (table.getNumElemsDeleted() == 0) << endtc;
	OmnCreateTc << (table.getNumErrorAdd()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedAllocation()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedQuery()  == 0) << endtc;
	return true;
}


bool
OmnPtrHashTester::testAddAndDelete()
{
	OmnBeginTest << "Test Add and Delete";
	mTcNameRoot = "PtrHash-AddDel-CTR";

	OmnPtrHash table;

	int i;
	int numToAdd = 1000000;
	for (i=0; i<numToAdd; i++)
	{
		table.addPtr((const void *)i);
	}

	OmnCreateTc << (table.getNumPtrsAdded() == numToAdd) << endtc;
	OmnCreateTc << (table.getNumPtrsDeleted() == 0) << endtc;

	OmnCreateTc << (table.getNumIdleElems() == 0) 
		<< "Actual: " << table.getNumIdleElems() << endtc;

	OmnCreateTc << (table.getNumElemsCreated() == numToAdd) 
		<< "Actual: " << table.getNumElemsCreated() << endtc;

	OmnCreateTc << (table.getNumElemsDeleted() == 0) << endtc;
	OmnCreateTc << (table.getNumErrorAdd()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedAllocation()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedQuery()  == 0) << endtc;

	// 
	// Remove them all
	//
	for (i=0; i<numToAdd; i++)
	{
		table.remove((const void *)i);
	}

	OmnCreateTc << (table.getNumPtrsAdded() == numToAdd) << endtc;

	OmnCreateTc << (table.getNumPtrsDeleted() == numToAdd) 
		<< "Actual: " << table.getNumPtrsDeleted() << endtc;

	OmnCreateTc << (table.getNumIdleElems() == 2000) << endtc;
	OmnCreateTc << (table.getNumElemsCreated() == numToAdd) << endtc;
	OmnCreateTc << (table.getNumElemsDeleted() == numToAdd - 2000) 
		<< "Actual: " << table.getNumElemsDeleted() << endtc;
	OmnCreateTc << (table.getNumErrorAdd()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedAllocation()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedQuery()  == 0) 
		<< "Actual: " << table.getNumFailedQuery() << endtc;


	// 
	// Add again
	// 
	for (i=0; i<numToAdd; i++)
	{
		table.addPtr((const void *)i);
	}

	OmnCreateTc << (table.getNumPtrsAdded() == 2*numToAdd) << endtc;
	OmnCreateTc << (table.getNumPtrsDeleted() == numToAdd) << endtc;

	OmnCreateTc << (table.getNumIdleElems() == 0) 
		<< "Actual: " << table.getNumIdleElems() << endtc;

	OmnCreateTc << (table.getNumElemsCreated() == 2*numToAdd - 2000) 
		<< "Actual: " << table.getNumElemsCreated() << endtc;

	OmnCreateTc << (table.getNumElemsDeleted() == numToAdd - 2000) << endtc;
	OmnCreateTc << (table.getNumErrorAdd()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedAllocation()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedQuery()  == 0) << endtc;

	// 
	// Query
	// 
	for (i=0; i<numToAdd; i++)
	{
		OmnCreateTc << (table.inTable((const void *)i, false)) << endtc;
	}

	OmnCreateTc << (table.getNumPtrsAdded() == 2*numToAdd) << endtc;
	OmnCreateTc << (table.getNumPtrsDeleted() == numToAdd) << endtc; //

	OmnCreateTc << (table.getNumIdleElems() == 0) 
		<< "Actual: " << table.getNumIdleElems() << endtc;

	OmnCreateTc << (table.getNumElemsCreated() == 2*numToAdd - 2000) 
		<< "Actual: " << table.getNumElemsCreated() << endtc;

	OmnCreateTc << (table.getNumElemsDeleted() == numToAdd - 2000) << endtc;
	OmnCreateTc << (table.getNumErrorAdd()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedAllocation()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedQuery()  == 0) << endtc;

	// 
	// Remove
	// 
	for (i=0; i<numToAdd; i++)
	{
		OmnCreateTc << (table.inTable((const void *)i, true)) << endtc;
	}

	OmnCreateTc << (table.getNumPtrsAdded() == 2*numToAdd) << endtc;
	OmnCreateTc << (table.getNumPtrsDeleted() == 2*numToAdd) << endtc;   //
	OmnCreateTc << (table.getNumIdleElems() == 2000) 
		<< "Actual: " << table.getNumIdleElems() << endtc;

	OmnCreateTc << (table.getNumElemsCreated() == 2*numToAdd - 2000) 
		<< "Actual: " << table.getNumElemsCreated() << endtc;

	OmnCreateTc << (table.getNumElemsDeleted() == 2*numToAdd - 2000) << endtc;   //
	OmnCreateTc << (table.getNumErrorAdd()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedAllocation()  == 0) << endtc;
	OmnCreateTc << (table.getNumFailedQuery()  == 0) << endtc;

	return true;
}

