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
// 2015/03/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SysIDOServer/IDOU64Svr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoAPI/Jimo.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoAPI/JimoSysIDOs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ClusterObj.h"
#include "SysIDOs/IDOU64.h"
#include "Thread/Mutex.h"
#include "UtilData/FN.h"


using namespace Jimo;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosU64IDOSvr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosU64IDOSvr(version);
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


AosU64IDOSvr::AosU64IDOSvr(const int version)
:
mLock(OmnNew OmnMutex())
{
	mJimoType = AosJimoType::eIDO;
}


AosU64IDOSvr::~AosU64IDOSvr()
{
	OmnDelete mLock;
}


bool 
AosU64IDOSvr::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u32 func_id =jimo_call.getFuncID();
	switch (func_id)
	{
	case AosU64IDO::eMethodGet:
		 return get(rdata, jimo_call);

	case AosU64IDO::eMethodSet:
		 return set(rdata, jimo_call);

	case AosU64IDO::eMethodCreateByName:
		 return createByName(rdata, jimo_call);

	default:
		 break;
	}

	AosLogError(rdata, true, "internal_error") << enderr;
	jimo_call.setLogicalFail();
	jimo_call.arg(AosFN::eErrmsg, "method_not_found");
	jimo_call.arg(AosFN::eFile, __FILE__);
	jimo_call.arg(AosFN::eLine, __LINE__);
	jimo_call.sendResp(rdata);
	return true;
}


bool 
AosU64IDOSvr::isValid() const
{
	return true;
}


bool
AosU64IDOSvr::createByName(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnString name = jimo_call.getOmnStr(rdata, AosFN::eName, "");
	bool override_flag = jimo_call.getBool(rdata, AosFN::eFlag, false);

	if (name == "")
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_objname");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	itr_t itr = mIDOMap.find(name);
	AosU64IDOObjPtr ido;
	if (itr != mIDOMap.end())
	{
		if (!override_flag)
		{
			mLock->unlock();
			jimo_call.setLogicalFail();
			jimo_call.arg(AosFN::eErrmsg, "object_already_exist");
			jimo_call.arg(AosFN::eFile, __FILE__);
			jimo_call.arg(AosFN::eLine, __LINE__);
			jimo_call.sendResp(rdata);
			return true;
		}
	}

	ido = jimoCreateU64IDO(rdata, name, override_flag);
	if (!ido)
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "internal_error");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mIDOMap[name] = ido;
	AosBuffPtr buff = OmnNew AosBuff();
	ido->serializeTo(rdata, buff.getPtr());
	jimo_call.setSuccess();
	jimo_call.arg(AosFN::eBuff, buff);
	jimo_call.sendResp(rdata);
	return true;
}

		
bool
AosU64IDOSvr::get(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnString name = jimo_call.getOmnStr(rdata, AosFN::eName, "");
	if (name == "")
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_objname");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	itr_t itr = mIDOMap.find(name);
	if (itr == mIDOMap.end())
	{
		mLock->unlock();
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "ido_not_found");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}
	u64 value = itr->second->get(rdata, 0);
	mLock->unlock();

	jimo_call.setSuccess();
	jimo_call.arg(AosFN::eValue, value);
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosU64IDOSvr::set(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnString name = jimo_call.getOmnStr(rdata, AosFN::eName, "");
	u64 value = jimo_call.getU64(rdata, AosFN::eValue, 0);
	if (name == "")
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_objname");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	itr_t itr = mIDOMap.find(name);
	if (itr == mIDOMap.end())
	{
		mLock->unlock();
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "ido_not_found");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}
	itr->second->set(rdata, value);
	mLock->unlock();

	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


AosJimoPtr 
AosU64IDOSvr::cloneJimo() const
{
	return OmnNew AosU64IDOSvr(*this);
}

