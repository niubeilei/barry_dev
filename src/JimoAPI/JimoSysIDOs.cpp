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
// 2015/03/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoSysIDOs.h"

//#include "JimoAPI/Jimo.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "SEInterfaces/SeqnoIDOObj.h"
#include "SEInterfaces/U64IDOObj.h"
#include "SEInterfaces/ClusterObj.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "UtilData/RepPolicy.h"

namespace Jimo
{

AosSeqnoIDOObjPtr jimoCreateSeqnoIDO(
		AosRundata *rdata, 
		const OmnString &name, 
		const int version,
		const bool override_flag)
{
	u32 rep_policy = AosRepPolicy::getRepPolicy(3, 0, 0);
	AosClusterObj *cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, name);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosSeqnoIDOSvr", AosSeqnoIDOObj::eMethodCreateByName,
		distr_id, rep_policy, cluster);

	jimo_call->arg(AosFN::eName, name);
	jimo_call->arg(AosFN::eFlag, override_flag);
	jimo_call->arg(AosFN::eVersion, version);
	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess())
	{
		// Did not create. 
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		if (errmsg != "")
		{
			AosLogError(rdata, false,  "failed_creating_IDO")
				<< AosFN("obj_name") << name 
				<< AosFN("Error Message") << errmsg << enderr;
		}
		else
		{
			AosLogError(rdata, false,  "failed_creating_IDO")
				<< AosFN("obj_name") << name << enderr;
		}

		return 0;
	}

	//AosBuff *buff = jimo_call->getBuffRaw(rdata, AosFN::eBuff, 0);
	AosBuffPtr buff = jimo_call->getBuff(rdata, AosFN::eBuff, 0);
	if (!buff)
	{
		AosLogError(rdata, true, "internal_error") << enderr;
		return 0;
	}

	//AosIDOPtr ido = AosIDO::createIDO(rdata, buff);
	AosIDOPtr ido = AosIDO::createIDO(rdata, buff.getPtrNoLock());
	aos_assert_rr(ido, rdata, 0);

	AosSeqnoIDOObjPtr seqno = dynamic_cast<AosSeqnoIDOObj *>(ido.getPtr());
	aos_assert_rr(seqno, rdata, 0);
	return seqno;
}


AosU64IDOObjPtr jimoCraeteU64IDO(
		AosRundata *rdata, 
		const OmnString &name, 
		const bool override_flag)
{
	u32 rep_policy = AosRepPolicy::getRepPolicy(3, 0, 0);
	AosClusterObj *cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, name);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosU64IDOSvr", AosU64IDOObj::eMethodCreateByName,
		distr_id, rep_policy, cluster);

	jimo_call->arg(AosFN::eName, name);
	jimo_call->arg(AosFN::eFlag, override_flag);
	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess())
	{
		// Did not create. 
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		if (errmsg != "")
		{
			AosLogError(rdata, false,  "failed_creating_IDO")
				<< AosFN("obj_name") << name 
				<< AosFN("Error Message") << errmsg << enderr;
		}
		else
		{
			AosLogError(rdata, false,  "failed_creating_IDO")
				<< AosFN("obj_name") << name << enderr;
		}

		return 0;
	}

	//AosBuff *buff = jimo_call->getBuffRaw(rdata, AosFN::eBuff, 0);
	AosBuffPtr buff = jimo_call->getBuff(rdata, AosFN::eBuff, 0);
	if (!buff)
	{
		AosLogError(rdata, true, "internal_error") << enderr;
		return 0;
	}

	//AosIDOPtr ido = AosIDO::createIDO(rdata, buff);
	AosIDOPtr ido = AosIDO::createIDO(rdata, buff.getPtrNoLock());
	aos_assert_rr(ido, rdata, 0);

	AosU64IDOObjPtr obj = dynamic_cast<AosU64IDOObj *>(ido.getPtr());
	aos_assert_rr(obj, rdata, 0);
	return obj;
}


};

