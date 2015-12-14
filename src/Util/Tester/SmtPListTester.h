////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmtPListTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_SmtPListTester_h
#define Omn_TestUtil_SmtPListTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"


class OmnTestMgr;


class OmnSmtPListTester : public OmnTestPkg
{
private:

public:
	OmnSmtPListTester() {mName = "OmnSmtPListTester";}
	~OmnSmtPListTester() {}

	virtual bool		start();

private:
	bool	testDefaultConstructor();
	bool	testAdd();
};


#endif

