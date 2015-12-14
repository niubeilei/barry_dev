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
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_MsgId_h
#define Omn_Message_MsgId_h

#include "Util/String.h"

class OmnMsgId
{
public:
	enum E
	{
		eInvalidMsgId,
		
		// 
		// !!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!
		// Starting from this point and before eLastValidMsg, they represent
		// valid message IDs. Do not manually assign values to any of them.
		// Any valid messages must be in this range.
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		eSmGetAppInfoReq, 
		eSmGetAppInfoResp,
		eSmRegisterAppNameReq,
		eSmRegisterAppNameResp,
		eSmRegisterAppReq,
		eSmRegisterAppResp,
		eTestCase,

		eBuffMsg,
		eRetrvQueryBlock,
		eBmpQueryFinalRslts,
		eQueryPhase2Req,
		eJimoCallReq,				// Chen Ding, 2014/12/04
		eJimoCallResp,				// Chen Ding, 2014/12/04

		// !!!!!!!!!!!!!!!! End of Valid Message ID section !!!!!!!!!!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		eLastValidEntry
	};

	static OmnString	toStr(E code);
	static E			strToEnum(const OmnString &name);
	static bool			isValid(const E c) 
	{
		return c > eInvalidMsgId && c < eLastValidEntry;
	}
};
#endif
