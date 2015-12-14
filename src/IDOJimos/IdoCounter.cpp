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
// This IDO simulates a count. The IDO is identified by "IDOType" and 
// "IDOName". The server that hosts the IDO is determined by taking
// the hash value of "IDOType" + "IDOName" and modding it by the total
// number of cubes or servers. 
//
// Clients calls the IDO's through JimoCalls. The JimoCall routes the 
// call to the corresponding IDO server. The server retrieves the IDO
// from its list. If not found, it may create it. After that it forwards
// the call to that IDO.
//
// Modification History:
// 2015/03/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IDOJimos/IdoCounter.h"

#include "JimoCall/JimoCall.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosIdoCounter_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosIdoCounter(version);
		aos_assert_r(jimo, 0);
		OmnScreen << "Jimo Created" << endl;
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}



AosIdoCounter::AosIdoCounter(const int version)
:
AosIDO(version),
mLock(OmnNew OmnMutex())
{
	mJimoType = AosJimoType::eIDO;
	mJimoVersion = version;
}


AosIdoCounter::~AosIdoCounter()
{
	OmnDelete mLock;
}


AosJimoPtr
AosIdoCounter::cloneJimo() const
{
	return OmnNew AosIdoCounter(*this);
}


bool 
AosIdoCounter::serializeFrom(AosRundata *rdata, AosBuff *buff)
{
	AosIDO::serializeFrom(rdata, buff);
	mCount = buff->getU32(0);
	mName = buff->getOmnStr("");
	return true;
}


bool 
AosIdoCounter::serializeTo(AosRundata *rdata, AosBuff *buff) const
{
	AosIDO::serializeTo(rdata, buff);
	buff->setU32(mCount);
	buff->setOmnStr(mName);
	return true;
}


bool 
AosIdoCounter::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnString func_name = jimo_call.getOmnStr(rdata, AosFN::eIDOFunc, "");
	if (func_name == "increment") return increment(rdata, jimo_call);

	jimo_call.setLogicalFail();
	jimo_call.arg(AosFN::eErrmsg, "method_not_found");
	jimo_call.arg(AosFN::eFile, __FILE__);
	jimo_call.arg(AosFN::eLine, __LINE__);
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosIdoCounter::increment(AosRundata *rdata, AosJimoCall &jimo_call)
{
	// This function increments the count by one (default) or 
	// by eIncrementValue (if it is specified).
	i64 inc_value = jimo_call.getI64(rdata, AosFN::eIncrementValue, 1);

	mLock->lock();
	i64 value = mCount;
	mCount += inc_value;
	mLock->unlock();

	jimo_call.arg(AosFN::eValue, value);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}

