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
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MsgProc_MsgProcTypes_h
#define Aos_MsgProc_MsgProcTypes_h

#include "Util/String.h"

#define AOSMSGPROCTYPE_INVALID					"invalid"
#define AOSMSGPROCTYPE_RECEIVER_PROC			"receiverproc"

class AosMsgProcType
{
public:
	enum E
	{
		eInvalid,

		eReceiverProc,

		eMax
	};

	static inline bool isValid(const E type)
	{
		return type > eInvalid && type < eMax;
	}

	static E toEnum(const OmnString &str);
	static OmnString toString(const E code);
	static bool addName(const OmnString &name, const E eid, OmnString &errmsg);
};
#endif



