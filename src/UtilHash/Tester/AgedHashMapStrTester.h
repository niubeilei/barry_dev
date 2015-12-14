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
// 01/20/2012:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilHash_Testers_AgedHashMapStrTester_h
#define Omn_UtilHash_Testers_AgedHashMapStrTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"


class OmnTestMgr;


class AosAgedHashMapStrTester : public OmnTestPkg
{
private:

public:
	AosAgedHashMapStrTester() {mName = "AosAgedHashMapStrTester";}
	~AosAgedHashMapStrTester() {}

	virtual bool		start();

private:
};


#endif

