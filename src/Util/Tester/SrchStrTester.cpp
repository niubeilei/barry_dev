////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SrchStrTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/SrchStrTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/SrchStr.h"

bool OmnSrchStrTester::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "Test SrchStr";
	mTcNameRoot = "SrchStr";
	OmnString filename = "d:\\Development\\SSN\\src\\Util\\SrchStr.h";
	OmnSrchStr srch(filename);
	srch.resetLoop();
	srch.setSrchChar('/');
	srch.setSrchChar('\\');
	OmnString path = srch.trimPrev();
	OmnCreateTc << (path == "d:\\Development\\SSN\\src\\Util") << endtc;
	path.append("\\",1);
	OmnString file = filename - path;
	OmnCreateTc << (file == "SrchStr.h") << endtc;

	OmnString next = srch.next();
	OmnCreateTc << (next == "d:") << endtc;

	OmnString crt = srch.crtValue();
	OmnCreateTc << (crt == "d:") << endtc;

	next = srch.next();
	OmnCreateTc << (next == "d:\\Development") << endtc;
	crt = srch.crtValue();
	OmnCreateTc << (crt == "d:\\Development") <<  endtc;


	OmnString remain = srch.remainingStr();
	OmnCreateTc << (remain == "\\SSN\\src\\Util\\SrchStr.h") << endtc;

	remain = srch.getRemaining();
	OmnCreateTc << (remain == "SSN\\src\\Util\\SrchStr.h") << endtc;

	char nextChar = srch.nextChar();
	OmnCreateTc << (nextChar == 'S') << endtc;

	next = srch.next();
	OmnCreateTc << (next == "d:\\Development\\SSN") << endtc;



	bool status = srch.previous();
	OmnCreateTc << (status) << endtc;
	crt = srch.crtValue();
	OmnCreateTc << (crt == "d:\\Development") << endtc;

	OmnString nextToken = srch.nextToken();
	OmnCreateTc << (nextToken == "SSN") << endtc;


	//
	// Test unix path
	//
	OmnSrchStr srch2("/usr/local/./qt/./../mysql/./bin/../data");
	srch2.resetLoop();
	srch2.setSrchChar('/');
	srch2.setSrchChar('\\');

	while (srch2.hasMore())
	{
		nextToken = srch2.nextToken();
		if (nextToken == "..")
		{
			srch2.removeToken();
			srch2.removeToken();
		}
		else if (nextToken == ".")
		{
			srch2.removeToken();
		}
	}
	crt = srch2.crtValue();
	OmnCreateTc << (crt == "/usr/local/mysql/data") << endtc;
	

	return true;
}
