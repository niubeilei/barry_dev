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
// 2013/02/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_BuffArrayTesterNew_h
#define Omn_TestUtil_BuffArrayTesterNew_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/CompareFun.h"



class AosBuffArrayTesterNew : public OmnTestPkg
{
public:
	OmnString mConf;
	AosCompareFunPtr	mCmpFun;
private:
	int		mTries;

public:
	AosBuffArrayTesterNew();
	~AosBuffArrayTesterNew() {}

	virtual bool		start();

private:
	bool		testInsert();
};
#endif

