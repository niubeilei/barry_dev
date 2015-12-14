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
// CubeMap is implemented through an In-memory Data Object (IDO). An 
// IDO is made of two objects: Client Object and Server Object. The 
// client object implements the API calls and the server object implements
// the logic. 
//
// Modification History:
// 2015/03/12 Created by Ketty Guo
////////////////////////////////////////////////////////////////////////////
#include "ClusterMgr/CubeMapSvr.h"

#include "Debug/Debug.h"
#include "JimoCall/JimoCall.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCubeMapSvr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCubeMapSvr(version);
		aos_assert_r(jimo, 0);
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

AosCubeMapSvr::AosCubeMapSvr(const int version)
{
	mJimoType = AosJimoType::eIDO;
	mJimoVersion = version;
	mLock = OmnNew OmnMutex();
}


AosCubeMapSvr::~AosCubeMapSvr()
{
	OmnDelete mLock;
}


bool 
AosCubeMapSvr::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u32 func_id =jimo_call.getFuncID();
	switch (func_id)
	{
	case AosCubeMapObj::eMethodGetByDocid:
		 return getByDocid(rdata, jimo_call);

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosCubeMapSvr::getByDocid(
		AosRundata *rdata, 
		AosJimoCall &jimo_call)
{
	// This function retrieves the cube map identified by the parameter
	// 'eDocid'.
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	if (docid == 0)
	{
		mLock->unlock();
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "missing_docid");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	mLock->lock();
	itr_t itr = mMap.find(docid);
	if (itr == mMap.end())
	{
		// Does not exist
		mLock->unlock();
		jimo_call.setLogicalFail();
		jimo_call.arg(AosFN::eErrmsg, "cubemap_not_exist");
		jimo_call.arg(AosFN::eFile, __FILE__);
		jimo_call.arg(AosFN::eLine, __LINE__);
		jimo_call.sendResp(rdata);
		return true;
	}

	AosCubeMapObjPtr cube_map = itr->second;
	mLock->unlock();

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	cube_map->serializeTo(rdata, buff.getPtr());

	jimo_call.setSuccess();
	jimo_call.arg(AosFN::eBuff, buff.getPtr());
	jimo_call.sendResp(rdata);
	return true;
}


AosJimoPtr 
AosCubeMapSvr::cloneJimo() const
{
	return OmnNew AosCubeMapSvr(mJimoVersion);
}

