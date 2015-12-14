////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KtcpvsTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Ktcpvs_KtcpvsTesterSuite_h
#define Omn_Ktcpvs_KtcpvsTesterSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnKtcpvsTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnKtcpvsTestSuite() {}
	~OmnKtcpvsTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

