////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ProcUtil.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/ProcUtil.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Util/File.h"
#include "Util/StrParser.h"

#include <sys/types.h>
#include <unistd.h>



int
OmnProcUtil::getProcId(const OmnString &name)
{
	// 
	// It uses "ps -efa | grep 'name' > tmp12354234
	//
	// The results contain all the processes (one process per line)
	// The second field is the process number. 
	//
	// It assumes the following:
	// 1. The caller has the write right in the current direction.
	// 2. The caller has the right to kill the process. 
	//
	OmnString filename = "tmp12354234";
	OmnString command = "ps -efa | grep ";
	command << name << " > " << filename;

	system(command.data());

	OmnString deleteCommand = "rm ";
	deleteCommand << filename;

	OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file.isGood())
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "Failed to create file: " << filename << enderr;

		system(deleteCommand.data());
		return -1;
	}

	while (1)
	{
		bool finished = false;
		OmnString line = file.getLine(finished);
		if (line.length() <= 0)
		{
			system(deleteCommand.data());
			return -1;
		}

		//
		// The line should be:
		//
		// ding 8202 8201 0 13:24 pts/0   00:00:00 Spnr.exe -s -c 
		//
		// The 8th word should be the program name. 
		//
		OmnStrParser1 parser(line, " ", false, false);

		parser.nextWord("");				// 1st word
		OmnString word2 = parser.nextWord("");	// 2nd word
		parser.nextWord("");				// 3rd word
		parser.nextWord("");				// 4th word
		parser.nextWord("");				// 5th word
		parser.nextWord("");				// 6th word
		parser.nextWord("");				// 7th word
		OmnString word8 = parser.nextWord("");	// 8th word

		if (word8 != name)
		{
			continue;
		}

		int pid = ::atoi(word2.data());
		if (pid <= 0)
		{
			OmnWarn << OmnErrId::eWarnProgramError
				<< "Failed to retrieve process ID: " 
				<< line << " for process: " << name << enderr;
			system(deleteCommand.data());
			return -1;
		}

		return pid;
	}

	system(deleteCommand.data());
	return -1;
}


bool
OmnProcUtil::killProc(const OmnString &name)
{
	int pid = getProcId(name);
	if (pid <= 0)
	{
		return false;
	}

	OmnString command = "kill -9 ";
	command << pid;

	OmnTrace << "To run command: " << command << endl;
	system(command.data());

	return true;
}


bool
OmnProcUtil::isProcRunning(const OmnString &name)
{
	int pid = getProcId(name);
	return pid > 0;
}

