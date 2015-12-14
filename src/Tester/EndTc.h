////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: EndTc.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_EndTc_h
#define OMN_Tester_EndTc_h

#include "Tester/Testcase.h"


class OmnTester
{
	OmnTestcase		mActiveTc;

public:
	OmnTester();
	~OmnTester();

	OmnTester & operator << (const OmnTestcase &rhs);
	OmnTester & operator << (const OmnString &rhs);
	OmnTester & operator << (const int rhs);
};

#endif