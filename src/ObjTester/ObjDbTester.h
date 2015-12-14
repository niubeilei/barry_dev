////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjDbTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ObjTester_ObjDbTester_h
#define Omn_ObjTester_ObjDbTester_h

#include "Tester/TestPkg.h"


class OmnTestMgr;

class OmnObjDbTester : public OmnTestPkg
{
public:
	OmnObjDbTester() {mName = "OmnObjDbTester";}
	~OmnObjDbTester() {}

	virtual bool		start();
};
#endif

