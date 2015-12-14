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
// 2013/02/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapEngine_Testers_BitmapCacheTester_h
#define Aos_BitmapEngine_Testers_BitmapCacheTester_h

#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"


class AosBitmapCacheTester : public OmnTestPkg
{
public:

private:
	AosRundataPtr 		mRundata;

public:
	AosBitmapCacheTester();
	~AosBitmapCacheTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	config();
};
#endif

