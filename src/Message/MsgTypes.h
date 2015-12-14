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
// 2013/03/03 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_Message_MsgTypes_h
#define Omn_Message_MsgTypes_h

#include "Util/String.h"

class AosMsgType
{
public:
	enum 
	{
		eBuffMsgPoison1 = 0x03624364,
		eBuffMsgPoison2 = 0x36467013
	};

	enum E
	{
		eInvalidMsgType,

		eBuffMsg,

		eLastMsgType
	};

	static bool	isValid(const E code) 
	{
		return code > eInvalidMsgType && code < eLastMsgType;
	}
	
	static bool setBuffMsgHead(const AosBuffPtr &buff);
	static bool verifyBuffMsg(const AosBuffPtr &buff);

	static int getNumMsgs() {return eLastMsgType;}
};
#endif
#endif
