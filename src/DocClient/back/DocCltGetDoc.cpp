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
// 07/20/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClient/DocClient.h"

#include "CompressUtil/Compress.h"
#include "DocClient/DocidMgr.h"
#include "DocLock/DocLock.h"
#include "DocServer/DocSvr.h"
#include "DocTrans/GetDocTrans.h"
#include "DocTrans/GetFixedDocTrans.h"
#include "DocTrans/GetCSVDocTrans.h"
#include "DocTrans/GetCommonDocTrans.h"
#include "DocTrans/BinaryDocAsyncTrans.h"
#include "DocTrans/BatchGetDocsAsyncTrans.h"
#include "EventMgr/Event.h"
#include "EventMgr/EventTriggers.h"
#include "EventMgr/Event.h"
#include "Thread/ThreadMgr.h"
#include "SearchEngine/DocServer1.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/UserMgmtObj.h"
#include "SEInterfaces/UserAcctObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SmartDoc/SmartDoc.h"
#include "UserMgmt/UserAcctMgr.h"
#include "Util/Opr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlUtil.h"
#include "XmlUtil/SystemDoc.h"
#include "XmlUtil/SystemDocMgr.h"
#include "XmlUtil/DocTypes.h"
#include "SEUtil/JqlTypes.h"
#include "Query/Ptrs.h"
#include "Query/TermAnd.h"
#include "Query/TermArith.h"
#include "SEUtil/IILName.h"
#include "SEInterfaces/TaskObj.h"

static AosDocMgrObjPtr sgDocMgr;

AosXmlTagPtr
AosDocClient::getDoc(
		const AosRundataPtr &rdata,
		const OmnString &docid, 
		const OmnString &objid, 
		bool &duplicated)
{
	// This function retrieves the entire document. 
	// If 'docid' is not empty, it uses 'docid' to retrieve the doc.
	// Otherwise, it uses [siteid, objid]. If the doc is retrieved, 
	// it will hold the 'header' through which the doc was retrieved.
	//
	// This function will not lock mLock. 
	//
	// Note that 'objid' should be the full objid.
	//
	// AOSMONITORLOG_ENTER(rdata);
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, 0);

	if (mIsStopping)
	{
		rdata->setError() << "Server is stopping";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	bool rslt;
	bool isunique = false;
	u64 did;
	if (docid == "")
	{
		// 'docid' is empty. Use [siteid, objid] to retrieve the doc.
		if(objid == "")
		{
			AosLogUserError(rdata, "no_identifiers_specified") << enderr;
			return 0;	
		}
		rslt = AosIILClientObj::getIILClient()->getDocidByObjid(
				siteid, objid, did, isunique, rdata);
		aos_assert_rr(rslt, rdata, NULL);
	}
	else
	{
		did = atoll(docid.data());
		isunique = true;
		rslt = true;
	}

	if (did == AOS_INVDID)
	{
		AosLogUserError(rdata, "invalid_docid") 
			<< "Objid: " << objid 
			<< ". Docid: " << docid << enderr;
		return 0;
	}

	if (docid == "" && !isunique)
	{
		AosLogUserError(rdata, "objid_not_unique") 
			<< "Objid: " << objid << enderr;
		return 0;
	}

	// Check whether it is a tmp obj
	AosXmlTagPtr thedoc = getDocByDocid(did, rdata);;
	if (AosDocType::isTempDoc(did))
	{
		thedoc = getTempDoc(did, rdata);
	}

	if (!thedoc)
	{
		AosLogUserError(rdata, "object_not_found") << enderr;
		return 0;
	}

	if (mIsRepairing)
	{
		return thedoc;
	}

	// Chen Ding, 01/21/2012
	if (!thedoc->isRootTag())
	{
		AosSetErrorUser(rdata, "internal_error") << objid << enderr;
		AOSLOG_LEAVE(rdata);
		thedoc = 0;
		return 0;
	}

	// Chen Ding, 01/21/2012
	rdata->setWorkingDoc(thedoc, false);
	rdata->setDocByVar(AOSARG_OLD_DOC, thedoc, false);
	
	if (!isLog(did) && !AosSecurityMgrObj::getSecurityMgr()->checkRead(thedoc, rdata)) 
	{
		// Access denied
		// AOSMONITORLOG_FINISH(rdata);
		thedoc = 0;
		return 0;
	}

	if (!mIsRepairing && !checkDocLock(rdata, did, "checkread"))
	{
		// AOSMONITORLOG_FINISH(rdata);
		thedoc = 0;
		return 0;
	}
	if (!AosEvent::checkEvent1(0, thedoc, AOSEVENTTRIGGER_DOC_PRE_READ, rdata))
	{
		rdata->setError() << "Failed to Container Member Verification";
		// AOSMONITORLOG_FINISH(rdata);
		thedoc = 0;
		return 0;
	}
	if (rdata->getArg1(AOSTAG_STATUS_RETURN) == "return")
	{                                                    
		thedoc = 0;
	    return 0;
	}

	rdata->setOk();

	if (!AosEvent::checkEvent1(0, thedoc, AOSEVENTTRIGGER_DOC_READ, rdata))
	{
		rdata->setError() << "Failed to Container Member Verification";
	}
	return thedoc;
}


u64 
AosDocClient::getDocidByCloudid(
		const u32 siteid, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(cid!="", 0);
	OmnString iilname = AosIILName::composeCloudidListingName(siteid);
	u64 docid = 0;
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, cid, docid, rdata);
	aos_assert_r(rslt, 0);
	return docid;
}


AosXmlTagPtr 
AosDocClient::getParentDoc(
		const OmnString &objid, 
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	// It retrieves the doc's parent doc. 
	AosXmlTagPtr doc;
	if (docid)
	{
		doc = getDocByDocid(docid, rdata);
	}
	else
	{
		if (objid == "")
		{
			AosSetError(rdata, AosErrmsgId::eNoObjectSpecified);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		doc = getDocByObjid(objid, rdata);
	}

	if (!doc) return 0;

	OmnString parent_objid = doc->getAttrStr(AOSTAG_PARENTC);
	if (parent_objid == "")
	{
		rdata->setError() << "No parent found!";
		doc = 0;
		return NULL;
	}

	doc = 0;
	return getDocByObjid(parent_objid, rdata);
}


AosXmlTagPtr 
AosDocClient::getDeletedDoc(
		const OmnString &objid, 
		const u64 &did, 
		const AosRundataPtr &rdata)
{
	// All deleted docs are stored in the IIL:
	// 	AosIILName::composeDeletedObjidIILName().
	//
	// This function retrieves the docid from this iil. If not found, it is not
	// a deleted doc. Otherwise, it retrieves the deleted doc.
	u64 docid = did;
	if (docid == 0)
	{
		OmnString iilname = AosIILName::composeDeletedObjidIILName();
		aos_assert_rr(iilname != "", rdata, NULL);
		bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, objid, docid, rdata);
		if (!rslt) 
		{
			rdata->setError() << "Failed retrieving the deleted doc docid: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
	}

	return getDocByDocid(docid, rdata);
}


AosUserAcctObjPtr
AosDocClient::getUserAcct(
		const u64 &user_docid,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr userdoc = getDocByDocid(user_docid, rdata);
	if (!userdoc) return 0;
	AosUserMgmtObjPtr usrmgmt_obj = AosUserMgmtObj::getUserMgmt();
	aos_assert_r(usrmgmt_obj, 0);
	return usrmgmt_obj->getUserAcct(userdoc, rdata);
}


AosXmlTagPtr
AosDocClient::getDocByCloudid(
		const OmnString &cloudid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(cloudid != "", 0);
	bool isunique = false;
	u64 docid = AOS_INVDID;
	AosIILClientObj::getIILClient()->getDocidByCloudid(rdata->getSiteid(), cloudid, docid, isunique, rdata);
	if (docid != AOS_INVDID)
	{
		AosXmlTagPtr doc = getDocByDocid(docid, rdata);
		aos_assert_r(doc, NULL);
		aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT, 0);
		aos_assert_r(doc->getAttrStr(AOSTAG_CLOUDID) == cloudid, 0);
		return doc;
	}

	// Backward compatibility
	OmnString objid = AosObjid::compose(AOSTAG_USERPREFIX, cloudid);
	aos_assert_r(objid!="", NULL);
	return getDocByObjid(objid, rdata);
}


AosXmlTagPtr
AosDocClient::getDocByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	// It retrieves the doc identified by 'objid'. 
	aos_assert_r(objid != "", 0);

	if (mShowLog) OmnScreen << "GetDocByObjid: " << objid << endl;

	if (OmnApp::getBindObjidVersion() == OmnApp::eDftBindObjidVersion)
	{
		u32 siteid = rdata->getSiteid();
		aos_assert_r(siteid, 0);

		bool isunique = false;
		u64 docid = AOS_INVDID;
		bool rslt = AosIILClientObj::getIILClient()->getDocidByObjid(
			siteid, objid, docid, isunique, rdata);
		if (!rslt) return NULL;
		if (!isunique) return NULL;
		if (docid == AOS_INVDID) return NULL;
		return getDocByDocid(docid, rdata);
	}

	AosBuffPtr resp;
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosGetDocTrans(objid, false, snap_id);
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	if (!rslt)
	{
		u64 docid = resp->getU64(0);
		if (docid == AOS_INVDID) return 0;
		return getDocByDocid(docid, rdata);
	}

	u32 doc_len = resp->getU32(0);
	aos_assert_r(doc_len, 0);
	AosBuffPtr doc_buff = resp->getBuff(doc_len, false AosMemoryCheckerArgs);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(doc_buff->data(), doc_len, "" AosMemoryCheckerArgs);
	aos_assert_r(doc && doc->getAttrStr(AOSTAG_OBJID, "") == objid, 0);
	aos_assert_r(doc->isRootTag(), 0);
	aos_assert_r(!doc->isDeleted(), 0);
	
	return doc;
}


AosXmlTagPtr 
AosDocClient::getLogByDocid(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid, 0);
	return 0;
}


AosXmlTagPtr 
AosDocClient::getDocByDocid(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid, 0);

	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, 0);

	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid, 0);
	
	AosXmlTagPtr doc = 0; //sgDocMgr->getDocByDocid(docid, rdata);
	if (doc)
	{
		// Chen Ding, 2013/05/18
		if (doc->isRootTag()) 
		{
			if (!doc->isDeleted()) 
			{
				return doc;
			}
			
			OmnAlarm << "Doc already deleted: " << docid << enderr;
		}
		else
		{
			OmnAlarm << "Doc not a root: " << docid << enderr;
		}
	}

	// Need to retrieve the doc from the server.
	if (isLog(docid))
	{
		// Chen Ding, 2013/06/15
		// Log entries cannot be mixed with normal docs for now.
		AosSetError(rdata, "feature not supported");
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return 0;
	}

	AosDocType::E type = AosDocType::getDocType(docid);
	bool rslt = AosDocType::isValid(type);
	aos_assert_r(rslt, 0);
	
	if (mShowLog) OmnScreen << "GetDocByDocid: " << docid << endl;

	AosBuffPtr resp;
	AosTransPtr trans;
	u64 snap_id = rdata->getSnapshotId();

	if (type == AosDocType::eGroupedDoc)
	{
		int record_len = AosGetDataRecordLenByDocid(rdata->getSiteid(), docid, rdata);
		trans = OmnNew AosGetCommonDocTrans(docid, record_len, true, true, snap_id);
	}
	else
	{
		trans = OmnNew AosGetDocTrans(docid, false, snap_id);
	}

	rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, 0);
	
	rslt = resp->getU8(0);
	if(!rslt)	return 0;
	
	if (type == AosDocType::eGroupedDoc)
	{
		u64 schema_docid = resp->getU64(0);
		aos_assert_r(schema_docid, 0);
		u32 data_len = resp->getU32(0);
		aos_assert_r(data_len, 0);
		AosBuffPtr doc_buff = resp->getBuff(data_len, false AosMemoryCheckerArgs);
		aos_assert_r(doc_buff, 0);
		AosXmlParser parser;
		doc = parser.parse(doc_buff->data(), data_len, "" AosMemoryCheckerArgs);

		AosXmlTagPtr schema_doc = AosGetDocByDocid(schema_docid, rdata);
		aos_assert_r(schema_doc, 0);

		AosXmlTagPtr rcd_doc = schema_doc->getFirstChild("datarecord");
		aos_assert_r(rcd_doc, 0);

		AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(type);
		aos_assert_r(engine, 0);

		AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(rcd_doc, 0, rdata.getPtr());
		aos_assert_r(record, 0);

		doc = engine->serializeToXmlDoc(doc, record, rdata);
	}
	else
	{
		u32 doc_len = resp->getU32(0);
		aos_assert_r(doc_len, 0);

		AosBuffPtr doc_buff = resp->getBuff(doc_len, false AosMemoryCheckerArgs);
		AosXmlParser parser;
		doc = parser.parse(doc_buff->data(), doc_len, "" AosMemoryCheckerArgs);

	}

	aos_assert_r(doc && doc->getAttrU64(AOSTAG_DOCID, 0) == docid, 0);
	aos_assert_r(doc->isRootTag(), 0);
	aos_assert_r(!doc->isDeleted(), 0);

	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, 0);
	sgDocMgr->addCopy(docid, doc, snap_id, rdata);

	return doc;
}


OmnString
AosDocClient::getCloudid(const u64 &user_docid, const AosRundataPtr &rdata)
{
	AosUserAcctObjPtr acct = getUserAcct(user_docid, rdata);
	if (!acct) return "";
	return acct->getCloudid();
}


AosSystemDocPtr 
AosDocClient::getSystemDocByDocid(
		const u64 &ref_docid, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(ref_docid, rdata, 0);
	OmnString objid = AosObjid::composeSystemDocObjid(ref_docid);
	aos_assert_rr(objid != "", rdata, 0);
	AosXmlTagPtr doc = getDocByObjid(objid, rdata);
	if (!doc)
	{
		AosSetError(rdata, AosErrmsgId::eFailedRetrieveSystemDoc) << ". Objid: " << objid;
		return 0;
	}

	return AosSystemDocMgr::getSelf()->getSystemDoc(doc, rdata);
}


u64
AosDocClient::getDocidByObjid(
		const OmnString &objid, 
		const AosRundataPtr &rdata)
{
	// It retrieves the doc identified by 'objid'. 
	aos_assert_r(objid!="", 0);
	bool isunique = false;
	u64 docid = AOS_INVDID;
	bool rslt = AosIILClientObj::getIILClient()->getDocidByObjid(
			rdata->getSiteid(), objid, docid, isunique, rdata);
	if (!rslt) return 0;
	if (!isunique) return 0;
	if (docid == AOS_INVDID) return 0;
	return docid;
}


AosBuffPtr 
AosDocClient::batchGetDocs(
		const OmnString &scanner_id,
		const int server_id,
		const AosRundataPtr &rdata)
{
	//Linda, 2013/09/23
	OmnNotImplementedYet;
	return 0;
}


void
AosDocClient::batchGetDocs(
		const AosRundataPtr &rdata,
		const int vid, 
		const u32 num_docs,
		const u32 blocksize,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &sizeid_len,
		const AosAsyncRespCallerPtr &resp_caller)
{
//	aos_assert(docid_buff->dataLen() / sizeof(u64) == num_docs);
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosBatchGetDocsAsyncTrans(
			resp_caller, vid, num_docs, blocksize, docid_buff, sizeid_len, snap_id, false, false);
	AosSendTransAsyncResp(rdata, trans);
}


AosXmlTagPtr
AosDocClient::retrieveBinaryDoc(
		const OmnString &objid, 
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(objid != "", 0);

	u64 docid = getDocidByObjid(objid, rdata);	
	if (!docid) return 0;
	AosXmlTagPtr doc = retrieveBinaryDoc(docid, buff, rdata);
	return doc;
}


AosXmlTagPtr
AosDocClient::retrieveBinaryDoc(
		const u64 &docid, 
		AosBuffPtr &docBuff, 
		const AosRundataPtr &rdata)
{
	AosBuffPtr resp;
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosGetDocTrans(docid, true, snap_id);
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	if(!rslt)	return 0;
	
	u32 doc_len = resp->getU32(0);
	aos_assert_r(doc_len, 0);
	AosBuffPtr doc_buff = resp->getBuff(doc_len, false AosMemoryCheckerArgs);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(doc_buff->data(), doc_len, "" AosMemoryCheckerArgs);
	aos_assert_rr(doc && doc->getAttrU64(AOSTAG_DOCID , 0) == docid, rdata, 0);

	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, 0);
	//aos_assert_r(doc->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE, "") == "", 0);
	//aos_assert_r(doc->getAttrU64(AOSTAG_BINARY_SOURCELEN, 0) != 0, 0);
	OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
	AosBuffPtr buff = doc->getNodeTextBinaryCopy(nodename AosMemoryCheckerArgs);
	if (!buff || buff->dataLen() <= 0)
	{
		return doc;
	}

	aos_assert_r(buff && buff->dataLen() > 0, 0);
	 
	// 2 .uncompress
	docBuff = buff;
	
	if (nodename == "")
	{
		doc->removeNodeTexts();
	}
	else
	{
		doc->removeNode(nodename, false, false);
	}
	return doc;
}


void
AosDocClient::retrieveBinaryDoc(
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// Ketty 2013/07/20
	//AosAsyncReqTransPtr trans = OmnNew AosBinaryDocAsyncTrans(docid, svr_id, false, false);
	//trans->setCaller(caller);
	//AosAsyncReqTransMgr::getSelf()->addRequestAsync(trans, rdata);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosBinaryDocAsyncTrans(
			resp_caller, docid, false, false, snap_id);
	AosSendTransAsyncResp(rdata, trans);
}


AosXmlTagPtr
AosDocClient::getParentContainer(
		const AosRundataPtr &rdata,
		const OmnString &container_objid,
		const bool ispublic)
{
	aos_assert_r(container_objid != "", 0);

	AosXmlTagPtr pctr_doc;
	u32 siteid = rdata->getSiteid();
	if (doesObjidExist(siteid, container_objid, rdata) == AOS_INVDID)
	{
		pctr_doc = createMissingContainer(rdata, container_objid, ispublic);
	}
	else
	{
		pctr_doc = getDocByObjid(container_objid, rdata);
	}

	if (!pctr_doc)
	{
		AosSetError(rdata, "failed to get parent container");
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return 0;
	}

	if (pctr_doc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_CONTAINER)
	{
		AosSetError(rdata, "Not a container: ") << container_objid;
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return 0;
	}

	return pctr_doc;
}

bool 
AosDocClient::getTableNames(
		const AosRundataPtr &rdata,
		vector<string> &values)
{
	bool islogin = AosCheckIsLogin(rdata);
	if(!islogin)
	{
		return false;
	}

	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

	query_rslt->setWithValues(true);

	if(query_context)
	{
		query_context->setOpr(eAosOpr_an);
		query_context->setStrValue("*");
		query_context->setReverse(false);
	}
	
	OmnString iilname = AosIILName::composeSQLTablenameIILName();
	AosQueryColumn(iilname, query_rslt, 0, query_context, rdata);

	bool find = false;
	while(query_rslt->hasMoreDocid())
	{
		u64 docid = 0;
		OmnString value = "";
		query_rslt->nextDocidValue(docid, value, rdata);
		values.push_back(value);
		find = true;
	}
	if(!find)
	{
		OmnString errmsg = "table is empty!";
		AosSetError(rdata, "docclient_empty_table") << errmsg;
		rdata->setReturnCode(AosReturnCode::eEmptyTable);
		return false;
	}
	return true;
}

bool
AosDocClient::getJQLDocs(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType &type,
		vector<string> &values)
{
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

	query_rslt->setWithValues(true);
	query_context->setOpr(eAosOpr_eq);
	query_context->setReverse(false);
	query_context->setPageSize(0);              
	query_context->setBlockSize(1000); 

	if(query_context)
	{
		switch(type)
		{
		case JQLTypes::eTableDoc:
			 query_context->setStrValue(AOSCTNR_TABLE_CONTAINER);
			 break;
		case JQLTypes::eIndexDoc:
			 query_context->setStrValue(AOSCTNR_INDEX_CONTAINER);
			 break;
		case JQLTypes::eSchemaDoc:
			 query_context->setStrValue(AOSCTNR_SCHEMA_CONTAINER);
			 break;
		case JQLTypes::eDatasetDoc:
			 query_context->setStrValue(AOSCTNR_DATASET_CONTAINER);
			 break;
		case JQLTypes::eServiceDoc:
			 query_context->setStrValue(AOSCTNR_SERVICE_CONTAINER);
			 break;
		case JQLTypes::eJobDoc:
			 query_context->setStrValue(AOSCTNR_JOB_CONTAINER);
			 break;
		case JQLTypes::eTaskDoc:
			 query_context->setStrValue(AOSCTNR_TASK_CONTAINER);
			 break;
		case JQLTypes::eDataProcDoc:
			 query_context->setStrValue(AOSCTNR_PROC_CONTAINER);
			 break;
		case JQLTypes::eDataScannerDoc:
			 query_context->setStrValue(AOSCTNR_SCANNER_CONTAINER);
			 break;
		case JQLTypes::eDataConnectorDoc:
			 query_context->setStrValue(AOSCTNR_CONNECTOR_CONTAINER);
			 break;
		case JQLTypes::eDataRecordDoc:
			 query_context->setStrValue(AOSCTNR_RECORD_CONTAINER);
			 break;
		case JQLTypes::eDataFieldDoc:
			 query_context->setStrValue(AOSCTNR_FIELD_CONTAINER);
			 break;
		case JQLTypes::eDatabaseDoc:
			 query_context->setStrValue(AOSCTNR_DATABASE_CONTAINER);
			 break;
		case JQLTypes::eMap:
			 query_context->setStrValue(AOSCTNR_MAP_CONTAINER);
			 break;
		case JQLTypes::eVirtualField:
			 query_context->setStrValue(AOSCTNR_VIRTUAL_FIELD_CONTAINER);
			 break;
		case JQLTypes::eStatisticsDoc:
			 //query_context->setStrValue("_zt00_zt4g_stat_ctnr");
			 query_context->setStrValue(AOSCTNR_STAT_CONTAINER);
			 break;
		default:
			return false;
		}
	}
	
	OmnString iilname = "";
	switch(type)
	{
	case JQLTypes::eTableDoc:
		 //iilname = AosIILName::composeSQLTablenameIILName();
		 iilname << AOSZTG_ANAME << "_" << AOSTAG_HPCONTAINER;
		break;
	case JQLTypes::eIndexDoc:
	case JQLTypes::eSchemaDoc:
	case JQLTypes::eDatasetDoc:
	case JQLTypes::eJobDoc:
	case JQLTypes::eServiceDoc:
	case JQLTypes::eTaskDoc:
	case JQLTypes::eDataProcDoc:
	case JQLTypes::eDataScannerDoc:
	case JQLTypes::eDataConnectorDoc:
	case JQLTypes::eDataRecordDoc:
	case JQLTypes::eDataFieldDoc:
	case JQLTypes::eDatabaseDoc:
	case JQLTypes::eStatisticsDoc:
		 iilname = AosIILName::composeAttrIILName(AOSTAG_HPCONTAINER);
		break;
	default:
		return false;
	}
	AosQueryColumn(iilname, query_rslt, 0, query_context, rdata);

	bool find = false;
	bool finished = false;
	while(query_rslt->hasMoreDocid())
	{
		u64 docid = query_rslt->nextDocid(finished);
		AosXmlTagPtr xml = AosGetDocByDocid(docid, rdata);
		OmnString v = xml->toString();
		string value(v.data(), v.length());
		values.push_back(value);
		find = true;
	}
	if(!find)
	{
		OmnString errmsg = "table is empty!";
		AosSetError(rdata, "jqlstatement_empty_table") << errmsg;
		rdata->setReturnCode(AosReturnCode::eEmptyTable);
		return false;
	}
	return true;
}

u64  
AosDocClient::getJQLDocidByTablename(
		const AosRundataPtr &rdata,
		const string &tablename)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	OmnString iilname = AosIILName::composeSQLTablenameIILName();
	u64 docid = 0;
	bool isunique = true;
	bool rslt = iilclient->getDocid(iilname, tablename, eAosOpr_eq, false, docid, isunique, rdata);
	if(!rslt)
	{
		OmnString errmsg = "failed to get table!";
		AosSetError(rdata, "jqlstatement_failed_to_get_table") << errmsg;
		rdata->setReturnCode(AosReturnCode::eNotDeleTable);
		return 0;
	}

	return docid;
}

bool
AosDocClient::getDocByJQL(	
		const AosRundataPtr &rdata,
		const string &objid,
		string &doc)
{
	bool islogin = AosCheckIsLogin(rdata);
	if(!islogin)
	{
		return false;
	}
	OmnString aos_objid(objid.data(), objid.length());
	AosXmlTagPtr xml = getDocByObjid(aos_objid, rdata);
	if(!xml) 
	{
		OmnString errmsg = "failed to get doc!";
		AosSetError(rdata, "docclient_failed_to_get_doc") << errmsg;
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return false;
	}

	OmnString v = xml->toString();
	string value(v.data(), v.length());
	doc = value;
	return true;
	
}

bool 
AosDocClient::getParentContainerByObjid(	
		const AosRundataPtr &rdata,
		const string &objid, string &doc)
{
	bool islogin = AosCheckIsLogin(rdata);
	if(!islogin)
	{
		return false;
	}
	OmnString aos_objid(objid.data(), objid.length());

	AosXmlTagPtr pdoc = getParentContainer(rdata, aos_objid, true);
	if(!pdoc)
	{
		return false;
	}
	OmnString docstr = pdoc->toString();
	string tmpdoc(docstr.data(), docstr.length());
	doc = tmpdoc;
	return true;
	
}

bool
AosDocClient::getParentContainerByDocid(	
		const AosRundataPtr &rdata,
		const int64_t docid, string &doc)
{
	bool islogin = AosCheckIsLogin(rdata);
	if(!islogin)
	{
		return false;
	}
	AosXmlTagPtr xml = getDocByDocid(docid, rdata);
	if(!xml)
	{
		return false;
	}
	OmnString objid = xml->getAttrStr(AOSTAG_OBJID, "");

	OmnString aos_objid(objid.data(), objid.length());

	AosXmlTagPtr pdoc = getParentContainer(rdata, aos_objid, true);
	if(!pdoc)
	{
		return false;
	}
	OmnString docstr = pdoc->toString();
	string tmpdoc(docstr.data(), docstr.length());
	doc = tmpdoc;
	return true;

}


// Chen Ding, 2014/01/12
AosXmlTagPtr 
AosDocClient::getDocByKey(
		const AosRundataPtr &rdata, 
		const OmnString &container, 
		const OmnString &key_name, 
		const OmnString &key_value)
{
	// This function assumes 'key_name' is a key defined in 'container'. 
	// A container-attr IIL is created for it. It uses that IIL to retrieve
	// the docid and then the doc.
	OmnString iilname = AosIILName::composeContainerAttrIILName(container, key_name);
	u64 docid;
	bool isunique;
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, key_value, 
			eAosOpr_eq, false, docid, isunique, rdata);
	aos_assert_rr(rslt, rdata, 0);
	if (!isunique)
	{
		AosSetErrorUser(rdata, "doccltgetdoc_key_not_unique") 
			<< "Container: " << container
			<< "; Key name: " << key_name 
			<< "; Key value: " << key_value << enderr;
		return 0;
	}

	if (docid == 0)
	{
		return 0;
	}

	return getDocByDocid(docid, rdata);
}

AosXmlTagPtr 
AosDocClient::getDocByKey(
		const AosRundataPtr &rdata, 
		const OmnString &tablename)
{
	OmnString iilname = AosIILName::composeSQLTablenameIILName();
	u64 docid;
	bool isunique;
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, tablename, 
			eAosOpr_eq, false, docid, isunique, rdata);
	aos_assert_rr(rslt, rdata, 0);
	if (!isunique)
	{
		AosSetErrorUser(rdata, "doccltgetdoc_key_not_unique") 
			<< "tablename: " << tablename << enderr;
		return 0;
	}

	if (docid == 0)
	{
		AosSetErrorUser(rdata, "doccltgetdoc_doc_not_found")
			<< "tablename: " << tablename << enderr;
		return 0;
	}

	return getDocByDocid(docid, rdata);
}


AosXmlTagPtr 
AosDocClient::getDocByKeys(
		const AosRundataPtr &rdata, 
		const OmnString &container, 
		const vector<OmnString> &key_names, 
		const vector<OmnString> &key_values)
{

	AosTermAndPtr termand = OmnNew AosTermAnd();
	for (size_t i=0; i<key_names.size(); i++)
	{
		AosTermArithPtr t = OmnNew AosTermArith(false);
		t->setOpr(eAosOpr_eq);
		OmnString iilname = AosIILName::composeAttrIILName(key_names[i]);
		t->setIILName(iilname);
		t->setValue(key_values[i]);
		AosQueryTermObjPtr tt = t;
		termand->addTerm(tt, rdata);
	}

	bool finished = false;
	u64 docid = 0;
	termand->nextDocid(0, docid, finished, rdata);
	if (docid == 0) return 0;

	u64 did = 0;
	termand->nextDocid(0, did, finished, rdata);
	aos_assert_r(!did, 0);
	aos_assert_r(finished, 0);
	
	return getDocByDocid(docid, rdata);
}


AosXmlTagPtr
AosDocClient::retrieveBinaryDocByStat(
		const u64 &docid, 
		AosBuffPtr &docBuff, 
		const u64 &normal_snap_id,
		const u64 &binary_snap_id,
		const AosRundataPtr &rdata)
{
	AosBuffPtr resp;
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosGetDocTrans(docid, true, snap_id);
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	if(!rslt)	return 0;
	
	u32 doc_len = resp->getU32(0);
	aos_assert_r(doc_len, 0);
	AosBuffPtr doc_buff = resp->getBuff(doc_len, false AosMemoryCheckerArgs);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(doc_buff->data(), doc_len, "" AosMemoryCheckerArgs);
	aos_assert_rr(doc && doc->getAttrU64(AOSTAG_DOCID , 0) == docid, rdata, 0);

	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, 0);
	OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
	AosBuffPtr buff = doc->getNodeTextBinaryCopy(nodename AosMemoryCheckerArgs);
	if (!buff || buff->dataLen() <= 0)
	{
		return doc;
	}

	aos_assert_r(buff && buff->dataLen() > 0, 0);
	 
	// 2 .uncompress
	docBuff = buff;
	if (nodename == "")
	{
		doc->removeNodeTexts();
	}
	else
	{
		doc->removeNode(nodename, false, false);
	}
	return doc;
}

