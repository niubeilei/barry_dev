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
// 05/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Job_Testers_JobTester_h
#define Aos_Job_Testers_JobTester_h

#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "XmlUtil/XmlTag.h"


class AosJobTester : public OmnTestPkg
{
public:
	AosJobTester();
	~AosJobTester() {};

	virtual bool		start();

	bool		createJobdoc(const AosRundataPtr &rdata);
	OmnString	randomGetJobId();
	OmnString	randomGetTaskId();
	OmnString	randomGetSpliterId();
	OmnString	randomGetTaskStr();
	OmnString	randomGetJobSplitter();
	OmnString	randomGetJobScheduler();

private:
	bool		basicTest();
};
#endif
