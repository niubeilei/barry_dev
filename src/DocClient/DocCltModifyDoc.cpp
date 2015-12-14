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
//  This class is used to create docs. It will:
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClient/DocClient.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CompressUtil/Compress.h"
#include "DocClient/DocProc.h"
#include "DocUtil/DocUtil.h"
#include "DocServer/DocSvr.h"
#include "DocLock/DocLock.h"
#include "DocTrans/IsDocDeletedTrans.h"
#include "DocTrans/IncrememtValueTrans.h"
#include "DocTrans/ModifyObjTrans.h"
#include "EventMgr/Event.h"
#include "EventMgr/EventTriggers.h"
#include "EventMgr/Event.h"
#include "DocClient/PartialDocMgr.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEUtil/SysLogName.h"
#include "SEModules/ObjidSvr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/ValueRslt.h"

static AosDocMgrObjPtr sgDocMgr;

bool	
AosDocClient::modifyObj(
		const AosRundataPtr &rdata,
 		const AosXmlTagPtr &newdoc,
		const OmnString &rename,
		const bool synobj)
{
	// It modifies the object 'newdoc':
	// 1. Retrieve the original doc. If not found, it is an error.
	// 2. Add all the words from the original doc into a temporary
	//    hashtable.
	// 3. Do the same thing for 'newdoc'
	// 4. Now we have two temporary hashtables. 
	//    a. Words in both hashtables are ignored
	//    b. Words in the original hashtable but not in 'newdoc', 
	//       need to be deleted.
	//    c. Words in 'newdoc' but not in the original, add it.
	// 5. Save the original doc into the version table.
	// 6. Replace the original doc with 'newdoc'.
	//
	// 'root' is the pointer pointing to the root of the XML doc. 
	// 'newdoc' is the pointer pointing to the doc. 'newdoc' 
	// should be a subtag of 'root'.
	AOSLOG_ENTER_R(rdata, false);
	aos_assert_r(newdoc, false);

	newdoc->normalize();
	// Set the rdata docs
	rdata->setWorkingDoc(newdoc, false);
	rdata->setDocByVar(AOSARG_NEW_DOC, newdoc, false);

	if (mIsStopping)
	{
		AosSetError(rdata, "server_stopping");
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Pending: modifying docs does not have to have a cid.
	OmnString cid = AosCloudidSvr::getCloudid(rdata->getUserid(), rdata);
	if (!mIsRepairing && cid == "")
	{
		AosSetError(rdata, "missing_cloudid") << ": " << rdata->getUserid();
		AOSLOG_LEAVE(rdata);
		return false;
	}

	newdoc->setAttr(AOSTAG_SITEID, rdata->getSiteid());
	u64 newdid = newdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID); 
	OmnString newobjid = newdoc->getAttrStr(AOSTAG_OBJID);
	AosObjidType::E objid_type;
	bool rslt = AosObjid::checkObjid(newobjid, objid_type, rdata);
	if (!rslt)
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Check the docid and objid:
	// 1. If 'newdoc' has objid but not docid: check whether the objid
	//    uniquely identifies a doc. If not, it is to create a new doc. Otherwise, it
	//    is an error.
	// 2. If 'newdoc' has docid but not objid, it is an error. 
	// 3. If 'newdoc' contains no objid and docid: check whether 'rename'
	//    is true. If yes, it is to create a new doc. Otherwise, it is 
	//    an error.
	// 4. If 'newdoc' has both objid and docid, it is to modify. 
	
	bool is_public = false;
	if (newobjid != "")
	{
		if (rename == "true")
		{
			// It is to rename, we will ignore the docid. 
			is_public = determinePublic(newdoc, 0, cid, newobjid);
			newdid = AOS_INVDID; 
			newdoc->removeAttr(AOSTAG_DOCID);
		}

		// There are two possibilities: one is to use 'newdid' to locate an existing
		// doc, and the other is to use 'newobjid' to locate an existing doc. 
		// They may or may not identify the same doc.
		AosXmlTagPtr origdoc = getDocByObjid(newobjid, rdata);
		if (!origdoc)
		{
			// The objid does not identify an existing doc. If 'newdid' is 
			// null, it is to create a new doc.
			if (newdid == AOS_INVDID)
			{
				// Case 1: 'newobjid' does not identify a doc and 'newdoc' 
				// does not have docid. We consider this as creating a new doc.
				newdoc->removeAttr(AOSTAG_DOCID, 1, true);

				rslt = createDoc1(rdata, 0, cid, newdoc, true, 0, 0, false);
				AOSLOG_LEAVE(rdata);
				return rslt;
			}
		}

		// It has objid, and the objid does identify a doc. 
		// check whether it has docid. 
		if (newdid == AOS_INVDID) 
		{
			// It does not have the docid. Will use the origdoc's. 
			newdid = origdoc->getAttrU64(AOSTAG_DOCID, 0);
			if (newdid == AOS_INVDID)
			{
				AosSetErrorUser(rdata, "internal_erro") << ": " << newobjid << enderr;
				origdoc = 0;
				return false;
			}

			newdoc->setAttr(AOSTAG_DOCID, newdid);
		}

		rslt = modifyObjChecked(rdata, cid, newdoc, newdid, rename, synobj, is_public);
		origdoc = 0;
		AOSLOG_LEAVE(rdata);
		return rslt;
	}

	// It has no objid. 
	if (newdid == AOS_INVDID)
	{
		// Case 3: it has no objid and docid. 
		// It is to create a new doc.
		newdoc->removeAttr(AOSTAG_DOCID, 1, true);
		rslt = createDoc1(rdata, 0, cid, newdoc, true, 0, 0, false);
		AOSLOG_LEAVE(rdata);
		return rslt;

		// Case 3: it has no objid and docid, and 'rename' is not true. 
		// It is an error
		// AosSetError(rdata, "modify_failed_005");
		// OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSLOG_LEAVE(rdata);
		// return false;
	}

	AosXmlTagPtr origdoc = getDocByDocid(newdid, rdata);
	if (!origdoc)
	{
		// It has no objid, but has docid, the docid does not identify
		// an existing doc. This is not allowed.
		AosSetErrorUser(rdata, "modify_failed_006") << ": " << newdid << enderr;
		return false;
	}

	newobjid = origdoc->getAttrStr(AOSTAG_OBJID);
	aos_assert_rr(newobjid != "", rdata, false);
	newdoc->setAttr(AOSTAG_OBJID, newobjid);

	rslt = modifyObjChecked(rdata, cid, newdoc, newdid, rename, synobj, is_public);
	AOSLOG_LEAVE(rdata);
	origdoc = 0;
	return rslt;
}	


bool	
AosDocClient::modifyObjChecked(
		const AosRundataPtr &rdata,
		const OmnString &cid,
 		const AosXmlTagPtr &newdoc,
		u64 &newdid,
		const OmnString &rename,
		const bool synobj, 
		const bool is_public)
{
	// This function assumes:
	AOSLOG_ENTER_R(rdata, false);

	aos_assert_r(newdoc, false);
	aos_assert_r(newdid, false);

	AosXmlTagPtr newxml = newdoc;
	AosXmlTagPtr root = rdata->getReceivedDoc();
	if (!root)
	{
		aos_assert_r(newxml->isRootTag(), false);
		root = newxml;
	}

	OmnString newobjid = newxml->getAttrStr(AOSTAG_OBJID);
	AosXmlTagPtr origdoc = getDocByDocid(newdid, rdata);
	if (!origdoc)
	{
		// It is to modify a doc but the doc does not exist. If the user
		// wants to create a new doc, it should not set docid. 
		AosSetErrorUser(rdata, "modify_failed_001") << ": " << newobjid 
			<< ":" << newdid << enderr;
		return false;
	}

	OmnString oldobjid = origdoc->getAttrStr(AOSTAG_OBJID);;
	aos_assert_r(oldobjid != "", false);
	aos_assert_r(newobjid != "", false);

	// Check whether it is to modify objid
	bool rslt = true;
	bool securityChecked = false;
	bool needRebind = false;
	if (oldobjid != newobjid)
	{
		// It means someone wants to modify a doc but the new doc's objid 
		// is different from the original one. This can mean four things:
		// 1. 'rename' is true and 'oldobjid' identifies a doc:
		// 	  Rename the current doc and override another existing doc; 
		// 2. 'rename' is true and 'oldobjid' not identify a doc:
		//    Rename the current doc;
		// 3. 'rename' is false and 'oldobjid' not identify a doc:
		//    Create a doc doc;
		// 4. 'rename' is false and 'oldobjid' identifies a doc:
		//    Overriding an existing doc;
		AosXmlTagPtr overridingdoc = getDocByObjid(newobjid, rdata);
		if (overridingdoc)
		{
			// The docid does identify a doc and the doc is different from
			// the newdoc. 
			OmnString override_flag = rename;
			if (override_flag == "") 
			{
				override_flag = newdoc->getAttrStr(AOSTAG_OVERRIDE_DOC_FLAG);
				newdoc->removeAttr(AOSTAG_OVERRIDE_DOC_FLAG);
			}
			if (override_flag != "true")
			{
				// 4. Overriding an existing doc. Not allowed. 
				AosSetErrorUser(rdata, "modify_failed_004") << ": " << newobjid << enderr;
				origdoc = 0;
				overridingdoc = 0;
				return false;
			}

			// 1. Rename the current doc to an existing doc. Need to check whether
			//    it is allowed. 
			if (!mIsRepairing)
			{
				rslt = AosSecurityMgrObj::getSecurityMgr()->checkModify(overridingdoc, rdata);
				if (!rslt)
				{
					// Did not pass the security check
					origdoc = 0;
					overridingdoc = 0;
					return false;
				}
			}

			securityChecked = true;
			u64 overriddenDocid = overridingdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
			newxml->setAttr(AOSTAG_CTIME, overridingdoc->getAttrStr(AOSTAG_CTIME));
			newxml->setAttr(AOSTAG_CT_EPOCH, overridingdoc->getAttrStr(AOSTAG_CT_EPOCH));
			newxml->setAttr(AOSTAG_DOCID, overriddenDocid);
			newdid = overriddenDocid;
		}
		else
		{
			// This means that the docid identifies a doc that is different 
			// from the one being bound to it, which means that we need to 
			// rebind the objid.
			// AOSMONITORLOG_LINE(rdata);
			needRebind = true;
		}
	}

	if (!mIsRepairing)
	{
		rslt = checkDocLock(rdata, newdid, "checkmodify");
		if (!rslt)
		{
			AOSLOG_LEAVE(rdata);
			origdoc = 0;
			return false;
		}
	}

	// 2. Check the home container
	rslt = checkHomeContainer(rdata, newxml, origdoc, cid);
	if (!rslt)
	{
		AOSLOG_LEAVE(rdata);
		origdoc = 0;
	 	return false;
	}

	// 3. Check the security
	OmnString newcreator = newxml->getAttrStr(AOSTAG_CREATOR, "");
	newxml->setAttr(AOSTAG_CREATOR, origdoc->getAttrStr(AOSTAG_CREATOR, ""));
	if (!securityChecked && !mIsRepairing)
	{
		rslt = AosSecurityMgrObj::getSecurityMgr()->checkModify(newxml, rdata);
		if (!rslt)
		{
			// Access denied
			AOSLOG_LEAVE(rdata);
			origdoc = 0;
			return false;
		}
	}
	newxml->setAttr(AOSTAG_CREATOR, newcreator);

	// When it gets to this point, we ensured:
	// 1. Old version obj is available: 'origdoc'
	// 2. New version obj is available: 'newxml'
	origdoc = getDocByDocid(newdid, rdata);
	if (!origdoc)
	{
		// This should not happen, but it is possible that during the time
		// between when this function was called and this point, the doc
		// was deleted by another thread (very rare, but theoretically 
		// possible).
		AosSetErrorUser(rdata, "modify_failed_002") << newobjid << ":" << newdid << enderr;
		return false;
	}

	if (newxml->getAttrStr(AOSTAG_DOCID) != origdoc->getAttrStr(AOSTAG_DOCID))
	{
		newxml->setAttr(AOSTAG_DOCID, origdoc->getAttrStr(AOSTAG_DOCID));
	}

	// Need to check whether the new doc's parent container objid is the
	// same as the objid's objid.
	if (newxml->getAttrStr(AOSTAG_PARENTC) == newobjid)
	{
		// This is not allowed.
		AosSetErrorUser(rdata, "modify_failed_007") << ":" << newobjid << enderr;
		origdoc = 0;
		return false;
	}

	// The new doc should have been set
	rdata->setDocByVar(AOSARG_OLD_DOC, origdoc, true);

	// Check whether the doc was deleted. If yes, it is to create a new one.
	AosBuffPtr resp;
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIsDocDeletedTrans(newdid, false, true, snap_id);
	rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);	
	aos_assert_r(rslt, false);

	bool is_deleted = resp->getU8(0); // It's result, true means deleted.
	if (is_deleted)
	{
		// The doc was deleted. It is to recover the object. 
		newxml->removeAttr(AOSTAG_DOCID, 1, true);
		rslt = createDoc1(rdata, 0, cid, newxml, true, 0, 0, true);
		AOSLOG_LEAVE(rdata);
		origdoc = 0;
		return rslt;
	}

	if (needRebind)
	{
		if (mShowLog) OmnScreen << "rebind objid: " << oldobjid
			<< ":" << newobjid << ":" << newdid << endl;
		//rslt = AosIILClientSelf->rebindObjid(
		rslt = AosIILClientObj::getIILClient()->rebindObjid(
			oldobjid, newobjid, newdid, rdata);
		aos_assert_r(rslt, false);
	}

	// When it gets to this point, the new and existing docs contain the 
	// same objid and docid. The next step is to check whether the original
	// doc is a template. If yes, handle the template case.
	// If the new doc AOSTAG_TEMPLATE value is AOSVALUE_CONVERT_TMPL,
	// it is to change the template into a non-template by removing
	// the attribute AOSTAG_TEMPLATE.
	if (newxml->getAttrStr(AOSTAG_TEMPLATE) == AOSVALUE_CONVERT_TMPL)
	{
		newxml->removeAttr(AOSTAG_TEMPLATE, 1, true);
	}
	else
	{
		// AOSMONITORLOG_LINE(rdata);
		// Check whether the newdoc is a template. If yes, check whether
		// 'rename' is 'modify_template'. If not, create a new
		// object based on the template
		if (origdoc && origdoc->getAttrBool(AOSTAG_TEMPLATE))
		{
			// It is a template. Check whether it is to modify the 
			// template itself. If yes, need to modify the doc. Otherwise, 
			// need to create a new doc based on 'newxml'.
			// AOSMONITORLOG_LINE(rdata);
			OmnString modtemp = newxml->getAttrStr(AOSTMP_MODIFY_TEMPLATE);
			if (modtemp.length() > 0)
			{
				// AOSMONITORLOG_LINE(rdata);
				newxml->removeAttr(AOSTMP_MODIFY_TEMPLATE, false, true);
			}
		}
	}
	newxml->removeAttr(AOSTAG_PUBLIC_DOC, false, true);

	// AOSMONITORLOG_LINE(rdata);
	bool doc_modified;
	rslt = AosPartialDocMgr::processPartialDoc(newxml, rdata, doc_modified);
	if (!rslt)
	{
		OmnAlarm << "failed processing" << enderr;
		AOSLOG_LEAVE(rdata);
		origdoc = 0;
		return false;
	}

	// We are about ready to modify the doc. Before doing it, need to check
	// whether the two docs are identical. If yes, do nothing.
	// Note:
	// The way how to determine whether two docs are identical is not correct
	// right now. Need to find a better way to do it (refer to Zky2686).
	const char *data1 = (char *)origdoc->getData();
	const char *data2 = (char *)newxml->getData();
	int len1 = origdoc->getDataLength();
	int len2 = newxml->getDataLength();
	if (data1 && data2)
	{
		// AOSMONITORLOG_LINE(rdata);
		if (len1 == len2 && memcmp((void *)data1, (void *)data2, len1) == 0)
		{
			// Nothing changed. Do nothing
			// OmnScreen << "Not changed!" << endl;
			//errcode = eAosXmlInt_Ok;
			rdata->setOk();
			AOSLOG_LEAVE(rdata);
			origdoc = 0;
			return true;
		}
	}

	// AOSMONITORLOG_LINE(rdata);
	checkModifying(origdoc, newxml, cid, rdata);

	rslt = AosEvent::checkEvent1(origdoc, newxml, AOSEVENTTRIGGER_DOC_PRE_MODIFY, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
		AOSLOG_LEAVE(rdata);
		origdoc = 0;
		return false;
	}

	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
	{
		AOSLOG_LEAVE(rdata);
		origdoc = 0;
		return false;
	}

	if (!mIsRepairing)
	{
		//Jozhi 2013/07/06
		//aos_assert_r(mVersionServerObj, false);
		//if (!mVersionServerObj->addVersionObj(newxml, rdata))
		//{
		//	// AOSMONITORLOG_LINE(rdata);
		//	OmnAlarm << "Failed creating the version obj: " << newxml->getAttrStr(AOSTAG_OBJID) << enderr;
		//}
	}

	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, false);
	sgDocMgr->addCopy(newdid, newxml, rdata);  

	AosXmlTagPtr newxml1 = newxml->clone(AosMemoryCheckerArgsBegin);
	AosTransPtr trans2 = OmnNew AosModifyObjTrans(newdid, newxml1, origdoc, true, false, snap_id);
	rslt = addReq(rdata, trans2);
	aos_assert_r(rslt, false);
	
	if (!rdata->isOk())
	{
		AOSLOG_LEAVE(rdata);
		origdoc = 0;
		newxml1 = 0;
		return false;
	}
	
	u64 userid = rdata->getUserid();
	AosDocReqPtr doc_req = AosDocProc::getSelf()->addModifyRequest(
		newxml1, userid, newxml1, origdoc, synobj, rdata);
	aos_assert_rr(doc_req, rdata, false);

	AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_MODIFYDOC, rdata);
	
	rdata->setOk();

	rslt = AosEvent::checkEvent1(origdoc, newxml, AOSEVENTTRIGGER_DOC_MODIFY, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to Container Member Verification";
		// AOSMONITORLOG_FINISH(rdata);
	}

	AOSLOG_LEAVE(rdata);
	origdoc = 0;
	newxml1 = 0;
	return true;
}


bool	
AosDocClient::modifyAttrStr1(
		const AosRundataPtr &rdata,
		const u64 &docid, 
		const OmnString &objid,		
		const OmnString &aname, 
		const OmnString &newvalue, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool checkSecurity,
		const bool need_trigger)
{
	// AOSMONITORLOG_ENTER(rdata);
	AosXmlTagPtr doc;
	if (docid == AOS_INVDID)
	{
		if (objid == "")
		{
			AosSetErrorU(rdata, "missing_objid") << enderr;
			// AOSMONITORLOG_FINISH(rdata);
			return false;
		}

		doc = getDocByObjid(objid, rdata);
		if (!doc)
		{
			AosSetErrorU(rdata, "failed_retrieve_doc") << ": " << objid << enderr;
			// AOSMONITORLOG_FINISH(rdata);
			return false;
		}
	}
	else
	{
		doc = getDocByDocid(docid, rdata);
		if (!doc)
		{
			AosSetErrorU(rdata, "failed_retrieve_doc") << ": " << docid << enderr;
			// AOSMONITORLOG_FINISH(rdata);
			return false;
		}
	}

	return modifyAttrStr(doc, aname, newvalue, value_unique,
		docid_unique, checkSecurity, rdata, need_trigger);
}


bool	
AosDocClient::modifyAttrStr(
		const AosXmlTagPtr &doc,
		const OmnString &aname, 
		const OmnString &newvalue, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool checkSecurity,
		const AosRundataPtr &rdata,
		const bool need_trigger)
{
	aos_assert_r(doc, false);


	// AOSMONITORLOG_ENTER(rdata);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);

	doc->normalize();
	if (doc->isDeleted())
	{
		rdata->setError() << "Doc already deleted: " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		rdata->setError() << "Missing Siteid: " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	doc->setAttr(AOSTAG_SITEID, siteid);
	bool rslt = true;
	if (checkSecurity)
	{
		rslt = AosSecurityMgrObj::getSecurityMgr()->checkModify(doc, rdata);
		if (!rslt)
		{
			// Access denied
			// AOSMONITORLOG_FINISH(rdata);
			return false;
		}
	}

	rslt = AosDocUtil::canAttrModified(aname.data(), aname.length());
	if (!rslt)
	{
		rdata->setError() << "Attribute cannot be modified: " << docid
			<< ". Attribute name: " << aname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	OmnString attrname;
	if (attrname == "") attrname = aname;

	bool exist;
	OmnString oldvalue = doc->getAttrStr1(attrname, exist, "");

	// Chen Ding, 06/04/2012
	// if (oldvalue == newvalue) 
	// {
	// 	// No need to modify
	// 	rdata->setOk();
	// 	// AOSMONITORLOG_FINISH(rdata);
	// 	return true;
	// }

	AosXmlTagPtr origdoc = doc->clone(AosMemoryCheckerArgsBegin);
	doc->setAttr(attrname, newvalue);

	if (!mIsRepairing)
	{
		//Jozhi 2013/07/06
		//aos_assert_r(mVersionServerObj, false);
		//if (!mVersionServerObj->addVersionObj(doc, rdata))
		//{
		//	// AOSMONITORLOG_LINE(rdata);
		//	OmnAlarm << "Failed creating the version obj: " << doc->getAttrStr(AOSTAG_OBJID) << enderr;
		//}
	}

	// Need to modify the modification time.
	rdata->setDocByVar(AOSARG_OLD_DOC, origdoc, true);
	rdata->setDocByVar(AOSARG_NEW_DOC, doc, true);
	rdata->setWorkingDoc(doc, true);

	if (need_trigger)
	{
		rslt = AosEvent::checkEvent1(origdoc, doc, AOSEVENTTRIGGER_DOC_PRE_MODIFY, rdata);
		if (!rslt)
		{
			rdata->setError() << "Failed to Container Member Verification: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			// AOSMONITORLOG_FINISH(rdata);
			origdoc = 0;
			return false;
		}
	}

	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
	{
		origdoc = 0;
		return false;
	}
	
	// If it is to modify otype and its new otype is user account, 
	// we may need to bind the cloudid
	if (attrname == AOSTAG_OTYPE)
	{
		// AOSMONITORLOG_LINE(rdata);
		if (newvalue == AOSOTYPE_USERACCT)
		{
			// AOSMONITORLOG_LINE(rdata);
			OmnString cid = doc->getAttrStr(AOSTAG_CLOUDID);
			if (AosCloudid::isValidCloudid(cid))
			{
				// AOSMONITORLOG_LINE(rdata);
				u64 uid;
				rslt = isCloudidBound(cid, uid, rdata);
				if (!rslt)
				{
					// It is not a valid cid yet.  Need to bind it.
					// AOSMONITORLOG_LINE(rdata);
					rslt = bindCloudid(cid, docid, rdata);
					if (!rslt)
					{
						// AOSMONITORLOG_LINE(rdata);
						OmnAlarm << "Failed to bind the cloudid" << enderr;
					}
				}
			}
		}
	}

	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, false);

	u64 snap_id = rdata->getSnapshotId();
	sgDocMgr->addCopy(docid, doc, rdata);  
	AosTransPtr trans = OmnNew AosModifyObjTrans(docid, doc, origdoc, true, false, snap_id);
	rslt = addReq(rdata, trans);
	aos_assert_r(rslt, false);
	
	if (!rdata->isOk())
	{
		origdoc = 0;
		return false; 
	}
		
	AosDocReqPtr doc_req = AosDocProc::getSelf()->addModifyAttrRequest(
		attrname, oldvalue, newvalue, exist,
		value_unique, docid_unique, docid, rdata);
	aos_assert_rr(doc_req, rdata, false);

	AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_MODIFYATTR, rdata);
	
	rdata->setOk();

	if (need_trigger)
	{
		rslt = AosEvent::checkEvent1(origdoc, doc, AOSEVENTTRIGGER_DOC_MODIFY, rdata);
		if (!rslt)
		{
			rdata->setError() << "Failed to Container Member Verification";
		}
	}

	// AOSMONITORLOG_FINISH(rdata);
	origdoc = 0;
	return true;
}


OmnString
AosDocClient::incrementValue(
		const u64 &docid, 
		const OmnString &objid,
		const OmnString &aname, 
		const OmnString &initvalue, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata) 
{
	// This function increments the value of the attribute 'aname' 
	// by one. The changes are saved back to the doc.
	// Note that the
	// attributes that can be incremented should be integral attributes.
	// Due to implementation complexity, we will treat them as string attributes.
	
	aos_assert_r(docid, "");
	AosXmlTagPtr origdoc;
	if (docid)
	{
		origdoc = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
	}
	else
	{
		if (objid == "")
		{
			AosSetError(rdata, AosErrmsgId::eNoObjectSpecified);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		origdoc = AosDocClient::getSelf()->getDocByObjid(objid, rdata);
	}

	if (!origdoc)
	{
		OmnAlarm << "To increment a doc but the doc does not exist: " << docid << enderr;
		return NULL;
	}

	AosXmlTagPtr newdoc = origdoc->clone(AosMemoryCheckerArgsBegin);

	bool exist;
	OmnString oldvalue = newdoc->getAttrStr1(aname, exist, initvalue);
	u64 ov = atoll(oldvalue.data());
	u64 nv = ov + 1;
	OmnString newvalue;
	newvalue << nv;
	newdoc->setAttr(aname, newvalue);

	rdata->setDocByVar(AOSARG_OLD_DOC, origdoc, true);
	rdata->setDocByVar(AOSARG_NEW_DOC, newdoc, true);
	rdata->setWorkingDoc(newdoc, true);
	if (!AosEvent::checkEvent1(origdoc, newdoc, AOSEVENTTRIGGER_DOC_PRE_MODIFY, rdata))
	{
		rdata->setError() << "Failed to Container Member Verification: " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		origdoc = 0;
		newdoc = 0;
		return 0;
	}
	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
	{
		origdoc = 0;
		newdoc = 0;
		return 0;
	}

	u32 siteid = newdoc->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid >= 0 && rdata->getSiteid() == siteid, 0);

	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, 0);
	u64 snap_id = rdata->getSnapshotId();
	sgDocMgr->addCopy(docid, newdoc, rdata);  

	// Ketty 2013/03/14
	// eModifyAttrStr not support.
	/*
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE <<  "=\"" << "modifyobj" << "\" "
		<< AOSTAG_DOC_REQTYPE << "=\"" << AosDocReq::eModifyAttrStr<< "\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" "
		<< "attrname" << "=\"" << aname << "\" "
		<< "newvalue" << "=\"" << newvalue<< "\" "
		<< "oldvalue" << "=\"" << oldvalue<< "\" "
		<< "value_unique " << "= \"" << value_unique << "\" "
		<< "docid_unique "<< "= \"" << docid_unique << "\" "
		<< "exist" << "=\"" << exist << "\">"
		<< "<doc>" << newdoc->toString() << "</doc>"
		<< "</trans>";

	// Ketty 2013/02/22
	//AosTaskTransPtr task_trans = addTask(rdata, docstr, docid);
	//addReq(rdata, task_trans, docid);
	
	//if (!rdata->isOk())
	//{
	//	return "";
	//}

	//AosDocReqPtr doc_req = AosDocProc::getSelf()->addModifyAttrRequest(
	//		aname, oldvalue, newvalue,
	//	    exist, value_unique, docid_unique, docid, rdata, task_trans);
	//aos_assert_rr(doc_req, rdata, NULL);
	addReq(rdata, docstr, docid, true);
	*/

	AosTransPtr trans = OmnNew AosModifyObjTrans(docid, newdoc, origdoc, true, false, snap_id);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, "");

	if (!rdata->isOk())
	{
		origdoc = 0;
		newdoc = 0;
		return "";
	}
	AosDocReqPtr doc_req = AosDocProc::getSelf()->addModifyAttrRequest(
			aname, oldvalue, newvalue,
			exist, value_unique, docid_unique, docid, rdata);
	aos_assert_rr(doc_req, rdata, "");

	AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_MODIFYATTR, rdata);
	
	if (!AosEvent::checkEvent1(origdoc, newdoc, AOSEVENTTRIGGER_DOC_MODIFY, rdata))
	{
		rdata->setError() << "Failed to Container Member Verification";
	}
	// AOSMONITORLOG_FINISH(rdata);
	origdoc = 0;
	newdoc = 0;
	return newvalue;

}


bool
AosDocClient::checkModifying(
		const AosXmlTagPtr &origdoc, 
		const AosXmlTagPtr &newdoc,
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(origdoc, rdata, false);
	aos_assert_rr(newdoc, rdata, false);
	newdoc->setAttr(AOSTAG_VERSION, origdoc->getAttrStr(AOSTAG_VERSION));
	newdoc->setAttr(AOSTAG_CTIME, origdoc->getAttrStr(AOSTAG_CTIME));
	newdoc->setAttr(AOSTAG_CT_EPOCH, origdoc->getAttrStr(AOSTAG_CT_EPOCH));
	newdoc->setAttr(AOSTAG_MTIME, OmnGetTime(AosLocale::getDftLocale()));
	newdoc->setAttr(AOSTAG_MT_EPOCH, (u32)OmnGetSecond());
	newdoc->setAttr(AOSTAG_MODUSER, cid);
	newdoc->setAttr(AOSTAG_SITEID, origdoc->getAttrStr(AOSTAG_SITEID));

	u32 siteid = origdoc->getAttrU32(AOSTAG_SITEID,0);

	aos_assert_rr(siteid, rdata, false);
	aos_assert_r(cid != "", false);
	u64 userid = getDocidByCloudid(siteid, cid, rdata);
	aos_assert_r(userid, false);

	//super user, it allows changing creator.
	bool rslt = AosSecurityMgrObj::getSecurityMgr()->isRoot(newdoc, rdata);
	if (!rslt)
	{
		newdoc->setAttr(AOSTAG_CREATOR, origdoc->getAttrStr(AOSTAG_CREATOR));
	}
	else
	{
		OmnString creator = newdoc->getAttrStr(AOSTAG_CREATOR,""); 
		if (creator == "")
		{
			newdoc->setAttr(AOSTAG_CREATOR, origdoc->getAttrStr(AOSTAG_CREATOR));
		}
		else
		{
			// Root is allowed to modify creator. Check whether the creator
			// exists. If not, restore it. Otherwise, keep it.
			AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByCloudid(creator, rdata);	
			if (!dd)
			{
				// It is not a valid creator. Restore it.
				newdoc->setAttr(AOSTAG_CREATOR, origdoc->getAttrStr(AOSTAG_CREATOR));
			}
			dd = 0;
		}
	}

	// If newdoc does not have otype, use the old one
	if (newdoc->getAttrStr(AOSTAG_OTYPE) == "") 
	{
		newdoc->setAttr(AOSTAG_OTYPE, origdoc->getAttrStr(AOSTAG_OTYPE));
	}

	// If newdoc does not have stype, use the old one
	if (newdoc->getAttrStr(AOSTAG_SUBTYPE) == "") 
	{
		newdoc->setAttr(AOSTAG_SUBTYPE, origdoc->getAttrStr(AOSTAG_SUBTYPE));
	}
	
	if (newdoc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT)
	{
		u64 did = origdoc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_rr(did, rdata, false);
		OmnString newcid = newdoc->getAttrStr(AOSTAG_CLOUDID, "");
		OmnString oldcid = origdoc->getAttrStr(AOSTAG_CLOUDID, "");
		aos_assert_rr(oldcid != "", rdata, false);
		aos_assert_rr(newcid != "", rdata, false);
		if (newcid != oldcid)
		{
			//remove old cid
			if (mShowLog) OmnScreen << "Remove value doc: " << oldcid << ":" << did << endl;
			//OmnString iilname = AosIILName::composeCloudidListingName(siteid);
			//aos_assert_rr(iilname != "", rdata, false);
			//AosIILClientSelf->removeValueDoc(iilname, oldcid, did, rdata);
			//AosIILClient::getSelf()->unbindCloudid(oldcid, did, rdata);

			//bind new cid
			//bool rslt1 = bindCloudid(newcid, did, rdata);
			//aos_assert_rr(rslt1, rdata, false);

			//rslt = AosIILClientSelf->rebindCloudid(
			rslt = AosIILClientObj::getIILClient()->rebindCloudid(
				oldcid, newcid, did, rdata); 
			aos_assert_r(rslt, false);
		}
	}

	// All counters cannot be modified
	OmnString cc = origdoc->getAttrStr(AOSTAG_COUNTERCM);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERCM, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERDW);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERDW, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERLK);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERLK, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERRC);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERRC, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERUP);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERUP, cc);

	cc = origdoc->getAttrStr(AOSTAG_COUNTERRD);
	if (cc != "") newdoc->setAttr(AOSTAG_COUNTERRD, cc);

	// All votes cannot be modified
	cc = origdoc->getAttrStr(AOSTAG_VOTEDOWN);
	if (cc != "") newdoc->setAttr(AOSTAG_VOTEDOWN, cc);

	cc = origdoc->getAttrStr(AOSTAG_VOTEUP);
	if (cc != "") newdoc->setAttr(AOSTAG_VOTEUP, cc);

	cc = origdoc->getAttrStr(AOSTAG_VOTETOTAL);
	if (cc != "") newdoc->setAttr(AOSTAG_VOTETOTAL, cc);

	return true;
}

/*
u64
AosDocClient::incrementValue(
		const u64 &docid, 
		const OmnString &objid,
		const OmnString &aname, 
		const u64 &initvalue, 
		const u64 &incValue,
		const u64 &dft_value,
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata) 
{
	// This function adds 'incValue' to the current value of the 
	// attribute 'aname'. Upon success, it returns the new value 
	// as the function's return value. If error, it returns 'dft_value'
	
	aos_assert_r(aname != "", 0);
	AosXmlTagPtr doc;
	if (docid)
	{
		doc = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
	}
	else
	{
		if (objid == "")
		{
			AosSetError(rdata, AosErrmsgId::eNoObjectSpecified);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		doc = AosDocClient::getSelf()->getDocByObjid(objid, rdata);
	}

	if (!doc)
	{
		OmnAlarm << "To increment a doc but the doc does not exist: " << docid << enderr;
		return NULL;
	}

	if (!AosSecurityMgrObj::getSecurityMgr()->checkModify(doc, rdata)) 
	{
		// Access denied
		// AOSMONITORLOG_FINISH(rdata);
		doc = 0;
		return false;
	}

	if (!AosDocUtil::canAttrModified(aname.data(), aname.length()))
	{
		rdata->setError() << "Attribute cannot be modified: " << docid
			<< ". Attribute name: " << aname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		doc = 0;
		return false;
	}
	u64 did = doc->getAttrU64(AOSTAG_DOCID, 0);
*/
	// Ketty 2013/03/14
	/*
	u32 siteid = rdata->getSiteid();
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "=\"" << "incrementvalue" << "\" "
		<< AOSTAG_SITEID << "=\"" << siteid << "\" "
		<< AOSTAG_DOCID << "=\"" << did << "\" "
		<< "aname" << "=\"" << aname << "\" "
		<< "initvalue" << "=\"" << initvalue << "\" "
		<< "incvalue" << "=\"" << incValue << "\" "
		<< "/>";

	AosXmlTagPtr resp;
	bool rslt = addReq(rdata, docstr, resp, did, true);
	if (!rslt) return 0;
	*/

/*
	AosBuffPtr resp;
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIncrememtValueTrans(did, aname, initvalue, incValue, true, true, snap_id);
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	u64	newvalue = resp->getU64(0);
	
	if (rslt) 
	{
		//AosDocReqPtr doc_req = AosDocProc::getSelf()->addModifyAttrRequest(
		//aname, oldvalue, newvalue,
		//exist, value_unique, docid_unique, docid, rdata, task_trans);
		//aos_assert_rr(doc_req, rdata, NULL);
		doc = 0;
		return newvalue;
	}
	doc = 0;
	return dft_value;
}
*/

// Chen Ding, 2013/06/10
bool 
AosDocClient::modifyAttrXPath(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const OmnString &aname, 
		const OmnString &newvalue, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool checkSecurity,
		const bool need_trigger)
{
	OmnNotImplementedYet;
	return false;
}


bool	
AosDocClient::modifyAttrU64(
		const AosXmlTagPtr &doc,
		const OmnString &aname, 
		const u64 &newvalue, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool checkSecurity,
		const AosRundataPtr &rdata,
		const bool need_trigger)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDocClient::modifyBinaryDocByStat(
		const AosXmlTagPtr &doc,
		const AosBuffPtr &buff,
		const u64 &normal_snap_id,
		const u64 &binary_snap_id,
		const AosRundataPtr &rdata)
{
	// doc format 
	// <doc zky_otype = AOSOTYPE_BINARYDOC>
	//		<![BDATA[buff data]]>
	// </doc>
	aos_assert_r(buff && buff->dataLen() > 0, false);
	aos_assert_r(doc, false);

	if (doc->getAttrStr(AOSTAG_OTYPE, "") != AOSOTYPE_BINARYDOC)
	{
		OmnAlarm << "zky_otype mismatch:" << doc->getAttrStr(AOSTAG_OTYPE, "") 
			<< ":" << AOSOTYPE_BINARYDOC << enderr;
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	}

	// 1. compress
	OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
	doc->setTextBinary(nodename, buff);

	u64 newdid = doc->getAttrU64(AOSTAG_DOCID, 0);
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosModifyObjTrans(newdid, doc, doc, false, false, snap_id);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, false);
	return true;
}
