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
#ifndef Aos_Util_JimoEncodingTester_JESIATorturer_H_
#define Aos_Util_JimoEncodingTester_JESIATorturer_H_

#include <vector>
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/JESIA.h"
#include "alarm_c/alarm.h"
#include "Random/RandomUtil.h"

class AosJESIATorturer
{
private:
	AosBuffPtr 			mBuff;
	AosBuff *      	 	mBuffRaw;
	int 				mflage;

	i64					mValuei64;
	i32					mValuei32;
	i16					mValuei16;
	i8					mValuei8;
	i64					def;

	int 				mSizeMax;

public:
	AosJESIATorturer();

	~AosJESIATorturer();

	bool	start(int runtimes);

private:
	bool	basicTest(int runtimes);

	bool	addValue();

	bool	verify();
};

#endif /* Aos_Util_JimoEncodingTester_JESIATorturer_H_ */
