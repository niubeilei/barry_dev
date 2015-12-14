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
#include "DocMgr/DocMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DocMgr/Ptrs.h"
#include "DocServer/DocSvr.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocServer.h"
#include "SEUtil/Objid.h"
#include "SEUtil/SeTypes.h"
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

static u64 sgHitCount = 0;
static u64 sgMissCount = 0;
static u64 sgErrorCount = 0;
static u64 sgReplaceCount = 0;
static u64 sgAgeOutCount = 0;
static OmnString sgLogs[2];
static int sgLogIdx = 0;
static int sgLogMaxLength = 300;

#define DocMgrLogAndCheck(opr, docid) \
	if (sgLogs[sgLogIdx].length() > sgLogMaxLength) \
	{ \
		sgLogIdx = 1 - sgLogIdx; \
		sgLogs[sgLogIdx] = ""; \
	} \
	sgLogs[sgLogIdx] << opr << ":" << __LINE__ << ":" << docid << ","

#define DocMgrLogOnly(opr, docid)\
	sgLogs[sgLogIdx] << opr << ":" << __LINE__ << ":" << docid << ","

static AosDocMgrPtr sgDocMgr;

AosDocMgr1::AosDocMgr1()
:
mLock(OmnNew OmnMutex()),
mDocHead(0),
mNumDocsCache(eMaxNumDocs2Cache)
{
	sgDocMgr = this;
}


AosDocMgr1::~AosDocMgr1()
{
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
	AosXmlTagPtr conf = config->getFirstChild("docmgr");
	if (!conf) return true;
	mNumDocsCache = conf->getAttrInt("cache_numdocs", eMaxNumDocs2Cache);
	if (mNumDocsCache <= 0 || mNumDocsCache > eMaxNumDocs2Cache)
	{
		OmnAlarm << "Invalid configuration: " << conf->toString() << enderr;
		mNumDocsCache = eMaxNumDocs2Cache;
	}
	return true;
}


AosXmlTagPtr 
AosDocMgr1::getDocByDocid(
		const OmnString &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid!="", NULL);
	u64 did;
	aos_assert_r(did = atoll(docid.data()), NULL);
	return getDocByDocid(did, rdata);
}


AosXmlTagPtr 
AosDocMgr1::getDocByDocid(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	// 1 Read doc from map
	//   if the doc exsit ,return doc ,put the doc to front of the link list 
	//   else retreive the doc from file, and put it to front of the link list
	//   if the link list if full, it will remove the end docs of the list 
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid, NULL);
	aos_assert_r(docid, NULL);
	mLock->lock();
	AosDocKey dockey(docid, siteid);
	mapitr_t itr = mDocMap.find(dockey);
	if (itr != mDocMap.end())
	{
		// This means that there are entries that match 'docid'. 
		AosXmlTagPtr node = itr->second;
		if (mDocMap.size() != 1)
		{
			// Move the doc to the front of the list. This will 
			// ensure that the doc will not be removed from 
			// the cache in the near future.
			moveNodeLocked(node);
		}

		if (node->getAttrU64(AOSTAG_DOCID, 0) != docid)
		{
			AosSetError(rdata, AosErrmsgId::eDocidMismatch) << docid
				<< ":" << node->getAttrU64(AOSTAG_DOCID, 0);
			OmnAlarm << "Docid mismatch: " << docid << ":"
				<< node->toString() << enderr;

			// Chen Ding, 2013/05/31
			node->resetLinkNode();

			mDocMap.erase(itr);
			sgErrorCount++;
			mLock->unlock();
			return 0;
		}
		
		sgHitCount++;
		mLock->unlock();
		return node;
	}

	sgMissCount++;
	mLock->unlock();
	return 0;
}


bool
AosDocMgr1::addDoc(
		const AosXmlTagPtr &doc, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(doc, rdata, false);
	aos_assert_rr(doc->isRootTag(), rdata, false);
	aos_assert_rr(!doc->isDeleted(), rdata, false);
	aos_assert_rr(docid > 0, rdata, false);
	aos_assert_rr(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, rdata, false);
	u32 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
	if (siteid == 0) siteid = rdata->getSiteid();
	aos_assert_rr(siteid > 0, rdata, false);

	// This function adds the doc 'doc' into the class. 
	// If the doc already exists, it replaces the doc.
	mLock->lock();
	DocMgrLogAndCheck("add", docid);

	AosDocKey dockey(docid, siteid);
	mapitr_t itr = mDocMap.find(dockey);
	if (itr != mDocMap.end())
	{
		// This means that the doc exists. 
		DocMgrLogOnly("exist", docid);
		sgHitCount++;
		AosXmlTagPtr node = itr->second; 
		if (node != doc)
		{
			// This is a new object for the same doc. Need to replace it.
			AosXmlTagPtr next = node->getNext();
			AosXmlTagPtr prev = node->getPrev();
			aos_assert_rl(prev, mLock, false);
			aos_assert_rl(next, mLock, false);
			doc->setNext(next);
			doc->setPrev(prev);
			prev->setNext(doc);
			next->setPrev(doc);
			itr->second = doc;
			sgReplaceCount++;
			if (node == mDocHead) mDocHead = doc;
		}

		if (mDocMap.size() > 1)
		{
			// Move the doc to the front of the list. This will 
			// ensure that the doc will not be removed from 
			// the cache in the near future.
			moveNodeLocked(doc);
		}
		mLock->unlock();
		return true;
	}

	sgMissCount++;

	// The doc does not exist. Add it.
	if (mDocMap.size() >= mNumDocsCache)
	{
		// Need to age out one doc
		AosXmlTagPtr node = mDocHead->getPrev();
		DocMgrLogOnly("aged", docid);
		aos_assert_r(node, false);
		u32 node_siteid = node->getAttrU32(AOSTAG_SITEID, 0);
		AosDocKey key(node->getAttrU64(AOSTAG_DOCID, 0), node_siteid); 
		mDocMap.erase(key);
		removeNodeLocked(node);

		// Chen Ding, 2013/05/31
		node->resetLinkNode();

		sgAgeOutCount++;
	}

	if (mDocMap.size() == 0) 
	{
		DocMgrLogOnly("first", docid);
		mDocMap[dockey] = doc;
		mDocHead = doc;
		doc->setPrev(doc);
		doc->setNext(doc);
		mLock->unlock();
		return true;
	}

	DocMgrLogOnly("added", docid);
	mDocMap[dockey] = doc;
	aos_assert_rl(mDocHead, mLock, false);

	// Place 'doc' to the front of the doc chains
	doc->setNext(mDocHead);
	doc->setPrev(mDocHead->getPrev());
	mDocHead->setPrev(doc);
	doc->getPrev()->setNext(doc);
	mDocHead = doc;
	mLock->unlock();
	return doc;
}


bool
AosDocMgr1::addCopy(
		const u64 &docid,
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	// This function puts the doc into the hash. If the doc
	// exists, it replaces it. 
	aos_assert_r(doc, false);
	AosXmlTagPtr doc_copy = doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(doc_copy, false);
	return addDoc(doc_copy, docid, rdata);
}


bool
AosDocMgr1::deleteDoc(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	DocMgrLogAndCheck("del", docid);
	u32 size = mDocMap.size();
	if (size == 0)
	{
		// The map is empty. Do nothing
		DocMgrLogOnly("empty", docid);
		aos_assert_rl(!mDocHead, mLock, false);
		mLock->unlock();
		return true;
	}

	aos_assert_rl(mDocHead, mLock, false);
	u32 siteid = rdata->getSiteid();
	AosDocKey dockey(docid, siteid);
	mapitr_t itr = mDocMap.find(dockey);
	if (itr == mDocMap.end())
	{
		// Not in the map. Do nothing.
		DocMgrLogOnly("notfd", docid);
		mLock->unlock();
		return true;
	}

	AosXmlTagPtr node = itr->second;
	mDocMap.erase(itr);
	if (size == 1)
	{
		// There are only one doc. 
		// Chen Ding, 2013/05/31
		node->resetLinkNode();

		DocMgrLogOnly("one", docid);
		aos_assert_rl(mDocHead == node, mLock, false);
		mDocHead = 0;
		aos_assert_rl(mDocMap.size() == 0, mLock, false);
		
		mLock->unlock();
		return true;
	}

	DocMgrLogOnly("removed", docid);
	bool rslt = removeNodeLocked(node);
	node->resetLinkNode();
	aos_assert_rl(rslt, mLock, false);
	aos_assert_rl(mDocMap.size() > 0, mLock, false);
	aos_assert_rl(mDocHead, mLock, false);
	mLock->unlock();
	return true;
}


bool
AosDocMgr1::removeNodeLocked(const AosXmlTagPtr &node)
{
	// It assumes mDocMap.size() > 1. 
	node->getNext()->setPrev(node->getPrev());
	node->getPrev()->setNext(node->getNext());
	if (node == mDocHead)
	{
		mDocHead = node->getNext();
	}
	return true;
}


bool
AosDocMgr1::moveNodeLocked(const AosXmlTagPtr &node)
{
	if (mDocHead == node) return true;	

	node->getPrev()->setNext(node->getNext());
	node->getNext()->setPrev(node->getPrev());
	node->setPrev(mDocHead->getPrev());
	mDocHead->getPrev()->setNext(node);
	node->setNext(mDocHead);
	mDocHead->setPrev(node);
	mDocHead = node;
	return true;
}


bool
AosDocMgr1::sanityCheck(const bool lock)
{
	if (lock) mLock->lock();
	if (mDocMap.size() == 0)
	{
		if (lock) 
		{
			aos_assert_rl(!mDocHead, mLock, false);
		}
		else
		{
			aos_assert_r(!mDocHead, false);
		}
		if (lock) mLock->unlock();
		return true;
	}

	if (lock)
	{
		aos_assert_rl(mDocHead, mLock, false);
	}
	else
	{
		aos_assert_r(mDocHead, false);
	}
	AosXmlTagPtr doc = mDocHead;
	u32 num_docs = 0;
	do
	{
		u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		u32 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
		AosDocKey dockey(docid, siteid);
		mapitr_t itr = mDocMap.find(dockey);
		if (lock)
		{
			aos_assert_rl(docid > 0, mLock, false)
			aos_assert_rl(siteid > 0, mLock, false)
			aos_assert_rl(itr != mDocMap.end(), mLock, false);
		}
		else
		{
			aos_assert_r(docid > 0, false)
			aos_assert_r(siteid > 0, false)
			aos_assert_r(itr != mDocMap.end(), false);
		}

		doc = doc->getNext();
		num_docs++;
	} while (num_docs < mDocMap.size() && doc != mDocHead);

	if (lock) 
	{
		aos_assert_rl(doc == mDocHead, mLock, false);
		aos_assert_rl(num_docs == mDocMap.size(), mLock, false);
		mLock->unlock();
	}
	else
	{
		aos_assert_r(doc == mDocHead, false);
		aos_assert_r(num_docs == mDocMap.size(), false);
	}
	return true;
}


OmnString
AosDocMgr1::getLogs()
{
	OmnString logs = sgLogs[sgLogIdx];
	logs << "---" << sgLogs[1-sgLogIdx];
	return logs;
}


AosXmlTagPtr	
AosDocMgr1::getLastDoc()
{
	return mDocHead->getPrev();
}


AosXmlTagPtr	
AosDocMgr1::getDocHead() const 
{
	return mDocHead;
}
