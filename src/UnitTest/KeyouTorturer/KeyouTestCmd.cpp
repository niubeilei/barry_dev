////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KeyouTestCmd.cpp
// Description:
//	An XML Item is expressed by this class as:
//		Tag
//		Tag Contents (a string)   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/KeyouTorturer/KeyouTestCmd.h"

OmnString AosKeyouTestCmd::eFtpHandshakeCmds[AosKeyouTestCmd::eFtpHandshakeTimes] =
{
	"USER lxx\r\n",
	"PASS lixx1234\r\n",
	"QUIT\r\n",
	"",
	""
};
	
OmnString AosKeyouTestCmd::eFtpHandshakeResps[AosKeyouTestCmd::eFtpHandshakeTimes] =
{
	"220 (vsFTPd 2.0.5)\r\n",
	"331 Please specify the password.\r\n",
	"230 Login successful.\r\n",
	"221 Goodbye.\r\n",
	""
};
	




