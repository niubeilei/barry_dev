////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef UtilHASH_UtilTestSuite_h
#define UtilHash_UtilTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosUtilHashTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosUtilHashTestSuite() {}
	~AosUtilHashTestSuite() {}

	static OmnTestSuitePtr		getSuite(const AosXmlTagPtr &config);
};
#endif

