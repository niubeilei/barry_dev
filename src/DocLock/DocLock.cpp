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
// 06/07/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DocLock/DocLock.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "Porting/Sleep.h"
//#include "SEInterfaces/TransServerObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Security/SecurityMgr.h"
#include "Thread/CondVar.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"
#include "Timer/TimerMgr.h"
#include <algorithm>

//#include "Tester/Test.h"
//#include "Tester/TestMgr.h"
//#include "Tester/Test.h"
//#include "DocLock/Tester/DocLockTester.h"

OmnSingletonImpl(AosDocLockSingleton,
				 AosDocLock,
				 AosDocLockSelf,
				"AosDocLock");

static bool						sgSanityCheck = true;
static OmnMutex					sgLock;

AosDocLock::AosDocLock()
:
mLock(OmnNew OmnMutex()),
mNumReqs(0),
mCondVar(OmnNew OmnCondVar())
{
	AosDocLockPtr thisptr(this, false);
	for (int i = 0; i< 1000; i++)
	{
		vector<AosStLockPtr> *v = OmnNew vector<AosStLockPtr>;
		mCtnrs.push(v);
	}

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "DocLockThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosDocLock::~AosDocLock()
{
	vector<AosStLockPtr> *v = 0;
	while(!mCtnrs.empty())
	{
		v = mCtnrs.front();
		mCtnrs.pop();
		OmnDelete v;
	}
}

//extern  AosDocLockTester  * docLockTester;

bool
AosDocLock::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		AosStLockReqPtr req = getReqFromQueue();
		if (mNumReqs == 0)
		{
			aos_assert_rl(!req, mLock, false);
			mCondVar->wait(mLock);
			mThreadStatus = true;
			mLock->unlock();
			continue;
		}
		
		mThreadStatus = true;
		
		aos_assert_rl(req, mLock, false);
		mNumReqs --;
		mLock->unlock();
		switch(req->reqtype)
		{
		case AosStLockReq::eReadUnLockReq:
//			 if (docLockTester)
//			 {
//				docLockTester->waitThread();
//			 }
			 readUnLock(req->rdata, req->docid, req->lockid);
//			 if (docLockTester && rslt)
//			 {
//				 docLockTester->timeout(req->rdata);
//			 }
//			 if (docLockTester)
//			 {
//				 docLockTester->signalThread();
//			 }
			 break;

		case AosStLockReq::eWriteUnLockReq:
//			 if (docLockTester)
//		 	 {
//				docLockTester->waitThread();
//			 }
			 writeUnLock(req->rdata, req->docid, req->lockid);
//			 if (docLockTester && rslt)
//			 {
//				 docLockTester->timeout(req->rdata);
//			 }
//			 if (docLockTester)
//			 {
//				 docLockTester->signalThread();
//			 }
			 break;

		default:
			 OmnAlarm << "Unrecognized doc request type: " << req->reqtype << enderr;
		}
		return true;
	}
	return true;
}


bool
AosDocLock::timeout(const u64 &timerid, const OmnString &udata, const AosRundataPtr &rdata)
{
	//cout << "req->lockid: "<< req->lockid << "  udata:" << udata << endl; 
	//AosStLockReqPtr req = OmnNew AosStLockReq;
	//AosStLockReq::REQ reqtype = (AosStLockReq::REQ)atoll(rdata->getArg1(AOSARG_REQUEST_TYPE));
	//aos_assert_r(reqtype > AosStLockReq::eInvalid && reqtype < AosStLockReq::eMax, false);
	//req->reqtype = reqtype;
	//req->docid = atoll(rdata->getArg1(AOSARG_DOCID));
	//aos_assert_r(req->docid, false);
	//req->lockid = atoll(rdata->getArg1(AOSARG_LOCKID));

	aos_assert_r(udata != "", false);
	vector<OmnString> attrnames;
	bool finished = false;
	attrnames.clear();
	AosStrSplit::splitStrByChar(udata.data(), ", ", attrnames, 100, finished);

	aos_assert_r(attrnames.size() >= 4, false);
	u64 type = atoll(attrnames[0].data());
	OmnString docid_str = attrnames[1]; 
	u64 docid = docid_str.parseU64(0);
	aos_assert_r(docid, false);

	OmnString lockid_str = attrnames[2];
	u64 lockid = lockid_str.parseU64(0);
	aos_assert_r(lockid, false);

	u32 siteid = atoll(attrnames[3].data());
	aos_assert_r(siteid, false);

	AosStLockReqPtr req = OmnNew AosStLockReq;
	AosStLockReq::REQ reqtype = (AosStLockReq::REQ)type;
	aos_assert_r(reqtype > AosStLockReq::eInvalid && reqtype < AosStLockReq::eMax, false);
	req->reqtype = reqtype;
	req->docid = docid;
	req->lockid = lockid;
	rdata->setSiteid(siteid);

	req->rdata = rdata;
	mLock->lock();
	mNumReqs ++;
	addReqToQueue(req);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


void
AosDocLock::addReqToQueue(const AosStLockReqPtr &req)
{
//	cout << "reqptr: "<< req.getPtr() <<" : "<<  req->lockid << endl;
	mReq.push(req);
	//aos_assert(mReq.size() == mNumReqs);
	//cout << "mReq.size: " << mReq.size() << " : " << "mNumReqs:" << mNumReqs<< endl;
}


bool
AosDocLock::createTimerPriv(
		const AosStLockReq::REQ &type,
		const u32 lock_timer,
		const u64 &docid,
		const u64 &lockid,
		const AosRundataPtr &rdata)
{
	//OmnString t, d, l;
	//t << type;
	//aos_assert_r(t != "", false);
	//rdata->setArg1(AOSARG_REQUEST_TYPE, t);
	//d << docid;
	//aos_assert_r(d != "", false);
	//rdata->setArg1(AOSARG_DOCID, d);
	//l << lockid;
	//aos_assert_r(l != "", false);
	//rdata->setArg1(AOSARG_LOCKID, l);
	u64 timerid = 0;
	AosDocLockPtr thisptr(this, false);
	OmnString udata;
	udata << type << "," << docid << "," << lockid << "," << rdata->getSiteid(); 
	bool rslt = AosTimerMgr::getSelf()->
		addTimer(0, lock_timer, timerid, udata, thisptr, rdata);
	return rslt;
}


AosStLockReqPtr 
AosDocLock::getReqFromQueue()
{
	AosStLockReqPtr l = 0;
	if (!mReq.empty())
	{
		l = mReq.front();
		mReq.pop();
	}
	return l;
}


bool
AosDocLock::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}

/*
bool
AosDocLock::checkThread(OmnString &err,  const int thrdLogicId) const
{
	return true;
}
*/

vector<AosStLockPtr>*
AosDocLock::getVectorPriv()
{
	vector<AosStLockPtr>* v;
	if (mCtnrs.empty())
	{
		v = OmnNew vector<AosStLockPtr>;
		mCtnrs.push(v);
	}
	else
	{
		v = mCtnrs.front();
		mCtnrs.pop();
		v->clear();
	}
	return v;
}

void
AosDocLock::returnVectorPriv(vector<AosStLockPtr>* v)
{
	mCtnrs.push(v);
}


bool
AosDocLock::start()
{
	return true;
}

bool
AosDocLock::stop()
{
	return true;
}


bool
AosDocLock::config(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosDocLock::canWriteLockPriv(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function determines whether the doc 'docid' 
	// can be write locked. It can be write locked if and only if
	// no one read locked and write locked the doc. 
	//Site, Linda
	//AosU642LockPairItr_t itermap = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());	
	AosU642LockPairItr_t itermap = mDocLock.find(key);
	if (itermap == mDocLock.end()) 
	{
		// No one locked the doc. 
		return true;
	}

	// Someone locked the doc
	vector<AosStLockPtr> *read = itermap->second.first;
	vector<AosStLockPtr> *write = itermap->second.second;
	if ((!write ||write->size() == 0)  && (!read || read->size() == 0))
	{
		if (read)returnVectorPriv(read);
		if (write)returnVectorPriv(write);
		// Site, Linda
		//mDocLock.erase(docid);
		mDocLock.erase(key);
		return true;
	}

	//create resp
	OmnString results;
	if (read)
	{
		vector<AosStLockPtr>::iterator iter = read->begin();
		for (;iter != read->end(); ++iter)
		{
			AosStLockPtr ll = *iter;
			aos_assert_r(ll, false);
			aos_assert_r(ll->req_userid, false);
			results << "<record " << "zky_doclock_owner " << "=\"" << ll->req_userid << "\"/>";
		}
	}

	if (write)
	{
		if (write->size() != 0)
		{
			if ((*write)[0]->req_userid == rdata->getUserid())
			{
				results << "You have locked some doc!";
			}
			else
			{
				results << "<record " << "zky_doclock_owner" << (*write)[0]->req_userid << "\"/>";
			}
		}
	}
	rdata->setResults(results);
	return false;	
}


bool
AosDocLock::canReadLockPriv(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether we can read lock the doc 'docid'. 
	// A doc can be read locked if it is not write locked. 
	// Site, Linda
	//AosU642LockPairItr_t itermap = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());
	AosU642LockPairItr_t itermap = mDocLock.find(key);
	if (itermap == mDocLock.end()) 
	{
		return true;
	}

	vector<AosStLockPtr> *write = itermap->second.second;
	if (!write || write->size() == 0)
	{
		u64 userid = rdata->getUserid();
		//bool found = false;
		vector<AosStLockPtr> *read = itermap->second.first;
		aos_assert_r(read, false);
		vector<AosStLockPtr>::iterator iter = read->begin();
		for (;iter != read->end(); ++iter)
		{
			AosStLockPtr dd = *iter;
			aos_assert_r(dd, false);
			aos_assert_r(dd->req_userid, false);
			if (dd->req_userid == userid)
			{
				rdata->setResults("You have locked some doc!");
				return false;
			}
		}
		return true;
	}
	OmnString resp;
	resp << "<record " << "zky_doclock_owner" << "=\"" << (*write)[0]->req_userid << "\"/>";
	rdata->setResults(resp);
	return false;
}


bool
AosDocLock::readLockPriv(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const u32 &lock_timer,
		const u64 &transid,
		u64 &lockid)
{
	// Someone tries to read lock the doc 'docid'
	aos_assert_r(docid, false);

	aos_assert_r(sanityCheck3(), false);
	//Site, Linda
	//AosU642LockPairItr_t itercheck  = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());
	AosU642LockPairItr_t itercheck  = mDocLock.find(key);

	AosStLockPtr lock = OmnNew AosStLock;
	lockid = transid;
	lock->lockid = lockid;
	lock->req_userid = rdata->getUserid();
	lock->timer = lock_timer; 
	//lock->transid = transid;

	// Retrieve the read lock vector
	vector<AosStLockPtr> *read = 0;
	if (itercheck != mDocLock.end())
	{
		read = itercheck->second.first;
		aos_assert_r(read, false);
		aos_assert_r(!itercheck->second.second, false);
	}
	else
	{
		read = getVectorPriv();
		aos_assert_r(read, false);
	}

	// Add the lock to the read lock vector, and add an entry to the map.
	read->push_back(lock);	
	aos_assert_r(read->size() != 0, false);
	//Site, Linda
	// vector<AosStLockPtr> *write = getVectorPriv();
	if (itercheck == mDocLock.end())
	{
		vector<AosStLockPtr> *write = 0;
		//Site, Linda
		//mDocLock.insert(make_pair(docid, make_pair(read, write)));
		mDocLock.insert(make_pair(key, make_pair(read, write)));
	}
	aos_assert_r(sanityCheck(rdata, itercheck, docid), false);
	aos_assert_rr(createTimerPriv(AosStLockReq::eReadUnLockReq, lock_timer, docid, lockid, rdata), rdata, false);
	return true;
}


bool
AosDocLock::sanityCheck(
			const AosRundataPtr &rdata,
			AosU642LockPairItr_t itercheck,
			const u64 &docid)
{
	if (!sgSanityCheck) return true;

	//for(itercheck = mDocLock.begin(); itercheck != mDocLock.end(); ++ itercheck)
	//{
		//cout<<"1: "<< itercheck->first << endl;;
		//cout<<"2: "<< itercheck->second <<endl;;
	//}
	
	// check
	// Site, Linda
	//AosU642LockPairItr_t itercheck2  = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());
	AosU642LockPairItr_t itercheck2  = mDocLock.find(key);
	aos_assert_r(itercheck2 != mDocLock.end(),false);

	AosU642LockPairItr_t itercheck3;
	for (itercheck3 = mDocLock.begin(); itercheck3 != mDocLock.end(); ++itercheck3)
	{
		vector<AosStLockPtr> *read = itercheck3->second.first;
		vector<AosStLockPtr> *write = itercheck3->second.second;
		if (read)
		{
			aos_assert_r(read->size() != 0, false);
			aos_assert_r(!write, false);
		}
		if (write)
		{
			aos_assert_r(write->size() != 0, false);
			aos_assert_r(!read, false);
		}
	}
	return true;
}

bool
AosDocLock::sanityCheck2(
			const AosRundataPtr &rdata, 
			AosU642LockPairItr_t itercheck,
			const u64 &docid)
{
	// 1. If it is read-locked, write-lock must be null. 
	// 2. If it is write-locked, read-lock must be null. 
	// 3. For any entry in the map, either the read-lock is not null, 
	//    or the write-lock is not null. There must be at least one
	//    lock that is not null. 
	// 3. If a lock timedout, it should not be in the map. 
	if (!sgSanityCheck) return true;

	//Site, Linda
	AosHashKey key(docid, rdata->getSiteid());
	AosU642LockPairItr_t itercheck2  = mDocLock.find(key);
	//AosU642LockPairItr_t itercheck2  = mDocLock.find(docid);
	aos_assert_r(itercheck2 == mDocLock.end(),false);

	AosU642LockPairItr_t itercheck3;
	for (itercheck3 = mDocLock.begin(); itercheck3 != mDocLock.end(); ++itercheck3)
	{
		vector<AosStLockPtr> *read = itercheck3->second.first;
		vector<AosStLockPtr> *write = itercheck3->second.second;
		if (read)
		{
			aos_assert_r(read->size() != 0, false);
			aos_assert_r(!write, false);
		}
		if (write)
		{
			aos_assert_r(write->size() != 0, false);
			aos_assert_r(!read, false);
		}
	}
	return true;
}

bool
AosDocLock::sanityCheck3()
{
	// 1. If it is read-locked, write-lock must be null. 
	// 2. If it is write-locked, read-lock must be null. 
	// 3. For any entry in the map, either the read-lock is not null, 
	//    or the write-lock is not null. There must be at least one
	//    lock that is not null. 
	// 3. If a lock timedout, it should not be in the map. 
	if (!sgSanityCheck) return true;

	AosU642LockPairItr_t itercheck3;
	for (itercheck3 = mDocLock.begin(); itercheck3 != mDocLock.end(); ++itercheck3)
	{
		vector<AosStLockPtr> *read = itercheck3->second.first;
		vector<AosStLockPtr> *write = itercheck3->second.second;
		if (read)
		{
			aos_assert_r(read->size() != 0, false);
			aos_assert_r(!write, false);
		}
		if (write)
		{
			aos_assert_r(write->size() != 0, false);
			aos_assert_r(!read, false);
		}
	}
	return true;
}


bool
AosDocLock::writeLockPriv(
				const AosRundataPtr &rdata,
				const u64 &docid,
				const u32 &lock_timer,
				const u64 &transid,
				u64 &lockid)
{
	// The caller should have checked it can be write locked. 
	// This function write-locks the doc 'docid'. 
	aos_assert_r(sanityCheck3(), false);
	aos_assert_r(docid, false);
	AosStLockPtr lock = OmnNew AosStLock;
	lockid = transid;
	lock->lockid = lockid;
	lock->req_userid = rdata->getUserid();
	lock->timer = lock_timer;
	//lock->transid = transid;

	// check
	// Site, Linda
	//AosU642LockPairItr_t itercheck  = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());
	AosU642LockPairItr_t itercheck  = mDocLock.find(key);
	aos_assert_r(itercheck == mDocLock.end(),false);
	
	vector<AosStLockPtr> *write = getVectorPriv();
	aos_assert_r(write, false);
	write->push_back(lock);
	aos_assert_r(write->size()!=0, false);
	vector<AosStLockPtr> *read = 0;
	//Site, Linda
	//mDocLock.insert(make_pair(docid, make_pair(read, write)));
	mDocLock.insert(make_pair(key, make_pair(read, write)));
	aos_assert_r(sanityCheck(rdata, itercheck, docid), false);
	aos_assert_rr(createTimerPriv(AosStLockReq::eWriteUnLockReq, lock_timer, docid, lockid, rdata), rdata, false);
	return true;
}


bool
AosDocLock::readUnLockPriv(
			const AosRundataPtr &rdata,
			const u64 &docid, 
			const u64 &lockid,
			const AosU642LockPairItr_t &itercheck)
{
	aos_assert_r(docid, false);

	aos_assert_r(sanityCheck3(), false);
	// check
	//AosU642LockPairItr_t itercheck  = mDocLock.find(docid);
	aos_assert_r(itercheck != mDocLock.end(),false);

	aos_assert_r(itercheck->second.first, false);
	aos_assert_r(itercheck->second.first->size() != 0, false);
	aos_assert_r(!itercheck->second.second, false);
	//aos_assert_r(itercheck->second.second->size() ==0, false);
	
	//delete vector entry
	vector<AosStLockPtr> *read = itercheck->second.first;
	aos_assert_r(read, false);
	int vsize = read->size();
	bool found = false;

	vector<AosStLockPtr>::iterator iter = read->begin();
	for (;iter != read->end(); ++iter)
	{
		AosStLockPtr dd = *iter;
		aos_assert_r(dd, false);
		aos_assert_r(dd->lockid, false);
		if (dd->lockid == lockid)
		{
			iter = read->erase(iter);
			found = true;
			break;
		}
	}

	aos_assert_r(found, false);
	int newvsize = read->size();
	aos_assert_r(vsize == newvsize+1, false);

	// finish read lock trans
	u64  transid = lockid;
	aos_assert_r(transid, false);
	vector<u64> trans_ids;
	trans_ids.push_back(transid);
	
	// Ketty 2013/01/22
	// Ketty 2013/07/17
	//AosFinishTrans(trans_ids);	

	if (newvsize == 0) 
	{
		if (itercheck->second.first) returnVectorPriv(itercheck->second.first);
		if (itercheck->second.second) returnVectorPriv(itercheck->second.second);
		//Site, Linda
		//mDocLock.erase(docid);
		AosHashKey key(docid, rdata->getSiteid());
		mDocLock.erase(key);
		aos_assert_r(sanityCheck2(rdata, itercheck, docid), false);
	}
	aos_assert_r(sanityCheck3(), false);
	return true;	
}


bool
AosDocLock::writeUnLockPriv(
				const AosRundataPtr &rdata,
				const u64 &docid,
				const u64 &lockid,
				const AosU642LockPairItr_t &itercheck)
{
	aos_assert_r(docid, false);
	aos_assert_r(sanityCheck3(), false);
	// check
	//AosU642LockPairItr_t itercheck  = mDocLock.find(docid);
	aos_assert_r(itercheck != mDocLock.end(),false);

	aos_assert_r(!itercheck->second.first, false);

	vector<AosStLockPtr> *write = itercheck->second.second;
	aos_assert_r(write, false);
	int vsize = write->size();
	aos_assert_r(vsize == 1, false);
	aos_assert_r(!write->empty(),false);
	write->clear();
	aos_assert_r(write->empty(), false);
	int newvsize = write->size();
	aos_assert_r(newvsize == 0, false);

	u64 transid = lockid;//(*write)[0]->transid;
	// finish write lock trans
	aos_assert_r(transid, false);
	vector<u64> trans_ids;
	trans_ids.push_back(transid);
	
	// Ketty 2013/01/22
	// Ketty 2013/07/17
	//AosFinishTrans(trans_ids);

	if (itercheck->second.first) returnVectorPriv(itercheck->second.first);
	if (itercheck->second.second) returnVectorPriv(itercheck->second.second);
	//Site, Linda
	//mDocLock.erase(docid);
	AosHashKey key(docid, rdata->getSiteid());
	mDocLock.erase(key);
	
	// check
	aos_assert_r(sanityCheck2(rdata, itercheck, docid), false); 
	return true;
}


bool
AosDocLock::findReadLockPriv(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const u64 &lockid,
			bool &found,
			AosU642LockPairItr_t &itermap)
{
	found = false;
	//Site, Linda
	//itermap  = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());
	itermap  = mDocLock.find(key);
	if (itermap == mDocLock.end()) 
	{
		found = false;
		return true;
	}
		
  	vector<AosStLockPtr> *read = itermap->second.first;
    vector<AosStLockPtr> *write = itermap->second.second;
	if (!read || read->size() == 0)
	{
		found = false;
		aos_assert_r(write, false);
		aos_assert_r(write->size()!=0, false);
		return true;
	}
	
	aos_assert_r(!write , false);
	vector<AosStLockPtr>::iterator iter = read->begin();
	for (;iter != read->end(); ++iter)
	{
		AosStLockPtr dd = *iter;
		aos_assert_r(dd, false);
		aos_assert_r(dd->lockid, false);
		if (dd->lockid == lockid)
		{
			found = true;
			return true;
		}
	}

	found = false;
	return true;
}


bool
AosDocLock::findWriteLockPriv(
			const AosRundataPtr &rdata,
			const u64 &docid, 
			const u64 &lockid,
			bool &found,
			AosU642LockPairItr_t &itermap)
{
	found = false;
	//Site, Linda
	//itermap  = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());
	itermap  = mDocLock.find(key);
	if (itermap == mDocLock.end()) return true; 
		
  	vector<AosStLockPtr> *read = itermap->second.first;
    vector<AosStLockPtr> *write = itermap->second.second;
	if (!write || write->size() == 0)return true;

	aos_assert_r(!read, false);
	int vsize = write->size();
	aos_assert_r(vsize == 1, false);
	if ((*write)[0]->lockid == lockid)
	{
		found = true;
		return true;
	}
	return true;	
}


// Request
bool
AosDocLock::readLock(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const u32 &lock_timer,
		const u64 &transid)
{
	// This function tries to read lock the doc 'docid'. If the doc is 
	// write locked, it cannot read lock the doc. Otherwise, it checks
	// whether the requester has the right to read lock the doc. If not, 
	// it responds with an error code. Otherwise, it read locks the doc.
	mLock->lock();

	OmnString contents = "<Contents>";
	bool rslt = canReadLockPriv(docid, rdata);
	if (!rslt)
	{
		// Need to construct <Contents> to contain the information about
		// who are locking the doc.
		rdata->setError() << "Failed check read lock";
		contents << "<record " << AOSTAG_DOCID << "=\"" << docid << "\" "
			<< "zky_status" << "=\"" << "failure" << "\" >" 
			<<rdata->getResults() <<"</record> </Contents>"; 
		rdata->setResults(contents);
		mLock->unlock();
		return true;
	}

	// Check whether the request has the right to lock the doc. 
	//if (!AosSecurityMgr::getSelf()->checkReadLock(docid, rdata))
	//{
	//	// It does not have the right to lock the doc.
	//	rdata->setError() << "Access Denied!";
	//	mLock->unlock();
	//	return false;
	//}

	// It can read lock the doc. 
	u64 lockid = 0;
	aos_assert_rl(readLockPriv(rdata, docid, lock_timer, transid, lockid), mLock, false);
	contents <<"<record "<<AOSTAG_LOCK_DID << "=\""<<lockid<<"\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" "
		<< "zky_status" << "=\"" << "success" << "\" >"
		<< "</record></Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	mLock->unlock();
	return true;
}


// Request
bool
AosDocLock::readUnLock(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const u64 &lockid)
{
	mLock->lock();
	bool found = false;
	AosU642LockPairItr_t itermap; 
	bool rslt = findReadLockPriv(rdata, docid, lockid, found, itermap);
	aos_assert_rl(rslt, mLock, false);
	OmnString contents = "<Contents>";
	if (!found)
	{
		// Either the lock is invalid or the lock is expired.
		rdata->setError() <<"Docid not found. docid:"<< docid;
		contents << "<record " << AOSTAG_DOCID << "=\"" << docid << "\" "
			 << "zky_status" << "=\"" << "failure" << "\" >"
			 << rdata->getErrmsg()<<"</record> </Contents>"; 
		rdata->setResults(contents);
		mLock->unlock();
		return true;
	}
	
	aos_assert_rl(itermap != mDocLock.end(), mLock, false); 
	rslt = readUnLockPriv(rdata, docid, lockid, itermap);
	aos_assert_rl(rslt, mLock, false);
	contents <<"<record " << AOSTAG_DOCID << "=\"" << docid << "\" "
		<< AOSTAG_LOCK_DID << "=\"" << lockid << "\" " 
		<< "zky_status" << "=\"" << "success" << "\" >"
		<< "Success.</record></Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	mLock->unlock();
	return true;
}


// Request
bool
AosDocLock::writeLock(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const u32 &lock_timer,
			const u64 &transid)
{
	mLock->lock();
	OmnString contents = "<Contents>";
	bool rslt = canWriteLockPriv(docid, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed check read lock";
		contents << "<record " << AOSTAG_DOCID << "=\"" << docid << "\" "
			 << "zky_status" << "=\"" << "failure" << "\" >"
			 << rdata->getResults() <<"</record></Contents>"; 
		rdata->setResults(contents);
		mLock->unlock();
		return true;
	}

	// Check whether the request has the right to lock the doc. 
	//if (!AosSecurityMgr::getSelf()->checkWriteLock(docid, rdata))
	//{
	//	// It does not have the right to lock the doc.
	//	rdata->setError() << "Access Denied!";
	//	mLock->unlock();	
	//	return true;
	//}

	u64 lockid = 0;
	rslt = writeLockPriv(rdata, docid, lock_timer, transid, lockid);
	aos_assert_rl(rslt, mLock, false);

	contents <<"<record "<<AOSTAG_LOCK_DID << "=\""<<lockid<<"\" "
		 << AOSTAG_DOCID << "=\"" << docid << "\" "
		 << "zky_status" << "=\"" << "success" << "\" >"
		 << "</record></Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	mLock->unlock();
	return true;
}


// Request
bool
AosDocLock::writeUnLock(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const u64 &lockid)
{
	mLock->lock();
	bool found;
	AosU642LockPairItr_t itermap;
	bool rslt = findWriteLockPriv(rdata, docid, lockid, found, itermap);
	aos_assert_rl(rslt, mLock, false);
	OmnString contents = "<Contents>";
	if (!found)
	{
		rdata->setError() <<"Docid not found. docid:"<< docid;
		contents << "<record " << AOSTAG_DOCID << "=\"" << docid << "\" "
			<< "zky_status" << "=\"" << "failure" << "\" >"
			<< rdata->getErrmsg()<<"</record> </Contents>"; 
		rdata->setResults(contents);
		mLock->unlock();
		return true;
	}
	aos_assert_rl(itermap != mDocLock.end(), mLock,  false); 
	rslt = writeUnLockPriv(rdata, docid, lockid, itermap);
	aos_assert_rl(rslt, mLock, false);
	contents <<"<record "<< AOSTAG_DOCID <<"=\"" << docid << "\" "
		<< AOSTAG_LOCK_DID << "=\"" << lockid << "\" "
		<< "zky_status" << "=\"" << "success" << "\" >"
		<<"Success.</record></Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	mLock->unlock();
	return true;
}

//checkModify
bool
AosDocLock::checkModify(
			const AosRundataPtr &rdata,
			const u64 &docid)
{
	// checks whether it is write locked. If yes read failed .
	aos_assert_r(docid, false);
	mLock->lock();
	bool rslt = checkModifyPriv(rdata, docid);
	if (!rslt)
	{
		rdata->setError() << "Missing Lock!";
		mLock->unlock();
		return false;
	}
	mLock->unlock();
	return true;
}

bool
AosDocLock::checkModifyPriv(const AosRundataPtr &rdata, const u64 &docid)
{
	aos_assert_r(sanityCheck4(rdata, docid), false);
	//Site, Linda
	//AosU642LockPairItr_t itermap  = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());
	AosU642LockPairItr_t itermap  = mDocLock.find(key);
	if (itermap == mDocLock.end()) return true; 

	vector<AosStLockPtr> *read = itermap->second.first;
	if (read) return false;

    vector<AosStLockPtr> *write = itermap->second.second;
	if (!write || write->size() == 0)return true;

	//It checks whether the lock owns the requested.
	u64 userid = rdata->getUserid();
	if ((*write)[0]->req_userid == userid)return true;
	return false;
}

bool
AosDocLock::sanityCheck4(const AosRundataPtr &rdata, const u64 &docid)
{
	if (!sgSanityCheck) return true;
	//Site, Linda
	//AosU642LockPairItr_t itermap  = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());
	AosU642LockPairItr_t itermap  = mDocLock.find(key);
	if (itermap == mDocLock.end()) return true; 
	vector<AosStLockPtr> *write = itermap->second.second;
	if (write)
	{
		aos_assert_r(write->size()!=0, false);
	}

	vector<AosStLockPtr> *read = itermap->second.first;
	if (read)
	{
		aos_assert_r(read->size()!=0, false);
	}
	//aos_assert_r(!read && !write, false);
	return true;
}


//checkRead
bool
AosDocLock::checkRead(
			const AosRundataPtr &rdata,
			const u64 &docid)
{
	//check whether write doc;
	aos_assert_r(docid, false);
	mLock->lock();
	bool rslt = checkReadPriv(rdata, docid);
	mLock->unlock();
	if (!rslt)
	{
		rdata->setError() << "Missing Lock!";
		return false;
	}
	return true;
}

bool
AosDocLock::checkReadPriv(
			const AosRundataPtr &rdata, 
			const u64 &docid)
{
	//it checks whether it is write locked.
	//Site, Linda
	//AosU642LockPairItr_t itermap  = mDocLock.find(docid);
	AosHashKey key(docid, rdata->getSiteid());
	AosU642LockPairItr_t itermap  = mDocLock.find(key);
	if (itermap == mDocLock.end()) return true; 
		
    vector<AosStLockPtr> *write = itermap->second.second;
	//It checks whether the lock owns the requested.
	if (write)
	{
		aos_assert_r(write->size() != 0, false);
		u64 userid = rdata->getUserid();
		if ((*write)[0]->req_userid != userid)
		{
			return false;
		}
	}
	return true;	
}
