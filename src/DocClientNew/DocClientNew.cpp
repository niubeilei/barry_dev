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
//
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClientNew/DocClientNew.h"

#include "JimoAPI/JimoDocEngine.h"
#include "XmlUtil/SystemDoc.h"


AosDocClientNew::AosDocClientNew()
{
}


AosDocClientNew::~AosDocClientNew()
{
}


bool
AosDocClientNew::isRepairing()
{
	return Jimo::jimoIsRepairing();
}


bool
AosDocClientNew::showLog() 
{
	return Jimo::jimoIsShowLog();
}


u64
AosDocClientNew::doesObjidExist(
		const u32 siteid,   
		const OmnString &objid,    
		const AosRundataPtr &rdata)
{
	return Jimo::jimoDoesObjidExist(rdata.getPtr(), siteid, objid);	
}


bool
AosDocClientNew::bindObjid(
			OmnString &objid,
			u64 &docid,          
			bool &objid_changed,
			const bool resolve,
			const bool keepDocid,
			const AosRundataPtr &rdata)
{
	return Jimo::jimoBindObjid(rdata.getPtr(), objid, docid, objid_changed, resolve, keepDocid);
}



bool
AosDocClientNew::bindCloudid(
		const OmnString &cid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoBindCloudid_doc(rdata.getPtr(), cid, docid);
}


bool
AosDocClientNew::isCloudidBound(
		const OmnString &cid, 
		u64 &userid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoIsCloudidBound(rdata.getPtr(), cid, userid);
}


AosXmlTagPtr
AosDocClientNew::createExclusiveDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet; 
	return 0;          

	// This function creates an exclusive doc, which means that there shall be one and
	// only one such doc (identified by the objid). The exclusiveness is achieved
	// by:
	// 	1. Retrieve the objid
	// 	2. Check whether the doc is already cached in AosDocMgr. If yes, return it.
	// 	3. Hash on the objid. This determines which virtual doc server to handle
	// 	   its creation. 
	// 	4. Send a request to that virtual doc server. 
	// 	5. When the virtual doc server receives the request, it checks whether the
	// 	   doc was created. If yes, it returns the doc. 
	// 	6. If not, it creates the doc, and then returns the doc. Note that it will 
	// 	   use locks to ensure the creation is exclusive.
	aos_assert_rr(doc, rdata, 0);
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0,rdata, 0);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	if (objid == "")
	{
		rdata->setError() << "Object missing docid. The doc: " << doc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosXmlTagPtr dd = getDocByObjid(objid, rdata);
	if (dd)
	{
		// The doc has already been created. 
		return dd;
	}

	// Need to send a request to the remote server. 
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "=\"" << "create_exc_doc" << "\">"
	    << "<newxml>" << doc->toString() << "</newxml>"
		<< "</trans>";

	// Need to add a sync-ed call. The server side should allocate a docid and
	// respond it immediately. 
	AosXmlTagPtr resp;
	// Ketty 2013/03/14 temp.
	OmnNotImplementedYet;
	//bool rslt = addReq(rdata, docstr, resp, doc->getAttrU64(AOSTAG_DOCID, 0), true);
	//if (!rslt) return 0;
	OmnNotImplementedYet;
	return 0;
}


bool
AosDocClientNew::checkHomeContainer(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc, 
		const AosXmlTagPtr &origdoc, 
		const OmnString &target_cid) 
{
	OmnNotImplementedYet;
	return false;         
/*
	// This function determines whether the parent container objid
	// is set in 'doc'. If not, it will create one depending on 
	// whether it is public or private. If it is public, it will 
	// assign the default public home container objid to it. Otherwise,
	// it will assign the default private home container objid 
	// to it. 
	//
	// In addition, if the parent container has not been created yet,
	// it will create it.
	//
	// If it returns false, it should abort the creation.
	// AOSMONITORLOG_ENTER(rdata);
	aos_assert_rr(doc, rdata, false);
		
	// In the current implementations, there can be only one parent container.
	// Other parents are put in 'memberof'.
	char *buff1;
	OmnString ctnr_objid = doc->getAttrStr(AOSTAG_PARENTC);
	if (ctnr_objid.length() > 5)
	{
		// Container names cannot start with '_zt' unless it is
		// '_zt00'
		buff1 = (char *)ctnr_objid.data();
		if (buff1[0] == '_' && buff1[1] == 'z' && buff1[2] == 't')
		{
			if (buff1[3] != '0' || buff1[4] != '0')
			{
				// check the container exsit ,if not ,escape the objid
				if (!getDocByObjid(ctnr_objid, rdata))
				{
					OmnString nn = "_zt00";
					nn << ctnr_objid;
					doc->setAttr(AOSTAG_PARENTC, nn);
				}
			}
		}
	}

	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	OmnString hctnr = doc->getAttrStr(AOSTAG_PARENTC);

	// Ice 2010/11/23 
	// If 'origdoc' exists and parent container is not assigned yet, 
	// assign it.
	if (origdoc && hctnr == "")
	{
		// AOSMONITORLOG_LINE(rdata);
		hctnr = origdoc->getAttrStr(AOSTAG_PARENTC);
		if (hctnr == "")
		{
			if (objid == AOSOBJIDBASE_SYSROOT) return true;
			AosSetError(rdata, AosErrmsgId::eMissingContainer);
			OmnAlarm << rdata->getErrmsg() << ". Objid: " << objid << enderr;
			return false;
		}

		doc->setAttr(AOSTAG_PARENTC, hctnr);
	}

	bool is_public = doc->getAttrBool(AOSTAG_CTNR_PUBLIC, false);
	if (origdoc) is_public = doc->getAttrBool(AOSTAG_CTNR_PUBLIC, true);

	// Now it is the time to determine the parent container objid.
	if (hctnr == "")
	{
		// The doc does not have a parent container yet. Note that it does not
		// check root container anymore. Root container is created specially 
		// so that all docs will have a parent container. 
		// Create the default container and assign it to 'doc'.
		// AOSMONITORLOG_LINE(rdata);
		if (is_public)
		{
			// Security Check
			// AOSMONITORLOG_LINE(rdata);
			if (!mIsRepairing &&
				!AosSecurityMgrObj::getSecurityMgr()->checkAddPubObjid(objid, rdata))
			{
				// AOSMONITORLOG_FINISH(rdata);
				doc->removeAttr(AOSTAG_CTNR_PUBLIC);
				return false;
			}
		}

		// Create the default container
		aos_assert_r(mObjMgrObj, false);
		AosXmlTagPtr ctnr = mObjMgrObj->createDftContainer(
				rdata, doc, is_public, target_cid);
		if (!ctnr || (hctnr = ctnr->getAttrStr(AOSTAG_OBJID)) == "")
		{
			AosSetError(rdata, "create_hctnr_fail_01") << ": " << is_public
				<< ":" << objid << ":" << target_cid;
			//OmnAlarm << rdata->getErrmsg() << enderr;
			doc->removeAttr(AOSTAG_CTNR_PUBLIC);
			ctnr = 0;
			return false;
		}

		doc->setAttr(AOSTAG_PARENTC, hctnr);
		ctnr = 0;
	}
	else
	{
		if (!is_public) appendCidToParentCtnrObjid(doc, target_cid, rdata);

		// The parent container is not empty. Check whether the parent container
		// was created. if not, it should create the parent container.
		//if (!checkContainers(rdata, doc, origdoc, target_cid, true))
		if (!checkContainers(rdata, doc, origdoc, target_cid, is_public))
		{
			// AOSMONITORLOG_FINISH(rdata);
			doc->removeAttr(AOSTAG_CTNR_PUBLIC);
			return false;
		}
	}

	AosTagMgr::getSelf()->setSysTags(doc);
	rdata->setOk();
	// AOSMONITORLOG_FINISH(rdata);
	doc->removeAttr(AOSTAG_CTNR_PUBLIC);
	return true;*/
}


bool
AosDocClientNew::checkContainers(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &xml, 
		const AosXmlTagPtr &docorig, 
		const OmnString &cid, 
		const bool ctnr_public)
{
	OmnNotImplementedYet; 
	return false;         
	/*
	// This function checks whether the parent container in 'xml'
	// exists in the system.
	//
	// IMPORTANT: This function assumes mLock is not locked.
	// AOSMONITORLOG_ENTER(rdata);
	u32 siteid = rdata->getSiteid();

	OmnString container_objid = xml->getAttrStr(AOSTAG_PARENTC);
	// Check whether the parent should be public or private, which
	// is determined by the attribute AOSTAG_CTNR_PUBLIC.
	bool ctnr_is_public = ctnr_public;

	if (doesObjidExist(siteid, container_objid, rdata) == AOS_INVDID)
	{
		// The container does not exist. 
		// AOSMONITORLOG_LINE(rdata);
		if (!mIsRepairing)
		{
			// The container does not exist. Create it.
			// AOSMONITORLOG_LINE(rdata);
			AosXmlTagPtr ddd = createMissingContainer(
					rdata, container_objid, ctnr_is_public);
			if (!ddd)
			{
				return false;
			}
			else
			{
				container_objid = ddd->getAttrStr(AOSTAG_OBJID);
			}
			ddd = 0;
		}
	}
	else
	{
		// The container does exist. Check its otype.
		AosXmlTagPtr dd = getDocByObjid(container_objid, rdata);
		if (!dd)
		{
			// If it is repairing, we do not enforce that 'container_objid'
			// must exist. 
			if (mIsRepairing)
			{
				return true;
			}
			rdata->setError() << "Not a container: " << container_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			// AOSMONITORLOG_FINISH(rdata);
			return false;
		}

		aos_assert_rr(dd->isRootTag(), rdata, false);
		OmnString otype = dd->getAttrStr(AOSTAG_OTYPE);
		if (otype != AOSOTYPE_CONTAINER &&
			otype != AOSTAG_TABLE)
		{
			rdata->setError() << "Not a container: " << container_objid;
			//OmnAlarm << rdata->getErrmsg() << enderr;
			// AOSMONITORLOG_FINISH(rdata);
			dd = 0;
			return false;
		}
		dd = 0;
	}
	rdata->setOk();
	// AOSMONITORLOG_FINISH(rdata);
	return true;
	*/
}


bool
AosDocClientNew::determinePublic(
		const AosXmlTagPtr &doc, 
		const AosXmlTagPtr &cmd,
		const OmnString &tcid,
		OmnString &objid)
{
	OmnNotImplementedYet; 
	return false;         
	/*// This funciton determines whether it is to create a public or
	// private doc. 
	// 1. If 'doc''s attribute AOSTAG_PUBLIC_DOC is "true", it is public.
	// 2. If 'cmd' is defined and its AOSTAG_PUBLIC_DOC is "true", it is public.
	// 3. If it is an access record, it is public.
	//
	// IMPORTANT: If it is private and the objid has not been associated with
	// the cloudid, it will attach the cloudid.
	OmnString is_pubdata = doc->getAttrStr(AOSTAG_PUBLIC_DOC);

	if (is_pubdata != "")
	{
		doc->removeAttr(AOSTAG_PUBLIC_DOC, false, true);
	}

	if (is_pubdata == "true")
	{
		return true;
	}

	if (cmd && cmd->getAttrStr(AOSTAG_PUBLIC_DOC) == "true")
	{
	 	return true;
	}

	// If it is an access record, it is public.
	if (doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD)
	{
		// It is an access record. Should be treated as public.
		return true;
	}

	// Otherwise, it is private.
	if (!mIsRepairing)
	{
		OmnString prefix, cc;
		AosObjid::decomposeObjid(objid, prefix, cc);
		if (cc != tcid)
		{
			objid = AosObjid::compose(objid, tcid);
			doc->setAttr(AOSTAG_OBJID, objid);
		}
	}
	return false;
	*/
}


AosXmlTagPtr
AosDocClientNew::getTempDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosDocClientNew::addCopy(
		const u64 &docid,
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;        
	/*
	if (!sgDocMgr) sgDocMgr = AosDocMgrObj::getDocMgr();
	aos_assert_r(sgDocMgr, false);

	u64 snap_id = rdata->getSnapshotId();
	return sgDocMgr->addCopy(docid, doc, snap_id, rdata);
	*/
}


bool
AosDocClientNew::appendCidToParentCtnrObjid(
		const AosXmlTagPtr &doc, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;        
/*
	// This function determines whether the parent container objid
	// needs to append a cid. 
	if (cid == "") return true;
	OmnString ctnr_objid = doc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_rr(ctnr_objid != "", rdata, false);
	OmnString prefix, ccid;
	AosObjid::decomposeObjid(ctnr_objid, prefix, ccid);

	if (ccid == cid) return true;
	OmnString objid = AosObjid::compose(ctnr_objid, cid);
	doc->setAttr(AOSTAG_PARENTC, objid);
	return true;
	*/
}


AosSystemDocPtr 
AosDocClientNew::getSystemDocByObjid(
		const OmnString &ref_objid, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;        

	/*
	// System docs are identified through objids that are composed based
	// on the referenced doc:
	// 		AOSZTG_SYSTEM_DOC + "_" + referenced_docid
	//
	// This is composed through the following member function:
	// 		AosObjid::composeSysdocObjid(const u64 &ref_docid)
	aos_assert_rr(ref_objid != "", rdata, 0);
	u64 ref_docid = AosIILClientObj::getIILClient()->getDocidByObjid(ref_objid, rdata);
	if (!ref_docid)
	{
		AosSetError(rdata, AosErrmsgId::eNoDocidByObjid) << ". Objid: " << ref_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	return getSystemDocByDocid(ref_docid, rdata);
	*/
}


bool
AosDocClientNew::procDocLock(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &req,
		const u64 &docid)
{
	return Jimo::jimoProcDocLock(rdata.getPtr(), req, docid);
}


bool
AosDocClientNew::checkDocLock(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const OmnString &type)
{
	OmnNotImplementedYet;
	return false;        
/*
	return true;
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosCheckDocLockTrans(docid, type, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);	
	if(!rslt)	return false;
	aos_assert_rr(resp, rdata, false);
	
	bool result = resp->getU8(0);
	return result;
	*/
}


bool
AosDocClientNew::parseCreateReqPriv(
		const AosXmlTagPtr &trans_doc,
		AosXmlTagPtr &root,
		AosXmlTagPtr &newxml)
{
	OmnNotImplementedYet;
	return false;        

	AosXmlTagPtr xml = trans_doc->getFirstChild("doc");
	aos_assert_r(xml, false);

	xml = xml->getFirstChild();
	aos_assert_r(xml, false);

	AosXmlParser parser;
	newxml = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_r(newxml, false);

	root = newxml;
	aos_assert_r(root, false);
	return true;
}

bool
AosDocClientNew::parseModifyReqPriv(
		const AosXmlTagPtr &trans_doc,
		AosXmlTagPtr &root,
		AosXmlTagPtr &newxml,
		AosXmlTagPtr &origdoc,
		bool &synobj)
{
	OmnNotImplementedYet;
	return false;        

	AosXmlTagPtr xml = trans_doc->getFirstChild("doc");
	aos_assert_r(xml, false);

	xml = xml->getFirstChild();
	aos_assert_r(xml, false);

	AosXmlParser parser;
	newxml = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_r(newxml, false);

	AosXmlTagPtr xml2 = trans_doc->getFirstChild("origdoc");
	aos_assert_r(xml2, false);

	xml2 = xml2->getFirstChild();
	aos_assert_r(xml2, false);

	AosXmlParser parser2;
	origdoc = parser.parse(xml2->toString(), "" AosMemoryCheckerArgs);
	aos_assert_r(origdoc, false);
	
	synobj = trans_doc->getAttrBool("synobj");
	root = newxml;	
	return true;
}

bool
AosDocClientNew::parseDeleteReqPriv(
		const AosXmlTagPtr &trans_doc,
		AosXmlTagPtr &origdoc)
{
	OmnNotImplementedYet;
	return false;        

	AosXmlTagPtr xml = trans_doc->getFirstChild("origdocroot");
	aos_assert_r(xml, false);

	xml = xml->getFirstChild();
	aos_assert_r(xml, false);

	AosXmlParser parser;
	origdoc = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_r(origdoc, false);
	return true;
}


bool
AosDocClientNew::parseModifyAttrReqPriv(
		const AosXmlTagPtr &trans_doc, 
		OmnString &attrname,
		OmnString &oldvalue,
		OmnString &newvalue,
		bool &exist, 
		bool &value_unique, 
		bool &docid_unique, 
		u64 &docid, 
		OmnString &fname, 
		int &line)
{
	OmnNotImplementedYet;
	return false;        

	attrname = trans_doc->getAttrStr("attrname" , "");
	aos_assert_r(attrname != "", false);

	oldvalue = trans_doc->getAttrStr("oldvalue", "");
	aos_assert_r(oldvalue != "", false);

	newvalue = trans_doc->getAttrStr("newvalue", "");
	aos_assert_r(newvalue != "", false);

	exist = trans_doc->getAttrBool("exist");
	value_unique = trans_doc->getAttrBool("value_unique");
	docid_unique = trans_doc->getAttrBool("docid_unique");
	docid = trans_doc->getAttrU64(AOSTAG_DOCID, 0);
	fname = trans_doc->getAttrStr("fname", "");
	line = trans_doc->getAttrInt("line", 0);
	return true;
}


OmnString
AosDocClientNew::getObjidByDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	return Jimo::jimoGetObjidByDocid(rdata.getPtr(), docid);
}


bool 
AosDocClientNew::handleObjidIssue(
		OmnString &objid, 
		const AosXmlTagPtr &newdoc,  
		const bool resolveObjid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;        

	// This function handles the case that the objid is also its 
	// parent container's objid. This is normally an error. 
	// If 'resolveObjid' is set to true, it will resolve the objid.
	aos_assert_rr(newdoc, rdata, false);
	if (!resolveObjid)
	{
		AosSetError(rdata, "invalid_objid_003") << ": " << objid;
		return false;
	}
	
	AosObjid::addNumToObjid(objid, rdata);
	newdoc->setAttr(AOSTAG_OBJID, objid);
	return true;
}


void
AosDocClientNew::procNotifyMsg(
		const u64 &docid, 
		const u32 siteid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoProcNotifyMsg(rdata.getPtr(), docid, siteid);
}


AosXmlTagPtr 
AosDocClientNew::writeLockDocByObjid(const OmnString &objid, const AosRundataPtr &rdata)
{
	return Jimo::jimoWriteLockDocByObjid(rdata.getPtr(), objid);
}


AosXmlTagPtr 
AosDocClientNew::writeLockDocByDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	return Jimo::jimoWriteLockDocByDocid(rdata.getPtr(), docid);
}


AosXmlTagPtr 
AosDocClientNew::releaseWriteLock(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	return Jimo::jimoReleaseWriteLock(rdata.getPtr(), doc);
}


AosXmlTagPtr 
AosDocClientNew::releaseWriteLockDocByObjid(const OmnString &objid, const AosRundataPtr &rdata)
{
	return Jimo::jimoReleaseWriteLockDocByObjid(rdata.getPtr(), objid);
}


AosXmlTagPtr 
AosDocClientNew::releaseWriteLockDocByDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	return Jimo::jimoReleaseWriteLockDocByDocid(rdata.getPtr(), docid);
}


bool 
AosDocClientNew::purgeDoc(const OmnString &objid, const AosRundataPtr &rdata)
{
	return Jimo::jimoPurgeDoc(rdata.getPtr(), objid);
}


bool 
AosDocClientNew::purgeDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	return Jimo::jimoPurgeDoc(rdata.getPtr(), docid);
}


bool 
AosDocClientNew::purgeDocByCloudid(const OmnString &cid, const AosRundataPtr &rdata)
{
	return Jimo::jimoPurgeDocByCloudid(rdata.getPtr(), cid);
}


bool
AosDocClientNew::batchSendDocids(
		const OmnString &scanner_id,
		const int serverid,
		const AosBitmapObjPtr &bitmap,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoBatchSendDocids(rdata.getPtr(), scanner_id, serverid, bitmap);
}

bool
AosDocClientNew::batchSendDocids(
		const OmnString &scanner_id,
		const int serverid,
		const AosBitmapObjPtr &bitmap,
		const AosDocClientCallerPtr &caller,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoBatchSendDocids(rdata.getPtr(), scanner_id, serverid, bitmap, caller);
}


bool
AosDocClientNew::batchSendStart(
		const int serverid,
		const OmnString &scanner_id,
		const u32 &queue_size,
		const OmnString &read_policy,
		const u64 &blocksize,
		const OmnString &batch_type,
		const AosXmlTagPtr &field_names,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
	//aos_assert_r(scanner_id != "", false);

	//AosBuffPtr cont = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	//cont->setU32(queue_size);
	//cont->setOmnStr(read_policy);
	//cont->setU64(blocksize);
	//cont->setOmnStr(batch_type);
	//if (field_names)
	//{
	//	cont->setOmnStr(field_names->toString());
	//}
	//else
	//{
	//	cont->setOmnStr("");
	//}

	//AosTransPtr trans = OmnNew AosBatchSendStatusTrans(scanner_id, 
	//		AosDocBatchReaderReq::eStart, cont, serverid, true, true);
	//AosBuffPtr resp;
	//bool rslt = addReq(rdata, trans, resp);
	//aos_assert_r(rslt, false);

	//aos_assert_r(resp , false);
	//rslt = resp->getU8(0);
	//if (!rslt)
	//{
	//	rdata->setError() << "Failed check docid from server" ;
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	AOSLOG_LEAVE(rdata);
	//	return false;
	//}
	//return true;
}


bool
AosDocClientNew::batchSendFinished(
		const int serverid,
		const OmnString &scanner_id,
		const int total_num_data_msgs,
		const AosRundataPtr &rdata)
{
	//Linda, 2013/09/23
	OmnNotImplementedYet;
	return false;
	//aos_assert_r(scanner_id != "", false);

	//AosBuffPtr cont = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	//cont->setInt(total_num_data_msgs);
	//
	//AosTransPtr trans = OmnNew AosBatchSendStatusTrans(scanner_id, 
	//		AosDocBatchReaderReq::eFinished, cont, serverid, true, true);
	//AosBuffPtr resp;
	//bool rslt = addReq(rdata, trans, resp);
	//aos_assert_r(resp , false);
	//rslt = resp->getU8(0);
	//if (!rslt)
	//{
	//	rdata->setError() << "Failed check docid from server" ;
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	AOSLOG_LEAVE(rdata);
	//	return false;
	//}
	return true;
}


bool
AosDocClientNew::batchDataReceived(
		const OmnString &scanner_id, 
		const AosBuffPtr &contents,
		const int serverid,
		const bool finished,
		const int num_call_back_data_msgs,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoBatchDataReceived(rdata.getPtr(), scanner_id, contents, serverid, finished, num_call_back_data_msgs);
}

u64
AosDocClientNew::createSnapshot(
		const AosRundataPtr &rdata,
		const int virtual_id,
		const u64 snap_id,
		const AosDocType::E doc_type,
		const u64 &task_docid)
{
	return Jimo::jimoCreateSnapshot(rdata.getPtr(), virtual_id, snap_id, doc_type, task_docid);
}


bool
AosDocClientNew::rollbackSnapshot(
		const AosRundataPtr &rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid)
{
	return Jimo::jimoRollbackSnapshot(rdata.getPtr(), virtual_id, doc_type, snap_id, task_docid);
}


bool
AosDocClientNew::commitSnapshot(
		const AosRundataPtr &rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid)
{
	return Jimo::jimoCommitSnapshot(rdata.getPtr(), virtual_id, doc_type, snap_id, task_docid);
}

bool
AosDocClientNew::mergeSnapshot(                                                 
		const AosRundataPtr &rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &target_snap_id,
		const u64 &merge_snap_id)
{
	return Jimo::jimoMergeSnapshot(rdata.getPtr(), virtual_id, doc_type, target_snap_id, merge_snap_id);
}


u64
AosDocClientNew::getCtnrDocidByChildDocid(
		AosRundata *rdata,
		const u64 &docid)
{
	OmnNotImplementedYet;
	return false;        

	AosXmlTagPtr doc = getDocByDocid(docid, rdata);
	aos_assert_r(doc, 0);

	OmnString ctnr_objid = doc->getAttrStr(AOSTAG_PARENTC, "");
	aos_assert_r(ctnr_objid != "", 0);

	u64 ctnr_docid = getDocidByObjid(ctnr_objid, rdata);
	aos_assert_r(ctnr_docid, 0);

	return ctnr_docid;
}


bool 
AosDocClientNew::readLockDocByDocid(
		AosRundata *rdata,
		const u64 &docid,
		const int waittimer,
		const int holdtimer)
{
	return Jimo::jimoReadLockDocByDocid(rdata, docid, waittimer, holdtimer);
}


bool         
AosDocClientNew::readUnlockDocByDocid(
		AosRundata *rdata, 
		const u64 &docid)
{
	return Jimo::jimoReadUnlockDocByDocid(rdata, docid);
}


bool
AosDocClientNew::writeLockDocByDocid(
		AosRundata *rdata,
		const u64 &docid,
		const int waittimer,
		const int holdtimer)
{
	return Jimo::jimoWriteLockDocByDocid(rdata, docid, waittimer, holdtimer);
}


bool
AosDocClientNew::writeUnlockDocByDocid(
		AosRundata *rdata,
		const u64 &docid,
		const AosXmlTagPtr &newdoc)
{
	return Jimo::jimoWriteUnlockDocByDocid(rdata, docid, newdoc);
}


bool
AosDocClientNew::addCreateLogRequest(
		const u64 &userid,
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoAddCreateLogRequest(rdata.getPtr(), userid, doc);
}


bool
AosDocClientNew::addDeleteLogRequest(
		const AosXmlTagPtr &doc,
		const u64 userid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoAddDeleteLogRequest(rdata.getPtr(), doc, userid);
}

