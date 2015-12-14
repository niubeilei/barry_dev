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
#include "DocClient/DocClient.h"

#include "API/AosApi.h"
#include "DocClient/DocProc.h"
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

bool	
AosDocClient::deleteObj(
		const AosRundataPtr &rdata,
		const OmnString &docid,
		const OmnString &theobjid, 
		const OmnString &container, 
		const bool deleteFlag)
{
	// It deletes a doc. The doc to be deleted is identified as below.
	// 1. If 'docid' is not empty, it uses 'docid' to identify the doc.
	//    If it failed to retrieve the doc, it is an error.
	// 2. Otherwise, it uses [siteid, theobjid] to identify the doc. 
	//    If it failed to retrieve the doc, it is an error.
	// 3. The above operation is lock protected. Once the doc is retrieved,
	//    it release the theobjid immediately before the lock is unlocked.
	//
	// Note that 'theobjid' should be the full objid.
	AOSLOG_ENTER_R(rdata, false);

	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, false);

	if (mIsStopping)
	{
		AosSetError(rdata, "Server_is_stopping!");
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return false;
	}

	// delete docs must be logged.                                          
	OmnString cid = AosCloudidSvr::getCloudid(rdata->getUserid(), rdata);
	if (cid == "")
	{
		AosSetError(rdata, "missing_cloudid");
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return false;
	}

	if (mShowLog) OmnScreen << "Total delete: " << ++mTotalDeleted
		<< ":" << siteid << ":" << docid << ":" << theobjid << endl;

	AosXmlTagPtr origdocroot;
	u64 did = AOS_INVDID;
	if (docid != "")
	{
		// AOSMONITORLOG_LINE(rdata);
		// Docid not empty. This is the doc to be deleted.
		did = atoll(docid.data());
		aos_assert_r(did != 0, false);

		origdocroot = getDocByDocid(did, rdata);
		if (!origdocroot)
		{
			AosSetError(rdata, "Failed_to_retrieved_the_object: ") << docid;
			rdata->setReturnCode(AosReturnCode::eInternalError);
			return false;
		}

		OmnString objid = origdocroot->getAttrStr(AOSTAG_OBJID);
		if (theobjid != "" && theobjid != objid)
		{
			// The objid retrieved from the doc is different from 
			// the one passed in. We consider this as an error.
			AosSetError(rdata, "objid_is_not_match")
				<< objid << " and the passed in objid: " << theobjid;
			rdata->setReturnCode(AosReturnCode::eInternalError);
			return false;
		}
	}
	else
	{
		// AOSMONITORLOG_LINE(rdata);
		// Retrieve the doc
		origdocroot = getDocByObjid(theobjid, rdata);
		if (!origdocroot)
		{
			AosSetError(rdata, "Failed_to_retrieved_the_object") 
				<< "SiteID: " << siteid << ", ObjectID: " << theobjid;
			rdata->setReturnCode(AosReturnCode::eInternalError);
			return false;
		}
	}

	bool rslt = deleteDoc(origdocroot, rdata, container, deleteFlag);
	AOSLOG_LEAVE(rdata);
	return rslt;
}


bool
AosDocClient::deleteDoc(
		const AosXmlTagPtr &origdocroot, 
		const AosRundataPtr &rdata, 
		const OmnString &ctnr_objid,
		const bool deleteFlag)
{
	// This function deletes the doc 'doc'. If 'deleteFlag' is false, 
	// it will delete the indexes only. After that, 'doc' is added
	// to 'ctnr_objid' (which is an objid). 
	AOSLOG_ENTER_R(rdata, false);

	if (!origdocroot)
	{
		rdata->setError() << "Missing the doc to delete";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	aos_assert_r(origdocroot->isRootTag(), false);

	rdata->setWorkingDoc(origdocroot, false);
	rdata->setDocByVar(AOSARG_DOC_TO_DELETE, origdocroot, false);
	rdata->setDocByVar(AOSARG_OLD_DOC, origdocroot, false);

	u64 did = origdocroot->getAttrU64(AOSTAG_DOCID, 0);
	OmnString objid = origdocroot->getAttrStr(AOSTAG_OBJID, "");
	if (did == AOS_INVDID)
	{
		rdata->setError() << "To delete a doc but the doc was not found!"
			<< "Docid: " << objid;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (mShowLog) OmnScreen << "To delete doc: "
		<< origdocroot->getData() << endl;

	bool rslt = AosSecurityMgrObj::getSecurityMgr()->checkDelete(origdocroot, rdata);
	if (!rslt)
	{
		// Access denied
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// The doc can be deleted. Need to lock the class and the doc itself. 
	if (origdocroot->isDeleted())
	{
		// It is possible that someone else removed the doc. 
		rdata->setError() << "The doc was already removed:"
			<< did << ":" << objid;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// AOSMONITORLOG_LINE(rdata);
	
	// Release the objid
	if (mShowLog) OmnScreen << "unbind objid: " << objid
		<< ":" << did << endl;

	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	
	if (OmnApp::getBindObjidVersion() == OmnApp::eDftBindObjidVersion)
	{
		rslt = iilclient->unbindObjid(objid, did, rdata);
	}
	else if (AosGetCubeId(objid) != AosGetCubeId(did))
	{
		rslt = iilclient->unbindObjid(objid, did, rdata);
	}
	
	if (!rslt)
	{
		rdata->setError() << "failed to unbind objid:"
			<< objid << ":" << did;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//Release the cid
	if (origdocroot->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT)
	{
		OmnString cid = origdocroot->getAttrStr(AOSTAG_CLOUDID, "");
		aos_assert_r(cid !="", false);

		if (mShowLog) OmnScreen << "unbind cloudid: "
			<< cid << ":" << did << endl;

		rslt = iilclient->unbindCloudid(cid, did, rdata);
		if (!rslt)
		{
			rdata->setError() << "failed to unbind cloundid:"
				<< cid << ":" << did;
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	rslt = AosEvent::checkEvent1(0, origdocroot,
		AOSEVENTTRIGGER_DOC_PRE_DELETE, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	if (!mIsRepairing)
	{
		//aos_assert_r(mVersionServerObj, false);
		//u64 logid = mVersionServerObj->addDeletedVerObj(origdocroot, rdata);
		//if (logid == 0)
		//{
	   	//	// AOSMONITORLOG_LINE(rdata);
	   	//	OmnAlarm << "Failed creating the version obj: " 
		//		<< origdocroot->getAttrStr(AOSTAG_OBJID) << enderr;
		//}
		// Chen Ding, 08/06/2011
		// When a doc is deleted, its objid will be added to the IIL: 
		// 		AosIILName::composeDeletedObjidIILname()
		//OmnString iilname = AosIILName::composeDeletedObjidIILName();
		//aos_assert_rr(iilname != "", rdata, false);
		//bool rslt = AosIILClient::getSelf()->addStrValueDocToTable(
	 	//		iilname, objid, logid, false, false, rdata);
		//if (!rslt)
		//{
		//	OmnAlarm << "Failed adding the deleted obj objid 
		//		into the IIL!" << enderr;
		//}
	}

	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, false);
	
	u64 snap_id = rdata->getSnapshotId();
	sgDocMgr->deleteDoc(did, snap_id, rdata);
	// Need to add a request.

	if (mShowLog) OmnScreen << "DeleteDocByDocid: " << did << endl;

	AosTransPtr trans = OmnNew AosDeleteObjTrans(
		did, origdocroot, true, false, snap_id);
	rslt = addReq(rdata, trans);
	aos_assert_r(rslt, false);
	if (!rdata->isOk())
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}

	u64 userid = rdata->getUserid();
	OmnString appname = rdata->getAppname();
	AosDocReqPtr doc_req = AosDocProc::getSelf()->addDeleteRequest(
		origdocroot, origdocroot, userid, appname, rdata);
	aos_assert_r(doc_req, false);

	AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_DELETEDOC, rdata);
	
	rdata->setOk();	
	rslt = AosEvent::checkEvent1(0, origdocroot, 
		AOSEVENTTRIGGER_DOC_DELETE, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
		// AOSMONITORLOG_FINISH(rdata);
	}

	AOSLOG_LEAVE(rdata);
	return true;
}


bool
AosDocClient::deleteBatchDocs(
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
AosDocClient::deleteBinaryDoc(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosDocClient::deleteBinaryDoc(
		const OmnString &objid, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDocClient::dropJQLTable( 
		const AosRundataPtr &rdata,
		const OmnString &table_name)
{

	bool islogin = AosCheckIsLogin(rdata);
	if (!islogin)
	{
		return false;
	}

	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();

	OmnString iilname = AosIILName::composeSQLTablenameIILName();
	u64 docid = 0;
	bool isunique = true;
	bool rslt = iilclient->getDocid(iilname, table_name,
		eAosOpr_eq, false, docid, isunique, rdata);
	if (!rslt)
	{
		OmnString errmsg = "failed to get table!";
		AosSetError(rdata, "docclient_failed_to_get_table") << errmsg;
		rdata->setReturnCode(AosReturnCode::eNotDeleTable);
		return false;
	}

	AosXmlTagPtr doc = getDocByDocid(docid, rdata);
	if (!doc) 
	{
		OmnString errmsg = "failed to get doc!";
		errmsg << docid;
		AosSetError(rdata, "docclient_failed_to_get_doc") << errmsg;
		rdata->setReturnCode(AosReturnCode::eDocNotFound);
		return false;
	}

	OmnString str;
	str << docid;
	rslt = deleteObj(rdata, str, "", "", false);;
	if(!rslt)
	{
		return false;
	}

	rslt = iilclient->removeStrValueDoc(iilname, table_name, docid, rdata);
	if (!rslt)
	{
		OmnString errmsg = "failed to remove entry from iil!";
		errmsg << docid;
		AosSetError(rdata, "docclient_failed_to_remove entry") << errmsg;
		rdata->setReturnCode(AosReturnCode::eNotDeleTable);
		return false;
	}

	return true;
}


bool
AosDocClient::deleteDocByObjid(	
		const AosRundataPtr &rdata,
		const string &objid)
{
	bool islogin = AosCheckIsLogin(rdata);
	if (!islogin)
	{
		return false;
	}

	OmnString str = objid;
	bool rslt = deleteObj(rdata, "", str, "", false);
	return rslt;
}


bool
AosDocClient::deleteDocByDocid(	
		const AosRundataPtr &rdata,
		const int64_t docid)
{
	bool islogin = AosCheckIsLogin(rdata);
	if (!islogin)
	{
		return false;
	}

	OmnString str;
	str << docid;
	bool rslt = deleteObj(rdata, str, "", "", false);
	return rslt;
}

