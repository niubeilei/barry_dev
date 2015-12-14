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
// 2015/09/15 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "Synchers/SyncherNull.h"

#include "Util1/Time.h"
#include "Util/Buff.h"

AosSyncherNull::AosSyncherNull()
:
AosSyncher(0)
{
	mCreateTimeSec = OmnTime::getCrtSecond();
}


AosSyncherNull::AosSyncherNull(const AosSyncherNull &rhs)
:
AosSyncher(0)
{
	mCreateTimeSec = rhs.mCreateTimeSec;
}


bool 
AosSyncherNull::proc()
{
	return true; 

	OmnShouldNeverComeHere;
	return false;
}


bool 
AosSyncherNull::isValid() const
{
	return false;
}


bool 
AosSyncherNull::isExpired() const
{
	i64 sec = OmnTime::getCrtSecond();
	aos_assert_r(sec > mCreateTimeSec, false);
	return (sec - mCreateTimeSec > eExpireTimer);
}


AosSyncherObjPtr 
AosSyncherNull::clone() const
{
	return OmnNew AosSyncherNull(*this);
}


AosBuffPtr 
AosSyncherNull::serializeToBuff()
{
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	buff->setOmnStr(mTestContents);
	//OmnNotImplementedYet;
	return buff;
}


bool 
AosSyncherNull::serializeFromBuff(const AosBuffPtr &buff)
{
	mTestContents = buff->getOmnStr("");
	if (mTestContents != "") 
		OmnScreen << "$$$$$$$$$$$$$$$$$: " << mTestContents << endl;
	//OmnNotImplementedYet;
	return true;
}
