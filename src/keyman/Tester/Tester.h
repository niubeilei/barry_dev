////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: Tester.h
//// Description:
////   
////
//// Modification History:
//// 11/21/2006   Created by Harry Long
////
//////////////////////////////////////////////////////////////////////////////
#ifndef Omn_KeymanTester_h
#define Omn_KeymanTester_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosKeymanTorturerSuite
{
	private:

	public:
		        AosKeymanTorturerSuite() {}
				~AosKeymanTorturerSuite() {}

				static OmnTestSuitePtr          getSuite();
};
#endif

