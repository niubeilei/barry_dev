////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CharPTreeTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_CharPTreeTester_h
#define Omn_TestUtil_CharPTreeTester_h

#include "aos/KernelEnum.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "aosUtil/CharPtree.h"


struct OmnCharPtreeTestData
{
	char	mKey[eAosMaxChar - eAosMinChar + 1];
	int		mValue;
	bool	mInsertFlag;
	bool	mIsDeleted;
};



class OmnCharPTreeTester : public OmnTestPkg
{
private:
	enum
	{
		eTries = 3000, 
	};

public:
	OmnCharPTreeTester()
	{
		mName = "OmnCharPTreeTester";
	}
	~OmnCharPTreeTester() {}

	virtual bool		start();

private:
	bool	addEntry(aosCharPtree * tree, 
					const char *key, 
					void *value, 
					bool override);
	bool	removeEntry(aosCharPtree *tree, 
					const char *key, 
					bool expected);
	bool	randomTester();
	bool	verify(struct aosCharPtree * tree, 
				   OmnCharPtreeTestData *data, 
				   int numEntries);
	bool	verify(struct aosCharPtree * tree, 
				   char *theKey,
				   OmnCharPtreeTestData *data, 
				   int numEntries);
	bool	isKeyInList(aosCharPtree *tree, 
								OmnCharPtreeTestData *data, 
								const int numEntries, 
								const char *key, 
								int &index);
	bool	specialCases();
	bool	specialTest1();
	bool	specialTest2();
	bool	specialTest3();
	bool	specialTest4();
	bool	specialTest5();
	bool	specialTest6();
	bool	specialTest7();
	bool	specialTest8();
	bool	specialTest9();
	bool	specialTest10();
	bool	specialTest11();
	bool	specialTest12();
	bool	specialTest13();
	bool	specialTest14();
	bool	specialTest14_1(aosCharPtree *tree);
	bool	specialTest14_2(aosCharPtree *tree);
	bool	specialTest14_3(aosCharPtree *tree);
	bool	specialTest15();
	bool	specialTest16();
};

#endif

