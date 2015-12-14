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
// 08/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_TesterNew_GicHtmlTreeSimpleTester_h
#define Aos_GICs_TesterNew_GicHtmlTreeSimpleTester_h

#include "Util/Opr.h"
#include "GICs/Ptrs.h"
#include "Tester/TestPkg.h"


class AosGicHtmlTreeSimpleTester : public OmnTestPkg
{

private:

public:
	AosGicHtmlTreeSimpleTester();
	~AosGicHtmlTreeSimpleTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};
#endif
