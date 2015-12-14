////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DirDescTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/DirDescTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/DirDesc.h"

bool OmnDirDescTester::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "Test Default Constructor";
	mTcNameRoot = "DirDesc-DFT-CTR";
	OmnString dirName = "~/AOS/src/Util";
	OmnDirDesc dd(dirName);
    OmnScreen << dd.getDirName() << endl;

	OmnString fname;
	bool status = dd.getFirstFile(fname);
	if (status) 
	{
		while (dd.getNextFile(fname))
		{
			cout << dd.numObjs() << " file: " << fname.data() << endl;
		}
		status = dd.close();
		OmnCreateTc << (status) << endtc;
	}

	// Test NonExist Dir
	OmnBeginTest << "Test NonExist Dir";
	mTcNameRoot = "DirDesc-NonExist-Dir";
	OmnString dirName1 = "~/AOS/src/Util1234";
	OmnDirDesc dd1(dirName1);
	status = dd1.getFirstFile(fname);
	OmnCreateTc << (status==false) << endtc;

	// Test Empty Dir
	/*
	OmnBeginTest << "Test Empty Dir";
	mTcNameRoot = "DirDesc-Empty-Dir";
	OmnString dirName2 = "/home/jgu/empty";
	OmnDirDesc dd2(dirName2);
	status = dd2.getFirstFile(fname);
	OmnCreateTc << (status) << endtc;
	OmnCreateTc << (fname == "/home/jgu/empty/.") << endtc;
	status = dd2.getNextFile(fname);
	OmnCreateTc << (fname == "/home/jgu/empty/..") << endtc;
	status = dd2.getNextFile(fname);
	OmnCreateTc << (status==false) << endtc;
	*/

	return true;
}

