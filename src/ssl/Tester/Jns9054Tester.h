////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Jns9054Tester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SslTester_Jns9054Tester_h
#define Omn_SslTester_Jns9054Tester_h

#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "ssl/Tester/Ptrs.h"
//#include "ssl/aosSslProc.h"
#include "ssl/SslCommon.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"


class AosJns9054Tester : public OmnTestPkg
{
private:

public:
	AosJns9054Tester()
	{
		mName = "AosJns9054Tester";
	}
	~AosJns9054Tester() {}

	virtual bool		start();

private:
	bool	basicTest();
};
#endif

