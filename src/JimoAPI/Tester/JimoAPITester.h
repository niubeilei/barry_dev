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
// 2014/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_Tester_JimoAPITester_h
#define Aos_JimoAPI_Tester_JimoAPITester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class AosJimoAPITester : public OmnTestPkg
{
private:
	int		mTries;

public:
	AosJimoAPITester();
	~AosJimoAPITester() {}
	virtual bool		start();
    bool basicTest();
private:
};
#endif

