////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CrlTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PkcsTester_crlTester_h
#define Omn_PkcsTester_crlTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class OmnCrlTester : public OmnTestPkg
{
private:

public:
	OmnCrlTester()
	{
		mName = "OmnCrlTester";
	}
	~OmnCrlTester() {}

	virtual bool		start();

private:
};
#endif

