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
// 07/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_DataTooShortPolicy_h
#define Aos_SEUtil_DataTooShortPolicy_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/ValueDefs.h"
#include "Rundata/Ptrs.h"

#define AOSDATATOOSHORT_POLICY_TREAT_AS_ERROR			"error"
#define AOSDATATOOSHORT_POLICY_IGNORE_SILENTLY			"ignore"
#define AOSDATATOOSHORT_POLICY_ALIGN_LEFT				"alignleft"
#define AOSDATATOOSHORT_POLICY_ALIGN_RIGHT				"alignright"
#define AOSDATATOOSHORT_POLICY_RESET_RECORD				"reset"
#define AOSDATATOOSHORT_POLICY_CSTR						"cstr"

class AosStrValueInfo;

class AosDataTooShortPolicy
{
public:
	enum E
	{
		eInvalid, 

		eTreatAsError,
		eIgnoreSilently,
		eResetRecord,
		eAlignLeft,
		eAlignRight,
		eCstr,			// Ketty 2014/01/11

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static E toEnum(const OmnString &name)
	{
		if (name == AOSDATATOOSHORT_POLICY_TREAT_AS_ERROR)		return eTreatAsError;
		if (name == AOSDATATOOSHORT_POLICY_IGNORE_SILENTLY)		return eIgnoreSilently;
		if (name == AOSDATATOOSHORT_POLICY_ALIGN_LEFT)			return eAlignLeft;		
		if (name == AOSDATATOOSHORT_POLICY_ALIGN_RIGHT)			return eAlignRight;
		if (name == AOSDATATOOSHORT_POLICY_RESET_RECORD)		return eResetRecord;
		if (name == AOSDATATOOSHORT_POLICY_CSTR)				return eCstr;		// Ketty 2014/01/11
		return eInvalid;
	}

	static bool handleDataTooShort(
        char *&record,
        int &record_len,
        const char *data,
        const int data_len,
        const AosStrValueInfo &handler,
        const AosRundataPtr &rdata);

};

#endif
