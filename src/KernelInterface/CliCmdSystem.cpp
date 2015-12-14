////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCmdSystem.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI
#include "KernelInterface/CliCmdSystem.h"

#include "Alarm/Alarm.h"
#include "aos/KernelApiStruct.h"
#include "aos/aosKernelAlarm.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelAPI/KernelAPI.h"
#include "Util/StrParser.h"
#include "Util/File.h"

#define eAosDefaultConfigFile "/usr/aos/data/.config"

#define aosRunSaveConfig(moduleId) \
	data.mOprId = moduleId; \
	ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data)); \
	if (ret) \
	{ \
		theRslt << "Failed to save config: " \
		<< moduleId << ": " << OmnKernelApi::getErr(ret); \
		status = false;\
	} \
	else \
	{ \
		file << data.mStr; \
	} 

/*
bool
OmnCliSystemLoadConfig::run(const OmnString &parms, OmnString &theRslt)
{
	// 
	// system load config <filename>
	//
	bool status = true;
	OmnString rslt;
	OmnStrParser parser(parms);
	OmnString filename = parser.nextWord();
	if (filename.length() <= 0)
	{
		// 
		// No name is specified. Use the default configuration file.
		//
		filename = eAosDefaultConfigFile;
	}

	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		rslt << "Failed to open the file: " << filename;
		return false;
	}

	bool finished;
	while (1)
	{
		OmnString line = file.getLine(finished);
		if (line.length() <= 0)
		{
			if (finished)
			{
				return status;
			}

			continue;
		}

		if (!OmnCliProc::getSelf()->runCli(line, rslt))
		{
			status = false;
			OmnAlarm << "Failed the command: " << line
				<< ". The result: " << rslt << enderr;

			if (theRslt.length() < 5000)
			{
				theRslt << "Failed the command: " << line << ", " 
					<< rslt << ". ";
			}
		}
	}

	return true;
}


bool
OmnCliSystemSaveConfig::run(const OmnString &parms, OmnString &theRslt)
{
	// 
	// system save config <filename>
	//
	bool status = true;
	OmnString rslt;
	OmnStrParser parser(parms);
	OmnString filename = parser.nextWord();
	if (filename.length() <= 0)
	{
		// 
		// No name is specified. Use the default configuration file.
		//
		filename = eAosDefaultConfigFile;
	}

	OmnFile file(filename, OmnFile::eCreate);
	if (!file.isGood())
	{
		rslt << "Failed to open the file: " << filename;
		return false;
	}

//	aosKernelApi_str10k data;
//	int ret;
//	aosRunSaveConfig(eAosKid_BridgeSaveConfig);

	return status;
}
*/
#endif

