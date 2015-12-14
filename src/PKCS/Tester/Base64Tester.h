////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Base64Tester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCSTester_Base64Tester_h
#define Omn_PKCSTester_Base64Tester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"


class AosBase64Tester : public OmnTestPkg
{
private:

public:
	AosBase64Tester()
	{
		mName = "AosBase64Tester";
	}
	~AosBase64Tester() {}

	virtual bool		start();

private:
	bool	basicTest();
};
#endif

