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
// 01/02/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DocClientObj.h"

#include "Rundata/Rundata.h"



AosDocClientObj* AosDocClientObj::smDocClient;
AosDocClientObjPtr AosDocClientObj::smDocClientPtr;


bool 
AosDocClientObj::removeDocByDocidStatic(
		const AosRundataPtr &rdata, 
		const u64 docid)
{
	aos_assert_rr(smDocClient, rdata, false);
	return smDocClient->deleteDocByDocid(rdata, docid);
}


void 
AosDocClientObj::setDocClient(const AosDocClientObjPtr &d)
{
	smDocClient = d.getPtrNoLock();
	smDocClientPtr = d;
}


bool 
AosDocClientObj::createDoc(
		AosRundata *rdata,                                   
		const AosXmlTagPtr &newdoc,
		const OmnString &objid,
		u64 &docid,
		const bool keep_objid,
		const u64 snap_id)
{
	return createDoc1(rdata, 0, "", newdoc, true, 0, 0, true);
}

/*
void
AosDocClientObj::setDocClient(const AosDocClientObjPtr &doc_client)
{
	sgDocClient = doc_client;
}


AosDocClientObjPtr 
AosDocClientObj::getDocClient()
{
	return sgDocClient;
}
*/
