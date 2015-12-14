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
// 2014/11/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClient/DocTransHandler.h"

#include "DocMgr/DocMgr.h"
#include "DocClient/DocClient.h"
#include "DocClient/DocProc.h"
#include "DocTrans/CreateDocTrans.h"
#include "DocTrans/CreateDocSafeTrans.h"


static AosDocMgrObj* sgDocMgr;

AosDocTransHandler::AosDocTransHandler()
{
}


AosDocTransHandler::~AosDocTransHandler()
{
}


bool
AosDocTransHandler::createDoc(
		AosRundata *rdata, 
		const u64 userid,
		AosDocClient *doc_client, 
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &newdoc, 
		const u64 docid) 
{
	// This function creates a doc.
	u64 snap_id = rdata->getSnapshotId();
	if (OmnApp::getDocClientVersion() == OmnApp::eDftDocClientVersion)
	{
		// Send a transaction to the backend to actually create the doc.
		AosTransPtr trans = OmnNew AosCreateDocTrans(docid, newdoc, true, false, snap_id);
		bool rslt = doc_client->addReq(rdata, trans);
		aos_assert_r(rslt, false);
	}
	else
	{
		OmnNotImplementedYet;
		return false;
	}

	// The doc has been created successfully. Add it to DocMgr. 
	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr().getPtr();
	aos_assert_r(sgDocMgr, false);
	sgDocMgr->addCopy(docid, newdoc, snap_id, rdata);
	aos_assert_r(docid, false);

	AosDocReqPtr doc_req = AosDocProc::getSelf()->addCreateRequest(
	 		root, userid, newdoc, 0, 0, rdata);
	aos_assert_rr(doc_req, rdata, false);

	return true;
}


bool 
AosDocTransHandler::createDoc( 	
		AosRundata *rdata, 
		AosDocClient *doc_client, 
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &newdoc, 
		const bool save_doc_flag,
		const u64 docid)
{
	u64 snap_id = rdata->getSnapshotId();
	if (OmnApp::getDocClientVersion() == OmnApp::eDftDocClientVersion)
	{
		// Send a transaction to the backend to actually create the doc.
		AosTransPtr trans = OmnNew AosCreateDocSafeTrans(docid,
				newdoc, save_doc_flag, true, false, snap_id);
		bool rslt = doc_client->addReq(rdata, trans);
		aos_assert_r(rslt, false);
	}
	else
	{
		OmnNotImplementedYet;
		return false;
	}

	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr().getPtr();
	aos_assert_r(sgDocMgr, 0);
	sgDocMgr->addCopy(docid, newdoc, snap_id, rdata);

	AosDocReqPtr doc_req = AosDocProc::getSelf()->addCreateRequest(root,
			rdata->getUserid(), newdoc, 0, 0, rdata);
	aos_assert_rr(doc_req, rdata, 0);

	return true;
}

