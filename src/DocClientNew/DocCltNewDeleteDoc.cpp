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
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClientNew/DocClientNew.h"
/*
#include "API/AosApi.h"
#include "DocClientNew/DocProc.h"
#include "DocServer/DocSvr.h"
//#include "DocTrans/BatchDelDocsTrans.h"
#include "DocTrans/DeleteObjTrans.h"
#include "EventMgr/Event.h"
#include "EventMgr/EventTriggers.h"
#include "EventMgr/Event.h"
#include "SEUtil/IILName.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/SysLogName.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/LockMonitorMgr.h"
#include "XmlUtil/XmlDoc.h"

static AosDocMgrObjPtr sgDocMgr;
*/
#include "JimoAPI/JimoDocEngine.h"

bool	
AosDocClientNew::deleteObj(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const OmnString &theobjid, 
		const OmnString &container, 
		const bool deleteFlag)
{
	return Jimo::jimoDeleteObj(rdata.getPtr(), docid, theobjid, container, deleteFlag);
}


bool
AosDocClientNew::deleteDoc(
		const AosXmlTagPtr &origdocroot, 
		const AosRundataPtr &rdata, 
		const OmnString &ctnr_objid,
		const bool deleteFlag)
{
	return Jimo::jimoDeleteDoc(rdata.getPtr(), origdocroot, ctnr_objid, deleteFlag);
}


bool
AosDocClientNew::deleteBatchDocs(
			const AosRundataPtr &rdata,
			const OmnString &scanner_id)
{
	OmnNotImplementedYet;
	return false;
	//// Ketty 2013/03/14
	////OmnString docstr = "<trans ";
	////docstr << AOSTAG_TYPE << "=\"" << "batchdeldocs" <<"\" "
	////	<< "zky_scanner_id" << "=\"" << scanner_id << "\" "
	////	<< AOSTAG_SITEID <<"=\"" << rdata->getSiteid() << "\" >"
	////	<< "</trans>";

	//// Ketty 2012/11/02
	////int num_phy = AosGetNumPhysicals();
	//int num_phy = AosGetNumPhysicals();
	//
	//int i = 1;
	//while (i <= num_phy)
	//{
	//	AosTransPtr trans = OmnNew AosBatchDelDocsTrans(scanner_id, i, true, false);
	//	//bool rslt = addReq(rdata, docstr, i++, true);
	//	bool rslt = addReq(rdata, trans);
	//	aos_assert_r(rslt, false);
	//}

	//if (!rdata->isOk())
	//{
	//	return false;
	//}
	//return true;
}


bool 
AosDocClientNew::deleteBinaryDoc(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoDeleteBinaryDoc(rdata.getPtr(), docid);
}


bool 
AosDocClientNew::deleteBinaryDoc(
		const OmnString &objid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoDeleteBinaryDoc(rdata.getPtr(), objid);
}


bool
AosDocClientNew::dropJQLTable( 
		const AosRundataPtr &rdata,
		const OmnString &table_name)
{
	return Jimo::jimoDropJQLTable(rdata.getPtr(), table_name);
}


bool
AosDocClientNew::deleteDocByObjid(	
		const AosRundataPtr &rdata,
		const string &objid)
{
	return Jimo::jimoDeleteDocByObjid(rdata.getPtr(), objid);
}


bool
AosDocClientNew::deleteDocByDocid(	
		const AosRundataPtr &rdata,
		const int64_t docid)
{
	return Jimo::jimoDeleteDocByDocid(rdata.getPtr(), docid);
}

