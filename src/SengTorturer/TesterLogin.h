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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_LoginTester_h
#define AosSengTester_LoginTester_h

#include "SengTorUtil/SengTester.h"
#include "SengTorUtil/StUtil.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosLoginTester : virtual public AosSengTester
{
private:
	int 	mUseCidLoginWeight;
	int		mValidUserWeight;
	int		mUserRightPasswdWeight;
	int		mUserUsernameWeight;

public:
	enum 
	{
		eNumLogin = 10,
		eMinCtnrObjidLen = 1,
		eMaxCtnrObjidLen = 10,
		eMinUsernameLen = 1,
		eMaxUsernameLen = 10,
		eMinPasswdLen = 1,
		eMaxPasswdLen = 10
	};
	AosLoginTester(const bool regflag);
	AosLoginTester();
	~AosLoginTester();

	virtual bool test();
	AosSengTesterPtr clone()
			{
				return OmnNew AosLoginTester();
			}

private:
	bool		logoutUser();
};
#endif

