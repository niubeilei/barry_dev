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
// 2011/01/15	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlServer_Testers_HtmlServerTester_h
#define Aos_HtmlServer_Testers_HtmlServerTester_h

#include "Util/Opr.h"
#include "HtmlServer/Ptrs.h"
#include "Tester/TestPkg.h"


class AosHtmlServerTester : public OmnTestPkg
{
public:

private:

public:
	AosHtmlServerTester();
	~AosHtmlServerTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};


#endif

