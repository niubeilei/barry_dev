////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// Nov 17, 2015 created by tracy
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_JimoEncodingTester_JEUIATorturer_H_
#define Aos_Util_JimoEncodingTester_JEUIATorturer_H_

#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/JEUIA.h"
#include "alarm_c/alarm.h"
#include "Random/RandomUtil.h"
#include <vector>

class AosJEUIATorturer
{
private:
	AosBuffPtr 			mBuff;
	AosBuff *      	 	mBuffRaw;
	int 				mflage;

	u64					mValueu64;
	u32					mValueu32;
	u16					mValueu16;
	u8					mValueu8;
	u8					mSizeMax;
	u64					def;

public:
	AosJEUIATorturer();

	~AosJEUIATorturer();

	bool	start(int runtimes);

private:
	bool	basicTest(int runtimes);

	bool	addValue();

	bool	verify();

};

#endif /* Aos_Util_JimoEncodingTester_JEUIATorturer_H_ */
