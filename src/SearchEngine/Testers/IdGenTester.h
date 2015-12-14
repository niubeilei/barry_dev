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
// 2009/10/09	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SearchEngine_Testers_IdGenTester_h
#define Aos_SearchEngine_Testers_IdGenTester_h

#include "Tester/TestPkg.h"


class AosIdGenTester : public OmnTestPkg
{
private:

public:
	AosIdGenTester() {mName = "OmnSIVTester";}
	~AosIdGenTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	testDocId();
};


#endif

