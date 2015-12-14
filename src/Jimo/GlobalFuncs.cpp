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
// 2013/11/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Jimo/Jimo.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DllMgrObj.h"
#include "XmlUtil/XmlTag.h"


// Chen Ding, 2014/12/19
AosJimoPtr AosCreateJimoByClassname(
		const AosRundataPtr &rdata, 
		const OmnString &classname, 
		const int version)
{
	return AosCreateJimoByClassname(rdata.getPtrNoLock(), classname, version);
}

// Chen Ding, 2014/11/30
AosJimoPtr AosCreateJimoByName(
		AosRundata *rdata, 
		const OmnString &jimo_name, 
		const int version) 
{
	AosDllMgrObj *mgr = AosDllMgrObj::getDllMgr1();
	aos_assert_rr(mgr, rdata, 0);
	return mgr->createJimoByName(rdata, AOSSYSTABLE_JIMOS, "jimo_name", jimo_name, version);
}

//xuqi, 2015/9/12
AosJimoPtr AosCreateJimoByName(
		AosRundata *rdata, 
		const OmnString &jimo_namespace,
		const OmnString &jimo_name, 
		const int version) 
{
	AosDllMgrObj *mgr = AosDllMgrObj::getDllMgr1();
	aos_assert_rr(mgr, rdata, 0);
	return mgr->createJimoByName(rdata, jimo_namespace, jimo_name, version);
}

AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const OmnString &jimo_objid, 
		const int version) 
{
	AosXmlTagPtr doc = AosGetDocByObjid(jimo_objid, rdata);
	if (!doc) return 0;

	OmnString otype = doc->getAttrStr(AOSTAG_OTYPE, "");
	if (otype == AOSOTYPE_JIMO)
	{
		// The doc is a jimo. It is to retrieve a jimo without a worker.
		return AosCreateJimo(rdata, 0, doc);
	}

	return AosCreateJimo(rdata, doc, version);
}


AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const int version) 
{
	AosDllMgrObj *mgr = AosDllMgrObj::getDllMgr1();
	aos_assert_rr(mgr, rdata, 0);
	return mgr->createJimo(rdata.getPtr(), worker_doc, version);
}


AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	AosDllMgrObj *mgr = AosDllMgrObj::getDllMgr1();
	aos_assert_rr(mgr, rdata, 0);
	return mgr->createJimo(rdata.getPtr(), worker_doc, jimo_doc, -1);
}

AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc,
		const int version)
{
	AosDllMgrObj *mgr = AosDllMgrObj::getDllMgr1();
	aos_assert_rr(mgr, rdata, 0);
	return mgr->createJimo(rdata.getPtr(), worker_doc, jimo_doc, version);
}


AosJimoPtr AosCreateJimo(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const OmnString &jimo_objid)
{
	aos_assert_rr(worker_doc, rdata, 0);
	AosXmlTagPtr jimo_doc = AosGetDocByObjid(jimo_objid, rdata);
	if (!jimo_doc)
	{
		AosSetErrorUser(rdata, "jimo_not_found") << jimo_objid
			<< ". Worker Doc: " << worker_doc->toString() << enderr;
		return 0;
	}

	return AosCreateJimo(rdata, worker_doc, jimo_doc);
}


AosJimoPtr AosCreateJimoByClassname(
		AosRundata *rdata, 
		const OmnString &classname, 
		const int version)
{
	AosDllMgrObj *dllmgr = AosDllMgrObj::getDllMgr1();
	aos_assert_rr(dllmgr, rdata, 0);
	return dllmgr->createJimoByClassname(rdata, classname, version);
}

