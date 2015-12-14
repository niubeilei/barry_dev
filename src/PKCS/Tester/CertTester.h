////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CertTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCSTester_CertTester_h
#define Omn_PKCSTester_CertTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"


class AosCertTester : public OmnTestPkg
{
private:

public:
	AosCertTester()
	{
		mName = "AosCertTester";
	}
	~AosCertTester() {}

	virtual bool		start();

private:
	bool	certDecodeTest();
};
#endif

