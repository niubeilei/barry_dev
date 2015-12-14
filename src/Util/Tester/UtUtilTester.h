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
#ifndef Omn_TestUtil_UtUtilTester_h
#define Omn_TestUtil_UtUtilTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"


class OmnTestMgr;


class AosUtUtilTester : public OmnTestPkg
{
private:

public:
	AosUtUtilTester() {mName = "AosUtUtilTester";}
	~AosUtUtilTester() {}

	virtual bool		start();

private:
	bool	basic();
	bool	testAosGetStrDiff();
	bool	testAosFindFirstIndex();
};


#endif

