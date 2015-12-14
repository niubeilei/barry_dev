////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HttpTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AppProc_http_proc_Tester_HttpTestSuite_h
#define Aos_AppProc_http_proc_Tester_HttpTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class HttpTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	HttpTestSuite() {}
	~HttpTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

