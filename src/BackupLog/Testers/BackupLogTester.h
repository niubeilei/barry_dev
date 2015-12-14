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
// 2013/05/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BackupLog_Testers_BackupLogTester_h
#define Aos_BackupLog_Testers_BackupLogTester_h

#include "BackupLog/Ptrs.h"


class AosBackupLogTester : public OmnTestPkg
{
private:

public:
	AosBackupLogTester();
	~AosBackupLogTester() {}

	virtual bool		start();

private:
	bool	init();
	bool	basicTest();
	bool	config();
};
#endif

