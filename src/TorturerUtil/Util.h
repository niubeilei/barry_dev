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
// Modification History:
// 04/07/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TorturerUtil_Util_h
#define AOS_TorturerUtil_Util_h

#include "Porting/GetTime.h"
#include "Util/String.h"

enum AosMakefileType
{
	eAosMakefileType_Exe,
	eAosMakefileType_Lib,
	eAosMakefileType_ExeWithCli
};

enum AosCodeGenMainType
{
	eAosCodeGenMainType_Normal, 
	eAosCodeGenMainType_Normal_with_Cli, 
	eAosCodeGenMainType_ApiTorturer
};

extern OmnString aos_get_file_declaimer();
extern OmnString aos_get_header_guard(const OmnString &srcDirName, 
			const OmnString &testDirName, 
			const OmnString &className);

class AosTorturerUtil
{
public:
	static bool createMakefile(const OmnString &srcDir, 
						const OmnString &testDir,
						const OmnString &targetName,
						const bool force, 
						const AosMakefileType type);
	static bool createMain(const OmnString &srcDir, 
						const OmnString &testDir,
						const OmnString &targetName,
						const AosCodeGenMainType type,
						const bool force);
	static bool createTestSuite(const OmnString &dirname, 
						const OmnString &srcdir, 
						const OmnString &testerDirname, 
						const OmnString &torturerName, 
						const OmnString &torturerDesc, 
						const OmnString &includes, 
						const OmnString &testers, 
						const bool force);
	static bool createTestSuiteCpp(const OmnString &dirname, 
						const OmnString &srcdir, 
						const OmnString &testerDirname, 
						const OmnString &torturerName, 
						const OmnString &torturerDesc, 
						const OmnString &includes, 
						const OmnString &testers, 
						const bool force);
	static bool createTestSuiteHeader(const OmnString &dirname, 
						const OmnString &torturerName,
						const OmnString &srcdir,
						const OmnString &testerDir,
						const bool force);
};

#endif

