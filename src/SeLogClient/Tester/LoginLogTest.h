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
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SeLogClient_Tester_LoginLogTest_h
#define Aos_SeLogClient_Tester_LoginLogTest_h 

#include "SeLogClient/Tester/LogReqProc.h"


class AosLoginLogTest : public AosLogReqProc	
{
public:
	AosLoginLogTest(const bool flag);
	AosLoginLogTest();
	~AosLoginLogTest();

	bool proc(const AosRundataPtr &rdata);

private:
	bool	loginOk(const AosRundataPtr &rdata);
	bool	loginWithWrongPasswd(const AosRundataPtr &rdata);
	bool	loginNoPasswd(const AosRundataPtr &rdata);
	bool	loginNoUnameNoPasswd(const AosRundataPtr &rdata);
	bool	loginNoContainer(const AosRundataPtr &rdata);
	bool	retrieveLog(const AosRundataPtr &rdata);
};
#endif
