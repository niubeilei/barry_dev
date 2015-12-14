////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: InitialJNS9054Card.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_JNS9054Card_Tester_InitialJNS9054Card_h
#define Omn_JNS9054Card_Tester_InitialJNS9054Card_h

#include "Tester/TestPkg.h"

class AosInitialJNS9054Card : public OmnTestPkg
{
private:

public:
	AosInitialJNS9054Card(const OmnTestMgrPtr &testMgr);
	~AosInitialJNS9054Card() {}

	virtual bool start();

private:
	bool initiate_JNS9054Card();

};

#endif

