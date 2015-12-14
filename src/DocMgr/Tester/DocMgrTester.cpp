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
//
// Modification History:
// 2013/05/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocMgr/Tester/DocMgrTester.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "DocMgr/DocMgr.h"
#include "Porting/GetTime.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/ThreadPool.h"
#include "Util/OmnNew.h"
#include "Util/Array3.h"
#include "XmlUtil/XmlTag.h"


OmnMutexPtr	AosDocMgrTester::smLock = OmnNew OmnMutex();
AosRundataPtr AosDocMgrTester::smRundata;
u32	AosDocMgrTester::smNumDocs;
vector<AosXmlTagPtr> AosDocMgrTester::smDocs;
vector<bool> AosDocMgrTester::smDocStatus;
u64 AosDocMgrTester::smDocid = 100;
AosDocMgrPtr AosDocMgrTester::smDocMgr;

static int64_t sgSeqno = 100;
static bool sgInited = false;

AosDocMgrTester::AosDocMgrTester()
:
OmnThrdShellProc("test"),
mId(0),
mMaxNewDocs(eMaxNewDocs),
mGetTries(10)
{
	init();
}


AosDocMgrTester::AosDocMgrTester(const int id)
:
OmnThrdShellProc("test"),
mId(id),
mMaxNewDocs(eMaxNewDocs)
{
	init();
}


AosDocMgrTester::~AosDocMgrTester()
{
}


bool 
AosDocMgrTester::init()
{
	if (sgInited) return true;

	sgInited = true;
	smDocMgr = AosDocMgr1::getSelf();
	smNumDocs = 10000;
	for (u32 i=0; i<smNumDocs; i++)
	{
		OmnString ss = "<doc seqno=\"";
		ss << i << "\" zky_docid=\"" << smDocid++ 
			<< "\" zky_objid=\"obj_" << i 
			<< "\"/>";
		AosXmlTagPtr doc = AosStr2Xml(smRundata, ss AosMemoryCheckerArgs);
		aos_assert_r(doc, false);
		doc->setAttr(AOSTAG_SITEID, rand());
		smDocs.push_back(doc);
		smDocStatus.push_back(false);
	}

	aos_assert_r(smNumDocs == smDocs.size(), false);
	smRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	smRundata->setSiteid(100);
	return true;
}


AosXmlTagPtr 
AosDocMgrTester::pickDoc(int &idx)
{
	smLock->lock();
	u32 size = smDocs.size();
	aos_assert_rl(size > 0, smLock, 0);
	idx = rand() % size; 
	AosXmlTagPtr doc = smDocs[idx];
	smLock->unlock();
	return doc;
}


bool 
AosDocMgrTester::start()
{
	return functionalTesting();
	// return threadTesting();
}


bool
AosDocMgrTester::functionalTesting()
{
	AOS_DECLARE_TIMESTAMPS(4, 100);

	OmnScreen << "-------------------------"
		<< lsTimestamps[0] << ":" 
		<< lsTimestamps[1] << ":" 
		<< lsTimestamps[2] << ":" 
		<< lsTimestamps[3] << endl;

	u64 tt1 = OmnGetTimestamp();
	int tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		AOS_PRINT_TIMESTAMP;

		if (i % 100 == 0) 
		{
			u64 tt2 = OmnGetTimestamp();
			cout << "Tries: " << i << ":" << tt2 - tt1 
				<< ". Num Cached: " << smDocMgr->getNumCached()
				<< ". Num New Objs: " << mNewDocs.size() << endl;
			tt1 = tt2;
		}

		testGetDoc();
		AOS_TIMESTAMP(0);
		aos_assert_r(smDocMgr->sanityCheck(), false);
		switch (rand() % 4)
		{
		case 0:
		case 2:
		case 3:
			 testAddDoc();
			 AOS_TIMESTAMP(1);
			 aos_assert_r(smDocMgr->sanityCheck(), false);
			 break;

		case 1:
			 testDeleteDoc();
			 AOS_TIMESTAMP(2);
			 aos_assert_r(smDocMgr->sanityCheck(), false);
			 break;
		
		default:
			 OmnAlarm << "internal error" << enderr;
			 break;
		}
		testGetDoc();
		AOS_TIMESTAMP(0);
		aos_assert_r(smDocMgr->sanityCheck(), false);
	}

	return true;
}


bool
AosDocMgrTester::testGetDoc()
{
	int tries = rand() % mGetTries;
	for (int i=0; i<tries; i++)
	{
		testGetOneDoc();
	}
	return true;
}


bool
AosDocMgrTester::testGetOneDoc()
{
	int nn = rand() % 2;
	AosXmlTagPtr doc;
	u64 docid;
	bool exist;
	u32 num_cached, siteid;
	int idx;
	switch (nn)
	{
	case 0:
		 // Pick a doc 
		 doc = pickDoc(idx);
		 aos_assert_r(doc, false);
		 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
		 aos_assert_r(siteid > 0, false);
		 smRundata->setSiteid(siteid);
		 exist = smDocStatus[idx];
		 num_cached = smDocMgr->getNumCached();

		 if (exist)
		 {
		 	 aos_assert_r(num_cached > 0, false);
		 	 doc = smDocMgr->getDocByDocid(docid, smRundata);
			 if (!doc)
			 {
				 // It is possible that the doc was aged out.
				 u32 num_cached = smDocMgr->getNumCached();
				 aos_assert_r(num_cached >= smDocMgr->getCacheSize() - 3, false);
			 }
			 else
			 {
		 	 	aos_assert_r(smDocMgr->getDocHead() == doc, false);
		 	 	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, false);
			 }
			 return true;
		 }

		 doc = smDocMgr->getDocByDocid(docid, smRundata);
		 aos_assert_r(!doc, false);
		 return true;

	case 1:
		 // Pick a new doc to get
		 if (mNewDocs.size() <= 0) return true;
		 nn = rand() % mNewDocs.size();
		 doc = mNewDocs[nn];
		 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		 exist = doc->getAttrBool("exist_flag", false);
		 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
		 smRundata->setSiteid(siteid);
		 aos_assert_r(siteid > 0, false);
		 num_cached = smDocMgr->getNumCached();

		 if (exist)
		 {
		 	 aos_assert_r(num_cached > 0, false);
		 	 AosXmlTagPtr doc1 = smDocMgr->getDocByDocid(docid, smRundata);
			 aos_assert_r(num_cached == smDocMgr->getNumCached(), false);
		 	 if (smDocMgr->getDocHead() != doc1)
			 {
		 	 	 doc1 = smDocMgr->getDocByDocid(docid, smRundata);
			 }
		 	 aos_assert_r(smDocMgr->getDocHead() == doc1, false);
		 	 return true;
		 }

		 doc = smDocMgr->getDocByDocid(docid, smRundata);
		 return true;
	
	case 2:
		 docid = rand() + smDocid;
		 smRundata->setSiteid(rand());
		 doc = smDocMgr->getDocByDocid(docid, smRundata);
		 aos_assert_r(!doc, false);
		 return true;

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDocMgrTester::testDeleteDoc()
{
	int nn = rand() % 3;
	u64 docid = 0;
	AosXmlTagPtr doc;
	bool exist;
	int idx;
	u32 num_cached, siteid;
	switch (nn)
	{
	case 0:
		 // Pick a doc to delete
		 doc = pickDoc(idx);
		 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
		 aos_assert_r(siteid > 0, false);
		 aos_assert_r(docid > 0, false);
		 smRundata->setSiteid(siteid);
		 exist = smDocStatus[idx];
		 num_cached = smDocMgr->getNumCached();
		 aos_assert_r(smDocMgr->deleteDoc(docid, smRundata), false);
		 aos_assert_r(!smDocMgr->getDocByDocid(docid, smRundata), false);
		 aos_assert_r(smDocMgr->getDocHead() != doc, false);
		 if (num_cached == smDocMgr->getNumCached())
		 {
			 // The doc was not removed. It is possible that the doc
			 // was just aged out from the cache.
		 	 aos_assert_r(num_cached+3 >= smDocMgr->getNumCached(), false);
		 }
		 else
		 {
		 	if (num_cached > 0 && exist) num_cached--;
		 	aos_assert_r(num_cached == smDocMgr->getNumCached(), false);
		 	smDocStatus[idx] = false;
		 }
		 return true;

	case 1:
		 // Pick a new doc to delete
		 if (mNewDocs.size() <= 0) return true;
		 nn = rand() % mNewDocs.size();
		 doc = mNewDocs[nn];
		 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		 exist = doc->getAttrBool("exist_flag", false);
		 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
		 aos_assert_r(siteid > 0, false);
		 smRundata->setSiteid(siteid);
		 num_cached = smDocMgr->getNumCached();
		 aos_assert_r(smDocMgr->deleteDoc(docid, smRundata), false);
		 aos_assert_r(!smDocMgr->getDocByDocid(docid, smRundata), false);
		 aos_assert_r(smDocMgr->getDocHead() != mNewDocs[nn], false);
		 mNewDocs[nn] = mNewDocs[mNewDocs.size()-1];
		 mNewDocs.pop_back();
		 return true;

	case 2:
		 // Pick a non-exist doc to delete
		 docid = rand() + smDocid;
		 siteid = rand();
		 smRundata->setSiteid(siteid);
		 num_cached = smDocMgr->getNumCached();
		 doc = smDocMgr->getDocHead();
		 aos_assert_r(smDocMgr->deleteDoc(docid, smRundata), false);
		 aos_assert_r(!smDocMgr->getDocByDocid(docid, smRundata), false);
		 aos_assert_r(smDocMgr->getDocHead() == doc, false);
		 aos_assert_r(num_cached == smDocMgr->getNumCached(), false);
		 return true;

	default:
		 OmnAlarm << "internal error" << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDocMgrTester::threadDeleteDoc()
{
	int nn = rand() % 3;
	u64 docid = 0;
	AosXmlTagPtr doc;
	int idx;
	u32 siteid;
	switch (nn)
	{
	case 0:
		 // Pick a doc to delete
		 doc = pickDoc(idx);
		 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
		 aos_assert_r(siteid > 0, false);
		 aos_assert_r(docid > 0, false);
		 smRundata->setSiteid(siteid);
		 aos_assert_r(smDocMgr->deleteDoc(docid, smRundata), false);
		 return true;

	case 1:
		 // Pick a new doc to delete
		 if (mNewDocs.size() <= 0) return true;
		 nn = rand() % mNewDocs.size();
		 doc = mNewDocs[nn];
		 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
		 aos_assert_r(siteid > 0, false);
		 smRundata->setSiteid(siteid);
		 aos_assert_r(smDocMgr->deleteDoc(docid, smRundata), false);
		 aos_assert_r(smDocMgr->getDocHead() != mNewDocs[nn], false);
		 mNewDocs[nn] = mNewDocs[mNewDocs.size()-1];
		 mNewDocs.pop_back();
		 return true;

	case 2:
		 // Pick a non-exist doc to delete
		 docid = rand() + smDocid;
		 siteid = rand();
		 smRundata->setSiteid(siteid);
		 doc = smDocMgr->getDocHead();
		 aos_assert_r(smDocMgr->deleteDoc(docid, smRundata), false);
		 return true;

	default:
		 OmnAlarm << "internal error" << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDocMgrTester::testAddDoc()
{
	int nn = rand() % 2;
	int add_copy = rand() % 2;
	if (nn)
	{
		// It picks a doc to add.
		int idx;
		AosXmlTagPtr doc = pickDoc(idx);
		bool status = smDocStatus[idx];
		if (status)
		{
			// The doc is already in the cache.
			// Need to determine whether to create a new one.
			if (rand() % 2)
			{
				// Need to create a new one.
				doc = doc->clone(AosMemoryCheckerArgsBegin);
				int mm = doc->getAttrInt("num_copies", 0);
				doc->setAttr("num_copies", ++mm);
			}
			
			u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
			aos_assert_r(docid > 0, false);
			u64 num_docs = smDocMgr->getNumCached();
			
			if (add_copy)
			{
				bool rslt = smDocMgr->addCopy(docid, doc, smRundata);
				aos_assert_r(rslt, false);
				doc = smDocMgr->getDocHead();
				u32 kk = smDocMgr->getNumCached();
				aos_assert_r(num_docs == kk, false);
				aos_assert_r(docid == doc->getAttrU64(AOSTAG_DOCID, 0), false);
			}
			else
			{
				bool rslt = smDocMgr->addDoc(doc, docid, smRundata);
				aos_assert_r(rslt, false);
				aos_assert_r(smDocMgr->getDocHead() == doc, false);
				u32 kk = smDocMgr->getNumCached();
				aos_assert_r(num_docs == kk, false);
			}
			return true;
		}
		
		// The doc is not in the cache yet.
		u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(docid > 0, false);
		u64 num_docs = smDocMgr->getNumCached();

		if (add_copy)
		{
			bool rslt = smDocMgr->addCopy(docid, doc, smRundata);
			aos_assert_r(rslt, false);
			doc = smDocMgr->getDocHead();
			if (num_docs < smDocMgr->getCacheSize()) num_docs++;
			aos_assert_r(num_docs == smDocMgr->getNumCached(), false);
			aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, false);
			smDocStatus[idx] = true;
		}
		else
		{
			bool rslt = smDocMgr->addDoc(doc, docid, smRundata);
			aos_assert_r(rslt, false);
			aos_assert_r(smDocMgr->getDocHead() == doc, false);
			if (num_docs < smDocMgr->getCacheSize()) num_docs++;
			aos_assert_r(num_docs == smDocMgr->getNumCached(), false);
			smDocStatus[idx] = true;
		}
		return true;
	}

	// Need to create a new doc
	if (mNewDocs.size() > mMaxNewDocs) return true;

	OmnString ss = "<doc/>";
	AosXmlTagPtr doc = AosStr2Xml(smRundata, ss AosMemoryCheckerArgs);
	mNewDocs.push_back(doc);
	u64 docid = smDocid++;
	doc->setAttr(AOSTAG_DOCID, docid);
	doc->setAttr("seqno", sgSeqno++);
	doc->setAttr(AOSTAG_SITEID, rand());
	u64 num_docs = smDocMgr->getNumCached();

	if (add_copy)
	{
		bool rslt = smDocMgr->addCopy(docid, doc, smRundata);
		aos_assert_r(rslt, false);
		doc = smDocMgr->getDocHead();
		if (num_docs < smDocMgr->getCacheSize()) num_docs++;
		aos_assert_r(num_docs == smDocMgr->getNumCached(), false);
		aos_assert_r(docid == doc->getAttrU64(AOSTAG_DOCID, 0), false);
	}
	else
	{
		bool rslt = smDocMgr->addDoc(doc, docid, smRundata);
		aos_assert_r(rslt, false);
		aos_assert_r(smDocMgr->getDocHead() == doc, false);
		if (num_docs < smDocMgr->getCacheSize()) num_docs++;
		aos_assert_r(num_docs == smDocMgr->getNumCached(), false);
	}
	doc->setAttr("exist_flag", "true");
	return true;
}


bool
AosDocMgrTester::threadAddDoc()
{
	int nn = rand() % 2;
	int add_copy = rand() % 2;
	if (nn)
	{
		// It picks a doc to add.
		int idx;
		AosXmlTagPtr doc = pickDoc(idx);

		// The doc is already in the cache.
		// Need to determine whether to create a new one.
		if (rand() % 2)
		{
			// Need to create a new one.
			doc = doc->clone(AosMemoryCheckerArgsBegin);
			int mm = doc->getAttrInt("num_copies", 0);
			doc->setAttr("num_copies", ++mm);
		}
		
		u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(docid > 0, false);
		
		if (add_copy)
		{
			bool rslt = smDocMgr->addCopy(docid, doc, smRundata);
			aos_assert_r(rslt, false);
			aos_assert_r(docid == doc->getAttrU64(AOSTAG_DOCID, 0), false);
		}
		else
		{
			bool rslt = smDocMgr->addDoc(doc, docid, smRundata);
			aos_assert_r(rslt, false);
		}
		return true;
	}

	// Need to create a new doc
	if (mNewDocs.size() > mMaxNewDocs) return true;

	OmnString ss = "<doc/>";
	AosXmlTagPtr doc = AosStr2Xml(smRundata, ss AosMemoryCheckerArgs);
	mNewDocs.push_back(doc);
	u64 docid = smDocid++;
	doc->setAttr(AOSTAG_DOCID, docid);
	doc->setAttr("seqno", sgSeqno++);
	doc->setAttr(AOSTAG_SITEID, rand());

	if (add_copy)
	{
		bool rslt = smDocMgr->addCopy(docid, doc, smRundata);
		aos_assert_r(rslt, false);
		aos_assert_r(docid == doc->getAttrU64(AOSTAG_DOCID, 0), false);
	}
	else
	{
		bool rslt = smDocMgr->addDoc(doc, docid, smRundata);
		aos_assert_r(rslt, false);
	}
	doc->setAttr("exist_flag", "true");
	return true;
}


bool
AosDocMgrTester::threadTesting()
{
	vector<OmnThrdShellProcPtr> runners;
	mNumRunners = 10;
	for (int i=0; i<mNumRunners; i++)
	{
		runners.push_back(OmnNew AosDocMgrTester(i));
	}

	OmnThreadPool pool("test");
	bool rslt = pool.procAsync(runners);
	aos_assert_r(rslt, false);
	return true;
}


bool		
AosDocMgrTester::run()
{
	// This function randomly picks a doc. It checks whether the
	// doc is in the cache. If yes, it should be able to retrieve
	// it from the cache.

	int idx = 0;
	u64 time1 = OmnGetTimestamp();
	while (1)
	{
		if (idx % 1000 == 0) 
		{
			u64 time2 = OmnGetTimestamp();
			cout << "Runner: " << mId << ":" << idx << ":" << time2 - time1
			<< ". Num Cached: " << smDocMgr->getNumCached() 
			<< ". Num Objs: " << mNewDocs.size() << endl;
			time1 = time2;
		}
		idx++;

		int nn = rand() % 2;
		switch (nn)
		{
		case 0: threadAddDoc(); break;
		case 1: threadDeleteDoc(); break;
		default:
				OmnAlarm << "internal error " << enderr;
				return false;
		}

		AosXmlTagPtr doc;
		if (rand() % 10 <= 2)
		{
			u64 docid = smDocid++;
			OmnString ss = "<doc seqno=\"";
			ss << docid << "\" zky_docid=\"" << docid 
				<< "\" zky_objid=\"obj_" << docid
				<< "\"/>";
			doc = AosStr2Xml(smRundata, ss AosMemoryCheckerArgs);
			aos_assert_r(doc, false);
			doc->setAttr(AOSTAG_SITEID, rand());
		}
		else
		{
			int idx;
			doc = pickDoc(idx);
		}
		aos_assert_r(doc, false);
		u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		AosXmlTagPtr dd = AosDocMgr1::getSelf()->getDocByDocid(docid, smRundata);

		bool rslt = AosDocMgr1::getSelf()->addDoc(doc, docid, smRundata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool		
AosDocMgrTester::procFinished()
{
	return true;
}

