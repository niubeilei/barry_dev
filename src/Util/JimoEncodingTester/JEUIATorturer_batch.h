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
#ifndef Aos_Util_JimoEncodingTester_JEUIATorturer_batch_H_
#define Aos_Util_JimoEncodingTester_JEUIATorturer_batch_H_

#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/JEUIA.h"
#include "alarm_c/alarm.h"
#include "Random/RandomUtil.h"
#include <vector>

class AosJEUIATorturer_batch
{
private:
	AosBuffPtr 			mBuff;
	AosBuff *      	 	mBuffRaw;
	std::vector<u64>	mValues;

public:
	AosJEUIATorturer_batch();

	~AosJEUIATorturer_batch();

	bool	start(int runtimes);

private:
	bool	basicTest(int runtimes);

	bool	addValues();

	bool	addValue();

	bool	verify();

};

#endif /* Aos_Util_JimoEncodingTester_JEUIATorturer_batch_H_ */
