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
// 2015/03/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IDOServer/IDOServer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "JimoCall/JimoCall.h"
#include "UtilData/FN.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosIDOServer_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosIDOServer(version);
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


AosIDOServer::AosIDOServer(const int version)
:
mLock(OmnNew OmnMutex())
{
	mJimoType = AosJimoType::eIDOServer;
	mJimoVersion = version;
}


AosIDOServer::~AosIDOServer()
{
	OmnDelete mLock;
}


bool 
AosIDOServer::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u32 func_id = jimo_call.getFuncID();
	switch (func_id)
	{
	case eMethodCreateIDOByName:
		 return createIDOByName(rdata, jimo_call);

	case eMethodGetIDOByDocid:
		 return getIDOByDocid(rdata, jimo_call);

	case eMethodRunIDOByName:
		 return runIDOByName(rdata, jimo_call);

	default:
		 break;
	}

	jimo_call.setLogicalFail();
	jimo_call.arg(AosFN::eErrmsg, "method_not_found");
	jimo_call.arg(AosFN::eFile, __FILE__);
	jimo_call.arg(AosFN::eLine, __LINE__);
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosIDOServer::createIDOByName(AosRundata *rdata, AosJimoCall &jimo_call)
{
	// This function creates an IDO by name. It expects the following
	// parameters:
	// 		eObjname,		the IDO name			mandatory
	//		eFlag,			the override flag		optional
	//		eBuff			the buff for the IDO	mandatory
	OmnString objname = jimo_call.getOmnStr(rdata, AosFN::eObjName, "");
	if (objname == "")
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_objname");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	bool override_flag = jimo_call.getBool(rdata, AosFN::eFlag, false);
	AosBuffPtr buff = jimo_call.getBuffRaw(rdata, AosFN::eBuff);
	if (!buff)
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_buff");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	AosIDOPtr ido = AosIDO::createIDO(rdata, buff.getPtr());
	if (!ido)
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "internal_error");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	itr1_t itr = mNameMap.find(objname);
	if (itr != mNameMap.end())
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

	mNameMap[objname] = ido;
	mLock->unlock();

	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


AosJimoPtr
AosIDOServer::cloneJimo() const
{
	return OmnNew AosIDOServer(mJimoVersion);
}


bool
AosIDOServer::getIDOByDocid(AosRundata *rdata, AosJimoCall &jimo_call)
{
	// It expects the following parameters:
	// 		eDocid
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	if (docid == 0)
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_docid");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	itr2_t itr = mDocidMap.find(docid);
	if (itr == mDocidMap.end())
	{
		mLock->unlock();
		return 0;
	}

	AosIDOPtr ido = itr->second;
	mLock->unlock();

	AosBuffPtr buff = OmnNew AosBuff();
	ido->serializeTo(rdata, buff.getPtr());

	jimo_call.arg(AosFN::eBuff, buff.getPtr());
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


bool
AosIDOServer::runIDOByName(AosRundata *rdata, AosJimoCall &jimo_call)
{
	// It requires the following parameters:
	// 	eIDOName			mandatory
	// 	eCreateAsNeeded		optiona, default to false
	// 	eClassname			if 'eCreateAsNeeded', this is mandatory
	// 	eVersion			optiona, default to 1
	//
	OmnString name = jimo_call.getOmnStr(rdata, AosFN::eIDOName, "");
	if (name == "")
	{
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_ido_name");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	itr1_t itr = mNameMap.find(name);
	AosIDOPtr ido;
	if (itr == mNameMap.end())
	{
		mLock->unlock();
		if (!jimo_call.getBool(rdata, AosFN::eCreateAsNeeded, false))
		{
			jimo_call.setLogicalFail();
			jimo_call.arg(AosFN::eErrmsg, "ido_not_found");
			jimo_call.arg(AosFN::eFile, __FILE__);
			jimo_call.arg(AosFN::eLine, __LINE__);
			jimo_call.sendResp(rdata);
			return false;
		}

		// Need to create the IDO. 
		OmnString classname = jimo_call.getOmnStr(rdata, AosFN::eClassname);
		if (classname == "")
		{
			jimo_call.setLogicalFail();
			jimo_call.arg(AosFN::eErrmsg, "missing_ido_classname");
			jimo_call.arg(AosFN::eFile, __FILE__);
			jimo_call.arg(AosFN::eLine, __LINE__);
			jimo_call.sendResp(rdata);
			return false;
		}

		int version = jimo_call.getInt(rdata, AosFN::eVersion, 1);
		AosJimoPtr jimo = AosCreateJimoByClassname(rdata, classname, version);
		if (!jimo)
		{
			jimo_call.setLogicalFail();
			jimo_call.arg(AosFN::eErrmsg, "ido_not_found");
			jimo_call.arg(AosFN::eFile, __FILE__);
			jimo_call.arg(AosFN::eLine, __LINE__);
			jimo_call.sendResp(rdata);
			return false;
		}

		ido = dynamic_cast<AosIDO*>(jimo.getPtr());
		if (!ido)
		{
			jimo_call.setLogicalFail();
			jimo_call.arg(AosFN::eErrmsg, "internal_error");
			jimo_call.arg(AosFN::eFile, __FILE__);
			jimo_call.arg(AosFN::eLine, __LINE__);
			jimo_call.sendResp(rdata);
			return false;
		}

		ido->setIDOName(name);
		mLock->lock();
		itr = mNameMap.find(name);
		if (itr == mNameMap.end())
		{
			mNameMap[name] = ido;
		}
		else
		{
			ido = itr->second;
		}
	}
	else
	{
		ido = itr->second;
	}
	mLock->unlock();

	return ido->jimoCall(rdata, jimo_call);
}

