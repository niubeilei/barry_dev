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
// 06/12/2011	Created by Tracy Huang
////////////////////////////////////////////////////////////////////////////
#include "SQLServer/SqlServer.h"

#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "MySQL/DataStoreMySQL.h"

#include "SingletonClass/SingletonMgr.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"
#include "SqlUtil/SqlReqid.h"
#include "SQLServer/SqlProc.h"

#include "Porting/Sleep.h"
#include "TransServer/TransServer.h"
#include "Rundata/Rundata.h"


AosSqlServer::AosSqlServer()
{
	
	OmnString errmsg;
	if (!startDatabase())
	{
		OmnExcept e(__FILE__, __LINE__, errmsg);
		throw e;
	}

}


AosSqlServer::~AosSqlServer()
{
}


bool
AosSqlServer::startDatabase()
{
	OmnDataStoreMySQL::startMySQL("root", "chen0318", "192.168.99.16", 3306, "dbtest");
	return true;
}


bool
AosSqlServer::proc(
		const AosTransPtr &trans, 
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata)
{
	// Retrieved the transaciton XML, which should be in the form:
	// 	<request type="xxx" ...>
	// 		<sdoc type='importdata'.../>
	//  </request>
	// trans->setStatus(AosTrans::eTransProcessed);
	
	aos_assert_r(trans, false);
	AosXmlTagPtr sdoc =	data->getFirstChild(); 
	AosSqlProcPtr proc = AosSqlProc::getProc(sdoc->getAttrStr("type"));
	cout << "type = "<<sdoc->getAttrStr("type")<<endl;
	if (!proc)
	{
		return false;
	}

	rdata->setReceivedDoc(sdoc);
	proc->proc(trans, rdata);
	sendResponse(trans, rdata);
	return true;
}


bool
AosSqlServer::sendResponse(
		const AosTransPtr &trans, 
		const AosRundataPtr &rdata)
{
	return false;
}


