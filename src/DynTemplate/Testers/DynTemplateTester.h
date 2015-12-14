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
// 2013/05/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapEngine_Testers_SampleTester_h
#define Aos_BitmapEngine_Testers_SampleTester_h

#include "Tester/TestPkg.h"


class AosSampleTester : public OmnTestPkg
{
private:
	enum
	{
		eDftTries = 1000
	};

	int 	mTries;
	bool	mIsDLL;

public:
	AosSampleTester();
	~AosSampleTester() {}

	virtual bool		start();

private:
	bool config();
};
#endif

