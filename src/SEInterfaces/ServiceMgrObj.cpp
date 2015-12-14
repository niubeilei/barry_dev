////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/08/14 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ServiceMgrObj.h"

AosServiceMgrObjPtr AosServiceMgrObj::smServiceMgr;

bool 
AosServiceMgrObj::addServiceStatic(
			const u64 &service_docid,
			const AosRundataPtr &rdata)
{
	aos_assert_r(smServiceMgr, false);
	return smServiceMgr->addService(service_docid, rdata);
}


bool
AosServiceMgrObj::startTaskProcCbStatic(const u32 logic_pid, const int proc_pid, const int svr_id) 
{
	aos_assert_r(smServiceMgr, false);
	return smServiceMgr->startTaskProcCb(logic_pid, proc_pid, svr_id);
}

bool
AosServiceMgrObj::stopTaskProcCbStatic(const u32 logic_pid, const int svr_id) 
{
	aos_assert_r(smServiceMgr, false);
	return smServiceMgr->stopTaskProcCb(logic_pid, svr_id);
}
