////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 2015/09/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Synchers/SyncherExample.h"

#include "SEInterfaces/SyncherType.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSyncherExample(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSyncherExample(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosLogError(rdata, false, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosSyncherExample::AosSyncherExample(const int version)
:
AosSyncher(version)
{
}


AosSyncherExample::~AosSyncherExample()
{
}


bool 
AosSyncherExample::proc()
{
	OmnScreen << "Processing Syncher: SyncherExample" << endl;
	return true;
}


AosBuffPtr
AosSyncherExample::serializeToBuff()
{
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	buff->setI64(mCreateTimeSec);
	return buff;
}


bool
AosSyncherExample::serializeFromBuff(const AosBuffPtr &buff)
{
	mCreateTimeSec = buff->getI64(-1);
	aos_assert_r(mCreateTimeSec > 0, false);
	return true;
}


AosJimoPtr 
AosSyncherExample::cloneJimo() const
{
	return OmnNew AosSyncherExample(*this);
}


