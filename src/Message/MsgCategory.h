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
// 2013/03/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Message_MsgCategory_h
#define Omn_Message_MsgCategory_h

#include "Util/String.h"

class AosMsgCategory
{
public:
	enum E
	{
		eInvalidMsgCategory,
		
		eBuffMsg,
		eQueryMsg,

		eLastValidEntry
	};

	static bool			isValid(const E c) 
	{
		return c > eInvalidMsgCategory  && c < eLastValidEntry;
	}
};
#endif
