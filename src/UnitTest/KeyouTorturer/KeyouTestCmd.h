////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KeyouTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_KeyouTorturer_KeyouTestCmd_h
#define Omn_UnitTest_KeyouTorturer_KeyouTestCmd_h

#include "Util/String.h"
#include "Util/IpAddr.h"

class AosKeyouTestCmd
{

public:
	enum
	{
		eFtpHandshakeTimes = 5,
	};
	
	static OmnString eFtpHandshakeCmds[eFtpHandshakeTimes];
	static OmnString eFtpHandshakeResps[eFtpHandshakeTimes];
};

#endif

