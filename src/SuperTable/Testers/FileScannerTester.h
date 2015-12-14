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
// 2013/10/29:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SuperTable_Testers_FileScannerTester_h
#define Aos_SuperTable_Testers_FileScannerTester_h

#include "Tester/TestPkg.h"
#include "Thread/Thread.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObjImp.h"
#include "Util/RCObject.h"
#include "Util/Tester/Ptrs.h"
#include "Util/SmtIdxVList.h"


class OmnTestMgr;


class AosFileScannerTester : public OmnTestPkg, public OmnThreadedObj
{
private:

public:
	AosFileScannerTester();
	~AosFileScannerTester() {}

	virtual bool		start();
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);

private:
	bool basic();
};
#endif

