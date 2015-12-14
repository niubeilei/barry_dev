////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilCommTester_UdpTester_h
#define Omn_UtilCommTester_UdpTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/BasicTypes.h"
#include "UtilComm/Ptrs.h"


class OmnTestMgr;

class OmnUdpTester : public OmnTestPkg
{
	OmnDefineRCObject;

private:

public:
	OmnUdpTester() {}
	~OmnUdpTester() {}

	virtual bool		start();
};
#endif

