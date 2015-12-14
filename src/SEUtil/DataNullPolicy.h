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
// 07/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_DataNullPolicy_h
#define Aos_SEUtil_DataNullPolicy_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/ValueDefs.h"
#include "Rundata/Ptrs.h"

#define AOSDATANULL_POLICY_TREAT_AS_ERROR			"error"
#define AOSDATANULL_POLICY_IGNORE_SILENTLY			"ignore"
#define AOSDATANULL_POLICY_RESET					"reset"


class AosStrValueInfo;

class AosDataNullPolicy
{
public:
	enum E
	{
		eInvalid, 

		eTreatAsError,
		eReset,
		eIgnoreSilently,

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static E toEnum(const OmnString &name)
	{
		if (name == AOSDATANULL_POLICY_TREAT_AS_ERROR)		return eTreatAsError;
		if (name == AOSDATANULL_POLICY_IGNORE_SILENTLY)		return eIgnoreSilently;
		if (name == AOSDATANULL_POLICY_RESET)				return eReset;
		return eInvalid;
	}

	static bool handleNullData(
						char *record, 
						const int record_len,
						const AosStrValueInfo &handler,
						const AosRundataPtr &rdata);
};

#endif
