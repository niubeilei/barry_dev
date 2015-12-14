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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
/*#include "CubeComm/DataCommSimple.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_DataCommSimple_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew DataCommSimple(version);
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



AosDataCommSimple::AosDataCommSimple(const int version)
:
AosDataComm(version)
{
}


int
AosDataCommSimple::createData(
		AosRundata *rdata, 
		const u32 cubeid,
		const u64 aseid,
		const AosReliabilityPolicy::E reliability_policy,
		const char * const data, 
		const int length, 
		u64 &docid, 
		const AosJimoCallerPtr &caller)
{
	// This function sends a request to the cube to create data. 
	// It the reliability policy to retrieve a replicator. If it fails
	// retrieving the replicator, AosErrmsgId::eReliabilityNotSupported
	// is returned. Otherwise, it uses the replicator to replicate the
	// data. 
	// This is an asynched call. When finishing, it will call back
	// the caller.
	
	AosReplicatorMgr *mgr = AosReplicatorMgr::getSelf();
	aos_assert_rr(mgr, rdata, 0);
	AosReplicator *replicator = mgr->getReplicator(rdata, reliability_policy);
	if (!replicator)
	{
		AosLogUserError(rdata, AosErrmsgId::eReliabilityNotSupported)
			<< AosFieldName::eReliabilityPolicy 
			<< AosReliabilityPolicy::toStr(reliability_policy)
			<< enderr;
		return 0;
	}

	return replicator->replicateData(rdata, cubeid, aseid, 
			reliability_policy, data, length, caller);
}
*/
