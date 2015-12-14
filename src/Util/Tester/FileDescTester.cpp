////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileDescTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/FileDescTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/FileDesc.h"

bool OmnFileDescTester::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "Test Default Constructor";
	mTcNameRoot = "String-DFT-CTR";
#ifdef OMN_PLATFORM_UNIX
    OmnString filename = "../Util/FileDesc.h";
#else
	OmnString filename = "d:\\Development\\SSN\\src\\Util\\FileDesc.h";
#endif
	OmnFileDesc fd(filename);
	cout << fd.toString().data() << endl;
	OmnCreateTc << (fd.getName() == "FileDesc.h") << endtc;
#ifdef OMN_PLATFORM_UNIX
	OmnCreateTc << (fd.getPath() == "/home/jgu/development/SSN/src/Util") << "Actural: " << fd.getPath() << endtc;
#else    
	OmnCreateTc << (fd.getPath() == "d:\\Development\\SSN\\src\\Util") << "Actural: " << fd.getPath() << endtc;
#endif


	//
	// Test getFileName(OmnString) 
	//
	OmnBeginTest << "Test getFileName";
	mTcNameRoot = "FileDesc-getFileName";
#ifdef OMN_PLATFORM_UNIX
	OmnString fullname = "../../../../development/./SSN/./src/Util/../Util/Tester/FileDescTester.h";
#else    
	OmnString fullname = "\\Development\\SSNProject\\..\\SSN\\.\\src\\Util\\..\\Util/Tester\\FileDescTester.h";
#endif
	OmnString sfilename = OmnFileDesc::getFileName(fullname); 
  	OmnCreateTc << (sfilename == "FileDescTester.h") << endtc;

 
	//
	// Test removeDotDot(OmnString) 
	//
	OmnString spath = OmnFileDesc::removeDotDot(fullname);
#ifdef OMN_PLATFORM_UNIX
	OmnCreateTc << (spath == "/home/jgu/development/SSN/src/Util/Tester/FileDescTester.h") << endtc;
#else    
	OmnCreateTc << (spath == "\\Development\\SSN\\src\\Util/Tester\\FileDescTester.h") << endtc;
#endif
 
 	//
	// Test getAbsoluteName() 
	//
	OmnBeginTest << "Test getAbsoluteName";
	mTcNameRoot = "FileDesc-getAbsoluteName";
	 
	//not exist file
#ifdef OMN_PLATFORM_UNIX
	OmnString name = "../DevLib/SSNData/lib/Tester.lib";
#else    
	OmnString name = "..\\DevLib\\SSNData\\lib\\Tester.lib";
#endif
	OmnString path, fname;
/*
	OmnLL size;
	OmnFileDesc::getAbsoluteName(name,path,fname,size,exist);

#ifdef OMN_PLATFORM_UNIX
	name = "../../DevLib/SSNData/lib";
#else    
	name = "..\\..\\DevLib\\SSNData\\lib";
#endif
	OmnFileDesc::getAbsoluteName(name,path,fname,size,exist);

#ifdef OMN_PLATFORM_UNIX
	name = "../../../../development/SSN/src/Util/FileDesc.h";
#else    
	name = "..\\..\\..\\Development\\SSN\\src\\Util\\FileDesc.h";
#endif
	OmnFileDesc::getAbsoluteName(name,path,fname,size,exist);

	//exist dir 
#ifdef OMN_PLATFORM_UNIX
	name = "../../../../development/SSN/src/Util";
#else    
	name = "..\\..\\..\\Development\\SSN\\src\\Util";
#endif
	OmnFileDesc::getAbsoluteName(name,path,fname,size,exist);
	//OmnCreateTc << (exist) << endtc;
*/

	return true;
}
