////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 01/06/2013	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapMgr_Tester_MultiCondTester_h
#define Aos_BitmapMgr_Tester_MultiCondTester_h

#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"

class OmnTestMgr;

class AosMultiCondTester : public OmnTestPkg
{
public:
private:
public:
	AosMultiCondTester();
	~AosMultiCondTester(); 

public:
	bool 				start();
private:
	bool 				basicTest();
};
#endif
