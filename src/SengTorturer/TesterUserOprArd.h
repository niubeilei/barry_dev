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
// 01/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_UserOprArdTester_h
#define AosSengTester_UserOprArdTester_h

#include "Rundata/Ptrs.h"
#include "SengTorturer/SengTester.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "SengTorturer/StUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosUserOprArdTester : virtual public AosSengTester
{
private:

public:
	AosUserOprArdTester(const bool regflag);
	AosUserOprArdTester();
	~AosUserOprArdTester();

	virtual bool test();
	AosSengTesterPtr clone()
	{
		return OmnNew AosUserOprArdTester();
	}

private:
	bool createUserOprArd();
	bool modifyUserOprArd();
	bool removeUserOprArd();
};
#endif

