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
// 2015/05/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocServer/DocSvr.h"
#if 0
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/ErrorMsg.h"
#include "SEInterfaces/DocFileMgrNewObj.h"

static AosVfsMgrObj* sgVfsMgr = 0;

bool
AosDocSvr::createDoc_3_1(
		AosRundata *rdata, 
		const u64 docid,
		const u64 snap_id,
		const AosBuff &doc_body)
{
	// This function is to replace 
	// 		AosDocSvr::createDoc(
	// 			AosRundataPtr &rdata, 
	// 			const AosXmlTagPtr &newdoc,
	// 			const AosTransId &trans_id, 
	// 			const u64 snap_id) 
	// It assumes it is to create a new doc. One should call modify doc
	// function if it is to modify a doc. DataletSE may check before
	// creating it.
	//
	// It verifies:
	// 	1. JPID (mandatory)
	// 	1. JSID (mandatory)
	// 	2. Docid (mandatory)
	rdata->setLogicError(true);
	u32 jpid = rdata->getJPID();
	aos_assert_rg(jpid == mJPID, rdata, false, 
			AosErrMsg("invalid_jpid")
			.field("expected_jpid", mJPID)
			.field("actual_jpid", rdata->getIPID()));
	aos_assert_rg(docid > 0, rdata, false, AosErrMsg("invalid_docid"));
	aos_assert_rg(mJimoPod.isValidJimoStore(rdata->getJSID(), docid), rdata, false, 
			AosErrMsg("invalid_jsid")
			.field("expected_jsid", rdata->getJSID())
			.field("actual_jsid", mJimoPod.getJSID(docid)));

	// Retrieve the docfilemgr. There is a docfilemgr per:
	// 	[jpid, module_id, cube]. 
	// That is, each JimoPod has: num(message_category) * cube docfilemgrs. 
	AosDocFileMgrNewObj* docfilemgr = getDocFileMgr_3_1(rdata, jpid, AosModuleId::eDoc, docid);
	aos_assert_rg(docfilemgr, rdata, false, AosErrMsg("internal_error"));
	if (!docfilemgr->createDoc(rdata, docid, snap_id, doc_body))
	{
		AosLogError(rdata, false, "failed_create_doc") << enderr;
		return false;
	}

	return true;
}


/*
bool
AosDocSvr::saveBinaryDoc_3_1(
		AosRundata *rdata,
		const u32 repository_id,
		const u64 docid,
		AosXmlTag* doc,
		AosBuff* binary_data,
		OmnString &signature,
		const u64 snap_id) 
{
	// This function is the new version of 
	//
	// bool
	// AosDocSvr::saveBinaryDoc(
	// 		const AosXmlTagPtr &doc,
	// 		const AosBuffPtr &buff,
	// 		OmnString &signature,
	// 		const AosRundataPtr &rdata,
	// 		const AosTransId &trans_id,
	// 		const u64 snap_id) 
	// A binary doc is a doc with a (normally) large binary data block. 
	// The binary data block is 'binary_data'. It saves the binary doc in 
	// two parts: the binary data and the XML doc. The XML doc keeps
	// the metadata for the binary data. 
	//
	// Note that the binary data may be empty.

	aos_very_likely(binary_data->dataLen() > 0);
	aos_assert_r(docid > 0, false);

	// 1. The signature is SH1(aname, secret_key). The results are then converted
	// 	  through base64 and saved in 'sign_name'. If 'binary_data' is empty, signature
	//    is set to empty.
	if (binary_data->dataLen() > 0)
	{
		signature = AosSecUtil::signValue(binary_data->data(), binary_data->dataLen());
		aos_assert_r(signature != "", false);
	}
	else
	{
		signature = "";
	}

	// 2. A binary doc has two blocks of data: the XML doc and the binrary doc. 
	//    Both are stored in the same DocFileMgr. The DocFileMgr is retrieved
	//    based on the CubeID. In Release 3.1, DocFileMgr is a proxy. It packages
	//    the data into a block of data and passes it to Raft. Raft will then 
	//    pass the request to the corresponding DocFileMgr. 
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	AosDocFileMgrObj* docfilemgr = getDocFileMgrByVid(vid, sgBinayrDocDfmConf, rdata);
	u64 did = parseDocid_3_1(docid); 
	aos_assert_rr(docfilemgr, rdata, false);

	bool rslt = AosXmlDoc::saveBinaryDoc(did, binary_data, docfilemgr, snap_id, trans_id, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "file_save_to_file") << enderr;
		return false;
	}
	return true;
}
*/


u64
AosDocSvr::parseDocid_3_1(const u64 &docid)
{
	// It converts the global docid 'docid' into its local docid. 
	// Note that the highest byte is reserved for doc types. This
	// type should remain the same when converting the docid.
	AosDocType::E type = AosDocType::getDocType(docid);
	u64 id = AosXmlDoc::getOwnDocid(docid);

	u64 local_id = id/AosGetNumCubes();
	AosDocType::setDocidType(type, local_id);
	return local_id;
}


AosDocFileMgrNewObj*
AosDocSvr::getDocFileMgr_3_1(
		AosRundata *rdata,
		const u32 repository_id,
		const int module_id,
		const u64 docid)
{
	aos_assert_rr(sgVfsMgr, rdata, 0);

	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	aos_assert_rg(repository_id > 0 && repository_id < mMaxRepositories, rdata, 0, 
			AosErrMsg("invalid_repository_id")
			.field("repository_id", repository_id)
			.field("current_repository_id", mMaxRepositories));

	aos_assert_rg(mDfms[repository_id].isValid(), rdata, 0,
			AosErrMsg("invalid_repository_id")
			.field("repository_id", repository_id)
			.field("Docid", docid)
			.field("current_repository_id", mMaxRepositories));

	AosDocFileMgrObj *dfm = mDfms[repository_id]->getDFM(rdata, module_id, vid);
	if (dfm) return dfm;

	aos_assert_rg(sgVfsMgr, rdata, 0, AosErrMsg("internal_error"));
	aos_assert_rg(module_id > 0 && module_id < eMaxDfmModuleIDs, 
			rdata, 0, 
			AosErrMsg("invalid_module_id")
			.field("ModuleID", module_id)
			.field("Repository ID", repository_id)
			.field("Docid", docid));

	dfm = sgVfsMgr->createDocFileMgr(rdata, repository_id, vid, config);
	aos_assert_rg(dfm, rdata, 0, 
			AosErrMsgMsg("failed_creating_docfilemgr")
			.field("RepositoryID", repository_id)
			.field("Virtual ID")); 

	mDfmMaps[repository_id].set(repository_id, module_id, vid, dfm);

	return dfm;
}
#endif
