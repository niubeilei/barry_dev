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
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ShortMsgClt_Tester_ShortMsgCltTester_h
#define Aos_ShortMsgClt_Tester_ShortMsgCltTester_h

#include "Tester/TestPkg.h"
#include "ShortMsgClt/Ptrs.h"
#include "ShortMsgClt/ShortMsgClt.h"


class AosShortMsgCltTester : public OmnTestPkg
{
private:
	enum
	{
		eNormalTries = 100
	};
	AosShortMsgCltPtr 			mShmClt;
public:
	AosShortMsgCltTester();
	~AosShortMsgCltTester() {}

	virtual bool	start();

private:
	bool basicTest();
	bool sendShortMsg(const OmnString &tortuer);
	OmnString getReceiver();
	OmnString getMessage();
};


#endif

