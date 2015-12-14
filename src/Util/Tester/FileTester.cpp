////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/FileTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"

bool OmnFileTester::start()
{
	// 
	// Test default constructor
	//
	// basicTest();
	testGetFiles();
	testSetContents();
	return true;
}


bool
OmnFileTester::basicTest()
{
	OmnBeginTest << "Test Default Constructor";
	mTcNameRoot = "File-DFT-CTR";
	OmnString filename = "d:\\Development\\SSN\\src\\Util\\File.h";
	OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
	char buff[10000];
	int size = file.readToBuff(100,500,buff);
	buff[500] = 0;
	cout << buff << endl;
	OmnCreateTc << (size>=0) << endtc;

	OmnFile wfile("d:\\Development\\testc.txt",OmnFile::eCreate);
	wfile.append(buff);

	OmnFile cfile("d:\\Development\\test.txt",OmnFile::eAppend);
	cfile.append(buff);

	return true;
}


bool
OmnFileTester::testGetFiles()
{
	/*
	OmnFile ff(AosMemoryCheckerArgsBegin);
	std::list<OmnString> names;
	int nn = ff.getFiles(".", names, true);
	cout << "nn: " << nn << endl;

	std::list<OmnString>::iterator it;
	for (it=names.begin(); it != names.end(); it++)
	{
		cout << "Retrieved File: " << (*it).data() << endl;
	}
	*/
	return true;
}


bool
OmnFileTester::testSetContents()
{
	OmnFile ff("testfile.txt", OmnFile::eReadWrite);
	ff.put(10, "chen ding", 9, true);
	return true;
}

