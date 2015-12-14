////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RCObjectTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_RCObjectTester_h
#define Omn_TestUtil_RCObjectTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class OmnRCObjectTester : public OmnTestPkg
{
private:
	int			mNumTries;

public:
	OmnRCObjectTester(const int numTries = 1000000)
		:
	mNumTries(numTries)
	{
		mName = "OmnRCObjectTester";
	}
	~OmnRCObjectTester() {}

	virtual bool		start();

private:
	bool testStackObjects();
	bool testHeapObjects();
	bool testDerivedObjects();
};





class TestA;     
void func1(const OmnSPtr<TestA> &p);

class OmnTestBaseA : public OmnRCObject
{
	OmnDefineRCObject;

protected:
	int		mOmnTestMember1;
	bool	mOmnTestMember2;

public:
	OmnTestBaseA();
	~OmnTestBaseA();

	static int		mObjectsCreated;
	static int		mObjectsDeleted;
	int func2();
};

class OmnTestDerivedFromA : public OmnTestBaseA
{
private:
	int		mMember3;
	
public:
	OmnTestDerivedFromA(const int m, OmnTestDerivedFromA *&ptr);
	~OmnTestDerivedFromA();
};




class OmnTestBase1 : public virtual OmnRCObject
{
private:
	char	c[0x10];

public:
	~OmnTestBase1();

	void printthis1() 
	{
		OmnTrace << "OmnTestBase1 this = " << this << endl;
	}

	OmnTestBase1 *	getBase1This() {return this;}
};


class OmnTestBase2 : public virtual OmnRCObject
{
private:
	char	c[0x10];

public:
	~OmnTestBase2();

	void printthis2() 
	{
		cout << "OmnTestBase2 this = " << this << endl;
	}

	OmnTestBase2 *	getBase2This() {return this;}
};


class OmnTestMember1 
{
private:
	char	c[0x10];

public:
	void printthism1() 
	{
		cout << "OmnTestMember1 this = " << this << endl;
	}
};


class OmnTestMember2 
{
private:
	char	c[0x10];

public:
	void printthism2() 
	{
		cout << "OmnTestMember2 this = " << this << endl;
	}
};


class OmnTestDerivedObj : public OmnTestBase1, public OmnTestBase2 
{
	OmnDefineRCObject;

	OmnTestMember1 m1;
	OmnTestMember2 m2;

public:
	~OmnTestDerivedObj();

	void printthis() 
	{
		cout << "OmnTestDerivedObj this = " << this << endl;
		printthis1();
		printthis2();

		m1.printthism1();
		m2.printthism2();
	}
};



#endif

