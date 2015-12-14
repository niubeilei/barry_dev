////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CertVerify_Tester_OcspTester_h
#define Omn_CertVerify_Tester_OcspTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"

class AosOcspTester : public OmnTestPkg
{
public:
	AosOcspTester()
	{
		mName = "AosOcspTester";
	}
	~AosOcspTester() {}
	virtual bool	start();

private:
	bool testOcsp_sendSerialnum();
    bool testOcsp_retriveResult();

};
#endif

