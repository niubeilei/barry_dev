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
#include "SengTorUtil/SengTesterThrd.h"


#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/SengTester.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/TesterXmlDoc.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StType.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StContainer.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StAccess.h"
#include "SengTorUtil/StOperationArd.h"
#include "SEBase/SecOpr.h"
#include "SEModules/SeRundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/String.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Docid.h"
#include "XmlUtil/SeXmlParser.h"

#include "Thrift/AosJimoAPI.h"
#include "ThriftClientCPP/Headers.h"     

extern shared_ptr<AosJimoAPIClient> gThriftClient;
static vector<OmnString>		sgOtypes;
static vector<OmnString>		sgStypes;
const u32 sgMaxOTypes = 200;
const u32 sgMaxSTypes = 200;

AosSengTestThrd::AosSengTestThrd(
		const AosSengTesterMgrPtr &mgr, 
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
mCrtDocid(5000),
mNumObjid(0),
mNumCtnrObjid(0),
mNumUserName(0),
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	aos_assert(mMgr);
	mCrtSsid = mMgr->getRootSsid();
	mCrtUrlDocid = mMgr->getRootUrlDocid();
	mRundata->setArg1(AOSARG_TRACK, "true");
	mRundata->setSiteid(mgr->getDftSiteid());
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "Thrd", ++mThreadId, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosSengTestThrd::AosSengTestThrd(
		const AosSengTesterMgrPtr &mgr, 
		const int tries, 
		const int thrdid, 
		const vector<AosSengTesterPtr> &testers)
:
mMgr(mgr),
mLock(OmnNew OmnMutex()),
mThreadId(thrdid),
mTries(tries),
mTotalSuccess(0),
mTotalFail(0),
mCrtUserid(0),
mCrtDocid(5000),
mNumObjid(0),
mNumCtnrObjid(0),
mNumUserName(0),
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	aos_assert(mMgr);
	mCrtSsid = mMgr->getRootSsid();
	mCrtUrlDocid = mMgr->getRootUrlDocid();
	mRundata->setArg1(AOSARG_TRACK, "true");
	mRundata->setSiteid(mgr->getDftSiteid());
	OmnThreadedObjPtr thisPtr(this, false);

	for (u32 i=0; i<testers.size(); i++)
	{
		mTestersNew.push_back(testers[i]->clone());
	}

	mThread = OmnNew OmnThread(thisPtr, "Thrd", ++mThreadId, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosSengTestThrd::~AosSengTestThrd()
{
	map<u64, AosStContainerPtr>::iterator iterCtnr;

	UserMapItr_t iterUser;
	for(iterUser = mUsers1.begin(); iterUser != mUsers1.end(); ++iterUser)
		delete iterUser->second;	

	//hash_map<u64, AosStAccessRecord*, u64_hash, u64_cmp> ::iterator iterAR
	//hash_map<const OmnString, AosStAccessRecord*, Omn_Str_hash, compare_str> ::iterator iterAR
	//	= mLocalDidArcds.begin();
	AosU642StArcdItr_t iterAR = mLocalDidArcds.begin();
	for(; iterAR!= mLocalDidArcds.end(); ++iterAR)
		delete iterAR->second;	

	// AosStr2StdocItr_t  iterObjid
	// 	= mDocsByObjid.begin();
	// for(; iterObjid != mDocsByObjid.end(); ++iterObjid)
	// 	delete iterObjid->second;	

	// AosU642StdocItr_t  iterLocdid
	// 	= mDocsByLocdid.begin();
	// for(; iterLocdid !=  mDocsByLocdid.end(); ++iterLocdid)
	// 	delete iterLocdid->second;	

	AosU642StArcdItr_t iterar = mOwnDocidArcds.begin();
	for(; iterar != mOwnDocidArcds.end(); ++iterar)
		delete iterar->second;	
}


bool
AosSengTestThrd::start()
{
	/*
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
	// API XXXXXXXXXXXX
	//AosXmlTagPtr rootdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
	//		siteid, mCrtSsid, mCrtUrlDocid, mMgr->getRootObjid(), true);
	//aos_assert_r(rootdoc, false);
	gThriftClient = AosJimoAPIClientMgr::getSelf()->getClient(); 
	JmoRundata rdata;
	JmoCallData handle;

	gThriftClient->getDocByObjid(rdata, handle, mMgr->getRootObjid());
    if(rdata.rcode != JmoReturnCode::SUCCESS)
		OmnAlarm << "cant't get the doc"; 

	//u32 siteid = mRundata->getSiteid();
	// 2. Retrieve the root's parent doc: should success since its
	// parent container's accesses should be configured that reading
	// is public (by default).
	//OmnString parent_objid = mMgr->getRootCtnr();
	//aos_assert_r(parent_objid != "", false);
	//AosXmlTagPtr parentdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
	//		siteid, mCrtSsid, mCrtUrlDocid, parent_objid, true);
	//aos_assert_r(parentdoc, false);
	gThriftClient->getDocByObjid(rdata, handle, mMgr->getRootCtnr());
    if(rdata.rcode != JmoReturnCode::SUCCESS)
		OmnAlarm << "cant't get the doc"; 

	// 3. Retrieve the root's home container. Should success
	// since its home container's read access is configured to
	// public by default.
	//OmnString home_objid = mMgr->getRootHomeCtnr();
	//aos_assert_r(home_objid != "", false);
	//AosXmlTagPtr homedoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
	//		siteid, mCrtSsid, mCrtUrlDocid, home_objid, true);
	//aos_assert_r(homedoc, false);
	gThriftClient->getDocByObjid(rdata, handle, mMgr->getRootHomeCtnr());
    if(rdata.rcode != JmoReturnCode::SUCCESS)
		OmnAlarm << "cant't get the doc"; 

	// 4. Login as root
	//bool rslt = loginAsRoot();
	//aos_assert_r(rslt, false);

	// 5. Retrieve the root doc: should success
	//rootdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
	//		siteid, mCrtSsid, mCrtUrlDocid, mMgr->getRootObjid(), true);
	//aos_assert_r(rootdoc, false);
	gThriftClient->getDocByObjid(rdata, handle, mMgr->getRootObjid());
    if(rdata.rcode != JmoReturnCode::SUCCESS)
		OmnAlarm << "cant't get the doc"; 

	// 6. Retrieve the root's parent doc: should success
	//parent_objid = rootdoc->getAttrStr(AOSTAG_PARENTC);
	//aos_assert_r(parent_objid != "", false);
	//parentdoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
	//		siteid, mCrtSsid, mCrtUrlDocid, parent_objid, true);
	//aos_assert_r(parentdoc, false);
	gThriftClient->getDocByObjid(rdata, handle, mMgr->getRootObjid());
    if(rdata.rcode != JmoReturnCode::SUCCESS)
		OmnAlarm << "cant't get the doc"; 

	//aos_assert_r(addContainer(AosStContainer1::eUserCtnr, parentdoc, mRundata), false);

	// 7. Retrieve the root's home container: should success
	//AosXmlTagPtr rootdoc;
	//OmnString home_objid = rootdoc->getAttrStr(AOSTAG_CTNR_HOME);
	//aos_assert_r(home_objid != "", false);

	//AosXmlTagPtr homedoc;
	//homedoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
	//		siteid, mCrtSsid, mCrtUrlDocid, home_objid, true);
	//aos_assert_r(homedoc, false);
	//aos_assert_r(addContainer(AosStContainer1::eDocCtnr, homedoc, mRundata), false);
	

	OmnString home_objid = rdata.getAttrStr(OSTAG_CTNR_HOME);
	aos_assert_r(home_objid != "", false);
	
	gThriftClient->getDocByObjid(rdata, handle, home_objid);
    if(rdata.rcode != JmoReturnCode::SUCCESS)
		OmnAlarm << "cant't get the doc"; 



	//Linda cid container 2011/01/05
	// OmnString cidctnr = homedoc->getAttrStr(AOSTAG_PARENTC);
	// aos_assert_r(cidctnr, false);
	// AosXmlTagPtr ciddoc= AosSengAdmin::getSelf()->retrieveDocByObjid(
	// 		siteid, mCrtSsid, cidctnr);
	// aos_assert_r(addContainer(AosStContainer1::eUserCtnr, ciddoc), false);
	//Linda

	//aos_assert_r(addUser(rootdoc, mRundata), false);
	AosSengTestThrdPtr thisptr(this, false);
	//aos_assert_r(AosCreateUserNewTester::createUserAccount(thisptr), false);
	*/
	return true;
	
	//OmnNotImplementedYet;
	//return false;
}


bool    
AosSengTestThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	start();
	AosSengTestThrdPtr thisptr(this, false);
	AosSengTester::cloneTesters(thisptr, mTesters, mMgr);
	while (state == OmnThrdStatus::eActive)
	{
		basicTest();
		state = OmnThrdStatus::eIdle;
		return true;	
	}
	return true;
}


bool    
AosSengTestThrd::signal(const int threadLogicId)
{
	return true;
}


bool    
AosSengTestThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosSengTestThrd::basicTest()
{
	if (mTestersNew.size() == 0)
	{
		AosSengTestThrdPtr thrd(this, false);
		if (mTries == 0) mTries = 0xffffffff;

		AosSengTester::startTestStatic(thrd, mMgr);
		return true;
	}
	
	// Calculate the total weights
	vector<int> tester_ids;
	int total_weights = 0;
	for (u32 i=0; i<mTestersNew.size(); i++)
	{
		int ww = mTestersNew[i]->getWeight();
		total_weights += ww;
		for (int k=0; k<ww; k++)
		{
			tester_ids.push_back(i);
		}
	}

	if (mTries <= 0) mTries = eDftTries;
	int group = 10000;
	int groupSize = 200;
	OmnScreen << "tries: " << mTries << ":" << mThreadId << endl;
	for (u32 i =0; i< mTries; i++)
	{
		if (group >= groupSize)
		{
			OmnScreen << "Tester tries: " << i << endl;
			group = 0;
		}

		group++;
		int ww = rand() % total_weights;
		int tester_id = tester_ids[ww];
		OmnScreen << "Run test(thd" << mThreadId << "):" 
			<< i << ":" << mTestersNew[tester_id]->getName() << endl;
			mTestersNew[tester_id]->test();
	}

	OmnScreen <<"Finished basic testing!" << endl;
	return true;
}


u64 
AosSengTestThrd::getNewDocId()
{
	u64 i =	mCrtDocid++;
	return i; 
}


int 
AosSengTestThrd::getThreadId() const
{	
	return mThreadId;
}


AosXmlTagPtr
AosSengTestThrd::retrieveAccessRcdOrParent(const u64 &svrdid)
{
	/* API XXXXXXXX
	AosXmlTagPtr ard = AosSengAdmin::getSelf()->getAccessRcd(getSiteid(), 
			svrdid, mMgr->getRootSsid(), mCrtUrlDocid, false, true);
	if (ard) return ard;
	return 0;
	*/
	OmnNotImplementedYet;
	return 0;
}


AosXmlTagPtr
AosSengTestThrd::retrieveAccessRcd(
		const u64 &svrdid, 
		const bool create_flag, 
		const bool parent_flag)
{
	// This function retrieves the access record from the server. If 
	// 'create_flag' is true, it will create the access record if it is not
	// there yet. If 'parent_flag' is true, it will retrieve its parent's
	// access record if it is not yet.
	/* API XXXXXXXXXXXX
	AosXmlTagPtr ard = AosSengAdmin::getSelf()->getAccessRcd(getSiteid(), 
			svrdid, mMgr->getRootSsid(), mCrtUrlDocid, create_flag, parent_flag);
	return ard;
	*/
	OmnNotImplementedYet;
	return 0;
}


AosStAccessRecord *
AosSengTestThrd::createAccessRecord(const u64 &locdid)
{
	// It retrieves the access record for the doc 'doc' (do not create
	// it yet)
	AosStDocPtr doc = getDocByLocalDid(locdid);
	aos_assert_r(doc, 0);
	AosXmlTagPtr svr_ard = retrieveAccessRcd(doc->svrdid, false, false);
	aos_assert_r(svr_ard, 0);
	return createAccessRecord(svr_ard, locdid);
}


AosStAccessRecord *
AosSengTestThrd::createAccessRecord(const AosXmlTagPtr &svr_ard, const u64 &locdid)
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
AosSengTestThrd::modifyAccessRecord(AosStAccessRecord *ard)
{
	const AosStDocPtr doc = pickDoc(ard->mOwnLocdid);
	aos_assert_r(doc, false);
	aos_assert_r(doc->svrdid == ard->mOwnSvrdid, false);
	aos_assert_r(doc->locdid == ard->mOwnLocdid, false);

	AosU642StArcdItr_t itr = mLocalDidArcds.find(ard->mOwnLocdid);
	if (itr != mLocalDidArcds.end())
	{
		AosStAccessRecord* arcd = itr->second;
		aos_assert_r(arcd, false);
		aos_assert_r(arcd->mOwnLocdid == ard->mOwnLocdid, false);
		aos_assert_r(arcd->mOwnSvrdid == ard->mOwnSvrdid, false);
		aos_assert_r(arcd->mSvrdid == ard->mSvrdid, false);
	}

	u64 ownsvrdid = ard->mOwnSvrdid;
	aos_assert_r(ownsvrdid, false);
	
	itr = mOwnDocidArcds.find(ownsvrdid);
	AosStAccessRecord *arcd = 0;
	if (itr != mOwnDocidArcds.end()) arcd = itr->second;
	if (arcd)
	{
		aos_assert_r(arcd->mOwnLocdid == ard->mOwnLocdid, false);
		aos_assert_r(arcd->mOwnSvrdid == ownsvrdid, false);
		aos_assert_r(arcd->mSvrdid == ard->mSvrdid, false);
	}
	
	//aos_assert_r(ownsvrdid == createArdObjid(ard->mOwnSvrdid), false);

	mLocalDidArcds[ard->mOwnLocdid] = ard;
	mOwnDocidArcds[ownsvrdid] = ard;
	return true;

}


bool
AosSengTestThrd::syncAccesses(
		AosStAccessRecord *ard, 
		const AosXmlTagPtr &svr_ard) 
{
	for (int opr=AosSecOpr::eInvalid+1; opr<AosSecOpr::eMax; opr++)
	{
		OmnString accesses = svr_ard->getAttrStr(AosSecOpr::toString(opr));
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


/*
bool
AosSengTestThrd::addAccessRecord(const AosXmlTagPtr &ar)
{
	// 
	// 1. Save to the local
	u64 local_docid = ar->getAttrU64(AOSTEST_DOCID, AOS_INVDID);
	aos_assert_r(local_docid != AOS_INVDID, false);
	aos_assert_r(saveNewDoc(local_docid, ar), false);

	// 2. Add to AccessRecord 
	AosStAccessRecord *ar1 = OmnNew AosStAccessRecord;
	ar1->mOwnLocdid = local_docid;

	ar1->mOwnSvrdid = ar->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_r(ar1->mOwnSvrdid!= AOS_INVDID, false);
	
	OmnString objid = ar->getAttrStr(AOSTAG_OBJID);
	ar1->mObjid = objid; 
	aos_assert_r(ar1->mObjid != "", false);
	
//	u64 own = ar->getAttrU64(AOSTAG_OWNER_DOCID, AOS_INVDID);
//	aos_assert_r(own!= AOS_INVDID, false);
//	ar1->own sdocid = own;

	mLocalDidArcds[local_docid] = ar1;
	mObjidArcds[objid] = ar1;

	// 3. A container is also a doc. Need to add to docs
	AosStDoc *doc = OmnNew AosStDoc;
	doc->locdid = local_docid;
	doc->svrdid = ar1->mOwnSvrdid;
	doc->objid = ar1->mObjid;
	doc->creator = ar->getAttrStr(AOSTAG_CREATOR);
	aos_assert_r(doc->creator != "", false);
	mDocs.push_back(doc);
	mDocsByObjid[doc->objid] = doc;
	mDocsByLocdid[local_docid] = doc;
	
	return true;
}

*/


AosStContainerPtr
AosSengTestThrd::pickContainer(const AosStContainer1::Type type)
{
	// It randomly picks a container.
	if (mCtnrs1.size() <= 0) return NULL;
		
	AosStContainerPtr cc;
	CtnrMapItr_t iter = mCtnrs1.begin();
	int idx = (rand() % mCtnrs1.size());
	for (int j = 0; j<idx; j++) iter++;

	while (iter != mCtnrs1.end())
	{
		cc = iter->second;
		if (cc->mType == type) return cc;
		iter++;
	}

	// Did not find it. Try it from the beginning. 
	iter = mCtnrs1.begin();
	for (int j = 0; j < idx; j++)
	{
		cc = iter->second;
		if (cc->mType == type) return cc;
		iter++;
	}

	// Did not find any container whose type is 'type'
	return NULL;
}


AosStContainerPtr
AosSengTestThrd::pickContainer()
{
	AosStContainer1::Type type = pickContainerType();
	return pickContainer(type);
}


AosStContainerPtr
AosSengTestThrd::getContainer(const u64 &locdid)
{
	CtnrMapItr_t item = mCtnrs1.find(locdid);
	if (item == mCtnrs1.end()) return 0;
	return item->second;
}


AosStContainerPtr
AosSengTestThrd::getContainer(const OmnString &objid)
{
	AosStDocPtr doc = getDocByObjid(objid);
	if (!doc) return 0;
	CtnrMapItr_t item = mCtnrs1.find(doc->locdid);
	if (item == mCtnrs1.end()) return 0;
	return item->second;
}


bool
AosSengTestThrd::addContainer(
		const OmnString &objid, 
		const AosStContainer1::Type type, 
		const AosRundataPtr &rdata)
{
	// It checks whether the container is already added. If yes, 
	// it does nothing. 
	AosStContainerPtr ctnr = getContainer(objid);
	if (ctnr) return true;

	// The container has not been added to the system yet. 
	// Retrieve it from the server.
	aos_assert_r(mMgr, false);
	AosXmlTagPtr doc = mMgr->retrieveXmlDoc(rdata->getSiteid(), objid);
	aos_assert_r(doc, false);
	aos_assert_r(addContainer(type, doc, rdata), false);
	return true;
}


bool
AosSengTestThrd::addContainer(
		const AosStContainer1::Type type, 
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
	AosStDocPtr doc1 = getDocByLocalDid(local_docid);

	CtnrMapItr_t itr = mCtnrs1.find(local_docid);
	AosStContainerPtr cc;
	if (itr != mCtnrs1.end()) cc = itr->second;
	if (doc1 && cc)
	{
		aos_assert_r(doc1->objid == cc->mObjid, false);
		aos_assert_r(doc1->objid == ctnr->getAttrStr(AOSTAG_OBJID), false);
		OmnScreen <<"Already exists!" << endl;
		return true;
	}
	aos_assert_r(!doc1 && !cc, false);

	aos_assert_r(saveNewDoc(local_docid, ctnr), false);

	// 2. Add to mCtnrs
	AosStContainerPtr container = OmnNew AosStContainer1;
	container->mObjid = ctnr->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(container->mObjid != "", false);
	container->mLocdid = local_docid;
	container->mSvrdid = ctnr->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_r(container->mSvrdid != AOS_INVDID, false);
	container->mType = type;
	addContainer(local_docid, container);
	aos_assert_r(getContainer(local_docid), false);

	// 3. A container is also a doc. Need to add to docs
	//addDoc(local_docid, ctnr);
	AosStDocPtr doc = OmnNew AosStDoc;
	doc->locdid = local_docid;
	doc->svrdid = container->mSvrdid;
	doc->objid = container->mObjid;
	doc->creator = ctnr->getAttrStr(AOSTAG_CREATOR);
	aos_assert_r(doc->creator != "", false);

	OmnString ctnr_objid = ctnr->getAttrStr(AOSTAG_PARENTC);
	if (ctnr_objid != "")
	{
		AosStr2StdocItr_t itr = mDocsByObjid.find(ctnr_objid);
		AosStDocPtr parentdoc;
		if (itr != mDocsByObjid.end()) parentdoc = itr->second;
		if(parentdoc) doc->parent_locdid = parentdoc->locdid;
	}

	aos_assert_r(checkDocThreadId(ctnr), false);
	addStDoc(doc);
	addDocByObjid(doc->objid, doc);
	addDocsByLocdid(local_docid, doc);
	addServerDidMap(doc->svrdid, local_docid);
	return true;
}


bool
AosSengTestThrd::addContainer(const u64 &docid, const AosStContainerPtr &ctnr)
{
	aos_assert_r(ctnr, false);
	mCtnrs1[docid] = ctnr;
	return true;
}


AosStUser*
AosSengTestThrd::getUser(const u64 &user_localid) 
{
	UserMapItr_t itr = mUsers1.find(user_localid);
	if (itr == mUsers1.end()) return 0;
	return itr->second;
}


AosStUser*
AosSengTestThrd::pickUser() 
{
	if (mUsers1.size() <= 0) return NULL;
	while(1)
	{
		UserMapItr_t iter = mUsers1.begin();
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
AosSengTestThrd::getTotalUser() 
{
	return mUsers1.size();
}


const AosStDocPtr
AosSengTestThrd::pickDoc(const bool own_doc) 
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


AosStDocPtr
AosSengTestThrd::getDocByLocalDid(const u64 &local_docid)
{
	AosU642StdocItr_t itr = mDocsByLocdid.find(local_docid);
	if (itr == mDocsByLocdid.end()) return 0;
	return itr->second;
}


AosStDocPtr	
AosSengTestThrd::getDocByObjid(const OmnString &objid) 
{
	AosStr2StdocItr_t itr = mDocsByObjid.find(objid);
	if (itr == mDocsByObjid.end()) 
	{
		return 0;
	}

	return itr->second;
}


bool
AosSengTestThrd::removeDoc(
				const u64 &local_docid,
				const OmnString objid,
				const u64 &server_docid)
{
	// 1. mDocs
	vector<AosStDocPtr>::iterator it = mDocs.begin();
	vector<AosStDocPtr>::iterator it1 = mDocs.begin();
	int i = 0;	
	bool found = false;
	for (; it!= mDocs.end(); ++it)
	{
		AosStDocPtr doc = *it;
		aos_assert_r(doc, false);
		if (doc->locdid == local_docid)
		{
			it1 = it;
			aos_assert_r(mDocs[i]->locdid == local_docid, false);
			it = mDocs.erase(it); 
			if (it1 != mDocs.end())
			{
				AosStDocPtr test = *it1;
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
	AosStr2StdocItr_t iter = mDocsByObjid.find(objid);
	if (iter != mDocsByObjid.end())
	{
		mDocsByObjid.erase(objid);
	}
	
	//3.mDocsByLocdid 
	//hash_map<u64, AosStDocPtr, u64_hash, u64_cmp>
	//hash_map<const OmnString, AosStDocPtr, Omn_Str_hash, compare_str>
	//	::iterator iter1 = mDocsByLocdid.begin();
	AosU642StdocItr_t iter1 = mDocsByLocdid.find(local_docid);
	if (iter1 != mDocsByLocdid.end())
	{
		mDocsByLocdid.erase(local_docid);
	}

	//4.mServerDidMap
	//hash_map<const OmnString, u64, Omn_Str_hash, compare_str>
	//	::iterator iter2 = mServerDidMap.begin();
	AosU642U64Itr_t iter2 = mServerDidMap.find(server_docid);
	if (iter2 != mServerDidMap.end())
	{
		mServerDidMap.erase(server_docid);
	}
	return true;
}


int
AosSengTestThrd::getStDocSize()
{
	return mDocs.size();
}


u64
AosSengTestThrd::getServeridByLocalid(const OmnString &server_docid)
{
	AosU642U64Itr_t itr = mServerDidMap.find(atoll(server_docid.data()));
	if (itr == mServerDidMap.end()) return 0;
	return itr->second;
}


u64 
AosSengTestThrd::getLocalidByCid(const OmnString  &cid)
{
	AosStr2U64Itr_t itr = mUserCidMap.find(cid);
	if (itr == mUserCidMap.end()) return 0;
	return itr->second;
}


AosStUser *
AosSengTestThrd::getUserByCid(const OmnString &cid)
{
	u64 local_did = getLocalidByCid(cid);
	if (local_did == 0) return 0;
	return getUser(local_did);
}


bool
AosSengTestThrd::addDoc1(const u64 &local_docid, const AosXmlTagPtr &xmldoc)
{
	// This function adds the doc 'xmldoc' to the local member data.
	aos_assert_r(local_docid > 0, false);
	aos_assert_r(xmldoc, false);
	
	// The doc should not be in the map yet.
	AosStDocPtr doc = getDocByLocalDid(local_docid);
	aos_assert_r(!doc, false);

	// Create the local StDoc
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
		const AosStDocPtr parentdoc = getDocByObjid(ctnr_objid);
		aos_assert_r(parentdoc, false);
		doc->parent_locdid = parentdoc->locdid;
	}
	aos_assert_r(checkDocThreadId(xmldoc), false);
	addStDoc(doc);
	addDocByObjid(doc->objid, doc);
	addDocsByLocdid(local_docid, doc);
	addServerDidMap(doc->svrdid, local_docid);

	aos_assert_r(saveNewDoc(local_docid, xmldoc), false);
	return true;
}


bool
AosSengTestThrd::modifyDocToMemory(const u64 &local_docid, const AosXmlTagPtr &xmldoc)
{
	aos_assert_r(local_docid > 0, false);
	aos_assert_r(xmldoc, false);

	AosStDocPtr doc = getDocByLocalDid(local_docid);
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
		const AosStDocPtr parentdoc = getDocByObjid(ctnr_objid);
		aos_assert_r(parentdoc, false);
		doc->parent_locdid = parentdoc->locdid;
	}

	aos_assert_r(modifyVector(doc), false);

	addDocByObjid(doc->objid, doc);
	addDocsByLocdid(local_docid, doc);

	//mServerDidMap[doc->svrdid] = local_docid;
	addServerDidMap(doc->svrdid, local_docid);

	aos_assert_r(modifyDocToFile(local_docid, xmldoc), false);
	return true;
}

bool
AosSengTestThrd::modifyVector(const AosStDocPtr &stdoc)
{
	u64 local_docid = stdoc->locdid;
	vector<AosStDocPtr>::iterator it = mDocs.begin();
	bool found = false;
	for (; it!= mDocs.end(); ++it)
	{
		AosStDocPtr doc = *it;
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
AosSengTestThrd::checkCreate(const OmnString &container_objid)
{
	// 1. Retrieve the container's access record. 
	// 2. Get accesses from the access record
	//
	return true;
	/*
	//AosSecOpr::eCreate
	const Session *session = getSession();
	OmnString ssid  = session ->sessionId;
	OmnString cid =  session -> cid;
	AosXmlTagPtr udoc = session -> session;
	if (!doc) return false;

	if (udoc ->getAttrStr(AOSTAG_USERTYPE) == AOSUSERTYPE_ROOT) return true;

	OmnString accesses = ar->getAttrStr(AOSTAG_WRITE_ACSTYPE);
	if (accesses == "") return false;
	AosSecOpr::E opr = AosSecOpr::eCreate;
	checkAccess(ar, doc, accesses, opr, cid);
	return true;
	*/
}


AosXmlTagPtr
AosSengTestThrd::createDoc()
{
	int depth = rand() % eMaxDepth;
	AosTestXmlDoc xml("", depth);
	AosXmlTagPtr doc = xml.createNewDoc();
	aos_assert_r(doc, 0);
	return doc;
}


AosXmlTagPtr
AosSengTestThrd::readLocalDocFromFile(const u64 &ldocid)
{
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	int tid = getThreadId();
	AosXmlTagPtr doc = ff->readDoc(mRundata, tid, ldocid AosMemoryCheckerArgs);
	return doc;
}


bool 
AosSengTestThrd::saveNewDoc(const u64 local_docid, const AosXmlTagPtr &doc)
{
	aos_assert_r(local_docid, false);
	aos_assert_r(doc, false);
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	aos_assert_r(ff, false);
	int tid = getThreadId();
	OmnString docstr = doc->toString();
	ff->createDoc(mRundata, tid, local_docid, docstr.data(), docstr.length());
	aos_assert_r(checkSaveNewDoc(local_docid), false);
	return true;
}


bool
AosSengTestThrd::modifyDocToFile(const u64 local_docid, const AosXmlTagPtr &doc)
{
	aos_assert_r(local_docid, false);
	aos_assert_r(doc, false);
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	aos_assert_r(ff, false);
	int tid = getThreadId();
	ff->modifyDoc(mRundata, tid, local_docid, doc->toString().data(), doc->getDataLength());
	aos_assert_r(checkSaveNewDoc(local_docid), false);
	return true;
}


bool
AosSengTestThrd::removeDocToFile(const u64 local_docid)
{
	aos_assert_r(local_docid, false);
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	aos_assert_r(ff, false);
	int tid = getThreadId();
	aos_assert_r(ff->removeDoc(mRundata, tid, local_docid), false);
	return true;
}


bool
AosSengTestThrd::isDeleteToFile(const u64 local_docid)
{
	aos_assert_r(local_docid, false);
	AosTestFileMgrPtr ff = mMgr->getFileMgr();
	aos_assert_r(ff, false);
	int tid = getThreadId();
	if (!ff->isDocDeleted(mRundata, tid, local_docid)) return false;
	return true;
}


bool
AosSengTestThrd::checkSaveNewDoc(const u64 local_docid)
{
	AosXmlTagPtr doc = readLocalDocFromFile(local_docid);
	aos_assert_r (doc, false);
	return true;
}


OmnString
AosSengTestThrd::pickNewObjid()
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

/*
OmnString
AosSengTestThrd::pickUsedObjid(bool &is_new_objid)
{
	is_new_objid = false;
	OmnString objid1;
	char c1 =' ';
	char c4 = ' ';
	int s = mDocs.size();
	int j = 0;
	while((c1 != '_' && c4!='_') && j<s)
	{
		const AosStDoc *doc = pickDoc();
		aos_assert_r(doc, "");
		objid1 << doc->objid;
		c1 = objid1.data()[1];
		c4 = objid1.data()[2];
		j++;
	}
	if (s == j)
	{
		is_new_objid = true;
		return pickNewObjid();
	}
	aos_assert_r(objid1 != "", "");
	int i = 0;
	OmnString tobjid;
	if (getDocByObjid(objid1))
	{
		while (1)
		{
			tobjid << objid1<<"("<< i <<")";
			if (!getDocByObjid(tobjid)) 
			{
				AosXmlTagPtr xml= AosSengAdmin::getSelf()->retrieveDocByObjid(
					getSiteid(), getCrtSsid(), tobjid, true, mCrtUrlDocid);
				aos_assert_r(!xml, "");
				break;
			}
			i++;
		}
	}
	char c3= objid1.data()[0];
	char c2 = objid1.data()[1];
	char c5 = objid1.data()[2];
	aos_assert_r(c3 >= '0' && c3 <= '9', "");
	aos_assert_r(c2 =='_' || c5 =='_', "");
	return objid1;	
}
*/

OmnString
AosSengTestThrd::pickUsedObjid()
{
	const AosStDocPtr doc = pickDoc();
	aos_assert_r(doc, "");
	OmnString objid = doc->objid;
	AosXmlTagPtr xml = mMgr->retrieveXmlDoc(getSiteid(), objid);
	aos_assert_r(xml, "");
	return objid;
}


AosStContainerPtr
AosSengTestThrd::pickUserContainer()
{
	AosStContainer1::Type type = AosStContainer1::eUserCtnr;
	AosStContainerPtr container = pickContainer(type);
	return container;
}	


const AosStUser*
AosSengTestThrd::pickValidUser(const u64 &ctnr_docid) 
{
	AosStContainerPtr ctnr = getContainer(ctnr_docid);
	if (ctnr->mMembers.size() != 0)
	{
		int i = rand() % ctnr->mMembers.size();
		u64 udocid = ctnr->mMembers[i];
		UserMapItr_t itr = mUsers1.find(udocid);
		if (itr == mUsers1.end()) return 0;
		return itr->second;
	}
	return NULL;
}

OmnString
AosSengTestThrd::createInvalidUsername()
{
	// All valid username starts with the thread id.
	// As long as the first letter is not a digit, it is an invalid
	// username.
	int guard = 0;
	while (guard++ < 10)
	{
		OmnString username = OmnRandom::letterStr(eMinUsernameLen, eMaxUsernameLen);
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
AosSengTestThrd::pickNewUsername()
{
	int tid = getThreadId(); 
	OmnString username;
	username << tid << "_username" << getTotalUser();
	return username;
}


AosXmlTagPtr 
AosSengTestThrd::retrieveDoc(const u64 &svrdid)
{
	/* API XXXXXXXXXXXXXX
	u32 siteid = getSiteid();
	OmnString ssid = getCrtSsid();
	AosXmlTagPtr xml= AosSengAdmin::getSelf()->retrieveDocByDocid(
		siteid, ssid, mCrtUrlDocid, svrdid, true);
	return xml;
	*/
	OmnNotImplementedYet;
	return 0;
}


AosXmlTagPtr
AosSengTestThrd::retrieveDocByObjid(const OmnString &objid)
{
	/* API XXXXXXXXXXXXXX
	aos_assert_r(objid != "", 0);
	u32 siteid = getSiteid();
	OmnString ssid = getCrtSsid();
	AosXmlTagPtr xml = AosSengAdmin::getSelf()->retrieveDocByObjid(
		siteid, ssid, mCrtUrlDocid, objid, true);
	//aos_assert_r(xml, 0);
	return xml;
	*/
	OmnNotImplementedYet;
	return 0;
}


void
AosSengTestThrd::setCtnrMembers(const AosStContainerPtr &ctnr, const u64 &ldocid)
{
	ctnr->mMembers.push_back(ldocid);
}


AosStContainer1::Type
AosSengTestThrd::pickContainerType()
{
	int i = rand()%2;
	AosStContainer1::Type type;
	if (i)
	{
		type = AosStContainer1::eDocCtnr;
	}
	else
	{
		type = AosStContainer1::eUserCtnr;
	}
	return type;
}


void
AosSengTestThrd::setSession(
		const OmnString &ssid, 
		const u64 &userid, 
		const AosXmlTagPtr &userdoc,
		const u64 &urldocid)
{
	mCrtSsid = ssid;
	mCrtUserid = userid;
	mCrtUserdoc = userdoc;
	mCrtUrlDocid = urldocid;
	aos_assert(userdoc);
	mCrtCid = userdoc->getAttrStr(AOSTAG_CLOUDID);

	if (mCrtCid == mMgr->getRootCid())
	{
		mMgr->setRootSsid(ssid);
	}
}


void
AosSengTestThrd::resetSession()
{
	mCrtSsid = "";
	mCrtUserid = 0;
	mCrtUserdoc = 0;
	mCrtCid = "";
	mCrtUrlDocid = 0;
	// There is no session, which means it is not a logged in user.
	// Treat it as a guest.
	mCrtCid = AOSCLOUDID_GUEST;
	mCrtUserid = AOSDOCID_GUEST;
}


bool			
AosSengTestThrd::isUserValid(const AosStContainerPtr &container, const u64 &user_docid)
{
	aos_assert_r(container, false);
	for (u32 i=0; i<container->mMembers.size(); i++)
	{
		if (user_docid == container->mMembers[i]) return true;
	}
	return false;
}


u64
AosSengTestThrd::pickUser(const AosStContainerPtr &container)
{
	aos_assert_r(container, 0);
	if (container->mMembers.size() == 0) return AOS_INVDID;
	int idx = rand() % container->mMembers.size();
	return container->mMembers[idx];
}


bool
AosSengTestThrd::addUser(const AosXmlTagPtr &userdoc, const AosRundataPtr &rdata)
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
AosSengTestThrd::addUser(
		const u64 &local_docid,
		const AosXmlTagPtr &userdoc)
{
	aos_assert_r(local_docid > 0, false);
	aos_assert_r(userdoc, false);

	aos_assert_r(addDoc1(local_docid, userdoc), false);
	
	aos_assert_r(userdoc->getAttrStr(AOSTAG_CLOUDID)!="", false);

	AosStUser *usr = new AosStUser;
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
	const AosStDocPtr parentdoc = getDocByObjid(pctnr);
	aos_assert_r(parentdoc, false);
	aos_assert_r(parentdoc->locdid, false);
AosStDocPtr pd = getDocByLocalDid(parentdoc->locdid);
aos_assert_r(pd, false);
	usr->parent_locdid = parentdoc->locdid;

	// Find the container
	u64 pt_locdid = parentdoc->locdid;
	CtnrMapItr_t item = mCtnrs1.find(pt_locdid);
	AosStContainerPtr ctnr;
	if (item != mCtnrs1.end()) ctnr = item->second;
	aos_assert_r(ctnr, false);
	aos_assert_r(!userInContainer(ctnr, local_docid), false);
	ctnr->mMembers.push_back(local_docid);
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
AosSengTestThrd::userInContainer(const AosStContainerPtr &ctnr, const u64 &user_locdid)
{
	aos_assert_r(ctnr, false);
	for (u32 i=0; i<ctnr->mMembers.size(); i++)
	{
		if (ctnr->mMembers[i] == user_locdid) return true;
	}

	return false;
}




OmnString
AosSengTestThrd::getRootSsid()
{
	return mMgr->getRootSsid();
}


bool
AosSengTestThrd::loginAsRoot()
{
	//multi-Thread	
	OmnString ssid = mMgr->getRootSsid();
	u64 userid = mMgr->getRootDocid();
	AosXmlTagPtr userdoc = mMgr->getRootUserDoc();
	u64 urldocid = mMgr->getRootUrlDocid();
	setSession(ssid, userid, userdoc, urldocid);
	return true;
}

OmnString
AosSengTestThrd::pickNewCtnrObjid()
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
}


OmnString
AosSengTestThrd::getNewUserName()
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
/*
AosStAccessRecord*
AosSengTestThrd::getLocalAccessRecord(const OmnString &objid)
{	
	AosStAccessRecord* ar = mObjidArcds[objid]; 
	if (ar) aos_assert_r(ar->mObjid == objid, false);
	return ar;
}
*/


AosStAccessRecord*
AosSengTestThrd::getLocalAccessRecord(const u64 &local_docid)
{	
	AosU642StArcdItr_t itr = mLocalDidArcds.find(local_docid);
	AosStAccessRecord* ar = 0;
	if (itr != mLocalDidArcds.end()) ar = itr->second;
	if (ar) aos_assert_r(ar->mOwnLocdid == local_docid, false);
	return ar;
}



bool
AosSengTestThrd::modifyObj(
		const AosXmlTagPtr &doc, 
		const bool check_error)
{
	/* API XXXXXXXXXXX
	aos_assert_r(doc, false);
	OmnString data = doc->toString();
	bool rslt =  AosSengAdmin::getSelf()->sendModifyReq(
			getSiteid(), getCrtSsid(), mCrtUrlDocid, data, mRundata, check_error);
	return rslt;
	*/
	OmnNotImplementedYet;
	return false;
}

bool
AosSengTestThrd::modifyARToServer(const AosXmlTagPtr &doc)
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
AosSengTestThrd::randomLogin()
{
	/* API XXXXXXXXXXXXX
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
	AosStDocPtr doc = getDocByLocalDid(user->parent_locdid);
	aos_assert_r(doc, false);

	OmnString ctnr_objid = doc->objid;
	aos_assert_r(ctnr_objid !="", false);

	OmnString ssid; 
	u64 userid;
	AosXmlTagPtr userdoc;
	u64 urldocid;
	bool success = AosSengAdmin::getSelf()->login(user->username, user->passwd,
							ctnr_objid, ssid, urldocid, userid, userdoc, getSiteid(), user->cid);

	aos_assert_r(success, false);
	aos_assert_r(ssid != "", false);
	aos_assert_r(userdoc, false);
	setSession(ssid, userid, userdoc, urldocid);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosSengTestThrd::logout()
{
	mCrtSsid = "";
	mCrtUrlDocid = 0;
	mCrtUserid = 0;
	mCrtUserdoc = 0;
	mCrtCid =""; 
	// There is no session, which means it is not a logged in user.
	// Treat it as a guest.
	mCrtCid = AOSCLOUDID_GUEST;
	mCrtUserid = AOSDOCID_GUEST;
	return true;
}


bool
AosSengTestThrd::login(const OmnString &cid)
{
	/* API XXXXXXXXXXX
	// This function logins the user 'cid'. 
	
	// 1. Retrieve local user record
	aos_assert_r(cid !="", false);
	u64 local_docid = getLocalidByCid(cid);
	aos_assert_r(local_docid, false);
	AosStUser *user = getUser(local_docid);
	aos_assert_r(user, false);
	
	// 2. Retrieve its parent local doc 
	AosStDocPtr parent_doc = getDocByLocalDid(user->parent_locdid);
	aos_assert_r(parent_doc, false);
	OmnString ctnr_objid = parent_doc->objid;
	aos_assert_r(ctnr_objid !="", false);

	aos_assert_r(user->cid == cid, false);

	OmnString ssid;
	u64 userid;
	AosXmlTagPtr userdoc;
	OmnString username ="";
	u64 urldocid;
	bool success = AosSengAdmin::getSelf()->login(username, user->passwd,
							ctnr_objid, ssid, urldocid, userid, userdoc, getSiteid(), user->cid);

	aos_assert_r(success, false);
	aos_assert_r(ssid != "", false);
	aos_assert_r(userdoc, false);
	setSession(ssid, userid, userdoc, urldocid);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosSengTestThrd::removeDocFromServer(const AosStDocPtr &doc1)
{
	/* API XXXXXXXXXXX
	// This function deletes the doc 'doc1' from the server.
	aos_assert_r(doc1->svrdid, false);
	bool rslt =  AosSengAdmin::getSelf()->removeDocFromServer(
			getSiteid(), getCrtSsid(), mCrtUrlDocid, "", doc1->svrdid);
	if (rslt)
	{
		OmnString server_docid;
		server_docid << doc1->svrdid;
		AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByDocid(
				 getSiteid(), getCrtSsid(), mCrtUrlDocid, doc1->svrdid, true);
		aos_assert_r(!doc, false);
		aos_assert_r(removeDocToFile(doc1->locdid), false);
		aos_assert_r(isDeleteToFile(doc1->locdid),false);
		aos_assert_r(removeDoc(doc1->locdid, doc1->objid, doc1->svrdid), false);
		aos_assert_r(!pickDoc(doc1->locdid), false);
		aos_assert_r(!getDocByObjid(doc1->objid), false);
		aos_assert_r(getServeridByLocalid(server_docid) == 0, false);
	}
	return rslt;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosSengTestThrd::removeDocFromServer(const AosXmlTagPtr &doc)
{
	/* API XXXXXXXXXXX
	// This function deletes the doc 'doc1' from the server.
	OmnString objid  = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid != "", false);
	bool rslt =  AosSengAdmin::getSelf()->removeDocFromServer(
			getSiteid(), getCrtSsid(), mCrtUrlDocid, objid, 0);
	if (rslt)
	{
		AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByObjid(
				 getSiteid(), getCrtSsid(), mCrtUrlDocid, objid, true);
		aos_assert_r(!doc, false);
	}
	return rslt;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosSengTestThrd::isRoot()
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
AosSengTestThrd::createArdObjid(const u64 docid)
{
	OmnString objid = AOSOBJIDPRE_ACCESS_RCD;
	objid << "_" << docid;
	return objid;
}


u64
AosSengTestThrd::JudgeLocalDocid(const AosXmlTagPtr &doc)
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
AosSengTestThrd::checkDocThreadId(const AosXmlTagPtr &doc)
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
AosSengTestThrd::getUserCid(const u64 &user_localid)
{
	AosStUser* user_doc = getUser(user_localid);
	if (!user_doc) return "";
	return user_doc->cid;
}

bool
AosSengTestThrd::loginAsNormalUser()
{
    return true;
}


bool 	
AosSengTestThrd::canDoOpr(const u64 &local_did, const AosSecOpr::E opr)
{
	// This function checks whether the user has the permission to perform
	// the operation.
	// In our security model, it works as below:
	// 	1. If it is root, return true.
	// 	2. If it is admin, return true.
	// 	3. It converts to a user defined operation. If yes, do 
	// 	   the following:
	// 	   a. If there is a domain operation access record, use it.
	// 	   b. If not granted, it checks whether it allows personal 
	// 	      operation access record.
	// 	   c. If yes, it uses the personal operation access record.
	// 	   d. If not granted, it checks whether it should rollback.
	// 	   e. If yes, it uses the normal access controls. 
	// 	4. It uses the normal access controls.
	AosStDocPtr local_doc = getDocByLocalDid(local_did);
	aos_assert_r(local_doc, false);
	AosStContainerPtr ctnr = getContainer(local_doc->parent_locdid);
	aos_assert_r(ctnr, false);

	AosSengTestThrdPtr thisptr(this, false);
	return ctnr->canDoOpr(local_doc, opr, thisptr);
}


AosStAccessRecord *
AosSengTestThrd::getAcd(
		const AosStDocPtr &doc, 
		bool &is_parent)
{
	/* API XXXXXXXXXXX
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
	u64 parent_locdid = doc->parent_locdid;
	AosStDocPtr parent_doc = getDocByLocalDid(parent_locdid);
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
	AosStDocPtr dd = 0;
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
	// bool rslt;// = AosSengAdmin::getSelf()->createAccessRcd(mRundata->getSiteid(), raw_arcd, 
	// 		//dd->objid, getRootSsid());
	// aos_assert_r(rslt, 0);
	svr_ard = retrieveAccessRcd(doc->svrdid, false, false);
	aos_assert_r(svr_ard, 0);
	arcd = createAccessRecord(svr_ard, doc->locdid);
	aos_assert_r(arcd, 0);
	return arcd;
	*/
	OmnNotImplementedYet;
	return false;
}


AosXmlTagPtr
AosSengTestThrd::createArcd()
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
AosSengTestThrd::checkSaveSdocFlag()
{
	//bool flag = mSaveSdocFlag;
	//mSaveSdocFlag = true;
	//return flag;
	OmnNotImplementedYet;
	return false;
}                                   


void
AosSengTestThrd::addStDoc(const AosStDocPtr &doc)
{
	//OmnScreen << "------docid:" << doc->locdid << endl;
	if (doc->locdid == 102)
		OmnMark;
	mDocs.push_back(doc);
}

AosStDocPtr
AosSengTestThrd::getStDoc(int idx)
{
	return mDocs[idx];
}

void
AosSengTestThrd::addDocByObjid(const OmnString objid, const AosStDocPtr &doc)
{
	mDocsByObjid[objid] = doc;
}

void
AosSengTestThrd::addDocsByLocdid(const u64 &local_docid, const AosStDocPtr &doc)
{
	mDocsByLocdid[local_docid] = doc;
}

void
AosSengTestThrd::addServerDidMap(const u64 &server_docid, const u64 &local_docid)
{
	mServerDidMap[server_docid] = local_docid;
}


OmnString
AosSengTestThrd::pickOtype()
{
	bool generate_new = false;
	if (sgOtypes.size() > sgMaxOTypes)
	{
		if ((rand() % 10000) > 9990) generate_new = true;
	}

	if ((rand() % 100) > 95 || sgOtypes.size() == 0 || generate_new)
	{
		int len = OmnRandom::intByRange(
				2, 5, 70, 
				6, 10, 20, 
				11, 20, 8,
				21, 40, 2);
		OmnString otype = OmnRandom::word(len);
		sgOtypes.push_back(otype);
		return otype;
	}
	return sgOtypes[rand() % sgOtypes.size()];
}


OmnString
AosSengTestThrd::pickStype()
{
	bool generate_new = false;
	if (sgStypes.size() > sgMaxSTypes)
	{
		if ((rand() % 10000) > 9990) generate_new = true;
	}

	if ((rand() % 100) > 95 || sgStypes.size() == 0 || generate_new)
	{
		int len = OmnRandom::intByRange(
				2, 5, 70, 
				6, 10, 20, 
				11, 20, 8,
				21, 40, 2);
		OmnString stype = OmnRandom::word(len);
		sgStypes.push_back(stype);
		return stype;
	}
	return sgStypes[rand() % sgStypes.size()];
}


AosStContainerPtr
AosSengTestThrd::pickValidContainer()
{
	// We need to use the known container as its parent container.
	AosStContainerPtr ctnr = pickContainer();
	if (!ctnr) return 0;

	const AosStDocPtr dd = getDocByLocalDid(ctnr->mLocdid);
	aos_assert_r(dd, 0);
	aos_assert_r(dd->objid == ctnr->mObjid, 0);
	AosXmlTagPtr xml = mMgr->retrieveXmlDoc(getSiteid(), ctnr->mObjid);
	aos_assert_r(xml, 0);
	aos_assert_r(xml->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER, 0);
	if (!isValidObjid(dd->objid))
	{
		return 0;
	}
	//aos_assert_r(isValidObjid(dd->objid), 0);
	return ctnr;
}


bool
AosSengTestThrd::isValidObjid(const OmnString &objid)
{
	if (objid.length() <= 0) return false;
	if (objid.length() <= 5) return true;
	
	// Objid cannot start with '_zt' unless it is
	// '_zt00'
	char *buff = (char *)objid.data();
	if (buff[0] != '_' || buff[1] != 'z' || buff[2] != 't') return true;
	if (buff[3] != 0 || buff[4] != 0) return false;
	return true;
}


OmnString
AosSengTestThrd::pickCid()
{
	AosStUser *user = pickUser();
	if (!user) return "";
	return user->cid;
}


OmnString
AosSengTestThrd::pickInvalidCid()
{
	// In the current implementations, cids are digit strings
	// with minimum 6 digits.
	int guard = 100;
	while (guard--)
	{
		OmnString cid = OmnRandom::digitStr(6, 10);
		AosStUser *user = getUserByCid(cid);
		if (!user) return cid;
	}
	OmnShouldNeverComeHere;
	return "";
}


AosStContainerPtr	
AosSengTestThrd::getRequesterUserDomain()
{
	AosStUser *user = getCrtUser();
	if (!user) return 0;
	return getContainer(user->parent_locdid);
}


AosRundataPtr
AosSengTestThrd::getRundata() const
{
	return mRundata;
}


AosXmlTagPtr
AosSengTestThrd::getOlddoc() const 
{
	return mOlddoc;
}


AosXmlTagPtr
AosSengTestThrd::getNewdoc() const 
{
	return mNewdoc;
}


AosXmlTagPtr
AosSengTestThrd::getUserOprArd() const
{
	OmnNotImplementedYet;
	return 0;
}


bool			
AosSengTestThrd::removeCid(OmnString &objid)
{
	OmnString cid = getCrtCid();
	if (cid == "") return true;
	OmnString prefix, cc;
	AosObjid::decomposeObjid(objid, prefix, cc);
	if (cc == cid) 
	{
		objid = prefix;
	}
	return true;
}


AosXmlTagPtr		
AosSengTestThrd::createDocOnServer(
		const AosXmlTagPtr &doc, 
		const OmnString &resolve_objid,
		const OmnString &saveas)
{
	// This function creates the doc 'doc' on the remote server.
	// If successful, it returns the doc being created. Note that
	// this function only tries to create the doc. It does not
	// update any thread member data.
	/* API XXXXXXXXXXX
	OmnString server_docid;
	AosXmlTagPtr created = AosSengAdmin::getSelf()->createDoc(
								doc, 
								getCrtSsid(), 
								getUrlDocid(), 
								mCrtCid, 
								resolve_objid, 
								saveas, 
								mRundata);

	if (created)
	{
		OmnString objid = created->getAttrStr(AOSTAG_OBJID);
		aos_assert_r(objid != "", 0);
		AosXmlTagPtr dd = AosSengAdmin::getSelf()->retrieveDocByObjid(
								getSiteid(), 
								getCrtSsid(), 
								getUrlDocid(), 
								objid, 
								true);
		aos_assert_r(dd, 0);

		u64 rdocid = doc->getAttrU64(AOSTEST_DOCID, AOS_INVDID);
		u64 sdocid = dd->getAttrU64(AOSTEST_DOCID, AOS_INVDID);
		aos_assert_r(rdocid == sdocid, 0);
		aos_assert_r(dd->getAttrStr(AOSTAG_OBJID) == objid, 0);
		aos_assert_r(dd->getAttrU64(AOSTAG_DOCID, 0) == 
				doc->getAttrU64(AOSTAG_DOCID, 0), 0);
		aos_assert_r(doc->getAttrU64(AOSTEST_DOCID, AOS_INVDID) ==
					dd->getAttrU64(AOSTEST_DOCID, AOS_INVDID), 0);
	}
	return created;
	*/
	OmnNotImplementedYet;
	return 0;
}	


AosXmlTagPtr	
AosSengTestThrd::getOwnerAccountDoc(const OmnString &objid)
{
	aos_assert_r(objid != "", 0);
	AosXmlTagPtr doc = retrieveDocByObjid(objid);
	aos_assert_r(doc, 0);
	OmnString owner = doc->getAttrStr(AOSTAG_CREATOR);
	aos_assert_r(owner != "", 0);
	return retrieveDocByCloudid(owner);
}


AosXmlTagPtr	
AosSengTestThrd::getOwnerDomainDoc(const OmnString &objid)
{
	AosXmlTagPtr acct_doc = getOwnerAccountDoc(objid);
	aos_assert_r(acct_doc, 0);
	OmnString ctnr_objid = acct_doc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(ctnr_objid != "", 0);
	return retrieveDocByObjid(ctnr_objid);
}


AosXmlTagPtr
AosSengTestThrd::retrieveDocByCloudid(const OmnString &cid)
{
	/* API XXXXXXXXXXX
	return AosSengAdmin::getSelf()->retrieveDocByCloudid(getSiteid(),
			cid, mCrtUrlDocid);
	*/
	OmnNotImplementedYet;
	return 0;
}


u32
AosSengTestThrd::getSiteid() const
{
	return mRundata->getSiteid();
}


