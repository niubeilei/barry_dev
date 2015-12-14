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
#ifndef Omn_EventMgr_EventHook_h
#define Omn_EventMgr_EventHook_h

#include "Util/String.h"

enum AosEventHook
{
	eAosHookInvalid,

	eAosHookReadAttempt,
	eAosHookReadAttemptFailed,
	eAosHookReadSuccess,
	eAosHookRetDelObjAttempt,
	eAosHookRetDelObjFailed,
	eAosHookRetDelObjSuccess,
	eAosHookRetVerAttempt,
	eAosHookRetVerAttemptFailed,
	eAosHookCounterByCtnr,
	eAosHookCounterBySdocIDs,
	eAosHookCreateDoc,

	eAosHookMax
};

enum AosHookPoint
{
	eAosHkptReadAttempt1, 
	eAosHkptReadAttemptFailed1,
	eAosHkptReadAttemptFailed2,
	eAosHkptReadAttemptFailed3,
	eAosHkptReadAttemptFailed4,
	eAosHkptReadAttemptFailed5,
	eAosHkptReadAttemptFailed6,
	eAosHkptReadSuccess1,
	eAosHkptRetDelObjAttempt1,
	eAosHkptRetDelObjSuccess1,
	eAosHkptRetDelObjAttemptFailed1,
	eAosHkptRetVerAttempt1,
	eAosHkptRetVerAttemptFailed1,
	eAosHkptCounter1,
	eAosHkptCounter2,
	eAosHkptCreateOrder,
};


extern inline bool AosIsValidHook(const AosEventHook code)
{
	return code > eAosHookInvalid && code < eAosHookMax;
}

extern inline OmnString	AosEventHookToStr(const AosEventHook code);
extern AosEventHook AosEventHookToEnum(const OmnString &name);
#endif

