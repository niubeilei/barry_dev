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
#include "DocClientNew/DocClientNew.h"
/*
#include "CompressUtil/Compress.h"
#include "DocClientNew/DocidMgr.h"
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
*/
#include "JimoAPI/JimoDocEngine.h"
#include "XmlUtil/SystemDoc.h"

AosXmlTagPtr
AosDocClientNew::getDoc(
		const AosRundataPtr &rdata,
		const u64 &docid, 
		const OmnString &objid, 
		bool &duplicated)
{
	return Jimo::jimoGetDoc(rdata.getPtr(), docid, objid, duplicated);
}


u64 
AosDocClientNew::getDocidByCloudid(
		const u32 siteid, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocidByCloudid(rdata.getPtr(), siteid, cid);
}


AosXmlTagPtr 
AosDocClientNew::getParentDoc(
		const OmnString &objid, 
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet; 
	return 0;             
	/*
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
	*/
}


AosXmlTagPtr 
AosDocClientNew::getDeletedDoc(
		const OmnString &objid, 
		const u64 &did, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDeletedDoc(rdata.getPtr(), objid, did);
}


AosUserAcctObjPtr
AosDocClientNew::getUserAcct(
		const u64 &user_docid,
		const AosRundataPtr &rdata)
{
	/*
	AosXmlTagPtr userdoc = getDocByDocid(user_docid, rdata);
	if (!userdoc) return 0;
	AosUserMgmtObjPtr usrmgmt_obj = AosUserMgmtObj::getUserMgmt();
	aos_assert_r(usrmgmt_obj, 0);
	return usrmgmt_obj->getUserAcct(userdoc, rdata);
	*/
	return Jimo::jimoGetUserAcct(rdata.getPtr(), user_docid);
}


AosXmlTagPtr
AosDocClientNew::getDocByCloudid(
		const OmnString &cloudid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocByCloudid(rdata.getPtr(), cloudid);
}


AosXmlTagPtr
AosDocClientNew::getDocByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocByObjid(rdata.getPtr(), objid);
}


AosXmlTagPtr 
AosDocClientNew::getLogByDocid(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;         
	/*
	aos_assert_r(docid, 0);
	return 0;
	*/
}


AosXmlTagPtr 
AosDocClientNew::getDocByDocid(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocByDocid(rdata.getPtr(), docid);
}


OmnString
AosDocClientNew::getCloudid(
		const u64 &user_docid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetCloudid(rdata.getPtr(), user_docid);
}


AosSystemDocPtr 
AosDocClientNew::getSystemDocByDocid(
		const u64 &ref_docid, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;        
/*
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
	*/
}


u64
AosDocClientNew::getDocidByObjid(
		const OmnString &objid, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;            
	/*
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
	*/
}


AosBuffPtr 
AosDocClientNew::batchGetDocs(
		const OmnString &scanner_id,
		const int server_id,
		const AosRundataPtr &rdata)
{
	//Linda, 2013/09/23
	OmnNotImplementedYet;
	return 0;
}


void
AosDocClientNew::batchGetDocs(
		const AosRundataPtr &rdata,
		const int vid, 
		const u32 num_docs,
		const u32 blocksize,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &sizeid_len,
		const AosAsyncRespCallerPtr &resp_caller)
{
/*
//	aos_assert(docid_buff->dataLen() / sizeof(u64) == num_docs);
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosBatchGetDocsAsyncTrans(
			resp_caller, vid, num_docs, blocksize, docid_buff, sizeid_len, snap_id, false, false);
	AosSendTransAsyncResp(rdata, trans);
	*/
	return Jimo::jimoBatchGetDocs(rdata.getPtr(), vid, num_docs, blocksize, docid_buff, sizeid_len, resp_caller);
}


AosXmlTagPtr
AosDocClientNew::retrieveBinaryDoc(
		const OmnString &objid, 
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoRetrieveBinaryDoc(rdata.getPtr(), objid, buff);
}


AosXmlTagPtr
AosDocClientNew::retrieveBinaryDoc(
		const u64 &docid, 
		AosBuffPtr &docBuff, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoRetrieveBinaryDoc(rdata.getPtr(), docid, docBuff);
}


void
AosDocClientNew::retrieveBinaryDoc(
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	/*
	// Ketty 2013/07/20
	//AosAsyncReqTransPtr trans = OmnNew AosBinaryDocAsyncTrans(docid, svr_id, false, false);
	//trans->setCaller(caller);
	//AosAsyncReqTransMgr::getSelf()->addRequestAsync(trans, rdata);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosBinaryDocAsyncTrans(
			resp_caller, docid, false, false, snap_id);
	AosSendTransAsyncResp(rdata, trans);
	*/
	return Jimo::jimoRetrieveBinaryDoc(rdata.getPtr(), resp_caller, docid);
}


AosXmlTagPtr
AosDocClientNew::getParentContainer(
		const AosRundataPtr &rdata,
		const OmnString &container_objid,
		const bool ispublic)
{
	return Jimo::jimoGetParentContainer(rdata.getPtr(), container_objid, ispublic);
}

bool 
AosDocClientNew::getTableNames(
		const AosRundataPtr &rdata,
		vector<string> &values)
{
	return Jimo::jimoGetTableNames(rdata.getPtr(), values);
}

bool
AosDocClientNew::getJQLDocs(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType &type,
		vector<string> &values)
{
	return Jimo::jimoGetJQLDocs(rdata.getPtr(), type, values);
}

u64  
AosDocClientNew::getJQLDocidByTablename(
		const AosRundataPtr &rdata,
		const string &tablename)
{
	return Jimo::jimoGetJQLDocidByTablename(rdata.getPtr(), tablename);
}

bool
AosDocClientNew::getDocByJQL(	
		const AosRundataPtr &rdata,
		const string &objid,
		string &doc)
{
	return Jimo::jimoGetDocByJQL(rdata.getPtr(), objid, doc);
	
}

bool 
AosDocClientNew::getParentContainerByObjid(	
		const AosRundataPtr &rdata,
		const string &objid,
		string &doc)
{
	return Jimo::jimoGetParentContainerByObjid(rdata.getPtr(), objid, doc);
	
}

bool
AosDocClientNew::getParentContainerByDocid(	
		const AosRundataPtr &rdata,
		const int64_t docid,
		string &doc)
{
	return Jimo::jimoGetParentContainerByDocid(rdata.getPtr(), docid, doc);

}


// Chen Ding, 2014/01/12
AosXmlTagPtr 
AosDocClientNew::getDocByKey(
		const AosRundataPtr &rdata, 
		const OmnString &container, 
		const OmnString &key_name, 
		const OmnString &key_value)
{
	return Jimo::jimoGetDocByKey(rdata.getPtr(), container, key_name, key_value);
}

AosXmlTagPtr 
AosDocClientNew::getDocByKey(
		const AosRundataPtr &rdata, 
		const OmnString &tablename)
{
	return Jimo::jimoGetDocByKey(rdata.getPtr(), tablename);
}


AosXmlTagPtr 
AosDocClientNew::getDocByKeys(
		const AosRundataPtr &rdata, 
		const OmnString &container, 
		const vector<OmnString> &key_names, 
		const vector<OmnString> &key_values)
{
	return Jimo::jimoGetDocByKeys(rdata.getPtr(), container, key_names, key_values);
}


AosXmlTagPtr
AosDocClientNew::retrieveBinaryDocByStat(
		const u64 &docid, 
		AosBuffPtr &docBuff, 
		const u64 &normal_snap_id,
		const u64 &binary_snap_id,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoRetrieveBinaryDocByStat(rdata.getPtr(), docid, docBuff, normal_snap_id, binary_snap_id);
}

