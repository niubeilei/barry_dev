////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjDbTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ObjTester/ObjDbTester.h"

#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


bool OmnObjDbTester::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "Test Default Constructor";
	mTcNameRoot = "ObjDb-DFT-CTR";
	return true;
}

