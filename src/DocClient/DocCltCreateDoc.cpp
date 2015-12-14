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

#include "API/AosApi.h"
#include "CompressUtil/Compress.h"
#include "DocClient/DocProc.h"
#include "DocClient/DocidMgr.h"
#include "DocClient/DocTransHandler.h"
#include "DocServer/DocSvr.h"
// #include "DocTrans/CreateDocTrans.h"
// #include "DocTrans/CreateDocSafeTrans.h"
#include "ErrorMgr/ErrmsgId.h"
#include "EventMgr/Event.h"
#include "EventMgr/EventTriggers.h"
#include "IdGen/IdGenMgr.h"
#include "Rundata/RdataUtil.h"
#include "SeLogClient/SeLogClient.h"
#include "SEUtil/SysLogName.h"
#include "SEUtil/FieldName.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/ObjidSvr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RemoteBkCltObj.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SmartDoc/SMDMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/LockMonitor.h"
// #include "TransClient/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/SeXmlUtil.h"
#include "SEUtil/IILName.h"
#include "SEInterfaces/TaskObj.h"
#include "Util/Opr.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "StorageEngine/SengineDocInfoCSV.h"
#include "StorageEngine/SengineDocInfoFixed.h"
#include "StatUtil/StatIdIDGen.h"
#include "StatUtil/StatDefineDoc.h"
#include "StatUtil/Statistic.h"
#include "StatUtil/Ptrs.h"
#include "StatUtil/StatModifyInfo.h"
#include "StatTrans/BatchSaveStatDocsTrans.h"
static AosDocMgrObjPtr sgDocMgr;

bool
AosDocClient::createDoc1(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &cmd, 
		const OmnString &target_cid,
		const AosXmlTagPtr &newdoc,
		const bool resolveObjid,
		const AosDocSvrCbObjPtr &caller, 
		void *userdata,
		const bool keepDocid) 
{
	// If 'objid' is empty, it assigns it to 'docid'. If 'objid' is
	// not unique, it checks whether the objid was reserved by 
	// this request. To reserve an objid, one should call 
	// 'reserveObjid(...)'. If success, it a docid should have been
	// created for it. The requester should include the docid 
	// in 'doc' or 'cmd'. If 'objid' is not empty and it is used
	// by someone else, it will attach the docid to objid to 
	// make it unique. If that is still not unique, it returns false
	// and aborts the operation.
	AOSLOG_ENTER_R(rdata, false);

	aos_assert_r(newdoc, false);
	rdata->setDocByVar(AOSARG_NEW_DOC, newdoc, false);
	rdata->setWorkingDoc(newdoc, false);

	AosRundata *rdata_raw = rdata.getPtr();
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, false);

	u64	userid = rdata->getUserid();
	AosXmlTagPtr root = rdata->getReceivedDoc(); 
	aos_assert_r(root, false);

	newdoc->normalize();
	AOSDOCSVRLOG_DEFINE;
	newdoc->removeAttr(AOSTAG_FULLDOC_SIGNATURE);

	if (mIsStopping)
	{
		rdata->setError() << "Server is stopping!";
		AOSDOCSVRLOG_CLOSE(newdoc, rdata);
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString cid;
	if (mIsRepairing)
	{
		cid = newdoc->getAttrStr(AOSTAG_CLOUDID);
		// AOSMONITORLOG_LINE(rdata);
	}
	else
	{
		// Pending: Creating docs does not have to have a cloudid
		cid = AosCloudidSvr::getCloudid(userid, rdata);
		// create docs must be logged.                  
		if (cid == "")
		{
			AosSetErrorU(rdata, "missing_cloudid") << ": " << rdata->getUserid() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	if (newdoc->getAttrStr(AOSTAG_OTYPE) == "")
	{
		newdoc->setAttr(AOSTAG_OTYPE, AOSOTYPE_UNKNOWN);
	}

	OmnString tcid = target_cid;
	if (tcid == "") tcid = cid;

	newdoc->setAttr(AOSTAG_SITEID, siteid);
	OmnString objid = newdoc->getAttrStr(AOSTAG_OBJID);
	if (objid == "" && cmd) 
	{
		// AOSMONITORLOG_LINE(rdata);
		objid = cmd->getAttrStr(AOSTAG_OBJID);
		if (objid != "")
		{
			// AOSMONITORLOG_LINE(rdata);
			newdoc->setAttr(AOSTAG_OBJID, objid);
			newdoc->normalize();
			AOSDOCSVRLOG << objid;
		}
	}

	bool rslt = true;
	bool is_public = false;
	if (objid != "")
	{
		AosObjidType::E objid_type;
		rslt = AosObjid::checkObjid(objid, objid_type, rdata);
		if (!rslt)
		{
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return false;
		}

		// AOSMONITORLOG_LINE(rdata);
		is_public = determinePublic(newdoc, cmd, tcid, objid);

		AOSDOCSVRLOG << objid;

		if (is_public)
		{
			// AOSMONITORLOG_LINE(rdata);
			if (!mIsRepairing)
			{
				rslt = AosSecurityMgrObj::getSecurityMgr()->checkAddPubObjid(objid, rdata);
				if (!rslt)
				{
					AOSDOCSVRLOG << rdata->getErrmsg();
					AOSDOCSVRLOG_CLOSE(newdoc, rdata);
					AOSLOG_LEAVE(rdata);
					return false;
				}
			}
		}
		else
		{
			AOSDOCSVRLOG << tcid;
			if (tcid == "")
			{
				// AOSMONITORLOG_LINE(rdata);
				if (!mIsRepairing)
				{
					rdata->setError() << "Missing Cloud ID";
					AOSDOCSVRLOG_CLOSE(newdoc, rdata);
					AOSLOG_LEAVE(rdata);
					return false;
				}
			}
		}
	}

	// There shall be one and only one home container
	OmnString pctnr_objid = newdoc->getAttrStr(AOSTAG_PARENTC);
	AosXmlTagPtr pctnr_doc;
	if (!mIsRepairing) 
	{
		// AOSMONITORLOG_LINE(rdata);
		rslt = checkHomeContainer(rdata, newdoc, 0, tcid);
		if (!rslt)
		{
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return false;
		}

		// Check whether the user can create the doc. Note that if 
		// 'mIsRepairing' is true, which means that the server is recovering
		// the database, security check is skipped.
		// AOSMONITORLOG_LINE(rdata);
		pctnr_objid = newdoc->getAttrStr(AOSTAG_PARENTC);
		if (pctnr_objid == "")
		{
			AosSetErrorU(rdata, "missing_parent_ctnr") << ": " << objid << enderr;
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return false;
		}

		pctnr_doc = getDocByObjid(pctnr_objid, rdata);
		if (!pctnr_doc)
		{
			AosSetErrorU(rdata, "eContainerNotFound") 
				<< newdoc->getAttrStr(AOSTAG_OBJID) 
				<< ":" << pctnr_objid << enderr;
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return false;
		}

		rslt = AosCheckCreateDoc(rdata, pctnr_doc, newdoc);
		if (!rslt)
		{
			// Creating the doc is not allowed.
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}
	else
	{
		aos_assert_r(pctnr_objid != "", false);
		pctnr_doc = getDocByObjid(pctnr_objid, rdata);
	}

	if (pctnr_objid == "")
	{
		AosSetErrorUser(rdata, "missing_parent_ctnr") << enderr;
		return false;
	}

	// Check whether any of the containers were created. If not, 
	// prompt the user for creating the containers.
	u64 docid = AOS_INVDID;
	if (mIsRepairing) 
	{
		docid = newdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
		// AOSMONITORLOG_LINE(rdata);
		aos_assert_r(docid != AOS_INVDID, false);
	
		// AOSMONITORLOG_LINE(rdata);
		if (newdoc->getAttrStr(AOSTAG_CREATOR) == "")
		{
			// AOSMONITORLOG_LINE(rdata);
			newdoc->setAttr(AOSTAG_CREATOR, AOSCLOUDID_UNKNOWN);
		}
	}
	else
	{
		// Set the creator
		if (cid == "")
		{
			// AOSMONITORLOG_LINE(rdata);
			newdoc->setAttr(AOSTAG_CREATOR, AOSCLOUDID_UNKNOWN);
		}
		else
		{
			newdoc->setAttr(AOSTAG_CREATOR, cid);
		}
	}

	if (!mIsRepairing)
	{
		// AOSMONITORLOG_LINE(rdata);
		OmnString ctime = OmnGetTime(AosLocale::getDftLocale());
		newdoc->setAttr(AOSTAG_CTIME, ctime);
		newdoc->setAttr(AOSTAG_MTIME, ctime);

		u32 systemsec = OmnGetSecond();
		newdoc->setAttr(AOSTAG_CT_EPOCH, systemsec);
		newdoc->setAttr(AOSTAG_MT_EPOCH, systemsec);

		newdoc->removeAttr(AOSTAG_VERSION);

		// AOSMONITORLOG_LINE(rdata);
		// Fixing the objid
		if (objid != "")
		{
			// AOSMONITORLOG_LINE(rdata);
			OmnString reserved = newdoc->getAttrStr(AOSTAG_OBJID_RESERVED);
			if (reserved != "true" && cmd)
			{
				// AOSMONITORLOG_LINE(rdata);
				reserved = cmd->getAttrStr(AOSTAG_OBJID_RESERVED);
			}

			if (reserved != "true")
			{
				// AOSMONITORLOG_LINE(rdata);
				// It was not reserved. Check whether 'objid' is unique
				// It is unique. Need to create it now. 
				docid = 0;
				//bool docid_allocated = false;
				if (keepDocid) 
				{
					// AOSMONITORLOG_LINE(rdata);
					docid = newdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);

					if (docid == AOS_INVDID) 
					{
						// AOSMONITORLOG_LINE(rdata);
						// Chen Ding, 03/14/2012, OBJID_CHANGE
						// docid = AosDocidMgr::getSelf()->nextDocid(rdata);
						docid = AosDocidMgr::getSelf()->nextDocid(objid, rdata);
						aos_assert_r(docid, false);
						//docid_allocated = true;
					}
				}
				else
				{
					docid = AosDocidMgr::getSelf()->nextDocid(objid, rdata);
					aos_assert_r(docid, false);
					//docid_allocated = true;
				}

				newdoc->setAttr(AOSTAG_DOCID, docid);
				bool objid_changed = false;

				// Chen Ding, 01/22/2012
				if (pctnr_objid == objid)
				{
					rslt = handleObjidIssue(objid, newdoc, resolveObjid, rdata);
					if (!rslt)
					{
						return false;
					}
				}
			
				rslt = AosDocidMgr::getSelf()->bindObjid(
					objid, docid, objid_changed, resolveObjid, true, rdata);
				if (!rslt)
				{
					AOSLOG_LEAVE(rdata);
					return false;
				}
				
				newdoc->setAttr(AOSTAG_OBJID, objid);
				AOSDOCSVRLOG;
			}
			else
			{
				// It is reserved. Retrieve the docid. Since it is reserved,
				// the docid and objid should have been bound already.
				// AOSMONITORLOG_LINE(rdata);
				bool exist;
				docid = newdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID, exist);
				if (docid == AOS_INVDID && cmd)
				{
					docid = cmd->getAttrU64(AOSTAG_DOCID, AOS_INVDID, exist);
					AOSDOCSVRLOG << docid;
					if (docid != AOS_INVDID)
					{
						newdoc->setAttr(AOSTAG_DOCID, docid);
					}
				}

				if (docid == AOS_INVDID)
				{
					// It said it is reserved but failed to retrieve
					// the docid. This is incorrect.
					rdata->setError() << "Missing docid: " << objid;
					OmnAlarm << rdata->getErrmsg() << enderr;
					AOSDOCSVRLOG_CLOSE(newdoc, rdata);
					AOSLOG_LEAVE(rdata);
					return false;
				}
			}

			// It comes to this point, 'objid' is not empty, 
			// docid is valid, and docid is associated with it.
		}
		else
		{
			// No objid being assigned yet. Will create an objid for it.
			// The objid is in the form: 'objid_' + <docid>
			// If it is not unique, we will add (n), where 'n' is a sequential
			// number, starting from 1.
			// AOSMONITORLOG_LINE(rdata);
			bool docid_allocated = false;
			if (keepDocid) 
			{
				// AOSMONITORLOG_LINE(rdata);
				docid = newdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);

				if (docid == AOS_INVDID) 
				{
					// AOSMONITORLOG_LINE(rdata);
					// Chen Ding, 03/14/2012, OBJID_CHANGE
					// docid = AosDocidMgr::getSelf()->nextDocid(rdata);
					docid = AosDocidMgr::getSelf()->nextDocid(objid, rdata);
					docid_allocated = true;
				}
			}
			else
			{
				// Chen Ding, 03/14/2012, OBJID_CHANGE
				// docid = AosDocidMgr::getSelf()->nextDocid(rdata);
				docid = AosDocidMgr::getSelf()->nextDocid(objid, rdata);
				docid_allocated = true;
			}

			if (docid == AOS_INVDID)
			{
				rdata->setError() << "Failed to allocate docid: " << objid;
				AOSDOCSVRLOG_CLOSE(newdoc, rdata);
				AOSLOG_LEAVE(rdata);
				return false;
			}
			newdoc->setAttr(AOSTAG_DOCID, docid);

			is_public = (newdoc->getAttrStr(AOSTAG_PUBLIC_DOC) == "true");
			newdoc->removeAttr(AOSTAG_PUBLIC_DOC, false, true);

			// Chen Ding, 03/14/2012, OBJID_CHANGE
			// objid = AosObjid::getDftObjid(docid, is_public, cid, rdata);
			// if (!rslt || objid == "")
			// {
			// 	rdata->setError() << "Failed to get default objid!";
			// 	AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			// 	AOSLOG_LEAVE(rdata);
			// 	return false;
			// }
			if (!is_public)
			{
				objid = AosObjid::compose(objid, cid);
			}
	
			if (pctnr_objid == objid)
			{
				if (!handleObjidIssue(objid, newdoc, resolveObjid, rdata))
				{
					return false;
				}
			}

			// Bind the objid
			bool objid_changed = false;
			rslt = AosDocidMgr::getSelf()->bindObjid(
				objid, docid, objid_changed, true, true, rdata);
			if (!rslt)
			{
				rdata->setError() << "Failed to bind objid: " << objid << ":" << docid;
				AOSDOCSVRLOG_CLOSE(newdoc, rdata);
				if (docid_allocated)
				{
					AosDocidMgr::getSelf()->returnDocid(docid, rdata);
				}
				AOSLOG_LEAVE(rdata);
				return false;
			}

			newdoc->setAttr(AOSTAG_OBJID, objid);
		}
	}
	else
	{
		// It is repairing, need to bind the objid and docid
		// AOSMONITORLOG_LINE(rdata);
		if (objid == "")
		{
			rdata->setError() << "Missing objid!";
			OmnAlarm << rdata->getErrmsg() << " " << newdoc->toString() << enderr;
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return false;
		}

		if (docid == AOS_INVDID)
		{
			rdata->setError() << "Missing docid (Internal Error)!";
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			OmnAlarm << rdata->getErrmsg() << " " << newdoc->toString() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		if (pctnr_objid == objid)
		{
			rslt = handleObjidIssue(objid, newdoc, resolveObjid, rdata);
			if (!rslt)
			{
				return false;
			}
		}

		bool objid_changed = false;
		rslt = AosDocidMgr::getSelf()->bindObjid(
			objid, docid, objid_changed, false, true, rdata);
		if (!rslt)
		{
			rdata->setError() << "Failed binding: " 
				<< objid << ":" << docid;
			//OmnAlarm << "Failed to bind: " << rdata->getErrmsg() 
			//	<< " " << newdoc->toString() << enderr;
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return false;
		}

		// AOSMONITORLOG_LINE(rdata);
		if (mRepairingMaxDocid < docid) mRepairingMaxDocid = docid;
	}

	// If it is a user account record, it should bind the cloud id.
	if (newdoc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT)
	{
		// AOSMONITORLOG_LINE(rdata);
		OmnString cid = newdoc->getAttrStr(AOSTAG_CLOUDID);
		if (cid != "")
		{
			// AOSMONITORLOG_LINE(rdata);
			u64 did = getDocidByCloudid(siteid, cid, rdata);
			if (did == 0)
			{
				rslt = bindCloudid(cid, docid, rdata);
				if (!rslt)
				{
					// AOSMONITORLOG_LINE(rdata);
					//OmnAlarm << "Failed to bind cloud id:" 
					//	<< cid << ":" << docid << enderr;
					AOSDOCSVRLOG << cid << ":" << docid;
				}
			}
			else
			{
				if (did != docid)
				{
					rdata->setError() << "Failed to create user account!";
					// AOSMONITORLOG_FINISH(rdata);
					AOSLOG_LEAVE(rdata);
					return 0;
				}
			}
		}
	}

	//add version
	if (!mIsRepairing)
	{
		//Jozhi 2013/07/05
		//aos_assert_r(mVersionServerObj, false);
		//if (!mVersionServerObj->addVersionObj(newdoc, rdata))
		//{
		//	// AOSMONITORLOG_LINE(rdata);
		//	OmnAlarm << "Failed creating the version obj: " << newdoc->getAttrStr(AOSTAG_OBJID) << enderr;
		//}
	}

	rslt = AosEvent::checkEvent1(0, newdoc, AOSEVENTTRIGGER_DOC_PRE_CREATED, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
		AOSDOCSVRLOG_CLOSE(newdoc, rdata);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// 1. Last double check on 'objid'
	if (objid == "")
	{
		rdata->setError() << "Missing objid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSDOCSVRLOG_CLOSE(newdoc, rdata);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (docid == AOS_INVDID)
	{
		rdata->setError() << "Missing docid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSDOCSVRLOG_CLOSE(newdoc, rdata);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Sanity Check
	if (objid != newdoc->getAttrStr(AOSTAG_OBJID))
	{
		// AOSMONITORLOG_LINE(rdata);
		rdata->setError() << "Internal error (objid mismatch)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		newdoc->setAttr(AOSTAG_OBJID, objid);
	}

	if (docid != newdoc->getAttrU64(AOSTAG_DOCID, 0))
	{
		// AOSMONITORLOG_LINE(rdata);
		rdata->setError() << "Docid mismatch: "
		 	<< docid << ":" << newdoc->getAttrStr(AOSTAG_DOCID);
		newdoc->setAttr(AOSTAG_DOCID, docid);
	}

	if (newdoc->getAttrStr(AOSTAG_PARENTC) == "")
	{
		AosSetError(rdata, "missing_parent_ctnr") << enderr;
		return false;
	}

	// Chen Ding, 2014/11/09
	// Add it to DocMgr. 
	// if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	// aos_assert_r(sgDocMgr, false);
	// u64 snap_id = rdata->getSnapshotId();
	// sgDocMgr->addCopy(docid, newdoc, snap_id, rdata);
	// AOSDOCSVRLOG;

	aos_assert_r(docid, false);
	// Send a transaction to the backend to actually create the doc.
	// Chen Ding, 2014/11/09
	// AosTransPtr trans = OmnNew AosCreateDocTrans(docid, newdoc, true, false, snap_id);
	// bool rslt = addReq(rdata, trans);
	// aos_assert_r(rslt, false);
	rslt = mTransHandler->createDoc(rdata_raw, userid, this, root, newdoc, docid);
	if (!rslt)
	{
		/*AosLogError(rdata_raw, true, AosErrmsgId::eFailedCreatingDatalet)
			<< AosEnumFieldName::eDocid << docid
			<< AosEnumFieldName::eSnapshot << rdata_raw->getSnapshotId() 
			<< AosEnumFieldName::eUserid << userid << enderr;*/
		return false;
	}

	// Chen Ding, 2014/11/09
	// if (!rdata->isOk())
	// {
	// 	AOSLOG_LEAVE(rdata);
	// 	return false;
	// }
	// AosDocReqPtr doc_req = AosDocProc::getSelf()->addCreateRequest(
	// 		root, userid, newdoc, 0, 0, rdata);
	// aos_assert_rr(doc_req, rdata, false);

	AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_CREATEDOC, rdata);
	rdata->setOk();

	rslt = AosEvent::checkEvent1(0, newdoc, AOSEVENTTRIGGER_DOC_CREATED, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
	}

	AOSLOG_LEAVE(rdata);
	return true;
}


AosXmlTagPtr
AosDocClient::createMissingContainer(
		const AosRundataPtr &rdata,
		const OmnString &container_objid, 
		const bool ispublic) 
{
	// This function creates the missing container 'container_objid'.
	// AOSMONITORLOG_ENTER(rdata);
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, NULL);

	// AOSMONITORLOG_LINE(rdata);
	aos_assert_r(container_objid.length() > 0, 0);

	OmnString prefix, ctnr_cid;
	OmnString ctnr_objid = container_objid;
	AosObjid::decomposeObjid(ctnr_objid, prefix, ctnr_cid);

	u64 userid  = rdata->getUserid();
	OmnString caller_cid = AosCloudidSvr::getCloudid(userid, rdata);

	// If the container is not public but the container does not 
	// contain the caller's cloud id or the cloud id is not the
	// caller's cid, append the caller's cloud id to it.
	OmnString parent_ctnr_objid;
	OmnString stype;
	bool rslt = true;
	bool is_public = ispublic;
	if (!is_public)
	{
		// It is a private container. The 'ctnr_cid' must not be empty.
		// AOSMONITORLOG_LINE(rdata);
		if (ctnr_cid.length() == 0)
		{
			rdata->setError() << "Container Cloudid is empty: " << container_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			// AOSMONITORLOG_FINISH(rdata);
			return 0;
		}
		
		AosXmlTagPtr userdoc;
		if (userid) userdoc = getDocByDocid(userid, rdata);
		if (userdoc)
		{
			// AOSMONITORLOG_LINE(rdata);
			parent_ctnr_objid = userdoc->getAttrStr(AOSTAG_CTNR_HOME);
			if (prefix == parent_ctnr_objid)
			{
				// This means that 'ctnr_objid' is the parent container. 
				// Need to set the parent container to AOSCTNR_CID
				// AOSMONITORLOG_LINE(rdata);
				parent_ctnr_objid = AOSCTNR_CID;
			}
		}

		if (parent_ctnr_objid == "")
		{
			// This means 'userid' is 0 or it is not a valid userid. 
			// AOSMONITORLOG_LINE(rdata);
			parent_ctnr_objid = AosObjid::compose(AOSOBJIDPRE_LOSTaFOUND, ctnr_cid);
		}

		// Since it is a private doc, its parent container should be either the user's
		// home container (if we can find the user doc) or the container cid's 
		// Lost and Found container.
		AosXmlTagPtr pctnr_doc = getDocByObjid(parent_ctnr_objid, rdata);
		if (!pctnr_doc)
		{
			AosSetErrorU(rdata, "eContainerNotFound") 
				<< parent_ctnr_objid << enderr;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		
		if (!mIsRepairing)
		{
			rslt = AosSecurityMgrObj::getSecurityMgr()->checkAddMember1(pctnr_doc, rdata);
			if (!rslt)
			{
	 			// Access Denied
				// AOSMONITORLOG_FINISH(rdata);
	 			return 0;
			}
		}
		
		stype = AOSSTYPE_AUTO;
	}
	else
	{
		// AOSMONITORLOG_LINE(rdata);

		// Since it is a public container, we will not change the container's objid.
		if (!mIsRepairing)
		{
			rslt = AosSecurityMgrObj::getSecurityMgr()->checkAddPubObjid(ctnr_objid, rdata);
			if (!rslt)
			{
				// AOSMONITORLOG_FINISH(rdata);
				return 0;
			}
		}

		parent_ctnr_objid = AosObjid::composeLostFoundCtnrObjid(siteid);

		if (!mObjMgrObj) mObjMgrObj = AosObjMgrObj::getObjMgr();
		aos_assert_r(mObjMgrObj, 0);
		AosXmlTagPtr parent_doc = mObjMgrObj->createLostFoundCtnr(rdata);
		if (!parent_doc)
		{
			AosSetErrorU(rdata, "eContainerNotFound") 
				<< parent_ctnr_objid << enderr;
			return 0;
		}

		stype = AOSSTYPE_LOSTaFOUND;
	}

	AosXmlTagPtr cmd = AosRdataUtil::getCommand(rdata);
	AosXmlTagPtr doc;
	OmnString sobjids = "";
	if(cmd)
	{
		sobjids = cmd->getAttrStr(AOSTAG_SDOCCTNR);
		cmd->removeAttr(AOSTAG_SDOCCTNR);
	}

	if(sobjids != "")
	{
		// AOSMONITORLOG_LINE(rdata);
		rdata->setArg1(AOSARG_CTNR_OBJID, ctnr_objid);
		rdata->setArg1(AOSARG_PARENT_CTNR_OBJID, parent_ctnr_objid);
		rdata->setArg1(AOSARG_CTNR_SUBTYPE, stype);

		OmnStrParser1 parser(sobjids, ",");
		OmnString sdocid;
		AosSmartDocObjPtr smtobj = AosSmartDocObj::getSmartDocObj();
		aos_assert_r(smtobj, 0);

		while ((sdocid = parser.nextWord()) != "")
		{
			smtobj->runSmartdocs(sdocid, rdata);
			doc = rdata->getRetrievedDoc();
		}

		// Check whether the container was created by these smart docs.
		doc = getDocByObjid(ctnr_objid, rdata);
	}
	
	if (!doc)
	{
		// AOSMONITORLOG_LINE(rdata);
		OmnString docstr = "<container ";
		docstr << AOSTAG_OBJID << "=\"" << ctnr_objid<< "\" "
			<< AOSTAG_SITEID << "=\"" << siteid << "\" "
			<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
			<< AOSTAG_PARENTC << "=\"" << parent_ctnr_objid << "\" "
			<< AOSTAG_CREATOR << "=\"" << caller_cid << "\" "
			<< AOSTAG_STYPE << "=\"" << stype 
			<< "\"/>";
		doc = createDocSafe1(rdata, docstr, "", "",
			true, false, false, false, false, false);
		if (!doc)
		{
			AosSetErrorU(rdata, "failed_creating_doc") 
				<< ":" << docstr << enderr;
			return 0;
		}

		OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
		AosXmlTagPtr doc_tt = getDocByObjid(objid, rdata);
		aos_assert_r(doc_tt, 0);
		doc_tt = 0;
	}
		
	if (!doc)
	{
		OmnAlarm << "Failed to create missing container: " 
				<< ctnr_objid<< enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}
		
	rdata->setOk();
	// AOSMONITORLOG_FINISH(rdata);
	return doc;
}


AosXmlTagPtr
AosDocClient::cloneDoc(
		const AosRundataPtr &rdata,
		const OmnString &cloudid,
		const OmnString &fromobjid, 
		const OmnString &toobjid, 
		const bool is_public)
{
	// AOSMONITORLOG_ENTER(rdata);
	bool rslt = AosObjid::isValidObjid(toobjid, rdata);
	// AOSMONITORLOG_LINE(rdata);
	aos_assert_r(rslt, NULL);

	AosXmlTagPtr xml_orig = getDocByObjid(fromobjid, rdata);
	// AOSMONITORLOG_LINE(rdata);
	aos_assert_r(xml_orig, NULL);

	OmnString docstr = xml_orig->toString();
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
	if (!xml)
	{
		rdata->setError() << "Failed cloning doc: " << fromobjid << ":" << toobjid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return NULL;
	}

	xml->setAttr(AOSTAG_OBJID, toobjid);
	// AOSMONITORLOG_FINISH(rdata);
	return createDocSafe3(rdata, xml, cloudid, "", is_public,
		false, false, false, false, false, true);
}


AosXmlTagPtr 
AosDocClient::createDocSafe1(
		const AosRundataPtr &rdata,
		const OmnString &docstr,
		const OmnString &cloudid,
		const OmnString &objid_base,
		const bool is_public,
		const bool checkCreation,
		const bool keepDocid, 
		const bool reserved, 
		const bool cid_required, 
		const bool check_ctnr) 
{
	// AOSMONITORLOG_ENTER(rdata);
	OmnString errmsg;
	if (mIsStopping)
	{
		errmsg = "Server is stopping!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	// Chen Ding, 01/20/2012
	// AosXmlTagPtr doc;
	// if (!root || !(doc = root->getFirstChild()))
	if (!doc)
	{
		errmsg = "Failed to parse the doc!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	doc->normalize();
	
	// AOSMONITORLOG_FINISH(rdata);
	return createDocSafe3(rdata, doc, cloudid, objid_base, 
		is_public, checkCreation, keepDocid, 
		reserved, cid_required, check_ctnr, true);
}


AosXmlTagPtr
AosDocClient::createDocSafe3(
		const AosRundataPtr &rdata_ptr,
		const AosXmlTagPtr &newdoc,
		const OmnString &cloudid,
		const OmnString &objid_base,
		const bool is_public,
		const bool checkCreation,
		const bool keepDocid, 
		const bool reserved, 
		const bool cid_required, 
		const bool check_ctnr, 
		const bool saveDocFlag)
{
	AosRundata *rdata = rdata_ptr.getPtr();
	aos_assert_rr(newdoc, rdata, 0);
	aos_assert_rr(newdoc->isRootTag(), rdata, 0);
	AosXmlTagPtr root = newdoc;
	rdata->setDocByVar(AOSARG_NEW_DOC, newdoc, true);
	rdata->setCreatedDoc(newdoc, true);
	rdata->setWorkingDoc(newdoc, true);
	AOSDOCSVRLOG_DEFINE;

	u64 userid = rdata->getUserid();
	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		rdata->setError() << "Missing SiteId!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSDOCSVRLOG << rdata->getErrmsg();
		AOSDOCSVRLOG_CLOSE(newdoc, rdata);
		AOSLOG_LEAVE(rdata);
		return 0;
	}
	
	newdoc->normalize();
	newdoc->removeAttr(AOSTAG_FULLDOC_SIGNATURE);
	newdoc->setAttr(AOSTAG_SITEID, siteid);

	OmnString cid = cloudid;
	if (cid == "")
	{
		// AOSMONITORLOG_LINE(rdata);
		cid = AosCloudidSvr::getCloudid(userid, rdata);
	}

	if (!mIsRepairing && cid_required && cid == "")
	{
		// Chen Ding, 06/11/2012
		cid = rdata->getCid();
		if (cid == "")
		{
			AosSetErrorU(rdata, "missing_cloudid") << ": " << userid << enderr;
			AOSDOCSVRLOG << rdata->getErrmsg();
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return 0;
		}
	}


	// If there is no parent container, it will create one
	if (check_ctnr) 
	{
		// AOSMONITORLOG_LINE(rdata);
		if (is_public)
		{
			newdoc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		}

		AOSDOCSVRLOG;
		bool rslt = checkHomeContainer(rdata, newdoc, 0, cid);
		if (!rslt)
		{
			// AOSMONITORLOG_LINE(rdata);
			// Ketty RlbTest tmp.
			OmnAlarm << "Failed the check: " 
				<< rdata->getErrmsg() << enderr;
			AOSDOCSVRLOG << rdata->getErrmsg();
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			return 0;
		}
	}

	// Chen Ding, 05/21/2011
	// Check whether any of the containers were created. If not, 
	// prompt the user for creating the containers.
	u64 docid = AOS_INVDID;

	// Chen Ding, 03/14/2012, OBJID_CHANGE
	OmnString objid = newdoc->getAttrStr(AOSTAG_OBJID);
	OmnString intended_objid;
	if (mIsRepairing || reserved)
	{
		docid = newdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
		aos_assert_r(docid != 0, NULL);
		aos_assert_r(objid != "", 0);
	}
	else
	{
		bool exist;
		if (keepDocid) docid = newdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID, exist);
		if (docid == AOS_INVDID) 
		{
	 		// AOSMONITORLOG_LINE(rdata);
			// Chen Ding, 03/14/2012, OBJID_CHANGE
	 		// docid = AosDocidMgr::getSelf()->nextDocid(rdata);
	 		docid = AosDocidMgr::getSelf()->nextDocid(intended_objid, rdata);
			if (docid == AOS_INVDID)
			{
				rdata->setError() << "Failed retrieving next docid";
				OmnAlarm << rdata->getErrmsg() << enderr;
				AOSLOG_LEAVE(rdata);
				return 0;
			}
	 		newdoc->setAttr(AOSTAG_DOCID, docid);
		}
	}
	
	bool rslt = true;
	bool securityChecked = false;
	// Chen Ding, 03/14/2012, OBJID_CHANGE
	// OmnString objid = newdoc->getAttrStr(AOSTAG_OBJID);
	if (objid == "")
	{
		// AOSMONITORLOG_LINE(rdata);
		aos_assert_rr(!reserved, rdata, 0);
		// No objid being assigned yet. Will create an objid for it.
		// The objid is in the form: 'objid_' + <docid>
		// If it is not unique, we will add (n), where 'n' is a sequential
		// number, starting from 1.
		if (objid_base != "")
		{
			// It will construct the objid as:
			// 		objid_base + docid
			// AOSMONITORLOG_LINE(rdata);
			OmnString prefix = objid_base;
			prefix << docid;
			AOSDOCSVRLOG << docid;

			// Chen Ding, 03/14/2012
			aos_assert_rr(intended_objid != "", rdata, 0);
			objid = prefix;
			objid << intended_objid;
			if (!is_public)
			{
				if (cid == "")
				{
					rdata->setError() << "Missing cloud id!";
					OmnAlarm << rdata->getErrmsg() << enderr;
					AOSDOCSVRLOG << rdata->getErrmsg();
					AOSDOCSVRLOG_CLOSE(newdoc, rdata);
					AOSLOG_LEAVE(rdata);
					return 0;
				}
				objid = AosObjid::compose(objid, cid);
			}
		}
		else
		{
			// AOSMONITORLOG_LINE(rdata);
			if (!is_public && cid == "")
			{
				rdata->setError() << "To create a private doc but with no cid";
				OmnAlarm << rdata->getErrmsg() << enderr;
				AOSDOCSVRLOG << rdata->getErrmsg();
				AOSDOCSVRLOG_CLOSE(newdoc, rdata);
				AOSLOG_LEAVE(rdata);
				return 0;
			}

			// Chen Ding, 03/14/2012, OBJID_CHANGE
			// objid = AosObjid::getDftObjid(docid, is_public, cid, rdata);
			aos_assert_rr(intended_objid != "", rdata, 0);
			if (is_public) 
			{
				objid = intended_objid;
			}
			else
			{
				objid = AosObjid::compose(intended_objid, cid);
			}
			AOSDOCSVRLOG << objid;
			if (objid == "")
			{
				rdata->setError() 
					<< "Failed to get default objid (Should never happen!)";
				OmnAlarm << rdata->getErrmsg() << enderr;
				AOSDOCSVRLOG << rdata->getErrmsg();
				AOSDOCSVRLOG_CLOSE(newdoc, rdata);
				AOSLOG_LEAVE(rdata);
				return 0;
			}
		}

		if (is_public)
		{
			// AOSMONITORLOG_LINE(rdata);
			if (checkCreation && !mIsRepairing)
			{
				rslt = AosSecurityMgrObj::getSecurityMgr()->checkAddPubObjid(objid, rdata);
				if (!rslt)
				{
					OmnAlarm << "Failed security check: " << rdata->getErrmsg() << enderr;
					AOSDOCSVRLOG << rdata->getErrmsg();
					AOSDOCSVRLOG_CLOSE(newdoc, rdata);
					AOSLOG_LEAVE(rdata);
					return 0;
				}
			}
			securityChecked = true;
		}

		// Chen Ding, 07/01/2011
		// else
		// {
		// 	if (cid == "")
		// 	{
		// 		rdata->setError() << "Missing Cloud ID";
		// 		OmnAlarm << rdata->getErrmsg() << enderr;
		// 		AOSDOCSVRLOG << rdata->getErrmsg();
		// 		AOSDOCSVRLOG_CLOSE(newdoc, rdata->getErrmsg());
		// 		AOSMONITORLOG_FINISH(rdata);
		// 		return 0;
		// 	}
		//	
		//	objid = AosObjid::compose(objid, cid);
		//	AOSDOCSVRLOG << objid;
		// }
		//bool rslt = AosObjid::isValidObjid(objid, rdata);
		//aos_assert_rr(rslt, rdata, 0);
	}
	else
	{
		// AOSMONITORLOG_LINE(rdata);
		AosObjidType::E objidType;
		rslt = AosObjid::checkObjid(objid, objidType, rdata);
		if (!rslt)
		{
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return 0;
		}

		if (!is_public) 
		{
			// AOSMONITORLOG_LINE(rdata);
			if (cid == "")
			{
				rdata->setError() << "Missing Cloud ID";
				OmnAlarm << rdata->getErrmsg() << enderr;
				AOSDOCSVRLOG << rdata->getErrmsg();
				AOSDOCSVRLOG_CLOSE(newdoc, rdata);
				AOSLOG_LEAVE(rdata);
				return 0;
			}

			OmnString prefix, ccid;
			AosObjid::decomposeObjid(objid, prefix, ccid);
			if (ccid != cid && !mIsRepairing)
			{
				// AOSMONITORLOG_LINE(rdata);
				objid = AosObjid::compose(objid, cid);
			}
		}
		else
		{
			OmnString prefix, ccc;
			AosObjid::decomposeObjid(objid, prefix, ccc);
			if (ccc != cid)
			{
				// AOSMONITORLOG_LINE(rdata);
				if (checkCreation && !mIsRepairing)
				{
					rslt = AosSecurityMgrObj::getSecurityMgr()->checkAddPubObjid(objid, rdata);
					if (!rslt)
			 		{
				 		OmnAlarm << "Not able to add objid: " << rdata->getErrmsg() << enderr;
						AOSDOCSVRLOG << rdata->getErrmsg();
						AOSDOCSVRLOG_CLOSE(newdoc, rdata);
						AOSLOG_LEAVE(rdata);
				 		return 0;
					}
				}
				securityChecked = true;
			}
		}
	}

	rslt = AosObjid::isValidObjid(objid, rdata);
	if (!rslt)
	{
		AOSDOCSVRLOG << rdata->getErrmsg();
		AOSDOCSVRLOG_CLOSE(newdoc, rdata);
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	// Chen Ding, 09/28/2010
	// objid might have been changed. Need to set to the doc
	newdoc->setAttr(AOSTAG_OBJID, objid);	
	
	// 2. Check whether the user can create the doc
	// AOSMONITORLOG_LINE(rdata);
	OmnString pctnr_objid = newdoc->getAttrStr(AOSTAG_PARENTC);
	AosXmlTagPtr pctnr_doc;
	if (pctnr_objid == "")
	{
		if (objid != AosObjid::composeSysRootAcctObjid(rdata->getSiteid()))
		{
			AosSetErrorU(rdata, "eObjidIsNull") 
				<< newdoc->getAttrStr(AOSTAG_OBJID) << enderr;
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return NULL;
		}
	}
	else 
	{
		pctnr_doc = getDocByObjid(pctnr_objid, rdata);
		if (!pctnr_doc)
		{
			AosSetErrorU(rdata, "eContainerNotFound") 
				<< newdoc->getAttrStr(AOSTAG_OBJID) << enderr;
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return 0;
		}

		// Chen Ding, 01/19/2012
		// 'checkCreate(...)' need the doc to be created.
		if (checkCreation && !mIsRepairing && !securityChecked)
		{
			rslt = AosCheckCreateDoc(rdata, pctnr_doc, newdoc);
			if (!rslt)
			{
				OmnAlarm << "Failed security: " << rdata->getErrmsg() << enderr;
				AOSDOCSVRLOG << rdata->getErrmsg();
				AOSDOCSVRLOG_CLOSE(newdoc, rdata);
				AOSLOG_LEAVE(rdata);
				return 0;
			}
		}
	}

	// Linda. 2013/05/07 move to 
	//// Chen Ding, 2011/02/02
	//// Check whether the doc is already created. 
	// Ketty RlbTest tmp.
	//AosXmlTagPtr thedoc1 = getDocByObjid(objid, rdata);
	//if (thedoc1)
	//{
		// The doc was already created. Do not create it anymore.
	//	AOSLOG_LEAVE(rdata);
	//	return thedoc1;
	//}

	if (cid == "")
	{
		// AOSMONITORLOG_LINE(rdata);
		OmnAlarm << "Missing user!" << enderr;
		newdoc->setAttr(AOSTAG_CREATOR, AOSCLOUDID_UNKNOWN);
	}
	else
	{
		newdoc->setAttr(AOSTAG_CREATOR, cid);
	}

	if (!mIsRepairing)
	{
		// AOSMONITORLOG_LINE(rdata);
		OmnString ctime = OmnGetTime(AosLocale::getDftLocale());
		newdoc->setAttr(AOSTAG_CTIME, ctime);
		newdoc->setAttr(AOSTAG_MTIME, ctime);

		u32 systemsec = OmnGetSecond();
		newdoc->setAttr(AOSTAG_CT_EPOCH, systemsec);
		newdoc->setAttr(AOSTAG_MT_EPOCH, systemsec);

		newdoc->removeAttr(AOSTAG_VERSION);
	}

	// Chen Ding, 01/22/2012
	if (pctnr_objid == objid)
	{
		rslt = handleObjidIssue(objid, newdoc, true, rdata);
		if (!rslt)
		{
			return 0;
		}
	}
	

	// Bind the objid 
	if (!reserved) 
	{
		// AOSMONITORLOG_LINE(rdata);
		bool objid_changed = false;
		// OmnScreen << "To bind: " << objid << ":" << docid << endl;
		rslt = AosDocidMgr::getSelf()->bindObjid(
			objid, docid, objid_changed, true, true, rdata);
		if (!rslt)
		{
			AOSDOCSVRLOG << rdata->getErrmsg();
			AOSDOCSVRLOG_CLOSE(newdoc, rdata);
			AOSLOG_LEAVE(rdata);
			return 0;
		}

		if (objid_changed)
		{
			// AOSMONITORLOG_LINE(rdata);
			newdoc->setAttr(AOSTAG_OBJID, objid);
		}
	}

	//Linda, 2013/05/07
	// Check whether the doc is already created. 
	AosXmlTagPtr thedoc = getDocByObjid(objid, rdata);
	if (thedoc)
	{
		// The doc was already created. Do not create it anymore.
		AOSLOG_LEAVE(rdata);
		return thedoc;
	}

	// If it is a user account record, it should bind the cloud id.
	if (newdoc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT)
	{
		// AOSMONITORLOG_LINE(rdata);
		OmnString cid = newdoc->getAttrStr(AOSTAG_CLOUDID);
		if (cid != "")
		{
			// AOSMONITORLOG_LINE(rdata);
			u64 did = getDocidByCloudid(siteid, cid, rdata);
			if (did == 0)
			{
				rslt = bindCloudid(cid, docid, rdata);
				if (!rslt)
				{
					// AOSMONITORLOG_LINE(rdata);
					//OmnAlarm << "Failed to bind cloud id:" 
					//	<< cid << ":" << docid << enderr;
					AOSDOCSVRLOG << cid << ":" << docid;
				}
			}
			else
			{
				if (did != docid)
				{
					rdata->setError() << "Failed to create user account!";
					// AOSMONITORLOG_FINISH(rdata);
					AOSLOG_LEAVE(rdata);
					return 0;
				}
			}
		}
	}

	//add version
	if (!mIsRepairing)
	{
		//Jozhi 2013/07/05
		//aos_assert_r(mVersionServerObj, 0);
		//if (!mVersionServerObj->addVersionObj(newdoc, rdata))
		//{
		//	// AOSMONITORLOG_LINE(rdata);
		//	OmnAlarm << "Failed creating the version obj: " << newdoc->getAttrStr(AOSTAG_OBJID) << enderr;
		//}
	}

	rslt = AosEvent::checkEvent1(0, newdoc, AOSEVENTTRIGGER_DOC_PRE_CREATED, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
		// AOSMONITORLOG_FINISH(rdata);
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
	{
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	// Add a copy of the doc. Note that the background thread
	// needs 'doc', but it will not modify it. If we do not 
	// add a copy of 'doc' to DocMgr, it is possible that the
	// doc will be modified while the background thread is
	// processing 'doc'. Chen Ding, 2011/02/05

	// Chen Ding, 2014/11/09
	/*
	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, 0);
	u64 snap_id = rdata->getSnapshotId();
	sgDocMgr->addCopy(docid, newdoc, snap_id, rdata);

	AosTransPtr trans = OmnNew AosCreateDocSafeTrans(docid, newdoc, saveDocFlag, true, false, snap_id);
	rslt = addReq(rdata, trans);
	
	if (!rdata->isOk())
	{
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	AosDocReqPtr doc_req = AosDocProc::getSelf()->addCreateRequest(
		root, userid, newdoc, 0, 0, rdata);
	aos_assert_r(doc_req, 0);

	if (newdoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_LOG) 
	{
		AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_CREATEDOC, rdata);
	}
	*/

	rslt = mTransHandler->createDoc(rdata, this, root, newdoc, saveDocFlag, docid);
	if (!rslt)
	{
		/*AosLogError(rdata, true, AosErrmsgId::eFailedCreatingDatalet)
			<< AosEnumFieldName::eDocid << docid
			<< AosEnumFieldName::eSnapshot << rdata->getSnapshotId() 
			<< AosEnumFieldName::eUserid << userid << enderr;*/
		return 0;
	}

	rdata->setOk();

	rslt = AosEvent::checkEvent1(0, newdoc, AOSEVENTTRIGGER_DOC_CREATED, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
	}

	AOSLOG_LEAVE(rdata);
	return newdoc;
}


AosXmlTagPtr
AosDocClient::createDocByTemplate1(
		const AosRundataPtr &rdata,
		const OmnString &cid,
		const OmnString &objid,
		const OmnString &template_objid)
{
	// This function creates a new object based on the template identified
	// by 'template_objid'. Note that this function does not check the
	// access. The caller should do the checking.
	AOSLOG_ENTER_R(rdata, 0);
	aos_assert_r(objid != "", 0);

	// 1. Make sure the object was not created yet
	AosXmlTagPtr doc = getDocByObjid(objid, rdata);
	if (doc)
	{
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return doc;
	}

	// 2. Retrieve the template
	AosXmlTagPtr tmpl = getDocByObjid(template_objid, rdata);
	if (!tmpl)
	{
		AosSetErrorU(rdata, "create_failed_001") << ": " << template_objid << enderr;
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	// Chen Ding, 01/21/2012
	if (tmpl->isRootTag())
	{
		AosSetErrorU(rdata, "internal_error") << ": " << template_objid << enderr;
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	// Chen Ding, 01/21/2012
	AosXmlTagPtr newdoc = tmpl;
	rdata->setCreatedDoc(newdoc, false);
	rdata->setDocByVar(AOSARG_NEW_DOC, newdoc, false);

	bool rslt = true;
	// 3. Determine the parent container
	OmnString parent_objid = newdoc->getAttrStr(AOSTAG_REAL_PARENT);
	if (parent_objid == "") 
	{
		// AOSMONITORLOG_LINE(rdata);
		parent_objid = AosObjid::compose(AOSOBJIDRES_USER_HOMEFOLDER, cid);
	}
	else
	{
		if (cid != "")
		{
			// AOSMONITORLOG_LINE(rdata);
			OmnString errmsg;
			rslt = AosObjid::procObjid(parent_objid, cid, errmsg);
			if (!rslt)
			{
				AosSetErrorU(rdata, errmsg) << enderr;
			}
		}
	}
	newdoc->removeAttr(AOSTAG_REAL_PARENT);
	newdoc->setAttr(AOSTAG_PARENTC, parent_objid);

	// 4. Determine otype
	OmnString otype = newdoc->getAttrStr(AOSTAG_REAL_OTYPE);
	if (otype == "") otype = AOSOTYPE_DOC;
	newdoc->setAttr(AOSTAG_OTYPE, otype);
	newdoc->removeAttr(AOSTAG_REAL_OTYPE);

	// 5. Set objid
	newdoc->setAttr(AOSTAG_OBJID, objid);

	newdoc->removeAttr(AOSTAG_TEMPLATE);
	newdoc->setAttr(AOSTAG_CREATOR, cid);

	// Time to create the doc
	rslt = createDoc1(rdata, 0, cid, newdoc, true, 0, 0, false);
	if (!rslt)
	{
		// Failed creating the doc.
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	AOSLOG_LEAVE(rdata);
	return newdoc;
}


AosXmlTagPtr
AosDocClient::createRootCtnr(
		const OmnString &docstr, 
		const AosRundataPtr &rdata_ptr)
{
	// Root container is the toppest container in the entire site. 
	// This should be the first doc to be created. The function 
	// should be called when the system starts up. There is no
	// security check. 
	AosRundata *rdata = rdata_ptr.getPtr();
	AOSLOG_ENTER_R(rdata, 0);

	AosXmlParser parser;
	AosXmlTagPtr newdoc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(newdoc, 0);

	// Chen Ding, 01/21/2012
	rdata->setWorkingDoc(newdoc, true);
	rdata->setDocByVar(AOSARG_NEW_DOC, newdoc, true);
	rdata->setCreatedDoc(newdoc, true);

	OmnString objid = newdoc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid != "", 0);

	OmnString ctime = OmnGetTime(AosLocale::getDftLocale());
	newdoc->setAttr(AOSTAG_CTIME, ctime);
	newdoc->setAttr(AOSTAG_MTIME, ctime);

	u32 systemsec = OmnGetSecond();
	newdoc->setAttr(AOSTAG_CT_EPOCH, systemsec);
	newdoc->setAttr(AOSTAG_MT_EPOCH, systemsec);
	newdoc->removeAttr(AOSTAG_VERSION);

	// Create the docid
	u64 docid = newdoc->getAttrU64(AOSTAG_DOCID, 0);
	if (docid == AOS_INVDID)
	{
		OmnAlarm << "Failed to retrieve docid!" << enderr;
		AOSLOG_LEAVE(rdata);
		return 0;
	}
		
	// Bind the objid and docid
	bool objid_changed = false;
	newdoc->setAttr(AOSTAG_DOCID, docid);
	bool rslt = AosDocidMgr::getSelf()->bindObjid(
		objid, docid, objid_changed, true, true, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to bind objid: " << objid 
			<< ":" << docid;
		//OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	newdoc->setAttr(AOSTAG_OBJID, objid);
	OmnString cid = rdata->getCid();
	if (cid == "")
	{
		rdata->setError() << "Missing cloudid: " << rdata->getUserid();
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	newdoc->setAttr(AOSTAG_CREATOR, cid); 
	 
	//add version
	if (!mIsRepairing)
	{
		//Jozhi 2013/07/05
		//aos_assert_r(mVersionServerObj, 0);
		//if (!mVersionServerObj->addVersionObj(newdoc, rdata))
		//{
		//	// AOSMONITORLOG_LINE(rdata);
		//	OmnAlarm << "Failed creating the version obj: " << newdoc->getAttrStr(AOSTAG_OBJID) << enderr;
		//}
	}

	// Chen Ding, 01/21/2012
	// Need to check the event
	rslt = AosEvent::checkEvent1(0, newdoc, AOSEVENTTRIGGER_DOC_PRE_CREATED, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
	{
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	// Chen Ding, 2014/11/09
	/*
	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, 0);
	
	u64 snap_id = rdata->getSnapshotId();
	sgDocMgr->addCopy(docid, newdoc, snap_id, rdata);

	AosTransPtr trans = OmnNew AosCreateDocTrans(docid, newdoc, true, false, snap_id);
	rslt = addReq(rdata, trans);
	
	if (!rdata->isOk())
	{
		return NULL;
	}

	AosDocReqPtr doc_req = AosDocProc::getSelf()->addCreateRequest(
			newdoc, 0, newdoc, 0, 0, rdata);
	aos_assert_rr(doc_req, rdata, 0);
	*/
	rslt = mTransHandler->createDoc(rdata, this, 0, newdoc, newdoc, docid);
	if (!rslt)
	{
		/*AosLogError(rdata, true, AosErrmsgId::eFailedCreatingDatalet)
			<< AosEnumFieldName::eDocid << docid
			<< AosEnumFieldName::eSnapshot << rdata->getSnapshotId() << enderr;*/
		return 0;
	}

	// Add System log Brian Zhang 23/12/2011
	//AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_CREATEDOC, rdata);
	rslt = AosEvent::checkEvent1(0, newdoc, AOSEVENTTRIGGER_DOC_CREATED, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
		// AOSMONITORLOG_FINISH(rdata);
	}

	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return newdoc;
}


bool
AosDocClient::createTempDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	// It saves the temporary doc into a file and creates a new objid
	// for the doc. The new objid is:
	if (AosDocClient::getSelf()->isRepairing())
	{
		return true;
	}

	aos_assert_r(doc, false);

	OmnString otype = doc->getAttrStr(AOSTAG_OTYPE);
	if (otype == AOSOTYPE_LOG || otype == AOSOTYPE_BINARYDOC)
	{
		return true;
	}

	// u64 docid = 0;

	// Ketty 2014/07/04
	return true;
	aos_assert_r(mSeLogClientObj, false);
	// Chen Ding, 2013/01/06
	// docid = mSeLogClientObj->addLogWithResp(
	// 		AOSCTNR_TEMPOBJ, AOSSYSLOGNAME_TEMPOBJ, doc->toString(), rdata);
	bool rslt = mSeLogClientObj->addLog(rdata, AOSCTNR_TEMPOBJ, 
		AOSSYSLOGNAME_TEMPOBJ, doc->toString());
	if (!rslt)
	{
		OmnAlarm << "Failed creating temp object: " << rdata->getErrmsg() << enderr;
		return false;
	}

	// Chen Ding, 2013/01/06
	// Need to rework on it!!!!!!!!!!!!!!!!
	// AosSetError(rdata, "create_tmpdoc") << ": " << docid;
	return true;
}


AosXmlTagPtr 
AosDocClient::createBinaryDoc(
		const OmnString &docstr,
		const bool is_public, 
		const AosBuffPtr &buff,
		const int &vid,
		const AosRundataPtr &rdata)
{
	// doc format 
	// <doc zky_otype = AOSOTYPE_BINARYDOC>
	//		<![BDATA[buff data]]>
	// </doc>
	aos_assert_r(buff && buff->dataLen() > 0, 0);
	aos_assert_r(docstr != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	if (!doc)
	{
		rdata->setError() << "Failed to parse the doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}

	if (doc->getAttrStr(AOSTAG_OTYPE, "") != AOSOTYPE_BINARYDOC)
	{
		OmnAlarm << "zky_otype mismatch:" << doc->getAttrStr(AOSTAG_OTYPE, "") 
			<< ":" << AOSOTYPE_BINARYDOC << enderr;
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	}

	//// 1. compress
	//Linda, 2013/04/10
	//doc->setAttr(AOSTAG_BINARY_SOURCELEN, buff->dataLen());
	//AosBuffPtr docBuff = buff;
	//OmnString ctnr_objid = doc->getAttrStr(AOSTAG_PARENTC, "");	
	//if (ctnr_objid != "")
	//{
	//	AosXmlTagPtr dd = getDocByObjid(ctnr_objid, rdata);
	//	if (dd)
	//	{
	//		OmnString compress_alg = dd->getAttrStr(AOSTAG_COMPRESS_ALG, "");
	//		if (compress_alg != "")
	//		{
	//			u64 destLen = 0;
	//			bool rslt = AosCompress::compressStatic(
	//					compress_alg, docBuff, destLen, buff,
	//					buff->dataLen(), rdata AosMemoryCheckerArgs);
	//			aos_assert_r(rslt, 0);
	//			aos_assert_r(docBuff->dataLen() > 0 && (u32)docBuff->dataLen() == destLen, 0);
	//		}
	//	}
	//}
	//OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
	//doc->setTextBinary(nodename, docBuff);
	
	OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
	doc->setTextBinary(nodename, buff);

	// 2. create doc 
	bool keepDocid = false;
	if (vid >= 0) 
	{
		keepDocid = true;
		OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
		u64 docid = AosDocidMgr::getSelf()->nextDocid(vid, objid, rdata);
		doc->setAttr(AOSTAG_DOCID, docid);
		doc->setAttr(AOSTAG_OBJID, objid);
		aos_assert_r(AosGetCubeId(AosXmlDoc::getOwnDocid(docid)) == vid, 0);
	}

	AosXmlTagPtr xml = createDocSafe3(rdata, doc, "", "",
		is_public, false, keepDocid, false, false, true, true);
	if (!xml)
	{
		AosSetErrorU(rdata, "failed_to_create") << enderr;
		return 0;
	}

	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, 0);
	
	// u64 snap_id = rdata->getSnapshotId();
	sgDocMgr->deleteDoc(xml->getAttrU64(AOSTAG_DOCID, 0), rdata);
	return xml;
}


bool 
AosDocClient::appendBinaryDoc(
		const OmnString &objid, 
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDocClient::createJQLTable(	
		const AosRundataPtr &rdata,
		const OmnString &table_def)
{
	bool islogin = AosCheckIsLogin(rdata);
	if (!islogin)
	{
		return false;
	}

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(table_def, "" AosMemoryCheckerArgs);
	if (!xml)
	{
		OmnString errmsg = "failed to parse xml !";
		AosSetError(rdata, "docclient_failed_to_parse_xml") << errmsg;
		rdata->setReturnCode(AosReturnCode::eSyntaxError);
		return false;
	}

	xml->setAttr(AOSTAG_HPCONTAINER, "jql_ctnr");
	OmnString tablename = xml->getAttrStr("tablename", "");
	if(tablename == "")
	{
		OmnString errmsg = "missing tablename !";
		AosSetError(rdata, "docclient_missing_table_name") << errmsg;
		rdata->setReturnCode(AosReturnCode::eMissingAttr);
		return false;
	}

	AosXmlTagPtr newdoc = createDoc(xml, rdata);
	if (newdoc)
	{
		u64 docid = newdoc->getAttrU64(AOSTAG_DOCID, 0);
		OmnString iilname = AosIILName::composeSQLTablenameIILName();
		AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
		bool rslt = iilclient->addStrValueDoc(iilname, tablename, docid, true, false, rdata);
		if(!rslt)
		{
			OmnString errmsg = "internal error!";
			AosSetError(rdata, "docclient_internal_error") << errmsg;
			rdata->setReturnCode(AosReturnCode::eInternalError);
			return false;
		}
	}
	else
	{
		OmnString errmsg = "failed to create doc!";
		AosSetError(rdata, "docclient_failed_to_create_doc") << errmsg;
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return false;
	}

	return true;
}

bool
AosDocClient::createDocByJQL( 
		const AosRundataPtr &rdata,
		const OmnString &container_objid,
		const OmnString &objid,
		const OmnString &doc)
{
	bool islogin = AosCheckIsLogin(rdata);
	if (!islogin)
	{
		return false;
	}

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(doc, "" AosMemoryCheckerArgs);
	if (!xml)
	{
		OmnString errmsg = "failed to parse the doc!";
		AosSetError(rdata, "docclient_failed_to_parse_doc") << errmsg;
		rdata->setReturnCode(AosReturnCode::eSyntaxError);
		return false;
	}

	xml->setAttr(AOSTAG_HPCONTAINER, container_objid);
	xml->setAttr(AOSTAG_OBJID, objid);
	rdata->setReceivedDoc(xml, true);

	bool rslt = createDoc1(rdata, 0, rdata->getCid(), xml, false, 0, 0, false); 
	//AosXmlTagPtr newdoc = AosGetDocByObjid(objid, rdata);
	if (!rslt)
	{
		OmnString errmsg = "failed to create the doc!";
		AosSetError(rdata, "docclient_failed_to_create_doc") << errmsg;
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return false;
	}

	return true;
}


/*
 * Chen Ding, 2014/11/09
 * This function is no longer needed
AosXmlTagPtr 
AosDocClient::createBinaryDocByStat(
		const OmnString &docstr,
		const AosBuffPtr &buff,
		const int &vid,
		const u64 &normal_snap_id,
		const u64 &binary_snap_id,
		const AosRundataPtr &rdata)
{
	// doc format 
	// <doc zky_otype = AOSOTYPE_BINARYDOC>
	//		<![BDATA[buff data]]>
	// </doc>
	aos_assert_r(buff && buff->dataLen() > 0, 0);
	aos_assert_r(docstr != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

	if (doc->getAttrStr(AOSTAG_OTYPE, "") != AOSOTYPE_BINARYDOC)
	{
		OmnAlarm << "zky_otype mismatch:" << doc->getAttrStr(AOSTAG_OTYPE, "") 
			<< ":" << AOSOTYPE_BINARYDOC << enderr;
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	}

	OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
	doc->setTextBinary(nodename, buff);

	//OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	//u64 docid = AosDocidMgr::getSelf()->nextDocid(vid, objid, rdata);
	//doc->setAttr(AOSTAG_DOCID, docid);
	//doc->setAttr(AOSTAG_OBJID, objid);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);	
	aos_assert_r(docid, 0);

	aos_assert_r(doc->getAttrStr(AOSTAG_OBJID) != "", 0);
	aos_assert_r(AosGetCubeId(AosXmlDoc::getOwnDocid(docid)) == vid, 0);
	doc->setAttr(AOSTAG_SITEID, rdata->getSiteid());

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosCreateDocTrans(docid, doc, false, false, snap_id);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, 0);
	aos_assert_r(rdata->isOk(), 0);

	return doc;
}

*/

bool
AosDocClient::insertInto(
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata)
{
	//insert data formate: 
	//<data zky_name="_zt44_table_name">
	//		<datafield zky_name="key_field1"><![CATA[xxxx]]></datafield>
	//		<datafield zky_name="key_field2"><![CATA[xxxx]]></datafield>
	//		<datafield zky_name="key_field3"><![CATA[xxxx]]></datafield>
	//</data>
	//
	//table describe
	//<sqltable zky_def_index="true" zky_use_schema="_zt4g_schema_t1_paser" zky_database="db">
	//	<columns>
	//		<column name="key_field1" datatype="double" size="8" offset="0"></column>
	//	    <column name="key_field2" datatype="string" size="20" offset="8"></column>
	//	    <column name="key_field3" datatype="string" size="20" offset="28"></column>
	//		<column name="vf_field1" datatype="string" size="10" type="virtual">
	//			<keys>
	//				<key><![CDATA[`key_field1`]]></key>
	//			</keys>
	//			<map zky_type="iilmap" zky_name="map1" zky_iilname="_zt44_map_t1_map1" datatype="string">
	//				<keys>
	//					<key><![CDATA[`key_field1`]]></key>
	//				</keys>
	//				<values>
	//					<value datatype="str" max_len="18" type="field"><![CDATA[`zky_docid`]]></value>
	//				</values>
	//			</map>
	//		</column>
	//	</columns>
	//	<indexes>
	//		<index zky_name="key_field1" zky_iilname="_zt44_idx_t1_key_field1" idx_name="_zt4g_idxmgr_idx_t1_key_field1"/>
	//	</indexes>
	//	<cmp_indexes>
	//		<cmp_index zky_iilname="_zt44_idx_t1_key_field1_key_field2_key_field3">
	//			<key_field1 zky_name="key_field1"/>
	//			<key_field2 zky_name="key_field2"/>
	//			<key_field3 zky_name="key_field3"/>
	//		</cmp_index>
	//	</cmp_indexes>
	//	<maps>
	//		<map zky_type="iilmap" zky_name="map1" zky_iilname="_zt44_map_t1_map1">
	//			<keys>
	//				<key><![CDATA[`key_field1`]]></key>
	//			</keys>
	//			<values>
	//				<value datatype="str" max_len="18" type="field"><![CDATA[`zky_docid`]]></value>
	//			</values>
	//		</map>
	//	</maps>
	//	<statistic stat_doc_objid="_zt4g_statistics_t1_db"/>
	//<sqltable>
	//
	AosRundata* rdata_raw = rdata.getPtr();
	OmnString table_name = data->getAttrStr("zky_name", "");
	aos_assert_r(table_name != "", false);
	AosXmlTagPtr table_doc = AosGetDocByObjid(table_name, rdata);
	aos_assert_r(table_doc, false);

	AosDataRecordObjPtr record = createDataRecord(table_doc, data, rdata_raw);
	aos_assert_r(record, false);

	//insert group doc
	bool rslt = insertDoc(record, rdata_raw);
	aos_assert_r(rslt, false);

	//insert index
	AosXmlTagPtr index_tags = table_doc->getFirstChild("indexes");
	if (index_tags)
	{
		rslt = insertIndex(index_tags, record, rdata_raw);
		aos_assert_r(rslt, false);
	}

	//insert cmp index
	AosXmlTagPtr cmp_index_tags = table_doc->getFirstChild("cmp_indexes");
	if(cmp_index_tags)
	{
		rslt = insertCmpIndex(cmp_index_tags, record, rdata_raw);
		aos_assert_r(rslt, false);
	}

	//insert map
	AosXmlTagPtr maps_tag = table_doc->getFirstChild("maps");	
	if (maps_tag)
	{
		rslt = insertMap(maps_tag, record, rdata_raw);
	}

	//insert statistic
	//<statistic zky_stat_name="db_t1_stat_uw_st5" zky_objid="_zt4g_statistics_t1_db">
	//	<statistic_defs>
	//		<statistic zky_stat_conf_objid="_zt4k_uw_st5"/>
	//	</statistic_defs>
	//	<internal_statistics>
	//		<statistic zky_stat_identify_key="key_field3" zky_stat_conf_objid="db_t1_stat_uw_st5_internal_0"/>
	//	</internal_statistics>
	//</statistic>
	AosXmlTagPtr statistic_tag = table_doc->getFirstChild("statistic");
	if (statistic_tag)
	{
		OmnString db_name = table_doc->getAttrStr("zky_database", "");
		aos_assert_r(db_name != "", false);
		OmnString stat_objid = statistic_tag->getAttrStr("stat_doc_objid", "");
		aos_assert_r(stat_objid != "", false);
		AosXmlTagPtr stat_doc = AosGetDocByObjid(stat_objid, rdata);
		aos_assert_r(stat_doc, false);
		AosXmlTagPtr norm_stat_tags = stat_doc->getFirstChild("statistic_defs");
		if (norm_stat_tags)
		{
			rslt = insertNormalStat(db_name, stat_doc, record, rdata_raw);
			aos_assert_r(rslt, false);
		}
		AosXmlTagPtr internal_stat_tags = stat_doc->getFirstChild("internal_statistics");
		if (internal_stat_tags)
		{
			rslt = insertInternalStat(internal_stat_tags, record, rdata_raw);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}

AosDataRecordObjPtr
AosDocClient::createDataRecord(
		const AosXmlTagPtr &table_doc,
		const AosXmlTagPtr &data,
		AosRundata* rdata_raw)
{
	//dataschema describe
	//<dataschema zky_dataschema_type="static" zky_name="t1_paser">
	//	<datarecord type="csv" zky_field_delimiter="," zky_name="t1_schm" zky_row_delimiter="LF" zky_text_qualifier="NULL">
	//		<datafields>
	//			<datafield type="double" zky_length="8" zky_name="key_field1"/>
	//			<datafield type="str" zky_datatooshortplc="cstr" zky_length="20" zky_name="key_field2"/>
	//			<datafield type="str" zky_datatooshortplc="cstr" zky_length="20" zky_name="key_field3"/> 
	//		</datafields>
	//	</datarecord>
	//</dataschema>
	OmnString schema_name = table_doc->getAttrStr("zky_use_schema", "");
	aos_assert_r(schema_name != "", 0);
	AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_name, rdata_raw);
	aos_assert_r(schema_doc, 0);
	AosXmlTagPtr record_tag = schema_doc->getFirstChild(true);
	aos_assert_r(record_tag, 0);
	u64 record_docid = record_tag->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(record_docid, 0);


	AosXmlTagPtr clone_record_tag = record_tag->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(clone_record_tag, 0);
	vector<AosXmlTagPtr> virtual_field_tags;// = AosDataFieldObj::getVirtualField(table_doc, rdata_raw);
	AosXmlTagPtr datafieldsNode = clone_record_tag->getFirstChild("datafields");
	aos_assert_r(datafieldsNode, 0);

	set<OmnString> fieldnames;
	for (size_t i=0; i<virtual_field_tags.size(); i++)
	{
		OmnString fieldName = virtual_field_tags[i]->getAttrStr("zky_name", "");
		aos_assert_r(fieldName != "", 0);
		if (fieldnames.count(fieldName) == 0)
		{
			datafieldsNode->addNode(virtual_field_tags[i]);
			fieldnames.insert(fieldName);
		}
	}

	AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(clone_record_tag, 0, rdata_raw AosMemoryCheckerArgs);
	aos_assert_r(record, 0);
	AosBuffPtr buff = OmnNew AosBuff(1024*1024 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	record->setMemory(buff->data(), buff->buffLen());

	int i=0;
	OmnString field_name;
	OmnString field_value;
	AosXmlTagPtr field_tag = data->getFirstChild(true);
	while(field_tag)
	{
		field_name = field_tag->getAttrStr("zky_name", "");
		int idx = record->getFieldIdx(field_name, rdata_raw);
		aos_assert_r(idx == i, 0);
		field_value = field_tag->getNodeText();
		AosValueRslt value(field_value);
		bool outofmem = false;
		record->setFieldValue(idx, value, outofmem, rdata_raw);
		if (value.isNull())
		{
			AosDataFieldObj* datafield = record->getFieldByIdx1(idx);
			aos_assert_r(datafield, 0);
			if (datafield->mFieldInfo.notnull)
			{
				return 0;
			}
		}

		i++;
		field_tag = data->getNextChild();
	}
	record->setDocid(AosGetNextDocid(record_docid, rdata_raw));
	return record;
}

bool
AosDocClient::insertDoc(
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	int rcd_len = record->getRecordLen();
	char * doc = record->getData(rdata_raw);
	u64 docid = record->getDocid();
	aos_assert_r(docid != 0, false);
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	buff->setU64(docid);
	buff->setInt(rcd_len);
	buff->setBuff(doc, rcd_len);
	buff->reset();

	u32 vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	u64 snapshot_id = 0;
	snapshot_id = createSnapshot(rdata_raw, vid, snapshot_id,  AosDocType::eGroupedDoc, 0);
	docid = buff->getU64(0);
	aos_assert_r(docid, false);
	AosSengineDocInfoPtr doc_info;
	AosDataRecordType::E type = record->getType();
	if (type == AosDataRecordType::eCSV)
	{
		doc_info = OmnNew AosSengineDocInfoCSV(AosGroupDocOpr::eBatchInsert, vid, snapshot_id, 1);
	}
	else if (type == AosDataRecordType::eFixedBinary)
	{
		doc_info = OmnNew AosSengineDocInfoFixed(AosGroupDocOpr::eBatchInsert, vid, snapshot_id, 1);
	}
	else
	{
		OmnNotImplementedYet;
		return false;
	}
	int record_len = AosGetDataRecordLenByDocid(rdata_raw->getSiteid(), docid, rdata_raw);
	doc_info->createMemory(1024*1024*4, record_len);
	aos_assert_r(doc_info, false);

	int record_size = buff->getInt(0);                                                                                 
	aos_assert_r(record_size, false);
	int64_t offset = buff->getCrtIdx();
	bool rslt = doc_info->addDoc(buff->data() + offset, record_size, docid, rdata_raw);
	aos_assert_r(rslt, false);
	doc_info->sendRequestPublic(rdata_raw);
	return true;
}

bool
AosDocClient::insertIndex(
		const AosXmlTagPtr &index_tags,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	aos_assert_r(index_tags, false);
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	OmnString iil_name, field_name, type;
	int idx = -1;
	AosXmlTagPtr index_tag = index_tags->getFirstChild(true);
	while(index_tag)
	{
		type = index_tag->getAttrStr("zky_type", "");
		if (type == "cmp")
		{
			index_tag = index_tags->getNextChild();
			continue;
		}
		field_name = index_tag->getAttrStr("zky_name", "");
		aos_assert_r(field_name != "", false);
		idx = record->getFieldIdx(field_name, rdata_raw);
		aos_assert_r(idx != -1, false);

		AosValueRslt value;
		bool outofmem = false;
		record->getFieldValue(idx, value, outofmem, rdata_raw);
		//if (value.isNull())
		//{
		//	AosDataFieldObj* datafield = record->getFieldByIdx1(idx);
		//	aos_assert_r(datafield, false);
		//	AosStrValueInfo info = datafield->getFieldInfo();
		//	if (info.notnull)
		//	{
		//		continue;
		//	}
		//}

		iil_name = index_tag->getAttrStr("zky_iilname", "");
		aos_assert_r(iil_name != "", false);
		bool rslt = iil_client->addStrValueDoc(iil_name, value.getStr(), record->getDocid(), false, true, rdata_raw);
		aos_assert_r(rslt, false);
		index_tag = index_tags->getNextChild();
	}
	return true;
}

bool
AosDocClient::insertCmpIndex(
		const AosXmlTagPtr &cmp_indexs_tag,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	//	<cmp_indexes>
	//		<cmp_index zky_iilname="_zt44_idx_t1_key_field1_key_field2_key_field3">
	//			<key_field1 zky_name="key_field1"/>
	//			<key_field2 zky_name="key_field2"/>
	//			<key_field3 zky_name="key_field3"/>
	//		</cmp_index>
	//	</cmp_indexes>
	//[key0x01key, docid]
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	int idx = -1;
	u64 docid = record->getDocid();
	OmnString field_name;
	OmnString key_sep = "0x01";
	AosConvertAsciiBinary(key_sep);
	AosXmlTagPtr cmp_index_tag = cmp_indexs_tag->getFirstChild(true);
	while(cmp_index_tag)
	{
		OmnString iil_name = cmp_index_tag->getAttrStr("zky_iilname");
		aos_assert_r(iil_name != "", false);
		OmnString key_str;
		int num = cmp_index_tag->getNumSubtags();
		for (int i=0; i<num; i++)
		{
			AosXmlTagPtr field_tag = cmp_index_tag->getChild(i);
			aos_assert_r(field_tag, false);
			field_name = field_tag->getAttrStr("zky_name", "");
			aos_assert_r(field_name != "", false);
			idx = record->getFieldIdx(field_name, rdata_raw);
			aos_assert_r(idx != -1, false);
			AosValueRslt key_rslt;
			record->getFieldValue(idx, key_rslt, false, rdata_raw);
			key_str << key_rslt.getStr();
			if (i != num - 1)
			{
				key_str << key_sep;
			}
		}
		bool rslt = iil_client->addStrValueDoc(iil_name, key_str, docid, false, true, rdata_raw);
		aos_assert_r(rslt, false);
		cmp_index_tag = cmp_indexs_tag->getNextChild();
	}
	return true;
}

bool
AosDocClient::insertMap(
		const AosXmlTagPtr &maps_tag,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	//<map zky_type="iilmap" zky_name="map1" zky_iilname="_zt44_map_t1_map1">
	//	<keys>
	//		<key><![CDATA[`key_field1`]]></key>
	//	</keys>
	//	<values>
	//		<value datatype="str" max_len="18" type="field"><![CDATA[`zky_docid`]]></value>
	//	</values>
	//</map>

	// For aggregation functions: "sum", "count", "distinct count", "max", "min"
	// 	the data field type must be numerical. Use the following 
	// 	format for IILs:
	// 	[key0x01key01..., value]	
	//
	// If there is no aggregation function, use the following format:
	// 	[key0x01key01...0x02value, docid]
	// "value" is converted to string.
	bool rslt = false;
	OmnString iil_name;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	OmnString key_sep = "0x01";
	AosConvertAsciiBinary(key_sep);
	OmnString value_sep = "0x02";
	AosConvertAsciiBinary(value_sep);
	AosXmlTagPtr map_tag = maps_tag->getFirstChild(true);
	while(map_tag)
	{
		OmnString key_name, value_name;
		iil_name = map_tag->getAttrStr("zky_iilname", "");
		aos_assert_r(iil_name != "", false);

		//keys
		OmnString key;
		AosXmlTagPtr keys_tag = map_tag->getFirstChild("keys");
		aos_assert_r(keys_tag, false);
		int num = keys_tag->getNumSubtags();
		for (int i=0; i<num; i++)
		{
			AosXmlTagPtr key_tag = keys_tag->getChild(i);
			aos_assert_r(key_tag, false);

			key_name = key_tag->getNodeText();
			key_name << ";";
			OmnString err;
			AosExprObjPtr expr = AosParseExpr(key_name, err, rdata_raw);
			AosValueRslt key_rslt;
			rslt = expr->getValue(rdata_raw, record.getPtr(), key_rslt);
			aos_assert_r(rslt, false);

			key << key_rslt.getStr();
			if (i != num -1)
			{
				key << key_sep;
			}
		}

		//values
		AosXmlTagPtr values_tag = map_tag->getFirstChild("values");
		aos_assert_r(values_tag, false);
		AosXmlTagPtr value_tag = values_tag->getFirstChild(true);
		aos_assert_r(value_tag, false);
		value_name = value_tag->getNodeText();
		value_name << ";";

		OmnString argtype = value_tag->getAttrStr("agrtype", "");
		if (argtype != "")
		{
			AosValueRslt value_rslt;
			OmnNotImplementedYet;
			rslt = iil_client->addStrValueDoc(iil_name, key, value_rslt.getU64(), true, true, rdata_raw);
		}
		else
		{
			AosValueRslt value_rslt;
			OmnString err;
			AosExprObjPtr expr = AosParseExpr(value_name, err, rdata_raw);
			aos_assert_r(expr, false);
			rslt = expr->getValue(rdata_raw, record.getPtr(), value_rslt);
			aos_assert_r(rslt, false);
			if (value_name == "`zky_docid`;")
			{
				rslt = iil_client->addStrValueDoc(iil_name, key, value_rslt.getU64(), true, true, rdata_raw);
			}
			else
			{
				key << value_sep << value_rslt.getStr();
				rslt = iil_client->addStrValueDoc(iil_name, key, 1, true, true, rdata_raw);
			}
		}
		aos_assert_r(rslt, false);
		map_tag = maps_tag->getNextChild();
	}
	return true;
}

bool
AosDocClient::insertInternalStat(
		const AosXmlTagPtr &stat_tags,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	return true;
	//<statistic zky_stat_identify_key="key_field3" zky_stat_name="db_t1_stat_uw_st5">
	//	<key_fields>
	//		<field field_name="key_field3"></field>
	//	</key_fields>
	//	<stat_key_conn/>
	//	<vector2ds>
	//		<vt2d>
	//			<vt2d_conn time_bucket_weight="1" start_time_slot="14610"/>
	//			<vt2d_info vt2d_name="vt2d_0" has_valid_flag="true">
	//				<measures>
	//					<measure zky_name="sum0x28key_field10x29" field_name="key_field1" agr_type="sum" zky_data_type="double"></measure>
	//				</measures>
	//			</vt2d_info>
	//		</vt2d>
	//	</vector2ds>
	//	<stat_cubes>
	//		<cube cube_id="0">
	//			<stat_key_conn meta_fileid="107"/>
	//			<vector2ds>
	//				<vt2d>
	//					<vt2d_conn time_bucket_weight="1" start_time_slot="14610" meta_fileid="108"/>
	//					<vt2d_info vt2d_name="vt2d_0" has_valid_flag="true">
	//						<measures>
	//							<measure zky_name="sum0x28key_field10x29" field_name="key_field1" agr_type="sum" zky_data_type="double"/>
	//						</measures>
	//					</vt2d_info>
	//				</vt2d>
	//			</vector2ds>
	//		</cube>
	//	</stat_cubes>
	//</statistic>

	AosXmlTagPtr stat_tag = stat_tags->getFirstChild(true);	
	OmnString conf_objid;
	while(stat_tag)
	{
		conf_objid = stat_tag->getAttrStr("zky_stat_conf_objid", "");
		aos_assert_r(conf_objid != "", false);
		OmnString idfy_key = stat_tag->getAttrStr("zky_stat_identify_key", "");
		aos_assert_r(idfy_key != "", false);
		AosXmlTagPtr stat_doc = AosGetDocByObjid(conf_objid, rdata_raw);
		aos_assert_r(stat_doc, false);
		//proc stat
		stat_tag = stat_tags->getNextChild();
	}
	return true;
}

bool
AosDocClient::insertNormalStat(
		const OmnString &db_name,
		const AosXmlTagPtr &mstat_tag,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	//<statistic 
	//	zky_stat_identify_key="ZGFYHZ_FFFS_ZGFYHZ_QYJHBH_ZGFYHZ_ZT" 
	//	zky_stat_name="huabao_hb_table_ZGFYHZ_stat_hb_stat_ZGFYHZ"
	//	zky_table_name="hb_table_ZGFYHZ">
	//	<key_fields>
	//		<field field_name="ZGFYHZ_FFFS"></field>
	//		<field field_name="ZGFYHZ_QYJHBH"></field>
	//		<field field_name="ZGFYHZ_ZT"></field>
	//	</key_fields>
	//	<measures>
	//		<measure field_name="ZGFYHZ_SFJE" agr_type="sum" zky_name="sum0x28ZGFYHZ_SFJE0x29" zky_data_type="int64"/>
	//	</measures>
	//	<time_field time_format ="yyyymmdd" grpby_time_unit="_day" time_field_name="ZGFYHZ_JZRQ"/>
	//	<stat_key_conn/>
	//	<vt2d_conn time_bucket_weight="1000" start_time_slot="14610"/>
	//</statistic>
	int idx = -1;
	OmnString key_sep = "0x01";
	AosConvertAsciiBinary(key_sep);
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);

	AosXmlTagPtr stat_tag_itl = mstat_tag->getFirstChild("internal_statistics");
	AosXmlTagPtr stat_tags = mstat_tag->getFirstChild("statistic_defs");
	
	int num = stat_tags->getNumSubtags();
	OmnString conf_objid;
	for (int n=0; n<num; n++)
	{
		AosXmlTagPtr stat_tag = stat_tags->getChild(n);
		conf_objid = stat_tag->getAttrStr("zky_stat_conf_objid", "");
		aos_assert_r(conf_objid != "", false);
		AosXmlTagPtr stat_doc = AosGetDocByObjid(conf_objid, rdata_raw);
		aos_assert_r(stat_doc, false);
		OmnString table_name = stat_doc->getAttrStr("zky_table_name", "");
		aos_assert_r(table_name != "", false);
		OmnString stat_name = stat_doc->getAttrStr("zky_stat_name", "");
		aos_assert_r(stat_name != "", false);

		OmnString stat_key = stat_doc->getAttrStr("zky_stat_identify_key", "");
		aos_assert_r(stat_key != "", false);
		u32 hash_key = AosGetHashKey(stat_key);
		int cube_num = AosGetNumCubes();
		aos_assert_r(cube_num > 0, -1);
		u32 cube_id =  hash_key % cube_num;

		//proc stat
		OmnString field_name;
		vector<OmnString> key_fields;
		AosXmlTagPtr key_tags = stat_doc->getFirstChild("key_fields");
		OmnString key_str;
		int num = key_tags->getNumSubtags();
		for (int i=0; i<num; i++)
		{
			AosXmlTagPtr key_tag = key_tags->getChild(i);
			aos_assert_r(key_tag, false);
			field_name = key_tag->getAttrStr("field_name", "");
			key_fields.push_back(field_name);
			idx = record->getFieldIdx(field_name, rdata_raw);
			aos_assert_r(idx != -1, false);
			AosValueRslt key_rslt;
			record->getFieldValue(idx, key_rslt, false, rdata_raw);
			key_str << key_rslt.getStr();
			if (i != num -1)
			{
				key_str << key_sep;
			}
		}

		//<time_field time_format ="yyyymmdd" grpby_time_unit="_day" time_field_name="ZGFYHZ_JZRQ"/>
		AosValueRslt time_value;
		OmnString  time_unit_str;
		AosXmlTagPtr time_field_tag = stat_doc->getFirstChild("time_field");
		AosStatTimeUnit::E grpby_time_unit;
		if (time_field_tag)
		{
			time_unit_str = time_field_tag->getAttrStr("grpby_time_unit", "");
			grpby_time_unit = AosStatTimeUnit::getTimeUnit(time_unit_str );
			aos_assert_r(grpby_time_unit != AosStatTimeUnit::eInvalid, false);

			OmnString time_format = time_field_tag->getAttrStr("time_format", "");
			OmnString time_field = time_field_tag->getAttrStr("time_field_name", "");
			if (time_field != "") 
			{
				idx = record->getFieldIdx(time_field, rdata_raw);
				aos_assert_r(idx != -1, false);
				record->getFieldValue(idx, time_value, false, rdata_raw);
			}
		}

		//iil_name ...?
		//_zt4k
		//db name [db]
		//table name [t1]
		//stat name [xxx]
		//_internal_0__$group
		OmnString iil_name = "_zt4k_";
		iil_name << stat_name << "_internal_" << n << "__" << cube_id ;
		u64 sdocid = 0;
		iil_client->getDocid(iil_name, key_str, sdocid, rdata_raw);
		if (sdocid == 0)
		{
			OmnString stat_name = stat_doc->getAttrStr("zky_stat_name", "");
			aos_assert_r(stat_name != "", false);
			AosStatIdIDGen* idGen = OmnNew AosStatIdIDGen(stat_name, 1);
			sdocid = idGen->nextDocid(rdata_raw, 0);
			aos_assert_r(sdocid != 0, false);
			iil_client->addStrValueDoc(iil_name, key_str, sdocid, true, true, rdata_raw);
		}

		// Create indexes for key fields
		//iilname ...?
		//_zt44
		//db name [db]
		//table name [t1]
		//stat name [xxxx]
		//_internal_0_key_
		//field_name [key_field1]
		for (u32 i=0; i<key_fields.size(); i++)
		{
			OmnString field_name = key_fields[i];
			iil_name = "_zt44_";
			iil_name << stat_name << "_internal_" << n << "_key_" << field_name;
			idx = record->getFieldIdx(field_name, rdata_raw);
			aos_assert_r(idx != -1, false);
			AosValueRslt key_rslt;
			record->getFieldValue(idx, key_rslt, false, rdata_raw);
			iil_client->addStrValueDoc(iil_name, key_rslt.getStr(), sdocid, true, true, rdata_raw);
		}

		//<measure field_name="ZGFYHZ_SFJE" agr_type="sum" zky_name="sum0x28ZGFYHZ_SFJE0x29" zky_data_type="int64"/>
		OmnString measure_name;
		
	//	OmnString time_unit_str;
		int vt2d_idx;
		AosVt2dInfo vt2d_info;


		vector<AosValueRslt> measures;
		AosXmlTagPtr measure_tags = stat_doc->getFirstChild("measures");
		AosXmlTagPtr measure_tag = measure_tags->getFirstChild(true);
		while (measure_tag)
		{
			measure_name = measure_tag->getAttrStr("field_name", "");
			AosValueRslt measure_rslt;
			idx = record->getFieldIdx(measure_name, rdata_raw);
			aos_assert_r(idx != -1, false);
			record->getFieldValue(idx, measure_rslt, false, rdata_raw);
			measures.push_back(measure_rslt);
			measure_tag = measure_tags->getNextChild();
		}
		//
		//craete input_data
		//
		AosBuffPtr input_data = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		input_data->appendU64(sdocid);
		int time_value_pos = input_data->getCrtIdx();
		input_data->appendU64(time_value.getU64());
		int measures_value_pos = input_data->getCrtIdx();
		for(size_t i = 0; i < measures.size();i++)
		{
			input_data->appendInt64(measures[i].getI64());
		}
		u32 keyPos = input_data->getCrtIdx();
		input_data->addBuff(key_str.data(),key_str.length());
		u32 newFlagPos = input_data->getCrtIdx();
		OmnString isnew("0");
		input_data->addBuff(isnew.data(),isnew.length());
		u64 record_len = input_data->dataLen();
//
//		a new record string value is in thr following format
//		|sdocid|time_value|value1..Valuen|key_str|isnew
//
		AosStatModifyInfo mStatMdfInfo;
		mStatMdfInfo.mSdocidPos = 0;
		mStatMdfInfo.mKeyPos = keyPos;
		mStatMdfInfo.mIsNewFlagPos = newFlagPos;
		mStatMdfInfo.mRecordLen = record_len;

		AosXmlTagPtr stat_doc_itl = stat_tag_itl->getFirstChild("statistic");
		aos_assert_r(stat_doc_itl,false);
		OmnString itl_conf_objid = stat_doc_itl->getAttrStr("zky_stat_conf_objid", "");
		AosXmlTagPtr itl_stat_doc = AosGetDocByObjid(itl_conf_objid, rdata_raw);
		aos_assert_r(itl_stat_doc,false);
		measure_tags = stat_doc->getFirstChild("measures");
		measure_tag = measure_tags->getFirstChild(true);
		while(measure_tag)
		{
			measure_name = measure_tag->getAttrStr("zky_name", "");
			aos_assert_r(measure_name!="",0);
			AosXmlTagPtr time_field_tag = stat_doc->getFirstChild("time_field","");
			
			if(time_field_tag)
			{
				time_unit_str = time_field_tag->getAttrStr("grpby_time_unit","");
				grpby_time_unit = AosStatTimeUnit::getTimeUnit(time_unit_str);
			}
			else
			{
				time_value_pos = -1;
				grpby_time_unit = AosStatTimeUnit::eInvalid;
			}
			AosStatisticPtr mStat = new AosStatistic();
			mStat->config(rdata_raw,itl_stat_doc);
			vt2d_idx = mStat->getVt2dIdxByMeasureName(measure_name,grpby_time_unit);
			aos_assert_r(vt2d_idx != 1,false);
			bool rslt = mStat->getVt2dInfo(vt2d_idx,vt2d_info);
			aos_assert_r(rslt,false);
			mStatMdfInfo.addMeasure(rdata_raw,measure_name,measures_value_pos,8*measures.size(),time_value_pos,vt2d_idx,vt2d_info);
			measure_tag = measure_tags->getNextChild();
		}
		AosXmlTagPtr stat_cubes = itl_stat_doc->getFirstChild("stat_cubes", "");
		AosXmlTagPtr cube_conf = stat_cubes->getFirstChild("cube");

		bool svr_death;
		AosBuffPtr resp;
		AosTransPtr trans = OmnNew AosBatchSaveStatDocsTrans(cube_id,cube_conf,input_data,mStatMdfInfo);
		AosSendTrans(rdata_raw,trans,resp,svr_death);
		if(svr_death)
		{
			OmnScreen << "batch save stat docs error! svr_death!" << svr_death;
			return false;
		}
		aos_assert_r(resp,0);
		bool rslt = resp->getU8(0);
		aos_assert_r(rslt,false);
	//	rslt = AosAddVector2D(cube_id,rdata_raw,cube_conf,input_data,mStatMdfInfo);
	//	aos_assert_r(rslt, false);
		stat_tag = stat_tags->getNextChild();
	}
	return true;
}
