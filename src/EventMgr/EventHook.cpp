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
// 2011/01/20	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "EventMgr/EventHook.h"

static OmnString sgNames[eAosHookMax+1];

bool AosEventHookInit()
{
	sgNames[eAosHookInvalid] 			= "Invalid";

	sgNames[eAosHookReadAttempt]		= "rdattempt";
	sgNames[eAosHookReadAttemptFailed]	= "rdattemptf";
	return true;
}


OmnString AosEventHookToStr(const AosEventHook code)
{
	if (code > eAosHookInvalid && code < eAosHookMax) return sgNames[eAosHookInvalid];
	return sgNames[code];
}


AosEventHook
AosEventHookToEnum(const OmnString &name)
{
	if (name.length() <= 0) return eAosHookInvalid;
	switch (name.data()[0])
	{
	case 'r':
		 if (name == sgNames[eAosHookReadAttempt]) return eAosHookReadAttempt;
		 if (name == sgNames[eAosHookReadAttemptFailed]) return eAosHookReadAttemptFailed;
		 break;
	
	default:
		 break;
	}

	return eAosHookInvalid;
}

