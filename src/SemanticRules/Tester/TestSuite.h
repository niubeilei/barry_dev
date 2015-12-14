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
// 12/06/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SemanticRules_Tester_TestSuite_h
#define Omn_SemanticRules_Tester_TestSuite_h

#include "Tester/Ptrs.h"


class AosSemanticRuleTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSemanticRuleTestSuite() {}
	~AosSemanticRuleTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

