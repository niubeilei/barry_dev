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
// 07/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_DataTooLongPolicy_h
#define Aos_SEUtil_DataTooLongPolicy_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/ValueDefs.h"

#define AOSDATATOOLONG_POLICY_TREAT_AS_ERROR			"error"
#define AOSDATATOOLONG_POLICY_IGNORE_SILENTLY			"ignore"
#define AOSDATATOOLONG_POLICY_TRIMLEFT					"trimleft"
#define AOSDATATOOLONG_POLICY_TRIMRIGHT					"trimright"
#define AOSDATATOOLONG_POLICY_RESET_RECORD				"resetrecord"


class AosStrValueInfo;

class AosDataTooLongPolicy
{
public:
	enum E
	{
		eInvalid, 

		eTreatAsError,
		eResetRecord,
		eIgnoreSilently,
		eTrimLeft,
		eTrimRight,

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static E toEnum(const OmnString &name)
	{
		if (name == AOSDATATOOLONG_POLICY_TREAT_AS_ERROR)		return eTreatAsError;
		if (name == AOSDATATOOLONG_POLICY_IGNORE_SILENTLY)		return eIgnoreSilently;
		if (name == AOSDATATOOLONG_POLICY_TRIMLEFT)				return eTrimLeft;		
		if (name == AOSDATATOOLONG_POLICY_TRIMRIGHT)			return eTrimRight;
		if (name == AOSDATATOOLONG_POLICY_RESET_RECORD)			return eResetRecord;
		return eInvalid;
	}

	static bool handleDataTooLong(
					char *&record, 
					int &record_len,
					const char *data,
					const int64_t &data_len,
					const AosStrValueInfo &handler,
					const AosRundataPtr &rdata);
};

#endif
