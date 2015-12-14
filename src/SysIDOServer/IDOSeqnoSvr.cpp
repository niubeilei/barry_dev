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
#include "SysIDOServer/IDOSeqnoSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Jimo/GlobalFuncs.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoAPI/Jimo.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ClusterObj.h"
#include "SysIDOs/IDOSeqno.h"
#include "Thread/Mutex.h"


using namespace Jimo;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSeqnoIDOSvr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSeqnoIDOSvr(version);
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


AosSeqnoIDOSvr::AosSeqnoIDOSvr(const int version)
:
mLock(OmnNew OmnMutex())
{
	mJimoType = AosJimoType::eIDO;
}


AosSeqnoIDOSvr::~AosSeqnoIDOSvr()
{
	OmnDelete mLock;
}


bool 
AosSeqnoIDOSvr::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u32 func_id =jimo_call.getFuncID();
	switch (func_id)
	{
	case AosSeqnoIDO::eMethodNextSeqno:
		 return nextSeqno(rdata, jimo_call);

	case AosSeqnoIDO::eMethodCreateByName:
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
AosSeqnoIDOSvr::isValid() const
{
	return true;
}


u64
AosSeqnoIDOSvr::createByName(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnString name = jimo_call.getOmnStr(rdata, AosFN::eName, "");
	int version = jimo_call.getInt(rdata, AosFN::eVersion, 1);
	bool override_flag = jimo_call.getBool(rdata, AosFN::eFlag, false);

	if (name == "")
	{
		// Syntax error
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_objname");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	itr_t itr = mIDOMap.find(name);
	AosSeqnoIDOObjPtr ido;
	if (itr != mIDOMap.end())
	{
		if (!override_flag)
		{
			// Object already exist. Treated as an error
			mLock->unlock();
			jimo_call.setLogicalFail();
			jimo_call.arg(AosFN::eErrmsg, "object_already_exist");
			jimo_call.arg(AosFN::eFile, __FILE__);
			jimo_call.arg(AosFN::eLine, __LINE__);
			jimo_call.sendResp(rdata);
			return true;
		}
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosSeqnoIDO", version);
	if (!jimo)
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "jimo_not_found");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	ido = dynamic_cast<AosSeqnoIDOObj *>(jimo.getPtr());
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

		
u64
AosSeqnoIDOSvr::nextSeqno(AosRundata *rdata, AosJimoCall &jimo_call)
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
	u64 seqno = itr->second->nextSeqno(rdata);
	mLock->unlock();

	jimo_call.setSuccess();
	jimo_call.arg(AosFN::eValue, seqno);
	jimo_call.sendResp(rdata);
	return true;
}


AosJimoPtr 
AosSeqnoIDOSvr::cloneJimo() const
{
	return OmnNew AosSeqnoIDOSvr(*this);
}

