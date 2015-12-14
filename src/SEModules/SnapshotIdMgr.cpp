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
// 07/29/2010: Created by James Kong
////////////////////////////////////////////////////////////////////////////
#include "SEModules/SnapshotIdMgr.h"

#include "Alarm/Alarm.h"
#include "MultiLang/LangTermIds.h"
#include "Porting/TimeOfDay.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Objid.h"
#include "SEUtil/ValueDefs.h"
#include "SEUtil/Docid.h"
#include "SearchEngine/Ptrs.h"
#include "SEServer/SeReqProc.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SEModules/ObjMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "API/AosApi.h"

OmnSingletonImpl(AosSnapshotIdMgrSingleton,
                 AosSnapshotIdMgr,
                 AosSnapshotIdMgrSelf,
                "AosSnapshotIdMgr");


AosSnapshotIdMgr::AosSnapshotIdMgr()
{
}


AosSnapshotIdMgr::~AosSnapshotIdMgr()
{
}


bool      	
AosSnapshotIdMgr::start()
{
	return true;
}


bool        
AosSnapshotIdMgr::stop()
{
	return true;
}


bool
AosSnapshotIdMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


u64 
AosSnapshotIdMgr::createSnapshotId(const AosRundataPtr &rdata)
{
	// 2015/01/14, Ken Lee
	return 0;
	OmnString iilname = AOSZTG_SNAPSHOTID;
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	aos_assert_r(query_context, 0);

	query_context->setOpr(eAosOpr_an);
	query_context->setReverse(false);
	query_context->setStrValue("*");
	query_context->setBlockSize(10000);

	u64 docid;
	bool rslt = true;
	bool finished = false;
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic(); 
	rslt = AosQueryColumn(iilname, query_rslt, 0, query_context, rdata);   
	aos_assert_r(rslt, 0);
	u64 snap_id = 0;

	while(1)
	{
		rslt = query_rslt->nextDocid(docid, finished, rdata);
		aos_assert_r(rslt, 0);
		
		if (finished)
		{
			snap_id = docid + 1;
			break;
		}

		if (docid != snap_id+1) 
		{
			snap_id = docid;
			break;
		}
	}

OmnScreen << " ###### create snap_id : " << snap_id << endl;

	aos_assert_r(snap_id, 0);
	aos_assert_r(snap_id <= 10000, 0);

	AosIILClientObj::getIILClient()->addU64ValueDoc(iilname, snap_id, snap_id, true, true, rdata);
	return snap_id;
}


bool
AosSnapshotIdMgr::deleteSnapshotId(
		const u64 snap_id,
		const AosRundataPtr &rdata)
{
	if (snap_id == 0) return true;
	OmnString iilname = AOSZTG_SNAPSHOTID;

	aos_assert_r(snap_id <= 10000, false);

	//bool rslt = AosIILClientObj::getIILClient()->removeU64ValueDoc(iilname, snap_id, snap_id, rdata);
	//bool rslt = AosIILClient::getSelf()->removeU64ValueDoc(iilname, snap_id, snap_id, rdata);
	bool rslt = AosIILClientObj::getIILClient()->removeU64ValueDoc(iilname, snap_id, snap_id, rdata);
	aos_assert_r(rslt, false);
OmnScreen << " ###### delete snap_id : " << snap_id << endl;
	return true;
}
