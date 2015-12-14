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
// Modification History:
// 2015/04/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SysIDOSvrs/IDOJimoProgSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoAPI/Jimo.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/IDOServerObj.h"
#include "SEInterfaces/IDOJimoProgObj.h"
#include "Thread/Mutex.h"


using namespace Jimo;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosIDOJimoProgSvr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosIDOJimoProgSvr(version);
		aos_assert_r(jimo, 0);
		OmnScreen << "Jimo Created" << endl;
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


AosIDOJimoProgSvr::AosIDOJimoProgSvr(const int version)
:
mLock(OmnNew OmnMutex())
{
	mJimoType = AosJimoType::eIDO;
	mJimoVersion = version;
}


AosIDOJimoProgSvr::~AosIDOJimoProgSvr()
{
	OmnDelete mLock;
}


bool 
AosIDOJimoProgSvr::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u32 func_id = jimo_call.getFuncID();
	switch (func_id)
	{
	case AosIDOJimoProgObj::eMethodGetStr:
		 return getStr(rdata, jimo_call);

	case AosIDOJimoProgObj::eMethodSetStr:
		 return setStr(rdata, jimo_call);

	default:
		 break;
	}

	jimo_call.reset();
	jimo_call.setLogicalFail();
	jimo_call.arg(AosFN::eErrmsg, "method_not_found");
	jimo_call.arg(AosFN::eMethod, func_id);
	jimo_call.arg(AosFN::eFile, __FILE__);
	jimo_call.arg(AosFN::eLine, __LINE__);
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosIDOJimoProgSvr::setStr(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnString name = jimo_call.getOmnStr(rdata, AosFN::eName, "");
	OmnString value = jimo_call.getOmnStr(rdata, AosFN::eValue, "");

	if (name == "")
	{
		jimo_call.reset();
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_name");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	mBSON.setValue(name, value);
	mLock->unlock();

	jimo_call.reset();
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);

	return true;
}


bool
AosIDOJimoProgSvr::getStr(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnString name = jimo_call.getOmnStr(rdata, AosFN::eName, "");
	if (name == "")
	{
		jimo_call.reset();
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_name");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	OmnString value = mBSON.getOmnStr(name);
	mLock->unlock();

	jimo_call.reset();
	jimo_call.setSuccess();
	jimo_call.arg(AosFN::eValue, value);
	jimo_call.sendResp(rdata);
	return true;
}


AosJimoPtr 
AosIDOJimoProgSvr::cloneJimo() const
{
	return OmnNew AosIDOJimoProgSvr(*this);
}


bool 
AosIDOJimoProgSvr::isValid() const
{
	return true;
}

