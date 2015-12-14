////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RecordTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SslTester_SslRecordTester_h
#define Omn_SslTester_SslRecordTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"


class AosSslRecordTester : public OmnTestPkg
{
private:

public:
	AosSslRecordTester()
	{
		mName = "AosSslRecordTester";
	}
	~AosSslRecordTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	oneRecordMultiMsg();
};
#endif
