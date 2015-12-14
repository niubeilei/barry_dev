////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SearchEngine/DocMgr1.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DocServer/DocSvr.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocServer.h"
#include "SEUtil/Objid.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/DocFileMgr.h"
#include "SEModules/LoginMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SmartDoc/SmartDoc.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "UserMgmt/UserAcctMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlUtil/AccessRcdMgr.h"

OmnSingletonImpl(AosDocMgr1Singleton, AosDocMgr1, AosDocMgr1Self, "AosDocMgr1");


AosDocMgr1::AosDocMgr1()
:
mLock(OmnNew OmnMutex()),
mIsLocal(true),
mNumVirtual(0)
{
	mDocNum = 0;
	mDocHead = OmnNew AosDocLink;
	mDocHead->docId = 10000;
}


AosDocMgr1::~AosDocMgr1()
{
	
	std::map<u64, AosDocLink*>::const_iterator itr = mDocMap.begin();
	for(; itr!=mDocMap.end(); ++itr)
	{
		delete itr->second;
	}
}


bool 
AosDocMgr1::start()
{
	return true;
}


bool 
AosDocMgr1::stop()
{
	return true;
}


bool 
AosDocMgr1::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr conf = config->getFirstChild("docclient");
	if (!conf) return true;
	mNumVirtual = conf->getAttrInt(AOSTAG_NUM_VIRTUALS, -1);
	if (mNumVirtual < 0)
	{
		OmnAlarm << "Incorrect or missing num_virtual attribute: " << mNumVirtual << enderr;
		exit(-1);
	}
	if (mNumVirtual) mIsLocal = false;

	return true;
}


AosXmlTagPtr 
AosDocMgr1::getDoc(
		const OmnString &objid, 
		const AosRundataPtr &rdata)
{
	// It retrieves the doc identified by 'objid'. 
	aos_assert_r(objid!="", NULL);
	bool isunique = false;
	u64 docid = AOS_INVDID;
	bool rslt = AosIILClient::getSelf()->getDocidByObjid(
			rdata->getSiteid(), objid, docid, isunique, rdata);
	if (!rslt) return NULL;
	if (!isunique) return NULL;
	if (docid == AOS_INVDID) return NULL;
	return getDoc(docid, objid, rdata);
}


AosXmlTagPtr 
AosDocMgr1::getDocByCloudid(
		const OmnString &cloudid, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(cloudid != "", 0);
	OmnString iilname = AosIILName::composeCloudidListingName(rdata->getSiteid());
	bool isunique = false;
	u64 docid = AOS_INVDID;

	AosIILClient::getSelf()->getDocid(iilname, cloudid, docid, isunique, rdata);
	if (docid != AOS_INVDID)
	{
		// OmnString objid = AosObjid::composeUserAcctObjid(cloudid);
		AosXmlTagPtr doc = getDoc(docid, "", rdata);
		aos_assert_r(doc, NULL);
		aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT, 0);
		aos_assert_r(doc->getAttrStr(AOSTAG_CLOUDID) == cloudid, 0);
		return doc;
	}

	// Backward compatibility
	OmnString objid = AosObjid::compose(AOSTAG_USERPREFIX, cloudid);
	aos_assert_r(objid!="", NULL);
	return getDoc(objid, rdata);
}


AosXmlTagPtr 
AosDocMgr1::getDocByDocid(
		const OmnString &docid, 
		const OmnString &objid, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid !="", NULL);
	u64 did;
	aos_assert_r(did = atoll(docid), NULL);
	return getDoc(did, objid, rdata);
}


AosXmlTagPtr 
AosDocMgr1::getDoc(
		const u64 &docid, 
		const OmnString &objid, 
		const AosRundataPtr &rdata)
{
	//1 read doc from map
	//  if the doc exsit ,return doc ,put the doc to front of the link list 
	//  else retreive the doc from file, and put it to front of the link list
	//  if the link list if full, it will remove the end docs of the list 
	aos_assert_r(docid, NULL);
	mLock->lock();
	std::map<u64, AosDocLink*>::iterator itr = mDocMap.find(docid);
	if (itr != mDocMap.end())
	{
		// This means that there are entries that match 'docid'. 
		// AosDocLink* node = mDocMap[docid];
		AosDocLink* node = itr->second;
		if (mDocNum != 1)
		{
			// Move the doc to the front of the list. This will 
			// ensure that the doc will not be removed from 
			// the cache in the near future.
			moveNode(node, false);
		}

		if (objid != "" && node->doc->getAttrStr(AOSTAG_OBJID) != objid)
		{
			AosSetError(rdata, AosErrmsgId::eObjidMismatch) << objid
				<< ":" << node->doc->getAttrStr(AOSTAG_OBJID);
			OmnAlarm << "Objid mismatch: " << objid << ":" 
				<< node->doc->getAttrStr(AOSTAG_OBJID) << enderr;
			mLock->unlock();
			return 0;
		}

		if (node->doc->getAttrU64(AOSTAG_DOCID, 0) != docid)
		{
			AosSetError(rdata, AosErrmsgId::eDocidMismatch) << docid
				<< ":" << node->doc->getAttrU64(AOSTAG_DOCID, 0);
			OmnAlarm << "Docid mismatch: " << docid << ":"
				<< node->doc->toString() << enderr;
			mLock->unlock();
			return 0;
		}
		
		mLock->unlock();
		return node->doc;
	}
	mLock->unlock();
	return 0;

	// Chen Ding, 09/01/2011
	// DocMgr no longer load docs directly from files.
	// Not in the cache. Load it from files.
	// AosXmlTagPtr doc;
	// if (mIsLocal)
	// {
	// 	AosDocFileMgrPtr docfilemgr = AosDocSvr::getSelf()->getDocFile(docid);
	// 	doc = AosXmlDoc::readFromFile(docid, docfilemgr, rdata);
	// 	if (!doc)
	// 	{
	// 		rdata->setError() << "Failed reading the doc: " << docid;
	// 	}
	// }
	// else
	// {
	// 	doc = AosDocClient::getSelf()->getDoc(rdata, docid);
	// }
	// if (!doc) 
	// {
	// 	// This can happen either the docid is invalid or 
	// 	// the doc was deleted.
	// 	mLock->unlock();
	// 	return 0;
	// }

	// AosDocLink* node;
	// if(mDocNum < eMaxNumDocs2Cache)
	// {
	// 	node = OmnNew AosDocLink(docid, doc);
	// 	addNode(node);
	// 	mDocNum++;
	// }
	// else
	// {
	// 	node = mDocHead->p;
	// 	moveNode(node, true);
	// 	node->doc = doc;
	// 	node->docId = docid;
	// }
	// 
	// mDocMap[docid] = node;
	// mLock->unlock();
	// return doc;
}


bool
AosDocMgr1::addDoc(const AosXmlTagPtr &doc, const u64 &docid)
{
	mLock->lock();
	bool rslt = addDocPriv(doc,docid);
	mLock->unlock();
	return rslt;
}


bool
AosDocMgr1::addDocPriv(const AosXmlTagPtr &doc, const u64 &docid)
{
	// This function adds the doc 'doc' into the class. 
	// If the doc already exists, it replaces the doc.
	aos_assert_r(doc, false);
	aos_assert_r(docid, false);
	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, false);
	// int num = mDocMap.count(docid);
	// if (num)
	std::map<u64, AosDocLink*>::iterator itr = mDocMap.find(docid);
	if (itr != mDocMap.end())	
	{
		// This means that the doc exists. Replace it.
		AosDocLink* node = itr->second;
		if (mDocNum != 1)
		{
			// Move the doc to the front of the list. This will 
			// ensure that the doc will not be removed from 
			// the cache in the near future.
			moveNode(node, false);
		}
		node->doc = doc;
		return true;
	}

	// The doc does not exist. Add it.
	AosDocLink* node;
	if(mDocNum < eMaxNumDocs2Cache)
	{
		node = OmnNew AosDocLink(docid, doc);
		addNode(node);
		mDocNum++;
	}
	else
	{
		node = mDocHead->p;
		moveNode(node, true);
		node->doc = doc;
		node->docId = docid;
	}
	mDocMap[docid] = node;
	return doc;
}


bool
AosDocMgr1::addCopy(
		const u64 &docId, 
		const AosXmlTagPtr &doc)
{
	// This function puts the doc into the hash. If the doc
	// exists, it replaces it. 
	aos_assert_r(doc, false);
	OmnString data = doc->toString();
	aos_assert_r(data!="", false);
	AosXmlParser parser;
	AosXmlTagPtr savedoc = parser.parse(data,"");
	aos_assert_r(savedoc, false);
	return addDoc(savedoc, docId);
}


bool
AosDocMgr1::deleteDoc(const u64 &docid)
{

	mLock->lock();
	if (!mDocHead) 
	{
		mLock->unlock();
		return true;
	}

	std::map<u64, AosDocLink*>::iterator itr = mDocMap.find(docid);
	if (itr != mDocMap.end())
	{
		AosDocLink* node = itr->second;
		if (mDocNum == 1)
		{
			aos_assert_rl(mDocHead == node, mLock, false);
			mDocHead = 0;
			mDocNum = 0;
			delete node;
			node = 0;
			mDocMap.erase(docid);
			mLock->unlock();
			return true;
		}

		// Remove the node from the list
		node->p->n = node->n;
		node->n->p = node->p;

		if (mDocHead == node)
		{
			mDocHead = node->n;
			aos_assert_rl(mDocHead, mLock, false);
			aos_assert_rl(mDocHead != node, mLock, false);
		}

		// if (mDocHead->n == node)
		// {
		// 	mDocHead->n = node->n;
		// }

		// if(mDocHead->p == node)
		// {
		//	mDocHead->p = node->p;
		//}

		mDocMap.erase(docid);
		delete node;
		node = 0;
		mDocNum--;
	}

	mLock->unlock();
	return true;
}


bool
AosDocMgr1::moveNode(AosDocLink* node, const bool rmMap)
{
	// If 'rmMap' is true, it removes the last entry in the
	// list. Otherwise, it simply moves the node.
	if (rmMap)
	{
		u64 mapKey = mDocHead->p->docId;
		mDocMap.erase(mapKey);
	}

	node->p->n = node->n;
	node->n->p = node->p;

	node->p = mDocHead->p;
	mDocHead->p->n = node;

	node->n = mDocHead;
	mDocHead->p = node;

	mDocHead = node;
	// addNode(node);
	return true;
}


bool
AosDocMgr1::addNode(AosDocLink* node)
{
	// It adds the node 'node' to the front of the list. 
	if(mDocNum == 0)
	{
		// This is the first node to add:
		// mDocHead next to the node
		// mDocHead prev to the node
		// node->next to mDocHead
		// node->prev to mDocHead
		mDocHead->n = node;
		mDocHead->p = node;
		node->p = mDocHead;
		node->n = mDocHead;
	}
	else
	{
		// Insert it to the front:
		// node next to mDocHead next
		// node prev to mDocHead
		// mDocHead next's prev to node
		// mDocHead next to node

		// 1. Remove 'node' from the list.
		// if (node->p) node->p->n = node->n;	
		// if (node->n) node->n->p = node->p;	

		// 2. Insert 'node' right after 'mDocHead'
		// node->p = mDocHead;
		// node->n = mDocHead->n;
		node->p = mDocHead->p;
		node->n = mDocHead;
		node->p->n = node;
		node->n->p = node;
		mDocHead = node;

		// 3. Modify mDocHead so that its next's prev points to 'node'
		// and mDocHead points to node.
		// mDocHead->n->p = node;
		// mDocHead->n = node;
	}
	return true;
}


AosAccessRcdPtr
AosDocMgr1::getAccessRcd2(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &ref_doc,
		const u64 &ref_docid, 
		const OmnString &ref_objid,
		const bool parent_flag)
{
	// This function retrieves the access record for a doc that is 
	// identified by one of 'ref_doc', 'ref_docid', or 'ref_objid'. 
	// If 'parent_flag' is not true, it will not retrieve its parent
	// or its home default access record. Otherwise, it will.
	AosXmlTagPtr the_doc = ref_doc;
	u64 docid = 0;
	if (ref_docid != 0)
	{
		docid = AosXmlDoc::setDocidType(eAosAccessDoc, ref_docid);
	}
	else if (the_doc)
	{
		docid = AosXmlDoc::setDocidType(eAosAccessDoc, 
			ref_doc->getAttrU64(AOSTAG_DOCID, 0));
	}
	else
	{
		if (ref_objid == "")
		{
			return 0;
		}

		the_doc = getDoc(ref_objid, rdata);
		if (!the_doc)
		{
			return 0;
		}

		docid = AosXmlDoc::setDocidType(eAosAccessDoc, the_doc->getAttrU64(AOSTAG_DOCID, 0));
	}

	if (!docid)
	{
		return 0;
	}

	// Now 'docid' is the access record's docid.
	AosXmlTagPtr dd = getDoc(docid, "", rdata);
	if (dd) 
	{
		return AosAccessRcdMgr::getSelf()->getAccessRecord(dd, false, rdata);
	}

	// Did not find the access record. Check whether it needs to retrieve its parent's.
	if (!parent_flag) return 0;

	if (!the_doc && ref_docid)
	{
		the_doc = getDoc(ref_docid, "", rdata);
		if (!the_doc)
		{
			return 0;
		}
	}

	if (!ref_doc && ref_objid != "")
	{
		the_doc = getDoc(ref_objid, rdata);
		if (!the_doc)
		{
			return 0;
		}
	}

	if (!the_doc)
	{
		return 0;
	}

	AosAccessRcdPtr arcd = getAccessRcd2(rdata, the_doc, 0, "", false);
	if (arcd) arcd->setParentFlag(true);
	return arcd;
}


AosAccessRcdPtr
AosDocMgr1::getParentAR3(
		const AosRundataPtr &rdata,
		const u64 &ref_docid, 
		const OmnString &ref_objid)
{
	OmnString siteid = rdata->getSiteid();
	AosXmlTagPtr doc = getDoc(ref_docid, ref_objid, rdata);
	if (!doc)
	{
		return 0;
	}

	OmnString pctnr_objid = doc->getPrimaryContainer();
	if (pctnr_objid == "") 
	{
		return 0;
	}

	AosAccessRcdPtr acd = getAccessRcd2(rdata, 0, 0, pctnr_objid, false);
	if (acd) acd->setParentFlag(true);
	return acd;
}


AosUserAcctObjPtr
AosDocMgr1::getUserAcct(const u64 &docid, const AosRundataPtr &rdata)
{
	AosXmlTagPtr userdoc = getDoc(docid, "", rdata);
	if (!userdoc) return 0;
	return AosUserAcctMgr::getSelf()->getUserAcct(userdoc, rdata);
}
#endif
