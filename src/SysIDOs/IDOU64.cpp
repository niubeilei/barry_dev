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
#include "SysIDOs/IDOU64.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoAPI/Jimo.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ClusterObj.h"

using namespace Jimo;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosU64IDO_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosU64IDO(version);
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


AosU64IDO::AosU64IDO(const int version)
:
AosU64IDOObj(version)
{
	mJimoType = AosJimoType::eIDO;
}


u64
AosU64IDO::get(AosRundata *rdata, const u64 dft) 
{
	aos_assert_rr(mName != "", rdata, dft);

	AosClusterObj *ido_cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(ido_cluster, rdata, dft);

	u32 distr_id = ido_cluster->getDistrID(rdata, mName);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosU64IDOSvr", eMethodGet,
		distr_id, mRepPolicy, ido_cluster);
	jimo_call->arg(AosFN::eName, mName);

	bool rslt = jimo_call->makeCall(rdata);
	if (!rslt)
	{
		AosLogError(rdata, false, "failed_jimo_call")
			<< AosFN("Error Message") 
			<< jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "") << enderr;
		return dft;
	}

	mValue = jimo_call->getU64(rdata, AosFN::eValue, 0);
	return mValue;
}


bool
AosU64IDO::set(AosRundata *rdata, const u64 value)
{
	aos_assert_rr(mName != "", rdata, false);

	AosClusterObj *ido_cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(ido_cluster, rdata, false);

	u32 distr_id = ido_cluster->getDistrID(rdata, mName);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosU64IDOSvr", eMethodSet,
		distr_id, mRepPolicy, ido_cluster);
	jimo_call->arg(AosFN::eName, mName);
	jimo_call->arg(AosFN::eValue, value);

	bool rslt = jimo_call->makeCall(rdata);
	if (!rslt)
	{
		AosLogError(rdata, false, "failed_jimo_call")
			<< AosFN("Error Message") 
			<< jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "") << enderr;
		return false;
	}

	mValue = value;
	return true;
}


bool 
AosU64IDO::serializeFrom(AosRundata *rdata, AosBuff *buff)
{
	AosIDO::serializeFrom(rdata, buff);
	mValue = buff->getU64(0);
	mName = buff->getOmnStr("");
	return true;
}


bool 
AosU64IDO::serializeTo(AosRundata *rdata, AosBuff *buff) const
{
	AosIDO::serializeTo(rdata, buff);
	buff->setU64(mValue);
	buff->setOmnStr(mName);
	return true;
}


AosJimoPtr 
AosU64IDO::cloneJimo() const
{
	return OmnNew AosU64IDO(*this);
}

