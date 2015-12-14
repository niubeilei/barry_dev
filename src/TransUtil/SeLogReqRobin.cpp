////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//	Description:
//	There are different ways to distribute the request about selog.
//	create log:
//		create a log need to know the container's docid.
//		distribute the request:
//			docid%mNumVirSvrs%mNumModules.
//	retrieve log:
//		We retrieve a log by logid. Logid is created by seqno, offset 
//		and moduleid. Here we split logid and get the moduleid, then 
//		get the physvrid.
//
//
// Modification History:
// 12/15/2011 	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/SeLogReqRobin.h"

#include "alarm_c/alarm.h"
#include "API/AosApiG.h"
#include "SeLogUtil/LogOpr.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SeLogUtil/LogId.h"
#include "SEUtil/SeTypes.h"
#include <map>


AosSeLogReqRobin::AosSeLogReqRobin()
{
	//mNumPhys = AosGetNumPhysicals();
	//aos_assert(mNumPhys >0 && mNumVirs>0);
	
	mSvrIds = AosGetServerIds();
	mCrtItr = mSvrIds.begin();
	
	mNumVirs = AosGetNumCubes();
}


AosSeLogReqRobin::~AosSeLogReqRobin()
{
}


int
AosSeLogReqRobin::routeReq(const u64 &distid)
{
	// SeLog distribute is different form others.
	// When add a log, we use ctnr's docid%vir_svr%serv.
	// When retrieve a log, we use logid to get moduleid,
	// and moduleid%num_ser to get server
	// Chen Ding, 02/29/2012
	// AosDocType type = AosXmlDoc::getDocType(distid);
	
	// This Func is just get the virtual_id, not the svr_id.
	u32 svr_num = mSvrIds.size();
	if(svr_num == 0)	return -1;

	u32 virtual_id = 0;
	AosDocType::E type = AosDocType::getDocType(distid);
	if (type == AosDocType::eLogDoc)
	{
		u32 seqno, offset, moduleid;
		AosLogId::decomposeLogid(distid, seqno, offset, moduleid);

		// Ketty 2012/11/27.
		// the moduleId is the virtual_id.
		//u32 idx = moduleid % svr_num;
		//return mSvrIds[idx];
		virtual_id = moduleid;
	}
	else
	{
		virtual_id = distid % mNumVirs;
	}

	return virtual_id;
	//return AosGetPhysicalIdByVirtualId(virtual_id); 
}

