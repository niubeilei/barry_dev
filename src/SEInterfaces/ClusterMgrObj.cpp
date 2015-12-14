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
// 2015/03/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ClusterMgrObj.h"

#include "Jimo/GlobalFuncs.h"
#include "Thread/Mutex.h"

static AosClusterMgrObjPtr sgClusterMgr = 0;
static AosClusterMgrObj* sgClusterMgrRaw = 0;
static OmnMutex sgLock;


AosClusterMgrObj::AosClusterMgrObj(const int version)
:
AosJimo(AosJimoType::eClusterMgr, version)
{
	mJimoType = AosJimoType::eClusterMgr;
}


/*
static bool sgRetrieveClusterMgr(AosRundata *rdata)
{
	sgLock.lock();
	if (sgClusterMgr)
	{
		sgLock.unlock();
		return true;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosClusterMgr", 1);
	if (!jimo)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}


	AosClusterMgrObj *clusterMgr = 0;
	clusterMgr = dynamic_cast<AosClusterMgrObj *>(jimo.getPtr());
	if (!clusterMgr)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	sgClusterMgr = AosClusterMgrObjPtr(clusterMgr, false); ;
	sgLock.unlock();
	return true;
}
*/


AosClusterMgrObj *
AosClusterMgrObj::getClusterMgr(AosRundata *rdata)
{
	// if (!sgClusterMgr) sgRetrieveClusterMgr(rdata);
	// aos_assert_rr(sgClusterMgr, rdata, 0);
	// return sgClusterMgr.getPtr();
	return sgClusterMgrRaw;
}


void
AosClusterMgrObj::setClusterMgr(const AosClusterMgrObjPtr &cluster_mgr)
{
	sgClusterMgr = cluster_mgr;
	sgClusterMgrRaw = cluster_mgr.getPtr();
}


