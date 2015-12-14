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
// 03/25/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/AllProcess.h"

#include "SvrProxyMgr/CubeProcess.h"
#include "SvrProxyMgr/BkpCubeProcess.h"
#include "SvrProxyMgr/FrontEndProcess.h"
#include "SvrProxyMgr/AdminProcess.h"
#include "SvrProxyMgr/TaskProcess.h"
#include "SvrProxyMgr/TorturerProcess.h"
#include "SvrProxyMgr/MsgProcess.h"
#include "SvrProxyMgr/IndexEngineProcess.h"
#include "SvrProxyMgr/DocEngineProcess.h"
#include "SvrProxyMgr/ServiceProcess.h"

AosProcessPtr 	sgProc[AosProcessType::eMax];	
AosAllProcess   gAosAllProcess;

AosAllProcess::AosAllProcess()
{
	static AosCubeProcess			sgAosCubeProcess(true);
	static AosBkpCubeProcess		sgAosBkpCubeProcess(true);
	static AosFrontEndProcess		sgAosFrontEndProcess(true);
	static AosAdminProcess			sgAosAdminProcess(true);
	static AosTaskProcess			sgAosTaskProcess(true);
	static AosTorturerProcess		sgAosTorturerProcess(true);
	static AosServiceProcess		sgAosServiceProcess(true);
	//static AosMsgProcess			sgAosMsgProcess(true);
	static AosIndexEngineProcess	sgAosIndexEngineProcess(true);
	static AosDocEngineProcess		sgAosDocEngineProcess(true);
}

