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
#ifndef Aos_Torturers_XmlDocTurturer_LogoutTester_h
#define Aos_Torturers_XmlDocTurturer_LogoutTester_h

#include "SengTorUtil/SengTester.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/OmnNew.h"


class AosLogoutTester : virtual public AosSengTester
{
private:
	enum
	{
		eLogoutTryWeight = 5,
		eMinSsidLen = 20,
		eMaxSsidLen = 28
	};

	int		mUseValidSsidWeight;
	int		mDoNotUseSsidWeight;
	int		mUseSsidWeight;

public:
	AosLogoutTester(const bool regflag);
	AosLogoutTester();
	~AosLogoutTester();

	virtual bool test();
	AosSengTesterPtr clone()
			{
				return OmnNew AosLogoutTester();
			}
};
#endif

