////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashBinTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AosUtil_Tester_HashBinTester_h
#define Omn_AosUtil_Tester_HashBinTester_h

#include "Tester/TestPkg.h"

struct aosAsnObjId;

class OmnTestMgr;

class AosHashBinTester : public OmnTestPkg
{
public:
	AosHashBinTester();
	~AosHashBinTester();

	virtual bool		start();

private:
	bool	normalTests();
	bool	abnormalTests();
	bool	addSameKey();
	bool	special1();
};
#endif

