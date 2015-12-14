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
#include "DocClientNew/DocClientNew.h"

#include "JimoAPI/JimoDocEngine.h"


AosAccessRcdPtr
AosDocClientNew::getParentAcrd(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &ref_doc)
{
	OmnNotImplementedYet 
	return 0;            
	/*// It retrieves the access record for the doc 'ref_doc'. If the doc
	// does not have the access record yet, and if 'parent_flag' is not
	// true, it returns 0. Otherwise, it retrieves its parent's access record.
	// AOSMONITORLOG_ENTER(rdata);

	// AOSMONITORLOG_LINE(rdata);
	aos_assert_r(ref_doc, 0);

	u64 ref_docid = ref_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(ref_docid, 0);

	// 2. Retrieve its parent objid.
	OmnString pctnr_objid = ref_doc->getPrimaryContainer();
	if (pctnr_objid != "") 
	{
		// AOSMONITORLOG_LINE(rdata);
		// 3. Retrieve its parent doc
		AosXmlTagPtr parent_doc = getDocByObjid(pctnr_objid, rdata);
		if (!parent_doc)
		{
			// Chen Ding, 2011/02/01. It is possible that the parent
			// container was not created. We will create it if ttl is
			// not too big.
			// AOSMONITORLOG_LINE(rdata);

			// We will create the container only if it is a private container
			OmnString prefix, ctnr_objid_cid;
			u64 uid = 0;
			AosObjid::decomposeObjid(pctnr_objid, prefix, ctnr_objid_cid);
			if (ctnr_objid_cid != "" && isCloudidBound(ctnr_objid_cid, uid, rdata))
			{
				// AOSMONITORLOG_LINE(rdata);
				parent_doc = createMissingContainer(rdata, pctnr_objid, false);
				if (!parent_doc) 
				{
					// This should not happen
					OmnAlarm << "Failed to retrieve home parent (access denied): " 
						<< pctnr_objid << enderr;
					// AOSMONITORLOG_FINISH(rdata);
					return 0;
				}
			}
			else
			{
				rdata->setError();
				// AOSMONITORLOG_FINISH(rdata);
				return 0;
			}
		}
		// Found the parent doc, check whether it has the access record
		u64 parent_docid = parent_doc->getAttrU64(AOSTAG_DOCID, 0);
		AosAccessRcdPtr arcd = getAccessRcd(rdata, 0,  parent_docid, "", false);
		if (arcd)
		{
			// Chen Ding, 2013/12/08
			// arcd->setParentFlag(true);
			if (!arcd->checkAccessRecord(rdata)) return 0;
			rdata->setOk();
			// AOSMONITORLOG_FINISH(rdata);
			return arcd;
		}
	}

	OmnString creator_cid = ref_doc->getAttrStr(AOSTAG_CREATOR);
	OmnString ref_objid = ref_doc->getAttrStr(AOSTAG_OBJID);
	if (ref_objid == AosObjid::composeUserHomeCtnrObjid(creator_cid))
	{
		// AOSMONITORLOG_LINE(rdata);
		AosAccessRcdPtr arcd = getUserHomeCtnrAR(rdata, creator_cid);
		if (arcd)
		{
			if (!arcd->checkAccessRecord(rdata)) return 0;
			rdata->setOk();
			// AOSMONITORLOG_FINISH(rdata);
			return arcd;
		}
	}
	// Still not found the access record. Need to retrieve the user's
	// home container access record.
		
	if (creator_cid == AOSCLOUDID_UNKNOWN)
	{
		// AOSMONITORLOG_LINE(rdata);
		// It is the unknown user. 
		OmnString objid = AosObjid::composeUserAcctObjid(AOSCLOUDID_UNKNOWN);
		AosXmlTagPtr unknown_userdoc = getDocByObjid(objid, rdata);
		if (!unknown_userdoc) 
		{
			aos_assert_r(mLoginMgrObj, 0);
			// AOSMONITORLOG_LINE(rdata);
			unknown_userdoc = mLoginMgrObj->createUnknownUser(rdata);
			aos_assert_r(unknown_userdoc, 0);
		}
		u64 unknown_userid = unknown_userdoc->getAttrU64(AOSTAG_DOCID, 0);
		AosAccessRcdPtr arcd = getAccessRcd(rdata, 0, unknown_userid, "", false);
		if (!arcd)
		{
			// AOSMONITORLOG_LINE(rdata);
			// Chen Ding, 2013/12/07
			// AosAccessRcdPtr arcd = mObjMgrObj->createAccessRecord1(
			// 		rdata, unknown_userdoc->getAttrStr(AOSTAG_CLOUDID), unknown_userid);
			arcd = AosCreateAccessRecord(
					rdata, unknown_userdoc->getAttrStr(AOSTAG_CLOUDID), unknown_userid);
			if (!arcd)
			{
				AosSetErrorUser(rdata, "DocClientNew_failed_create_acd") << enderr;
				return 0;
			}
		}

		if (!arcd->checkAccessRecord(rdata)) return 0;
		rdata->setOk();
		return arcd;
	}

	AosAccessRcdPtr arcd = getUserHomeCtnrAR(rdata, creator_cid);
	if (!arcd)
	{
		rdata->setError();
		return 0;
	}
	rdata->setOk();
	return arcd;
	*/
}


bool 
AosDocClientNew::writeAccessDoc(
		const u64 &owndocid, 
		AosXmlTagPtr &arcd,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoWriteAccessDoc(rdata.getPtr(), owndocid, arcd);
}


AosAccessRcdPtr
AosDocClientNew::getUserHomeCtnrAR(
		const AosRundataPtr &rdata,
		const OmnString &cid)
{
	OmnNotImplementedYet;  
	return 0;              
	
	/*
	// Chen Ding, 10/22/2010
	// This function retrieves the user's home container access record.
	// If not created yet, create it.
	// AOSMONITORLOG_ENTER(rdata);

	// AOSMONITORLOG_LINE(rdata);
	if (cid == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingCloudid);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}


	// Chen Ding, 08/03/2011
	// If it is root, there is no home container AR
	if (AosCloudid::isRootCloudid(cid))
	{
		// It is root cloud id. There is no access records for
		// root docs.
		return 0;
	}
	
	aos_assert_r(mLoginMgrObj, 0);
	OmnString objid = AosObjid::composeUserHomeCtnrObjid(cid);
	AosXmlTagPtr doc = getDocByObjid(objid, rdata);
	if (!doc)
	{
		// The user does not have a home container yet. Need to create it.
		// AOSMONITORLOG_LINE(rdata);
		AosXmlTagPtr userdoc = getDocByCloudid(cid, rdata);
		if (!userdoc)
		{
			// AOSMONITORLOG_LINE(rdata);
			if (cid == AOSCLOUDID_ROOT)
			{
				// It is the system root. Create it.
				// AOSMONITORLOG_LINE(rdata);
				userdoc = mLoginMgrObj->createSuperUser(rdata);
				if (!userdoc)
				{
					OmnAlarm << "Failed to create the root account: "
						<< rdata->getErrmsg() << enderr;
					// AOSMONITORLOG_FINISH(rdata);
					return 0;
				}
			}
		}

		if (!userdoc)
		{
			// AOSMONITORLOG_FINISH(rdata);
			return 0;
		}
		doc = mLoginMgrObj->createHomeFolder3(rdata, cid);
		// AOSMONITORLOG_LINE(rdata);
		aos_assert_r(doc, 0);
	}

	// AOSMONITORLOG_LINE(rdata);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	AosAccessRcdPtr arcd = getAccessRcd(rdata, NULL, docid, "",false);
	if (arcd) 
	{
		// AOSMONITORLOG_FINISH(rdata);
		arcd->checkAccessRecord(rdata);
		return arcd;
	}

	// Not there. Create it.
	u64 userid = getDocidByCloudid(rdata->getSiteid(), cid, rdata);
	// AOSMONITORLOG_LINE(rdata);
	aos_assert_r(userid, 0);
	u64 orig_userid = rdata->setUserid(userid);
	aos_assert_r(mObjMgrObj, 0);
	arcd = mObjMgrObj->createAccessRecord1(rdata, cid, docid);
	rdata->setUserid(orig_userid);
	
	if (!arcd)
	{
		// AOSMONITORLOG_LINE(rdata);
		return 0;
	}
	// AOSMONITORLOG_FINISH(rdata);
	arcd->checkAccessRecord(rdata);
	return arcd;
	*/
}


AosAccessRcdPtr
AosDocClientNew::getParentArcd(
		const OmnString &objid, 
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;  
	return 0;              

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

		doc = getDocByDocid(docid, rdata);
	}
	if (!doc) return 0;

	OmnString parent_objid = doc->getAttrStr(AOSTAG_PARENTC);
	if (parent_objid == "")
	{
		rdata->setError() << "No parent found!";
		return NULL;
	}

	AosXmlTagPtr parent_doc = getDocByObjid(parent_objid, rdata);
	if (!parent_doc)
	{
		rdata->setError() << "Failed retrieving parent doc: " << parent_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	u64 parent_docid = parent_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(parent_docid, rdata, 0);
	
	return getAccessRcd(rdata, parent_doc, parent_docid, parent_objid, false);
}


AosAccessRcdPtr
AosDocClientNew::getAccessRcd(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &ref_doc,
		const u64 &ref_docid, 
		const OmnString &ref_objid,
		const bool parent_flag)
{
	return Jimo::jimoGetAccessRcd(rdata.getPtr(), ref_doc, ref_docid, ref_objid, parent_flag);
}


AosAccessRcdPtr
AosDocClientNew::getParentAR3(
		const AosRundataPtr &rdata,
		const u64 &ref_docid, 
		const OmnString &ref_objid)
{
	OmnNotImplementedYet; 
	return 0;             

	AosXmlTagPtr doc;
	if (ref_docid)
	{
		doc = getDocByDocid(ref_docid, rdata);
	}
	else
	{
		if (ref_objid == "")
		{
			AosSetError(rdata, AosErrmsgId::eNoObjectSpecified);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		doc = getDocByObjid(ref_objid, rdata);
	}

	if (!doc)
	{
		return 0;
	}

	OmnString pctnr_objid = doc->getPrimaryContainer();
	if (pctnr_objid == "") 
	{
		return 0;
	}

	AosAccessRcdPtr acd = getAccessRcd(rdata, 0, 0, pctnr_objid, false);
	return acd;
}

