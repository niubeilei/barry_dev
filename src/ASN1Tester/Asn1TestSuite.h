////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Asn1TestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Asn1Tester_Asn1TestSuite_h
#define Omn_Asn1Tester_Asn1TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnAsn1TestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnAsn1TestSuite();
	~OmnAsn1TestSuite();

	static OmnTestSuitePtr		getSuite();
};
#endif

