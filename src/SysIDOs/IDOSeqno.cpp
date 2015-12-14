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
#include "SysIDOs/IDOSeqno.h"

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

AosJimoPtr AosCreateJimoFunc_AosSeqnoIDO_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSeqnoIDO(version);
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


AosSeqnoIDO::AosSeqnoIDO(const int version)
:
AosSeqnoIDOObj(version)
{
	mJimoType = AosJimoType::eIDO;
}


u64
AosSeqnoIDO::nextSeqno(AosRundata *rdata)
{
	AosClusterObj *ido_cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(ido_cluster, rdata, 0);

	u32 distr_id = ido_cluster->getDistrID(rdata, mIDOName);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosIDOServer", eMethodNextSeqno,
		distr_id, mRepPolicy, ido_cluster);
	jimo_call->arg(AosFN::eName, mIDOName);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	u64 seqno = jimo_call->getU64(rdata, AosFN::eValue, 0);
	aos_assert_rr(seqno > mSeqno, rdata, false);
	mSeqno = seqno;
	return mSeqno;
}


bool 
AosSeqnoIDO::serializeFrom(AosRundata *rdata, AosBuff *buff)
{
	AosIDO::serializeFrom(rdata, buff);
	mSeqno = buff->getU64(0);
	return true;
}


bool 
AosSeqnoIDO::serializeTo(AosRundata *rdata, AosBuff *buff) const
{
	AosIDO::serializeTo(rdata, buff);
	buff->setU64(mSeqno);
	return true;
}


AosJimoPtr 
AosSeqnoIDO::cloneJimo() const
{
	return OmnNew AosSeqnoIDO(*this);
}

