////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 08/06/2009:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_StrSplitTester_h
#define Omn_TestUtil_StrSplitTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"


class OmnTestMgr;


class AosStrSplitTester : public OmnTestPkg
{
private:
	enum
	{
		eMaxNumWords = 100,
		eMaxWordLen = 15,
		eMaxSepLength = 5
	};

public:
	AosStrSplitTester() {mName = "AosStrSplitTester";}
	~AosStrSplitTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool 	testSplitByChar();
	bool 	testSplitBySubstr();
	OmnString createSeparator();
};


#endif

