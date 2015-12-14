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
// 03/18/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef XmlInterface_Testers_ReqProcTester_h
#define XmlInterface_Testers_ReqProcTester_h

#include "Tester/TestPkg.h"


class AosXmlInterfaceReqProcTester : public OmnTestPkg
{
private:

public:
	AosXmlInterfaceReqProcTester() {mName = "XmlInterfaceTester";}
	~AosXmlInterfaceReqProcTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};
#endif

