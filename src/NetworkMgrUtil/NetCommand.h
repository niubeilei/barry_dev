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
// 05/01/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NetworkMgrUtil_NetCommand_h
#define Aos_NetworkMgrUtil_NetCommand_h

#include "Util/String.h"

#define AOSCOMMAND_RESTARTJOB					"restartjob"
#define AOSCOMMAND_STARTJOB						"startjob"
#define AOSCOMMAND_PAUSEJOB						"pausejob"
#define AOSCOMMAND_STOPJOB						"stopjob"
#define AOSCOMMAND_FAILJOB						"failjob"
#define AOSCOMMAND_FINISHJOB					"finishjob"
#define AOSCOMMAND_SWITCHJOBMASTER				"switchjobmaster"

#define AOSCOMMAND_STARTTASK					"starttask"
#define AOSCOMMAND_PAUSETASK					"pausetask"
#define AOSCOMMAND_STOPTASK						"stoptask"
#define AOSCOMMAND_FAILTASK						"failtask"
#define AOSCOMMAND_FINISHTASK					"finishtask"


class AosNetCommand
{
public:
	enum E
	{
		eInvalid,

		eReStartJob,
		eStartJob,
		ePauseJob,
		eStopJob,
		eFailJob,
		eFinishJob,
		eSwitchJobMaster,

		eStartTask,
		ePauseTask,
		eStopTask,
		eFailTask,
		eFinishTask,
		
		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static E toEnum(const OmnString &name);
	static bool init();
	static bool addName(const OmnString &name, const E code);
};
#endif

