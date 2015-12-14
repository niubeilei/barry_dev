////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: JNS9054CardTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_JNS9054Card_Tester_JNS9054CardTestSuite_h
#define Omn_JNS9054Card_Tester_JNS9054CardTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnJNS9054CardTestSuite {
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnJNS9054CardTestSuite() {}


	static OmnTestSuitePtr		getSuite();
};
#endif

