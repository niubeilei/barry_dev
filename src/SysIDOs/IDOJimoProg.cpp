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
#include "SysIDOs/IDOJimoProg.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoAPI/Jimo.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/IDOServerObj.h"

using namespace Jimo;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosIDOJimoProg_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosIDOJimoProg(version);
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


AosIDOJimoProg::AosIDOJimoProg(const int version)
{
	mJimoType = AosJimoType::eIDO;
	mJimoVersion = version;
}


bool
AosIDOJimoProg::setStr(
		AosRundata *rdata, 
		const OmnString &name,
		const OmnString &value)
{
	AosClusterObj *ido_cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(ido_cluster, rdata, false);

	u32 distr_id = ido_cluster->getDistrID(rdata, mIDOName);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosIDOServer", AosIDOServerObj::eMethodRunIDOByName,
		distr_id, AosRepPolicy::getRepPolicy(2, 1, 0), ido_cluster);
	jimo_call->arg(AosFN::eIDOName, mIDOName);
	jimo_call->arg(AosFN::eName, name);
	jimo_call->arg(AosFN::eValue, value);
	jimo_call->arg(AosFN::eOperation, eMethodSetStr);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	return true;
}


OmnString
AosIDOJimoProg::getStr(
		AosRundata *rdata, 
		const OmnString &name,
		const OmnString &dft) const
{
	AosClusterObj *ido_cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(ido_cluster, rdata, dft);

	u32 distr_id = ido_cluster->getDistrID(rdata, mIDOName);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosIDOServer", AosIDOServerObj::eMethodRunIDOByName,
		distr_id, AosRepPolicy::getRepPolicy(2, 1, 0), ido_cluster);
	jimo_call->arg(AosFN::eIDOName, mIDOName);
	jimo_call->arg(AosFN::eName, name);
	jimo_call->arg(AosFN::eOperation, eMethodGetStr);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return dft;
	}

	return jimo_call->getOmnStr(rdata, AosFN::eValue);
}


bool 
AosIDOJimoProg::serializeFrom(AosRundata *rdata, AosBuff *buff)
{
	AosIDO::serializeFrom(rdata, buff);
	AosBuffPtr bb = buff->getAosBuff(true);
	mBSON.setBuff(bb);
	return true;
}


bool 
AosIDOJimoProg::serializeTo(AosRundata *rdata, AosBuff *buff) const
{
	AosIDO::serializeTo(rdata, buff);
	AosBuff *bb = mBSON.getBuffRaw();
	aos_assert_rr(bb, rdata, false);
	buff->setAosBuff(bb);
	return true;
}


AosJimoPtr 
AosIDOJimoProg::cloneJimo() const
{
	return OmnNew AosIDOJimoProg(*this);
}

