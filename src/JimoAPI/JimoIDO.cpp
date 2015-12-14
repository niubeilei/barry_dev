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
// 2015/03/18 Created by Gavin
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoIDO.h"

#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoAPI/JimoFunc.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/IDOServerObj.h"
#include "Thread/Mutex.h"
#include "Util/String.h"


namespace Jimo
{

AosIDOPtr jimoGetIDO(AosRundata *rdata, const u64 docid)
{
	AosClusterObj *cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);

	u32 distr_id = cluster->getDistrID(rdata, docid);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosIDOServer", AosIDOServerObj::eMethodGetIDOByDocid,
		distr_id, gRepPolicyMgr.getCubeMapRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);

	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess())
	{
		AosLogError(rdata, false, "failed_retrieving_cube_map") << enderr;
		return 0;
	}

	//AosBuff *buff = jimo_call->getBuffRaw(rdata, AosFN::eReturnValue, 0);
	AosBuffPtr buff = jimo_call->getBuff(rdata, AosFN::eReturnValue, 0);
	if (!buff)
	{
		AosLogError(rdata, false,  "failed_retrieving_IDO")
			<< AosFN("Docid") << docid << enderr;
		return 0;
	}

	//AosIDOPtr ido = AosIDO::createIDO(rdata, buff);
	AosIDOPtr ido = AosIDO::createIDO(rdata, buff.getPtrNoLock());
	if (!ido)
	{
		AosLogError(rdata, false, "failed_creating_IDO")  << enderr;
		return 0;
	}
	return ido;
}


bool jimoCreateIDO( 
		AosRundata *rdata, 
		const OmnString &obj_name, 
		AosIDO *ido,
		const bool override_flag)
{
	AosClusterObj *cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(cluster, rdata, false);

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	ido->serializeTo(rdata, buff.getPtr());

	u32 distr_id = cluster->getDistrID(rdata, obj_name);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosIDOServer", AosIDOServerObj::eMethodCreateIDOByName,
		distr_id, gRepPolicyMgr.getCubeMapRepPolicy(), cluster);

	jimo_call->arg(AosFN::eObjName, obj_name);
	jimo_call->arg(AosFN::eFlag, override_flag);
	jimo_call->arg(AosFN::eBuff, buff.getPtr());

	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess())
	{
		// Did not create. 
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		if (errmsg != "")
		{
			AosLogError(rdata, false,  "failed_creating_IDO")
				<< AosFN("obj_name") << obj_name 
				<< AosFN("Error Message") << errmsg << enderr;
		}
		else
		{
			AosLogError(rdata, false,  "failed_creating_IDO")
				<< AosFN("obj_name") << obj_name << enderr;
		}

		return false;
	}

	return true;
}

/*
bool jimoIncCounter(
		AosRundata *rdata, 
		const OmnString &counter_name,
		const i32 inc_value, 
		i64 &crt_value)
{
	AosClusterObj *cluster = jimoGetIDOCluster(rdata);
	aos_assert_rr(cluster, rdata, false);

	AosBuffPtr buff = OmnNew AosBuff();
	ido->serializeTo(rdata, buff.getPtr());

	OmnString id_str = AOSIDONAME_COUNTER;
	id_str << "_" << counter_name;

	u32 distr_id = cluster->getDistrID(rdata, id_str);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		"AosIDOServer", AosIDOServerObj::eMethodRunIDOByName,
		distr_id, gRepPolicyMgr.getCubeMapRepPolicy(), cluster);

	jimo_call->arg(AosFN::eIDOType, AOSIDONAME_COUNTER);
	jimo_call->arg(AosFN::eIDOName, counter_name);
	jimo_call->arg(AosFN::eCreationFlag, true);
	jimo_call->arg(AosFN::eIncrementValue, inc_value);
	jimo_call->arg(AosFN::eIDOFunc, "increment");

	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess())
	{
		// Did not create. 
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		if (errmsg != "")
		{
			AosLogError(rdata, false,  "failed_creating_IDO")
				<< AosFN("obj_name") << obj_name 
				<< AosFN("Error Message") << errmsg << enderr;
		}
		else
		{
			AosLogError(rdata, false,  "failed_creating_IDO")
				<< AosFN("obj_name") << obj_name << enderr;
		}

		return false;
	}

	crt_value = jimo_call->arg(rdata, AosFN::eValue, -1);
	return true;
}
*/

};


