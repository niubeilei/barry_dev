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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "QueryTorturer/QueryTesterThrd.h"


#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "QueryTorturer/TesterQuery.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SecUtil/SecOpr.h"
#include "SecUtil/SecTypes.h"
#include "SEModules/SeRundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/String.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/SeXmlParser.h"


AosQueryTesterThrd::AosQueryTesterThrd(
		const AosQueryTesterMgrPtr &mgr, 
		const int tries, 
		const int thrdid)
:
mMgr(mgr),
mLock(OmnNew OmnMutex()),
mThreadId(thrdid),
mTries(tries),
mTotalSuccess(0),
mTotalFail(0),
//mCrtUserid(0),
//mCrtSiteid("100"),
//mCrtDocid(100),
//mNumObjid(0),
//mNumCtnrObjid(0),
//mNumUserName(0),
mRundata(OmnApp::getRundata())
{
}


AosQueryTesterThrd::~AosQueryTesterThrd()
{
}

bool    
AosQueryTesterThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
//	start();
	while (state == OmnThrdStatus::eActive)
	{
		mTester->basicTest();
		state = OmnThrdStatus::eIdle;
		return true;	
	}
	return true;
}

bool    
AosQueryTesterThrd::signal(const int threadLogicId)
{
	return true;
}


bool    
AosQueryTesterThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosQueryTesterThrd::start()
{
	mTester = OmnNew AosTesterQuery();
	mTester->setSiteId(mSiteId);
	mRundata->setSiteid(mSiteId);
	mTester->setRundata(mRundata);

	mTester->setSsid(mSsid);
	mTester->setUrldocid(mUrldocid);
	mTester->init();

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "Thrd", ++mThreadId, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}
/*
AosQueryTesterThrd::AosQueryTesterThrd(
		const AosQueryTesterMgrPtr &mgr, 
		const int tries, 
		const int thrdid)
:
mMgr(mgr),
mLock(OmnNew OmnMutex()),
mThreadId(thrdid),
mTries(tries),
mTotalSuccess(0),
mTotalFail(0),
mCrtUserid(0),
mCrtSiteid("100"),
mCrtDocid(100),
mNumObjid(0),
mNumCtnrObjid(0),
mNumUserName(0),
mRundata(OmnApp::getRundata())
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "Thrd", ++mThreadId, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosQueryTesterThrd::~AosQueryTesterThrd()
{
	map<u64, AosStContainer*>::iterator iterCtnr;
	for(iterCtnr = mCtnrs1.begin(); iterCtnr != mCtnrs1.end(); ++iterCtnr)
		delete iterCtnr->second;	

	map<u64, AosStUser*>::iterator iterUser;
	for(iterUser = mUsers1.begin(); iterUser != mUsers1.end(); ++iterUser)
		delete iterUser->second;	

	//hash_map<u64, AosStAccessRecord*, u64_hash, u64_cmp> ::iterator iterAR
	//hash_map<const OmnString, AosStAccessRecord*, Omn_Str_hash, compare_str> ::iterator iterAR
	//	= mLocalDidArcds.begin();
	AosU642StArcdItr_t	iterAR = mLocalDidArcds.begin();
	for(; iterAR!= mLocalDidArcds.end(); ++iterAR)
		delete iterAR->second;	

	AosStr2StdocItr_t  iterObjid
		= mDocsByObjid.begin();
	for(; iterObjid != mDocsByObjid.end(); ++iterObjid)
		delete iterObjid->second;	

	AosU642StdocItr_t  iterLocdid
		= mDocsByLocdid.begin();
	for(; iterLocdid !=  mDocsByLocdid.end(); ++iterLocdid)
		delete iterLocdid->second;	

	AosU642StArcdItr_t iterar
		= mOwnDocidArcds.begin();
	for(; iterar != mOwnDocidArcds.end(); ++iterar)
		delete iterar->second;	
}


bool
AosQueryTesterThrd::start()
{
	// It assumes the root account has been created. It logs in as 
	// the root and retrieves the following docs:
	//
	// 1. Root doc (by objid: AOSOBJID_ROOT): should fail
	// 2. Retrieve the root's parent doc, which should be "system" + siteid: should fail
	// 3. Retrieve the root's home container, which should be "home" + cid: should fail
	// 4. Login as root
	// 5. Root doc (by objid: AOSOBJID_ROOT): should success
	// 6. Retrieve the root's parent doc, which should be "system" + siteid: should success
	// 7. Retrieve the root's home container, which should be "home" + cid: should success
	
	// 1. Retrieve the root doc, should fail since the root doc
	// should have been configured that all its operatoins are
	// private.
	//mRundata->setLogging(true);
	AosXmlTagPtr rootdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
			mCrtSiteid, mCrtSsid, mMgr->getRootObjid(), true);
	aos_assert_r(rootdoc, false);

	// 2. Retrieve the root's parent doc: should success since its
	// parent container's accesses should be configured that reading
	// is public (by default).
	OmnString parent_objid = mMgr->getRootCtnr();
	aos_assert_r(parent_objid != "", false);
	AosXmlTagPtr parentdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
			mCrtSiteid, mCrtSsid, parent_objid, true);
	aos_assert_r(parentdoc, false);

	// 3. Retrieve the root's home container. Should success
	// since its home container's read access is configured to
	// public by default.
	OmnString home_objid = mMgr->getRootHomeCtnr();
	aos_assert_r(home_objid != "", false);
	AosXmlTagPtr homedoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
			mCrtSiteid, mCrtSsid, home_objid, true);
	aos_assert_r(homedoc, false);

	// 4. Login as root
	bool rslt = loginAsRoot();
	aos_assert_r(rslt, false);

	// 5. Retrieve the root doc: should success
	rootdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
			mCrtSiteid, mCrtSsid, mMgr->getRootObjid(), true);
	aos_assert_r(rootdoc, false);

	// 6. Retrieve the root's parent doc: should success
	parent_objid = rootdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(parent_objid != "", false);
	parentdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
			mCrtSiteid, mCrtSsid, parent_objid, true);
	aos_assert_r(parentdoc, false);

	aos_assert_r(addContainer(AosStContainer::eUserCtnr, parentdoc, mRundata), false);

	// 7. Retrieve the root's home container: should success
	home_objid = rootdoc->getAttrStr(AOSTAG_CTNR_HOME);
	aos_assert_r(home_objid != "", false);
	homedoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
			mCrtSiteid, mCrtSsid, home_objid, true);
	aos_assert_r(homedoc, false);
	aos_assert_r(addContainer(AosStContainer::eDocCtnr, homedoc, mRundata), false);

	aos_assert_r(addUser(rootdoc, mRundata), false);
	AosQueryTesterThrdPtr thisptr(this, false);
	aos_assert_r(AosCreateUserNewTester::createUserAccount(thisptr), false);
	return true;
}


bool    
AosQueryTesterThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	start();
	while (state == OmnThrdStatus::eActive)
	{
		basicTest();
		state = OmnThrdStatus::eIdle;
		return true;	
	}
	return true;
}



bool
AosQueryTesterThrd::basicTest()
{
	AosQueryTesterThrdPtr thrd(this, false);
	if (mTries == 0) mTries = 0xffffffff;

	AosSengTester::startTestStatic(thrd, mMgr);
	return true;
}


u64 
AosQueryTesterThrd::getNewDocId()
{
	u64 i =	mCrtDocid++;
	return i; 
}


int 
AosQueryTesterThrd::getThreadId() const
{	
	return mThreadId;
}

AosXmlTagPtr
AosQueryTesterThrd::retrieveAccessRcdOrParent(const u64 &svrdid)
{
	AosXmlTagPtr ard = AosSengAdmin::getSelf()->getAccessRcd(getSiteid(), 
			svrdid, mMgr->getRootSsid(), false, true);
	if (ard) return ard;
	return 0;
}

AosXmlTagPtr
AosQueryTesterThrd::retrieveAccessRcd(
		const u64 &svrdid, 
		const bool create_flag, 
		const bool parent_flag)
{
	// This function retrieves the access record from the server. If 
	// 'create_flag' is true, it will create the access record if it is not
	// there yet. If 'parent_flag' is true, it will retrieve its parent's
	// access record if it is not yet.
	AosXmlTagPtr ard = AosSengAdmin::getSelf()->getAccessRcd(getSiteid(), 
			svrdid, mMgr->getRootSsid(), create_flag, parent_flag);
	return ard;
}


AosStAccessRecord *
AosQueryTesterThrd::createAccessRecord(const u64 &locdid)
{
	// It retrieves the access record for the doc 'doc' (do not create
	// it yet)
	//AosStDoc *doc = mDocsByLocdid[locdid];
	AosStDoc *doc = getDocFromMemory(locdid);
	aos_assert_r(doc, 0);
	AosXmlTagPtr svr_ard = retrieveAccessRcd(doc->svrdid, false, false);
	aos_assert_r(svr_ard, 0);
	return createAccessRecord(svr_ard, locdid);
}


AosStAccessRecord *
AosQueryTesterThrd::createAccessRecord(const AosXmlTagPtr &svr_ard, const u64 &locdid)
{
	// It creates an access record for the doc. There are two maps
	// for access records, one maps local docid to access records (mLocalDidArcs)
	// and the other maps access record objids to access records (mOwnDocidArcds).
	AosStAccessRecord *ard = OmnNew AosStAccessRecord;
	ard->mOwnLocdid = locdid;
	ard->mOwnSvrdid = svr_ard->getAttrU64(AOSTAG_OWNER_DOCID, 0);
	ard->mSvrdid = svr_ard->getAttrU64(AOSTAG_DOCID, 0); 

	// Sync the accesses                                                    
	syncAccesses(ard, svr_ard);

	mLocalDidArcds[locdid] = ard;
	mOwnDocidArcds[ard->mOwnSvrdid] = ard;
	return ard;
}


bool
AosQueryTesterThrd::modifyAccessRecord(AosStAccessRecord *ard)
{
	const AosStDoc *doc = pickDoc(ard->mOwnLocdid);
	aos_assert_r(doc, false);
	aos_assert_r(doc->svrdid == ard->mOwnSvrdid, false);
	aos_assert_r(doc->locdid == ard->mOwnLocdid, false);

	if(mLocalDidArcds[ard->mOwnLocdid])
	{
		aos_assert_r(mLocalDidArcds[ard->mOwnLocdid]->mOwnLocdid == ard->mOwnLocdid, false);
		aos_assert_r(mLocalDidArcds[ard->mOwnLocdid]->mOwnSvrdid == ard->mOwnSvrdid, false);
		aos_assert_r(mLocalDidArcds[ard->mOwnLocdid]->mSvrdid == ard->mSvrdid, false);
	}

	u64 ownsvrdid = ard->mOwnSvrdid;
	aos_assert_r(ownsvrdid, false);
	if(mOwnDocidArcds[ownsvrdid])
	{
		aos_assert_r(mOwnDocidArcds[ownsvrdid]->mOwnLocdid == ard->mOwnLocdid, false);
		aos_assert_r(mOwnDocidArcds[ownsvrdid]->mOwnSvrdid == ownsvrdid, false);
		aos_assert_r(mOwnDocidArcds[ownsvrdid]->mSvrdid == ard->mSvrdid, false);
	}
	
	//aos_assert_r(ownsvrdid == createArdObjid(ard->mOwnSvrdid), false);

	mLocalDidArcds[ard->mOwnLocdid] = ard;
	mOwnDocidArcds[ownsvrdid] = ard;
	return true;

}


bool
AosQueryTesterThrd::syncAccesses(
		AosStAccessRecord *ard, 
		const AosXmlTagPtr &svr_ard) 
{
	for (int opr=AosSecOpr::eInvalid+1; opr<AosSecOpr::eMax; opr++)
	{
		OmnString accesses = svr_ard->getAttrStr(AosSecOpr::toString((AosSecOpr::E)opr));
		if (accesses == "") continue;
		OmnStrParser1 parser(accesses, ", ");
		OmnString word;
		while ((word = parser.nextWord()) != "")
		{
			aos_assert_r(ard->addAccess(
				(AosSecOpr::E)opr, AosAccessType::toEnum(word)), false);
		}
	}
	return true;
}

AosStContainer*
AosQueryTesterThrd::pickContainer(const AosStContainer::Type type)
{
	// It randomly picks a container.
	if (mCtnrs1.size() <= 0) return NULL;
		
	AosStContainer *cc;
	map<u64, AosStContainer*>::iterator iter = mCtnrs1.begin();
	int idx = (rand() % mCtnrs1.size());
	for (int j = 0; j<idx; j++) iter++;

	while (iter != mCtnrs1.end())
	{
		cc = iter->second;
		if (cc->type == type) return cc;
		iter++;
	}

	// Did not find it. Try it from the beginning. 
	iter = mCtnrs1.begin();
	for (int j = 0; j < idx; j++)
	{
		cc = iter->second;
		if (cc->type == type) return cc;
		iter++;
	}

	// Did not find any container whose type is 'type'
	return NULL;
}


AosStContainer*
AosQueryTesterThrd::pickContainer()
{
	AosStContainer::Type type = pickContainerType();
	return pickContainer(type);
}


AosStContainer*
AosQueryTesterThrd::getContainer(const u64 &locdid)
{
	map<u64, AosStContainer*>::iterator item = mCtnrs1.find(locdid);
	return item->second;
}


AosStContainer*
AosQueryTesterThrd::getContainer(const OmnString &objid)
{
	AosStDoc *doc = getDocFromMemory(objid);
	if (!doc) return 0;
	map<u64, AosStContainer*>::iterator item = mCtnrs1.find(doc->locdid);
	return item->second;
}


bool
AosQueryTesterThrd::addContainer(
		const OmnString &objid, 
		const AosStContainer::Type type, 
		const AosRundataPtr &rdata)
{
	// It checks whether the container is already added. If yes, 
	// it does nothing. 
	AosStContainer *ctnr = getContainer(objid);
	if (ctnr) return true;

	// The container has not been added to the system yet. 
	// Retrieve it from the server.
	aos_assert_r(mMgr, false);
	AosXmlTagPtr doc = mMgr->retrieveXmlDoc(mCrtSiteid, objid);
	aos_assert_r(doc, false);
	aos_assert_r(addContainer(type, doc, rdata), false);
	return true;
}


bool
AosQueryTesterThrd::addContainer(
		const AosStContainer::Type type, 
		const AosXmlTagPtr &ctnr, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(ctnr->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER, false);
	// 1. Save to the local
	u64 local_docid = ctnr->getAttrU64(AOSTEST_DOCID, AOS_INVDID);
	if (local_docid == AOS_INVDID)
	{
		local_docid = JudgeLocalDocid(ctnr);
		aos_assert_r(local_docid, false);
		ctnr->setAttr(AOSTEST_DOCID, local_docid);
		u64 server_docid = ctnr->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
		aos_assert_r(server_docid != AOS_INVDID, false);

		bool rslt = modifyObj(ctnr, rdata); 
		aos_assert_r(rslt, false);
	}

	aos_assert_r(local_docid, false);
	//AosStDoc *doc1 = mDocsByLocdid[local_docid];
	AosStDoc *doc1 = getDocFromMemory(local_docid);
	AosStContainer *cc = mCtnrs1[local_docid];
	if (doc1 && cc)
	{
		aos_assert_r(doc1->objid == cc->objid, false);
		aos_assert_r(doc1->objid == ctnr->getAttrStr(AOSTAG_OBJID), false);
		OmnScreen <<"Already exists!" << endl;
		return true;
	}
	aos_assert_r(!doc1 && !cc, false);

	aos_assert_r(saveNewDoc(local_docid, ctnr), false);

	// 2. Add to mCtnrs
	AosStContainer *container = OmnNew AosStContainer;
	container->objid = ctnr->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(container->objid != "", false);
	container->locdid = local_docid;
	container->svrdid = ctnr->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_r(container->svrdid != AOS_INVDID, false);
	container->type = type;
	//mCtnrs1[local_docid] = container;
	addContainer(local_docid, container);
aos_assert_r(getContainer(local_docid), false);

	// 3. A container is also a doc. Need to add to docs
	//addDoc(local_docid, ctnr);
	AosStDoc *doc = OmnNew AosStDoc;
	doc->locdid = local_docid;
	doc->svrdid = container->svrdid;
	doc->objid = container->objid;
	doc->creator = ctnr->getAttrStr(AOSTAG_CREATOR);
	aos_assert_r(doc->creator != "", false);

	OmnString ctnr_objid = ctnr->getAttrStr(AOSTAG_PARENTC);
	if (ctnr_objid != "")
	{
		//OmnStrParser1 parser(parents, AOS_CTNR_SEP, false, false);
		//OmnString ctnr_objid;
		//while ((ctnr_objid= parser.nextWord()) != "")
		//{
		//	AosStDoc *parentdoc = mDocsByObjid[ctnr_objid];
		//	if(parentdoc)doc->parents_locdid.push_back(parentdoc->locdid);
		//}
		//AosStDoc *parentdoc = mDocsByObjid[ctnr_objid];
		AosStDoc *parentdoc = mDocsByObjid[ctnr_objid];
		if(parentdoc) doc->parents_locdid.push_back(parentdoc->locdid);
	}

aos_assert_r(checkDocThreadId(ctnr), false);
	//mDocs.push_back(doc);
	addStDoc(doc);
	//mDocsByObjid[doc->objid] = doc;
	addDocsByObjid(doc->objid, doc);
	//mDocsByLocdid[local_docid] = doc;
	addDocsByLocdid(local_docid, doc);

	//mServerDidMap[doc->svrdid] = local_docid;
	addServerDidMap(doc->svrdid, local_docid);
	return true;
}


bool
AosQueryTesterThrd::addContainer(const u64 &docid, AosStContainer *ctnr)
{
	aos_assert_r(ctnr, false);
	mCtnrs1[docid] = ctnr;
	return true;
}


AosStUser*
AosQueryTesterThrd::getUser(const u64 &user_localid) 
{
	return mUsers1[user_localid];
}


AosStUser*
AosQueryTesterThrd::pickUser() 
{
	if (mUsers1.size() <= 0) return NULL;
	while(1)
	{
		map<u64, AosStUser*>::iterator iter = mUsers1.begin();
		int i = (rand()%mUsers1.size());
		for(int j = 0; j < i; j++)
		{
			if (iter == mUsers1.end()) break;
			++iter;
		}
		if (iter->second && iter != mUsers1.end()) 	return iter->second;
	}
	return NULL;
}


u64
AosQueryTesterThrd::getTotalUser() 
{
	return mUsers1.size();
}


const AosStDoc*
AosQueryTesterThrd::pickDoc(const bool own_doc) 
{
	// It randomly picks a doc. If 'own_doc' is true, and if the user
	// has logged in, it will pick a doc that was created by that
	// user. 
	if (getStDocSize() == 0) return 0;

	if (own_doc && mCrtCid != "")
	{
		// Pick a doc whose creator is mCrtCid
		int tries = 10;
		while (tries--)
		{
			int i = rand() %getStDocSize(); 
			if (getStDoc(i)->creator == mCrtCid) 
			{
				return getStDoc(i);
			}
		}
	}

	// Either it is not to pick an own doc or it failed picking 
	// own doc.
	int i = rand() % getStDocSize(); 
	return getStDoc(i);
}


AosStDoc*
AosQueryTesterThrd::getDocFromMemory(const u64 &local_docid)
{
	return mDocsByLocdid[local_docid];
}


AosStDoc*		
AosQueryTesterThrd::getDocFromMemory(const OmnString &objid) 
{
	return mDocsByObjid[objid];
}


bool
AosQueryTesterThrd::removeDoc(
				const u64 &local_docid,
				const OmnString objid,
				const u64 &server_docid)
{
	// 1. mDocs
	vector<AosStDoc*>::iterator it = mDocs.begin();
	vector<AosStDoc*>::iterator it1 = mDocs.begin();
	int i = 0;	
	bool found = false;
	for (; it!= mDocs.end(); ++it)
	{
		AosStDoc *doc = *it;
		aos_assert_r(doc, false);
		if (doc->locdid == local_docid)
		{
			it1 = it;
			aos_assert_r(mDocs[i]->locdid == local_docid, false);
			it = mDocs.erase(it); 
			if (it1 != mDocs.end())
			{
				AosStDoc *test = *it1;
				aos_assert_r(test->locdid!= local_docid, false);
				aos_assert_r(mDocs[i]->locdid != local_docid, false);
			}
			found = true;
			break;
		}
		i++;
	}
	aos_assert_r(found, false);

	//2.mDocsByObjid
	hash_map<const OmnString, AosStDoc*, Omn_Str_hash, compare_str>
		::iterator iter = mDocsByObjid.begin(); 
	//empty() is null, return true;
	iter = mDocsByObjid.find(objid);
	if (iter!= mDocsByObjid.end())
	{
		if(!mDocsByObjid.empty())	
		{
			mDocsByObjid.erase(objid);
		}
	}
	
	//3.mDocsByLocdid 
	//hash_map<u64, AosStDoc*, u64_hash, u64_cmp>
	//hash_map<const OmnString, AosStDoc*, Omn_Str_hash, compare_str>
	//	::iterator iter1 = mDocsByLocdid.begin();
	AosU642StdocItr_t::iterator iter1 = mDocsByLocdid.begin();
	iter1 = mDocsByLocdid.find(local_docid);
	if (iter1 != mDocsByLocdid.end())
	{
		if (!mDocsByLocdid.empty())
		{
			mDocsByLocdid.erase(local_docid);
		}
	}

	//4.mServerDidMap
	//hash_map<const OmnString, u64, Omn_Str_hash, compare_str>
	//	::iterator iter2 = mServerDidMap.begin();
	AosU642U64Itr_t1::iterator iter2 = mServerDidMap.begin();
	iter2 = mServerDidMap.find(server_docid);
	if (iter2 != mServerDidMap.end())
	{
		if (!mServerDidMap.empty())
		{
			mServerDidMap.erase(server_docid);
		}
	}
	return true;
}


int
AosQueryTesterThrd::getStDocSize()
{
	return mDocs.size();
}


const AosStDoc*
AosQueryTesterThrd::pickByObjid(const OmnString &objid)
{
	const AosStDoc *doc = mDocsByObjid[objid];
	return doc;
}


u64
AosQueryTesterThrd::getServeridByLocalid(const OmnString &server_docid)
{
	return mServerDidMap[atoll(server_docid)]; 
}


u64 
AosQueryTesterThrd::getLocalidByCid(const OmnString  &cid)
{
	return mUserCidMap[cid];
}


bool
AosQueryTesterThrd::addDoc(const u64 &local_docid, const AosXmlTagPtr &xmldoc)
{
	aos_assert_r(local_docid > 0, false);
	aos_assert_r(xmldoc, false);
	
	//AosStDoc *doc = mDocsByLocdid[local_docid];
	AosStDoc *doc = getDocFromMemory(local_docid);
	aos_assert_r(!doc, false);

	doc = OmnNew AosStDoc;
	doc->locdid = local_docid;
	doc->svrdid = xmldoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_r(doc->svrdid != AOS_INVDID, false);

	doc->objid = xmldoc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(doc->objid != "", false);

	doc->creator = xmldoc->getAttrStr(AOSTAG_CREATOR);
	aos_assert_r(doc->creator != "", false);

	OmnString ctnr_objid = xmldoc->getAttrStr(AOSTAG_PARENTC);
	if (ctnr_objid != "")
	{
		//OmnStrParser1 parser(parents, AOS_CTNR_SEP, false, false);
		//OmnString ctnr_objid;
		//while ((ctnr_objid= parser.nextWord()) != "")
		//{
		//	AosStDoc *parentdoc = mDocsByObjid[ctnr_objid];
		//	aos_assert_r(parentdoc, false);
		//	doc->parents_locdid.push_back(parentdoc->locdid);
		//}
		//AosStDoc *parentdoc = mDocsByObjid[ctnr_objid];
		const AosStDoc *parentdoc = pickByObjid(ctnr_objid);
		aos_assert_r(parentdoc, false);
		doc->parents_locdid.push_back(parentdoc->locdid);
	}
aos_assert_r(checkDocThreadId(xmldoc), false);
	//mDocs.push_back(doc);
	addStDoc(doc);
	addDocsByObjid(doc->objid, doc);
	//mDocsByLocdid[local_docid] = doc;
	addDocsByLocdid(local_docid, doc);

	//mServerDidMap[doc->svrdid] = local_docid;
	addServerDidMap(doc->svrdid, local_docid);

	aos_assert_r(saveNewDoc(local_docid, xmldoc), false);
	return true;
}


bool
AosQueryTesterThrd::modifyDocToMemory(const u64 &local_docid, const AosXmlTagPtr &xmldoc)
{
	aos_assert_r(local_docid > 0, false);
	aos_assert_r(xmldoc, false);

	//AosStDoc *doc = mDocsByLocdid[local_docid];
	AosStDoc *doc = getDocFromMemory(local_docid);
	aos_assert_r(doc, false);

	doc = OmnNew AosStDoc;
	doc->locdid = local_docid;
	doc->svrdid = xmldoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_r(doc->svrdid != AOS_INVDID, false);

	doc->objid = xmldoc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(doc->objid != "", false);

	doc->creator = xmldoc->getAttrStr(AOSTAG_CREATOR);
	aos_assert_r(doc->creator != "", false);

	OmnString ctnr_objid = xmldoc->getAttrStr(AOSTAG_PARENTC);
	if (ctnr_objid != "")
	{
		//OmnStrParser1 parser(parents, AOS_CTNR_SEP, false, false);
		//OmnString ctnr_objid;
		//while ((ctnr_objid= parser.nextWord()) != "")
		//{
		//	AosStDoc *parentdoc = mDocsByObjid[ctnr_objid];
		//	aos_assert_r(parentdoc, false);
		//	doc->parents_locdid.push_back(parentdoc->locdid);
		//}
		//AosStDoc *parentdoc = mDocsByObjid[ctnr_objid];
		const AosStDoc *parentdoc = pickByObjid(ctnr_objid);
		aos_assert_r(parentdoc, false);
		doc->parents_locdid.push_back(parentdoc->locdid);
	}

	aos_assert_r(modifyVector(doc), false);

	//mDocsByObjid[doc->objid] = doc;
	addDocsByObjid(doc->objid, doc);
	//mDocsByLocdid[local_docid] = doc;
	addDocsByLocdid(local_docid, doc);

	//mServerDidMap[doc->svrdid] = local_docid;
	addServerDidMap(doc->svrdid, local_docid);

	aos_assert_r(modifyDocToFile(local_docid, xmldoc), false);
	return true;
}

bool
AosQueryTesterThrd::modifyVector(AosStDoc *stdoc)
{
	u64 local_docid = stdoc->locdid;
	vector<AosStDoc*>::iterator it = mDocs.begin();
	bool found = false;
	for (; it!= mDocs.end(); ++it)
	{
		AosStDoc *doc = *it;
		aos_assert_r(doc, false);
		if (doc->locdid == local_docid)
		{
			*it = stdoc;
			found = true;
			break;
		}
	}
	aos_assert_r(found, false);
	return true;
}


bool
AosQueryTesterThrd::checkCreate(const OmnString &container_objid)
{
	// 1. Retrieve the container's access record. 
	// 2. Get accesses from the access record
	//
	return true;
}

AosXmlTagPtr
AosQueryTesterThrd::createDoc()
{
	int depth = rand() % eMaxDepth;
	AosTestXmlDoc xml("", depth);
	AosXmlTagPtr doc = xml.createNewDoc();
	aos_assert_r(doc, 0);
	return doc;
}


AosXmlTagPtr
AosQueryTesterThrd::readLocalDocFromFile(const u64 &ldocid)
{
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	AosXmlTagPtr doc;
	int tid = getThreadId();
	ff->readDoc(tid, ldocid, doc);
	return doc;
}


bool 
AosQueryTesterThrd::saveNewDoc(const u64 local_docid, const AosXmlTagPtr &doc)
{
	aos_assert_r(local_docid, false);
	aos_assert_r(doc, false);
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	aos_assert_r(ff, false);
	int tid = getThreadId();
	OmnString data = doc->toString();
	ff->createDoc(tid, local_docid, data);
	aos_assert_r(checkSaveNewDoc (local_docid), false);
	return true;
}


bool
AosQueryTesterThrd::modifyDocToFile(const u64 local_docid, const AosXmlTagPtr &doc)
{
	aos_assert_r(local_docid, false);
	aos_assert_r(doc, false);
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	aos_assert_r(ff, false);
	int tid = getThreadId();
	ff->modifyDoc(tid, local_docid, doc->toString());
	aos_assert_r(checkSaveNewDoc(local_docid), false);
	return true;
}


bool
AosQueryTesterThrd::removeDocToFile(const u64 local_docid)
{
	aos_assert_r(local_docid, false);
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	aos_assert_r(ff, false);
	int tid = getThreadId();
	aos_assert_r(ff->removeDoc(tid, local_docid), false);
	return true;
}


bool
AosQueryTesterThrd::isDeleteToFile(const u64 local_docid)
{
	aos_assert_r(local_docid, false);
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	aos_assert_r(ff, false);
	int tid = getThreadId();
	if (!ff->isDocDeleted(tid, local_docid)) return false;
	return true;
}


bool
AosQueryTesterThrd::checkSaveNewDoc(const u64 local_docid)
{
	AosXmlTagPtr doc = readLocalDocFromFile(local_docid);
	aos_assert_r (doc, false);
	return true;
}


OmnString
AosQueryTesterThrd::pickNewObjid()
{
	// It generates a new objid, which is in the format:
	// 		tid_sengt_xxx
	int tid = getThreadId();
	aos_assert_r(tid == mThreadId, "");
	OmnString objid;
	objid << tid << "_" << "sengt" << mNumObjid;
	AosXmlTagPtr xml = retrieveDocByObjid(objid);
	aos_assert_r(!xml, "");
	mNumObjid ++;
	return objid;
}


OmnString
AosQueryTesterThrd::pickUsedObjid()
{
	const AosStDoc *doc = pickDoc();
	aos_assert_r(doc, "");
	OmnString objid = doc->objid;
	AosXmlTagPtr xml = mMgr->retrieveXmlDoc(getSiteid(), objid);
	aos_assert_r(xml, "");
	return objid;
}


AosStContainer*
AosQueryTesterThrd::pickUserContainer()
{
	AosStContainer::Type type = AosStContainer::eUserCtnr;
	AosStContainer *container = pickContainer(type);
	return container;
}	


const AosStUser*
AosQueryTesterThrd::pickValidUser(const u64 &ctnr_docid) 
{
	AosStContainer *ctnr = getContainer(ctnr_docid);
	if (ctnr->members.size() != 0)
	{
		int i = rand() % ctnr->members.size();
		u64 udocid = ctnr->members[i];
		return mUsers1[udocid];
	}
	return NULL;
}

OmnString
AosQueryTesterThrd::createInvalidUsername()
{
	// All valid username starts with the thread id.
	// As long as the first letter is not a digit, it is an invalid
	// username.
	int guard = 0;
	while (guard++ < 10)
	{
		OmnString username = OmnRandom::nextLetterStr(
				AosLoginTester::eMinUsernameLen, 
				AosLoginTester::eMaxUsernameLen);
		if (username.length() > 0)
		{
			char *data = (char *)username.data();
			if (data[0] >= '0' && data[0] <= '9')
			{
				data[0] = 'k';
			}
			return username;
		}
	}
	OmnShouldNeverComeHere;
	return "dqertkadjat";
}
	

OmnString
AosQueryTesterThrd::pickNewUsername()
{
	int tid = getThreadId(); 
	OmnString username;
	username << tid << "_username" << getTotalUser();
	return username;
}


AosXmlTagPtr 
AosQueryTesterThrd::retrieveDoc(const u64 &svrdid)
{
	OmnString siteid = getSiteid();
	OmnString ssid = getCrtSsid();
	AosXmlTagPtr xml= AosSengAdmin::getSelf()->retrieveDocByDocid(
		siteid, ssid, svrdid, true);
	return xml;
}


AosXmlTagPtr
AosQueryTesterThrd::retrieveDocByObjid(const OmnString &objid)
{
	aos_assert_r(objid != "", false);
	OmnString siteid = getSiteid();
	OmnString ssid = getCrtSsid();
	AosXmlTagPtr xml = AosSengAdmin::getSelf()->retrieveDocByObjid(
		siteid, ssid, objid, true);
	//aos_assert_r(xml, 0);
	return xml;
}


void
AosQueryTesterThrd::setCtnrMembers(AosStContainer *ctnr, const u64 &ldocid)
{
	ctnr->members.push_back(ldocid);
}


AosStContainer::Type
AosQueryTesterThrd::pickContainerType()
{
	int i = rand()%2;
	AosStContainer::Type type;
	if (i)
	{
		type = AosStContainer::eDocCtnr;
	}
	else
	{
		type = AosStContainer::eUserCtnr;
	}
	return type;
}


void
AosQueryTesterThrd::setSession(
		const OmnString &ssid, 
		const u64 &userid, 
		const AosXmlTagPtr &userdoc)
{
	mCrtSsid = ssid;
	mCrtUserid = userid;
	mCrtUserdoc = userdoc;
	aos_assert(userdoc);
	mCrtCid = userdoc->getAttrStr(AOSTAG_CLOUDID);

	if (mCrtCid == mMgr->getRootCid())
	{
		mMgr->setRootSsid(ssid);
	}
}


void
AosQueryTesterThrd::resetSession()
{
	mCrtSsid = "";
	mCrtUserid = 0;
	mCrtUserdoc = 0;
	mCrtCid = "";
}


bool			
AosQueryTesterThrd::isUserValid(const AosStContainer *container, const u64 &user_docid)
{
	aos_assert_r(container, false);
	for (u32 i=0; i<container->members.size(); i++)
	{
		if (user_docid == container->members[i]) return true;
	}
	return false;
}


u64
AosQueryTesterThrd::pickUser(const AosStContainer *container)
{
	aos_assert_r(container, 0);
	if (container->members.size() == 0) return AOS_INVDID;
	int idx = rand() % container->members.size();
	return container->members[idx];
}


bool
AosQueryTesterThrd::addUser(const AosXmlTagPtr &userdoc, const AosRundataPtr &rdata)
{
	u64 local_docid = userdoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID);
	if (local_docid == AOS_INVDID)
	{
		local_docid = JudgeLocalDocid(userdoc);
		aos_assert_r(local_docid, false);
		userdoc->setAttr(AOSTEST_DOCID, local_docid);
		u64 server_docid = userdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
		aos_assert_r(server_docid!=AOS_INVDID, false);

		bool rslt = modifyObj(userdoc, rdata); 
		if (!rslt)
		{
			OmnAlarm << "Failed to modify doc: " << mRundata->getErrmsg() << enderr;
			return false;
		}
	}
	return addUser(local_docid, userdoc);
}


bool
AosQueryTesterThrd::addUser(
		const u64 &local_docid,
		const AosXmlTagPtr &userdoc)
{
	aos_assert_r(local_docid > 0, false);
	aos_assert_r(userdoc, false);

	aos_assert_r(addDoc(local_docid, userdoc), false);
	
	aos_assert_r(userdoc->getAttrStr(AOSTAG_CLOUDID)!="", false);

	AosStUser *usr = OmnNew AosStUser;
	usr->locdid = local_docid;
	aos_assert_r(userdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID)!= AOS_INVDID, false);
	usr->svrdid = userdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);

	aos_assert_r(userdoc->getAttrStr(AOSTAG_CLOUDID)!="", false);
	usr->cid = userdoc->getAttrStr(AOSTAG_CLOUDID);

	aos_assert_r(usr->svrdid != AOS_INVDID, false);

	aos_assert_r(userdoc->getAttrStr(AOSTAG_OBJID) != "", false);
	usr->objid = userdoc->getAttrStr(AOSTAG_OBJID);

	aos_assert_r(userdoc->getAttrStr(AOSTAG_USERNAME) != "", false);
	usr->username = userdoc->getAttrStr(AOSTAG_USERNAME);

	AosXmlTagPtr node = userdoc->getFirstChild(AOSTAG_PASSWD);
	aos_assert_r(node, false);
	aos_assert_r(node->getNodeText()!="", false);
	usr->passwd = node->getNodeText();

	OmnString pctnr = userdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(pctnr != "", false);
	//AosStDoc *parentdoc = mDocsByObjid[pctnr];
	const AosStDoc *parentdoc = pickByObjid(pctnr);
	aos_assert_r(parentdoc, false);
	aos_assert_r(parentdoc->locdid, false);
//AosStDoc *pd = mDocsByLocdid[parentdoc->locdid];
AosStDoc *pd = getDocFromMemory(parentdoc->locdid);
aos_assert_r(pd, false);
	usr->parent_locdid = parentdoc->locdid;

	// Find the container
	u64 pt_locdid = parentdoc->locdid;
	map<u64, AosStContainer*>::iterator item = mCtnrs1.find(pt_locdid);
	AosStContainer *ctnr = item->second;
	aos_assert_r(ctnr, false);
	aos_assert_r(!userInContainer(ctnr, local_docid), false);
	ctnr->members.push_back(local_docid);
aos_assert_r(checkDocThreadId(userdoc), false);
	usr->groups = userdoc->getAttrStr(AOSOTYPE_GROUP,"");
	mUsers1[local_docid] = usr;
aos_assert_r(getUser(local_docid), false);
	//u64 cid = atoll(usr->cid.data());
	//aos_assert_r(cid, false);
	mUserCidMap[usr->cid] = local_docid;
	return true;
}


bool
AosQueryTesterThrd::userInContainer(const AosStContainer *ctnr, const u64 &user_locdid)
{
	aos_assert_r(ctnr, false);
	for (u32 i=0; i<ctnr->members.size(); i++)
	{
		if (ctnr->members[i] == user_locdid) return true;
	}

	return false;
}




OmnString
AosQueryTesterThrd::getRootSsid()
{
	return mMgr->getRootSsid();
}


bool
AosQueryTesterThrd::loginAsRoot()
{
	//multi-Thread	
	OmnString ssid = mMgr->getRootSsid();
	u64 userid = mMgr->getRootDocid();
	AosXmlTagPtr userdoc = mMgr->getRootUserDoc();
	setSession(ssid, userid, userdoc);
	return true;
}

OmnString
AosQueryTesterThrd::pickNewCtnrObjid()
{
	// It generates a new objid, which is in the format:
	// 		tid_sengt_xxx
	int tid = getThreadId();
	aos_assert_r(tid == mThreadId, "");
	OmnString objid;
	objid <<tid<<"_"<<"ctnr"<<mNumCtnrObjid;
	AosXmlTagPtr xml= retrieveDocByObjid(objid);
	aos_assert_r(!xml, "");
	mNumCtnrObjid++;
	return objid;
//	OmnString cid = getCrtCid();
//	int tid = getThreadId();
//	OmnString name, objid, r;
//	const AosStDoc *doc, *doc1;
//	do
//	{
//		name = "";
//		name << tid << "_ctnr" << getTotalCtr() << r;
//		doc = pickByObjid(name);
//		if (!doc)
//		{
//			objid << name <<"."<<cid;
//			doc1 = pickByObjid(objid);
//		}
//		r << rand();
//	}while(doc ||doc1);
//
//	aos_assert_r(name!="", "");
//	return name;
}


OmnString
AosQueryTesterThrd::getNewUserName()
{
	// It generates a new username, which is in the format:
	// 		tid_user_xxx
	int tid = getThreadId();
	aos_assert_r(tid == mThreadId, "");
	OmnString username;
	username << tid << "_" << "user" << mNumUserName;
	mNumUserName ++;
	return username;
}


AosStAccessRecord*
AosQueryTesterThrd::getLocalAccessRecord(const u64 &local_docid)
{	
	AosStAccessRecord* ar = mLocalDidArcds[local_docid]; 
	if (ar) aos_assert_r(ar->mOwnLocdid == local_docid, false);
	return ar;
}



bool
AosQueryTesterThrd::modifyObj(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata,
		const bool check_error)
{
	aos_assert_r(doc, false);
	OmnString data = doc->toString();
	bool rslt =  AosSengAdmin::getSelf()->sendModifyReq(
			getSiteid(), getCrtSsid(), data, rdata, check_error);
	return rslt;
}

bool
AosQueryTesterThrd::modifyARToServer(const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid!="", false);

	OmnString ssid = mMgr->getRootSsid();
	//bool rslt =  AosSengAdmin::getSelf()->createAccessRcd(
	//		getSiteid(), doc, objid, ssid);
	//return rslt;
	return true;
}


bool
AosQueryTesterThrd::randomLogin()
{
 	AosStUser *user;
	int count = 10;
	while(count--)
	{
		user = pickUser();
		aos_assert_r(user, false);
		if (user->cid != mMgr->getRootCid()) break;
	}
	aos_assert_r(user, false);

	aos_assert_r(user->parent_locdid, false);
	//AosStDoc *doc = mDocsByLocdid[user->parent_locdid];
	AosStDoc *doc = getDocFromMemory(user->parent_locdid);
	aos_assert_r(doc, false);

	OmnString ctnr_objid = doc->objid;
	aos_assert_r(ctnr_objid !="", false);

	OmnString ssid; 
	u64 userid;
	AosXmlTagPtr userdoc;
	bool success = AosSengAdmin::getSelf()->login(user->username, user->passwd,
							ctnr_objid, ssid, userid, userdoc, getSiteid(), user->cid);

	aos_assert_r(success, false);
	aos_assert_r(ssid, false);
	aos_assert_r(userdoc, false);
	setSession(ssid, userid, userdoc);
	return true;
}


bool
AosQueryTesterThrd::logout()
{
	mCrtSsid = "";
	mCrtUserid = 0;
	mCrtUserdoc = 0;
	mCrtCid =""; 
	return true;
}


bool
AosQueryTesterThrd::login(const OmnString &cid)
{
	// This function logins the user 'cid'. 
	
	// 1. Retrieve local user record
	aos_assert_r(cid !="", false);
	u64 local_docid = getLocalidByCid(cid);
	aos_assert_r(local_docid, false);
	AosStUser *user = getUser(local_docid);
	aos_assert_r(user, false);
	
	// 2. Retrieve its parent local doc 
	//AosStDoc *parent_doc = mDocsByLocdid[user->parent_locdid];
	AosStDoc *parent_doc = getDocFromMemory(user->parent_locdid);
	aos_assert_r(parent_doc, false);
	OmnString ctnr_objid = parent_doc->objid;
	aos_assert_r(ctnr_objid !="", false);

	aos_assert_r(user->cid == cid, false);

	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	OmnString username ="";
	bool success = AosSengAdmin::getSelf()->login(username, user->passwd,
							ctnr_objid, ssid, userid, userdoc, getSiteid(), user->cid);

	aos_assert_r(success, false);
	aos_assert_r(ssid, false);
	aos_assert_r(userdoc, false);
	setSession(ssid, userid, userdoc);
	return true;
}


bool
AosQueryTesterThrd::deleteDoc(const AosStDoc *doc1)
{
	// This function deletes the doc 'doc1' from the server.
	aos_assert_r(doc1->svrdid, false);
	OmnString server_docid;
	server_docid  << doc1->svrdid;
	bool rslt =  AosSengAdmin::getSelf()->sendDeleteReq(
			getSiteid(), getCrtSsid(), server_docid);
	if (rslt)
	{
		AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByDocid(
				 getSiteid(), getCrtSsid(), doc1->svrdid, true);
		aos_assert_r(!doc, false);
		aos_assert_r(removeDocToFile(doc1->locdid), false);
		aos_assert_r(isDeleteToFile(doc1->locdid),false);
		aos_assert_r(removeDoc(doc1->locdid, doc1->objid, doc1->svrdid), false);
		aos_assert_r(!pickDoc(doc1->locdid), false);
		aos_assert_r(!pickByObjid(doc1->objid), false);
		aos_assert_r(getServeridByLocalid(server_docid) ==0, false);
	}
	return rslt;
}


bool
AosQueryTesterThrd::isRoot()
{
	if (mCrtSsid == "") return false;
	aos_assert_r(mCrtUserdoc && mCrtUserdoc->getAttrStr(AOSTAG_CLOUDID, "") == mCrtCid, false);
	aos_assert_r(mCrtUserdoc->getAttrU64(AOSTAG_DOCID, 0) == mCrtUserid, false);
	OmnString name = mCrtUserdoc->getAttrStr("zky_usrtp", "");
	aos_assert_r(name != "", false);
	if (name == "zky_root") 
	{
		return true;
	}

	return false;
}


OmnString
AosQueryTesterThrd::createArdObjid(const u64 docid)
{
	OmnString objid = AOSOBJIDPRE_ACCESS_RCD;
	objid << "_" << docid;
	return objid;
}


u64
AosQueryTesterThrd::JudgeLocalDocid(const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, 0);
	u64 local_docid;
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid!="", 0);
	char c1 = objid.data()[1];
	char c2 = objid.data()[2];
	if (c1 !='_' && c2 !='_' )
	{
		local_docid = mMgr->getSpecialDocid();
	}
	else
	{
		local_docid = getNewDocId();
	}
	aos_assert_r(local_docid, 0);
	return  local_docid;
}


bool
AosQueryTesterThrd::checkDocThreadId(const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid != "", false);
	char c1= objid.data()[0];
	if (c1 >= '0' && c1 <= '9')
	{
		aos_assert_r(((int) c1)-48 == mThreadId, false);
	}
	return true;

}


OmnString
AosQueryTesterThrd::getUserCid(const u64 &user_localid)
{
	AosStUser* user_doc = getUser(user_localid);
	if (!user_doc) return "";
	return user_doc->cid;
}

bool
AosQueryTesterThrd::loginAsNormalUser()
{
    return true;
}


bool 	
AosQueryTesterThrd::canDoOpr(
		const u64 &local_did,
		const AosSecOpr::E opr, 
		const OmnString &requester_cid)
{
	// This function checks whether the requester 'requester_cid' can perform
	// the operation 'opr' on the doc 'local_did'. It is done by retrieving 
	// its local access record. 
	// If the local access record is not there yet, it tries to get its 
	// parent access record.
	//
	// 1. If 'local_did' does not find a local doc, it is an error.
	// 2. If the doc does not have its local access record, retrieve its 
	//    access record from the server. 
	// 3. If it does not have an access record on the server, retrieve the 
	// 	  doc's parent doc. If not found, it is an error. 
	// 4. Retrieve its parent's access record from the server. 
	// 5. If the server access record (either itself or its parent), its
	//    local access record is created. 
	// 6. If no access record is found, use the default. 
	// 7. Otherwise, use the access record to check the access.
	//AosStDoc *doc = mDocsByLocdid[local_did];
	AosStDoc *doc = getDocFromMemory(local_did);
	if (!doc)
	{
		// 'local_did' does not identify a doc
		OmnAlarm << "Local doc does not identify a doc: " << local_did << enderr;
		return false;
	}
	OmnString owner_cid = doc->creator;

	AosStAccessRecord *arcd = getLocalAccessRecord(local_did);
	bool is_parent = false;
	if (!arcd)
	{
		// Retrieve the access record from the server
		AosXmlTagPtr svr_ard = retrieveAccessRcd(doc->svrdid, false, false);
		if (!svr_ard)
		{
			// Did not retrieve its access record from the server. 
			// Retrieve its parent's access record. 
			aos_assert_r(doc->parents_locdid.size() > 0, false);
			u64 parent_locdid = doc->parents_locdid[0];
			//AosStDoc *parent_doc = mDocsByLocdid[parent_locdid];
			AosStDoc *parent_doc = getDocFromMemory(parent_locdid);
			if (!parent_doc)
			{
				OmnAlarm << "Failed retrieving the parent: " << parent_locdid << enderr;
				return false;
			}

			arcd = getLocalAccessRecord(parent_locdid);
			if (!arcd)
			{
				svr_ard = retrieveAccessRcd(parent_doc->svrdid, false, false);
				if (svr_ard)
				{
					arcd = createAccessRecord(svr_ard, parent_locdid);
					is_parent = true;
				}
			}
		}
		else
		{
			arcd = createAccessRecord(svr_ard, local_did);
		}
	}

	if (arcd)
	{
		return arcd->canDoOpr(opr, owner_cid, mCrtCid);
	}

	// No access record yet. 
	return AosStAccessRecord::canDoOprDefault(opr, owner_cid, mCrtCid);
}


AosStAccessRecord *
AosQueryTesterThrd::getAcd(
		const AosStDoc *doc, 
		bool &is_parent)
{
	// This function retrieves the local access record for the given doc
	// 'doc'. If not found, it retrieves its parent's local access record.
	// If not found, it randomly determines whether to create its own
	// access record or its parent's access record.
	is_parent = false;
	aos_assert_r(doc, 0);

	// Retrieve the doc's own access record
	AosStAccessRecord *arcd = getLocalAccessRecord(doc->locdid);
	if (arcd)
	{
		aos_assert_r(doc->locdid == arcd->mOwnLocdid, 0);
		aos_assert_r(doc->svrdid == arcd->mOwnSvrdid, 0);
		return arcd;
	}

	// Did not find its local access record. 
	// Retrieve its parent's local access record. 
	aos_assert_r(doc->parents_locdid.size() > 0, false);
	u64 parent_locdid = doc->parents_locdid[0];
	//AosStDoc *parent_doc = mDocsByLocdid[parent_locdid];
	AosStDoc *parent_doc = getDocFromMemory(parent_locdid);
	if (!parent_doc)
	{
		OmnAlarm << "Failed retrieving the parent: " << parent_locdid << enderr;
		return 0;
	}

	arcd = getLocalAccessRecord(parent_locdid);
	if (arcd) 
	{
		is_parent = true;
		return arcd;
	}

	// Not found. Retrieve the access record from the server
	AosXmlTagPtr svr_ard = retrieveAccessRcd(doc->svrdid, false, false);
	if (svr_ard)
	{
		// Retrieved its access record from the server. Create the local
		// access record based on the server access record.
		arcd = createAccessRecord(svr_ard, doc->locdid);
		aos_assert_r(arcd, 0);
		return arcd;
	}

	// Did not find its server access record. Retrieve its parent's 
	// server access record.
	svr_ard = retrieveAccessRcd(parent_doc->svrdid, false, false);
	if (svr_ard)
	{
		// Found it. Create its parent local access record.
		arcd = createAccessRecord(svr_ard, parent_locdid);
		aos_assert_r(arcd, 0);
		is_parent = true;
		return arcd;
	}

	// Determine whether to create its own access record or its parent
	// access record. 
	const AosStDoc *dd = 0;
	if (rand() % 100 < 90)
	{
		is_parent = true;
		dd = parent_doc;
	}
	else
	{
		dd = doc;
	}

	AosXmlTagPtr raw_arcd = createArcd();
	aos_assert_r(raw_arcd, 0);
	bool rslt;// = AosSengAdmin::getSelf()->createAccessRcd(mCrtSiteid, raw_arcd, 
			//dd->objid, getRootSsid());
	aos_assert_r(rslt, 0);
	svr_ard = retrieveAccessRcd(doc->svrdid, false, false);
	aos_assert_r(svr_ard, 0);
	arcd = createAccessRecord(svr_ard, doc->locdid);
	aos_assert_r(arcd, 0);
	return arcd;
}


AosXmlTagPtr
AosQueryTesterThrd::createArcd()
{
	// This function randomly creates an access record, which is
	// in the form: 
	// 	<records>
	// 		<operation .../>
	// 		<operation .../>
	// 		...
	// 	</records>
	int num = rand() % AosSecOpr::eMax;
	int oprs[AosSecOpr::eMax];
	for (int i=0; i<AosSecOpr::eMax; i++) oprs[i] = AosSecOpr::eInvalid;
	for (int i=0; i<num; i++)
	{
		int k = rand() % AosSecOpr::eMax;
		oprs[k] = 1;
	}

	OmnString docstr = "<accesses>";
	for (int i=0; i<AosSecOpr::eMax; i++)
	{
		AosAccessType::E type = (AosAccessType::E)(rand() % (AosAccessType::eMax-1) + 1);
		if (oprs[i] == AosSecOpr::eInvalid) continue;
		docstr << "<record opr=\"" << AosSecOpr::toString((AosSecOpr::E)i)
			<< "\" accesses=\"" << AosAccessType::toString(type);

		// Check whether it needs to create more accesses
		if (rand() % 100 >= 90)
		{
			int num = rand() % 5;
			for (int k=0; k<num; k++)
			{
				type = (AosAccessType::E)(rand() % (AosAccessType::eMax-1) + 1);
				docstr << "," << AosAccessType::toString(type);
			}
		}
		docstr << "\"/>";
	}
	docstr << "</accesses>";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	return doc;
}

bool
AosQueryTesterThrd::checkSaveSdocFlag()
{
	//bool flag = mSaveSdocFlag;
	//mSaveSdocFlag = true;
	//return flag;
}                                   


void
AosQueryTesterThrd::addStDoc(AosStDoc *doc)
{
	//OmnScreen << "------docid:" << doc->locdid << endl;
	if (doc->locdid == 102)
		OmnMark;
	mDocs.push_back(doc);
}

AosStDoc *
AosQueryTesterThrd::getStDoc(int idx)
{
	return mDocs[idx];
}

void
AosQueryTesterThrd::addDocsByObjid(const OmnString objid, AosStDoc *doc)
{
	mDocsByObjid[objid] = doc;
}

void
AosQueryTesterThrd::addDocsByLocdid(const u64 &local_docid, AosStDoc *doc)
{
	mDocsByLocdid[local_docid] = doc;
}

void
AosQueryTesterThrd::addServerDidMap(const u64 &server_docid, const u64 &local_docid)
{
	mServerDidMap[server_docid] = local_docid;
}

*/
#endif
