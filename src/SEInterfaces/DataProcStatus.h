////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataProcStatus_h
#define Aos_SEInterfaces_DataProcStatus_h

#include "Util/String.h"


#define AOSDATAPROCSTATUS_INVALID			"invalid"
#define AOSDATAPROCSTATUS_ERROR				"error"
#define AOSDATAPROCSTATUS_EXIT				"exit"
#define AOSDATAPROCSTATUS_CONTINUE			"continue"
#define AOSDATAPROCSTATUS_EQ				"=="
#define AOSDATAPROCSTATUS_LT				"<"
#define AOSDATAPROCSTATUS_GT				">"

class AosDataProcStatus 
{
public:
	enum E
	{
		eInvalid,

		eEQ,
		eLT,
		eGT,
		eDataRetrieved,
		eRetrievingData,
		eNeedRetrieveData,
		eNoMoreData,
		eOk,
		eRejected,
		eDataTooShort,
		eRecordFiltered,
		eRerunTask,
		eRemoteRerunTask,
		eRollbackTasks,

		eContinue,
		eExit,
		eError,

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static E toEnum(const OmnString &name);
	static OmnString toStr(const E code);
	static bool addName(const OmnString &name, const E code);
	static bool init();
};
#endif
