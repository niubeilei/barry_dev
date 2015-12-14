////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: JNS9054CardTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_JNS9054Card_Tester_JNS9054CardTester_h
#define Omn_JNS9054Card_Tester_JNS9054CardTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"

class AosJNS9054CardTester : public OmnTestPkg
{
public:
	AosJNS9054CardTester()
	{
		mName = "AosJNS9054CardTester";
	}
	~AosJNS9054CardTester() {}
	virtual bool	start();

private:
	bool testJNS9054Card_Start();
    bool testJNS9054Card_Retrive();

};
#endif

