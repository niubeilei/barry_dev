////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RCObjectTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/RCObjectTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


static bool sgObjDelFlag = false;
static int	sgTestBase1Deleted = 0;
static int	sgTestBase2Deleted = 0;
static int	sgTestDerivedObjDeleted =0;

bool OmnRCObjectTester::start()
{
	// 
	// Test default constructor
	//
	testStackObjects();
	testHeapObjects();
	testDerivedObjects();
	return true;
}


bool
OmnRCObjectTester::testStackObjects()
{
	// 
	// Stack objects shall never be deleted by smart pointers. 
	// 
	/*
	 * Chen Ding, 09/30/2010
	OmnBeginTest << "Test Stack Objects";
	mTcNameRoot = "RCObject-Stack";

	int	numPtrsAdded = OmnRCObject::getNumPtrsAdded();
	int	numPtrsDeleted = OmnRCObject::getNumPtrsDeleted();
	int	numIdleElems = OmnRCObject::getNumIdleElems();
	int	numElemsCreated = OmnRCObject::getNumElemsCreated();
	int	numElemsDeleted = OmnRCObject::getNumElemsDeleted();
	int	numErrorAdd = OmnRCObject::getNumErrorAdd();
	int	numFailedAllocation = OmnRCObject::getNumFailedAllocation();
	int	numFailedQuery = OmnRCObject::getNumFailedQuery();

	OmnTestBaseA a;

	sgObjDelFlag = false;
	a.func2();
	OmnTC(OmnExpected<bool>(false), OmnActual<bool>(sgObjDelFlag)) << endtc;

	OmnTestBaseA b;
	sgObjDelFlag = false;

	// 
	// When calling the following function, the smart pointer will try to delete
	// 'a', but it should not since 'a' is a stack object.
	//
	b.func2();
	OmnTC(OmnExpected<bool>(false), OmnActual<bool>(sgObjDelFlag)) << endtc;

	// 
	// Check it.
	//
	OmnTC(OmnExpected<int>(OmnRCObject::getNumPtrsAdded()), OmnActual<int>(numPtrsAdded)) << endtc;

	OmnTC(OmnExpected<int>(OmnRCObject::getNumPtrsAdded()), OmnActual<int>(numPtrsAdded)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumPtrsDeleted()), OmnActual<int>( numPtrsDeleted)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumIdleElems()), OmnActual<int>( numIdleElems)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumElemsCreated()), OmnActual<int>( numElemsCreated)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumElemsDeleted()), OmnActual<int>( numElemsDeleted)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumErrorAdd()), OmnActual<int>( numErrorAdd)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumFailedAllocation()), OmnActual<int>( numFailedAllocation)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumFailedQuery()), OmnActual<int>( numFailedQuery + 1)) << endtc;

	// 
	// In the following, we will create a large number of smart pointers pointing
	// to a local variable. 
	//
	for (int i=0; i<mNumTries; i++)
	{
		// 
		// Object 'a' is a stack variable. We will create a smart pointer 
		// pointing to it. When the smart pointer exits, 'a' should not 
		// be deleted.
		//
		OmnTestBaseA a;
		OmnTestBaseA::mObjectsDeleted = 0;
		{
			OmnSPtr<OmnTestBaseA> ptr(&a);
		}
		OmnTC(OmnExpected<int>(OmnTestBaseA::mObjectsDeleted), OmnActual<int>(0)) << endtc;
	}

	OmnTC(OmnExpected<int>(OmnRCObject::getNumPtrsAdded()), OmnActual<int>( numPtrsAdded)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumPtrsDeleted()), OmnActual<int>( numPtrsDeleted)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumIdleElems()), OmnActual<int>( numIdleElems)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumElemsCreated()), OmnActual<int>( numElemsCreated)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumElemsDeleted()), OmnActual<int>( numElemsDeleted)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumErrorAdd()), OmnActual<int>( numErrorAdd)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumFailedAllocation()), OmnActual<int>( numFailedAllocation)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumFailedQuery()), OmnActual<int>( numFailedQuery + mNumTries + 1)) << endtc;
	*/

	// 
	// There is no test case here. If the program not working correctly, it will core dump
	// at this point.
	//
	return true;
}


bool
OmnRCObjectTester::testHeapObjects()
{
	// 
	// We create a large number of heap objects and then delete
	// them. After that, we check whether the objects were created
	// and deleted correctly. 
	//
	/*
	 * Chen Ding, 09/30/2010
	OmnBeginTest << "Test Heap Objects";
	mTcNameRoot = "RCObject-Heap";
	
	OmnTestBaseA::mObjectsCreated = 0;
	OmnTestBaseA::mObjectsDeleted = 0;

	int	numPtrsAdded = OmnRCObject::getNumPtrsAdded();
	int	numPtrsDeleted = OmnRCObject::getNumPtrsDeleted();
	int	numIdleElems = OmnRCObject::getNumIdleElems();
	int	numElemsCreated = OmnRCObject::getNumElemsCreated();
	int	numElemsDeleted = OmnRCObject::getNumElemsDeleted();
	int	numErrorAdd = OmnRCObject::getNumErrorAdd();
	int	numFailedAllocation = OmnRCObject::getNumFailedAllocation();
	int	numFailedQuery = OmnRCObject::getNumFailedQuery();
 
	for (int i=0; i<mNumTries; i++)
	{
		OmnSPtr<OmnTestBaseA> ptr = OmnNew OmnTestBaseA();
	}

	OmnTC(OmnExpected<int>(OmnTestBaseA::mObjectsCreated), OmnActual<int>(mNumTries)) << endtc;
	OmnTC(OmnExpected<int>(OmnTestBaseA::mObjectsDeleted), OmnActual<int>(mNumTries)) << endtc;

	int idles = (numIdleElems == 0)?1:numIdleElems;
	int created = (numIdleElems == 0)?numElemsCreated+1:numElemsCreated;

	OmnTC(OmnExpected<int>(OmnRCObject::getNumPtrsAdded()), OmnActual<int>( numPtrsAdded + mNumTries)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumPtrsDeleted()), OmnActual<int>( numPtrsDeleted + mNumTries)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumIdleElems()), OmnActual<int>( idles)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumElemsCreated()), OmnActual<int>( created)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumElemsDeleted()), OmnActual<int>( numElemsDeleted)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumErrorAdd()), OmnActual<int>( numErrorAdd)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumFailedAllocation()), OmnActual<int>( numFailedAllocation)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumFailedQuery()), OmnActual<int>( numFailedQuery)) << endtc;
	*/

	return true;
}

void
OmnTestFunc2(OmnTestDerivedFromA *ptr);

bool
OmnRCObjectTester::testDerivedObjects()
{
	// 
	// Derived objects with multiple parent classes may have multiple
	// 'this' pointers. This can be a problem for smart pointer to 
	// determine whether to delete an object or not. In the current
	// implementation, we use dynamic_cast<...> to translate 'this'
	// pointer to the beginning of an object, regardless of whether 
	// it is a derived multi-inherited object or not. This function
	// will test this capability.
	//
	OmnBeginTest << "Test Heap Objects";
	mTcNameRoot = "RCObject-Derived";
	
	OmnTestDerivedFromA *ptrC;
	OmnTestDerivedFromA *ptr = OmnNew OmnTestDerivedFromA(1, ptrC);
	OmnTestBaseA *ptrA = (OmnTestBaseA*)ptr;
	OmnTestBaseA *ptrB = (OmnTestDerivedFromA *)ptrA;
	OmnTestFunc2((OmnTestDerivedFromA *)ptrB);
	OmnTestFunc2((OmnTestDerivedFromA *)ptrA);


	sgTestBase1Deleted = 0;
	sgTestBase2Deleted = 0;
	sgTestDerivedObjDeleted = 0;

	/*
	 * Chen Ding, 09/30/2010
	int	numPtrsAdded = OmnRCObject::getNumPtrsAdded();
	int	numPtrsDeleted = OmnRCObject::getNumPtrsDeleted();
	int	numIdleElems = OmnRCObject::getNumIdleElems();
	int	numElemsCreated = OmnRCObject::getNumElemsCreated();
	int	numElemsDeleted = OmnRCObject::getNumElemsDeleted();
	int	numErrorAdd = OmnRCObject::getNumErrorAdd();
	int	numFailedAllocation = OmnRCObject::getNumFailedAllocation();
	int	numFailedQuery = OmnRCObject::getNumFailedQuery();

	for (int i=0; i<mNumTries; i++)
	{
		OmnSPtr<OmnTestDerivedObj> mi = OmnNew OmnTestDerivedObj;
		OmnTestBase1 *ptr1 = mi->getBase1This();
		OmnTestBase2 *ptr2 = mi->getBase2This();

		// 
		// The following pointers are different in values but should
		// point to the same object. This is to test whether OmnRCObject
		// can handle this type of objects correctly.
		//
		OmnSPtr<OmnTestDerivedObj> sptr1 = (OmnTestDerivedObj*)ptr1;
		OmnSPtr<OmnTestDerivedObj> sptr2 = (OmnTestDerivedObj*)ptr2;

		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ptr1->isOnHeap())) << endtc;
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ptr2->isOnHeap())) << endtc;

		OmnTestDerivedObj d;
		OmnSPtr<OmnTestDerivedObj> ptr3(&d);
		OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ptr3->isOnHeap())) << endtc;
	}

	OmnTC(OmnExpected<int>(sgTestBase1Deleted), OmnActual<int>(2*mNumTries)) << endtc;
	OmnTC(OmnExpected<int>(sgTestBase2Deleted), OmnActual<int>(2*mNumTries)) << endtc;
	OmnTC(OmnExpected<int>(sgTestDerivedObjDeleted), OmnActual<int>(2*mNumTries)) << endtc;

	int idles = (numIdleElems == 0)?1:numIdleElems;
	int created = (numIdleElems == 0)?numElemsCreated+1:numElemsCreated;

	OmnTC(OmnExpected<int>(OmnRCObject::getNumPtrsAdded()), OmnActual<int>(mNumTries + numPtrsAdded)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumPtrsDeleted()), OmnActual<int>(mNumTries + numPtrsDeleted)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumIdleElems()), OmnActual<int>(idles)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumElemsCreated()), OmnActual<int>( created)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumElemsDeleted()), OmnActual<int>(numElemsDeleted)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumErrorAdd()), OmnActual<int>( numErrorAdd)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumFailedAllocation()), OmnActual<int>( numFailedAllocation)) << endtc;
	OmnTC(OmnExpected<int>(OmnRCObject::getNumFailedQuery()), OmnActual<int>(numFailedQuery + 2*mNumTries)) << endtc;
	*/

	return true;
}


void
OmnTestFunc2(OmnTestDerivedFromA *ptr)
{
}


// 
// Below are helping classes and functions
//
int OmnTestBaseA::mObjectsCreated = 0;
int OmnTestBaseA::mObjectsDeleted = 0;

OmnTestBaseA::OmnTestBaseA() 
{
	mObjectsCreated++;
}


void OmnTestFunc1(const OmnSPtr<OmnTestBaseA> &ptr);

int 
OmnTestBaseA::func2()
{
	OmnSPtr<OmnTestBaseA> thisPtr(this, false);
	OmnTestFunc1(thisPtr);
	return 0;
}


OmnTestBaseA::~OmnTestBaseA() 
{
	sgObjDelFlag = true;
	mObjectsDeleted++;
}


void OmnTestFunc1(const OmnSPtr<OmnTestBaseA> &p)
{
	static OmnSPtr<OmnTestBaseA> ptr;

	ptr = p;
	return;
}


OmnTestDerivedFromA::OmnTestDerivedFromA(const int m, OmnTestDerivedFromA *&ptr)
:
mMember3(m)
{
	ptr = this;
	mObjectsCreated++;
}


OmnTestDerivedFromA::~OmnTestDerivedFromA()
{
	sgObjDelFlag = true;
	mObjectsDeleted++;
}


OmnTestBase1::~OmnTestBase1()
{
	sgTestBase1Deleted++;
}

OmnTestBase2::~OmnTestBase2()
{
	sgTestBase2Deleted++;
}

OmnTestDerivedObj::~OmnTestDerivedObj()
{
	sgTestDerivedObjDeleted++;
}

