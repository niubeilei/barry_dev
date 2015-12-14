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
// 01/04/2008: Created by Allen Xu 
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Torturer_TorUtil_h
#define Aos_Torturer_TorUtil_h

#include <string>

class AosTorturerLogType
{
public:
	enum E
	{
		eNoLog = 0, 
		eOnScreen, 
		eFile,

		eLastEntry
	};

	static std::string enum2Str(const AosTorturerLogType::E type);
	static AosTorturerLogType::E str2Enum(const std::string& name);
};

enum AosMakefileType
{
	eAosMakefileType_Exe,
	eAosMakefileType_Lib
};

enum AosCodeGenMainType
{
	eAosCodeGenMainType_Normal, 
	eAosCodeGenMainType_CliTorturer,
	eAosCodeGenMainType_FuncTorturer
};


class AosTorturerUtil
{
public:
	static std::string aos_get_file_declaimer();
	static std::string aos_get_header_guard(const std::string &srcDirName, 
						const std::string &testDirName, 
						const std::string &className);
	static bool createMakefile(const std::string &srcDir, 
						const std::string &testDir,
						const std::string &targetName,
						const bool force, 
						const AosMakefileType type);
	static bool createMain(const std::string &srcDir, 
						const std::string &testDir,
						const std::string &targetName,
						const AosCodeGenMainType type,
						const bool force);
};


#endif
