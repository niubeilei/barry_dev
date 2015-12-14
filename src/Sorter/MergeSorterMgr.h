////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Sorter_MergeSorterMgr_h
#define AOS_Sorter_MergeSorterMgr_h

#include "Rundata/Rundata.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include <queue>


/*
OmnDefineSingletonClass(AosMergeSorterMgrSingleton,
						AosMergeSorterMgr,
						AosMergeSorterMgrSelf,
						OmnSingletonObjId::eMergeSorterMgr,
						"MergeSorterMgr");
*/

template <class T>
struct ReqComp
{
	bool operator () (const T &lhs, const T &rhs) const
	{
/*
cout << "Pr " << rhs->getPriority() << "," << lhs->getPriority() << endl;
cout << "Lv " << rhs->getLevel() << "," << lhs->getLevel() << endl;
cout << "Tm " << rhs->getTimeStamp() << "," << lhs->getTimeStamp() << endl;
*/
		return ((rhs->getPriority()-lhs->getPriority()) + 
				(rhs->getLevel()-lhs->getLevel()) + 
				(lhs->getTimeStamp() - rhs->getTimeStamp())/4) >= 0;
		
		/*if (lhs->getPriority() == rhs->getPriority())
		{
			if (lhs->getLevel() == rhs->getLevel())
			{
				return lhs->getTimeStamp() < rhs->getTimeStamp();
			}

			return lhs->getLevel() > rhs->getLevel();
		}
		return (lhs->getPriority() > rhs->getPriority())
		*/
	}
};

template <class T>
class AosMergeSorterMgr :  public OmnThreadedObj
{
	OmnDefineRCObject;
private:
	enum
	{
		eMaxThrds = 16,
		eMaxReqs = 1000
	};

	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	//deque<T>					mRequests;
	priority_queue<T, vector<T>, ReqComp<T> >	 		mRequests;
	vector<OmnThreadPtr>		mThreads;

	static AosMergeSorterMgr<T>* smMergeSorterMgr;

    // Singleton class interface
public:
	AosMergeSorterMgr()					// Chen Ding, 2013/07/26
	:
	mLock(OmnNew OmnMutex()),
	mCondVar(OmnNew OmnCondVar())
	{
	}

	static AosMergeSorterMgr<T>* getSelf()
	{
		static OmnMutex lsLock;		// Chen Ding, 2013/07/26
									// It appears that 'getSelf()' might be called 
									// concurrently by multiple threads. In the core
									// dump, somebody called ::addReq(...). It core
									// dumped because 'mLock' is null and mThreads
									// is empty. If 'mThreads' is empty, it means 
									// 'start()' has not finished yet. This means
									// one thread was trying to call 'start()', while
									// another thread was trying to 'addReq()'. 

		// Chen Ding, 2013/07/26
		// If 'smMergeSorterMgr' is null, it needs to create one. Note
		// that multiple threads may call this function concurrently. 
		// It uses a lock. 
		lsLock.lock();
		if (!smMergeSorterMgr)
		{
			smMergeSorterMgr = OmnNew AosMergeSorterMgr<T>();
			smMergeSorterMgr->start();
		}
		lsLock.unlock();
		return smMergeSorterMgr;
	}

	void start()
	{
		// Chen Ding, 2013/07/26
		// mLock = OmnNew OmnMutex();
		// mCondVar = OmnNew OmnCondVar();
		OmnThreadedObjPtr thisPtr(this, false);
		for (int i=0; i<eMaxThrds; i++)
		{
			OmnThreadPtr thread = OmnNew OmnThread(thisPtr, "MergeSortMgrThrd", i, true, true, __FILE__, __LINE__);
			thread->start();
			mThreads.push_back(thread);
		}
	}

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
	{
		while (state == OmnThrdStatus::eActive)
		{
			mLock->lock();
			if (mRequests.size() <= 0)
			{
				mCondVar->wait(mLock);
				mLock->unlock();
				continue;
			}
			T req = mRequests.top();
			//T req = mRequests.front();
			mRequests.pop();
			//mRequests.pop_front();
			mLock->unlock();
			procOneReq(req);
		}
		return true;
	}
	virtual bool    signal(const int threadLogicId) {return true;}
	virtual void    heartbeat(const int tid) {}
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) {return true;}

	bool addReq(const T &req, const AosRundataPtr &rdata)
	{
		mLock->lock();
//cout << "Pr " << req->getPriority() << endl;
//cout << "Lv " << req->getLevel() << endl;
//cout << "Tm " << req->getTimeStamp() << endl;
		mRequests.push(req);
//OmnScreen << " requset size : " << mRequests.size() << endl;
		//mRequests.push_back(req);
		if (mRequests.size() < eMaxThrds)
		{
			mCondVar->signal();
		}
		mLock->unlock();
		return true;
	}

	bool addHighPriorityReq(const T &req, const int priority, const AosRundataPtr &rdata)
	{
		mLock->lock();
		req->setPriority(priority);
		mRequests.push(req);
		//mRequests.push_back(req);
		if (mRequests.size() < eMaxThrds)
		{
			mCondVar->signal();
		}
		mLock->unlock();
		return true;
	}

	bool isMemoryFull()
	{
		bool isfull = false;
		mLock->lock();
		if (mRequests.size() > eMaxReqs)
		{
OmnScreen << " MergeSorterMgr Memory if Full " << endl;
			isfull = true;
		}
		mLock->unlock();
		return isfull;
	}
private:
	//bool procOneReq(const T &req)
	bool procOneReq(const T &req)
	{
		// This function merges the two buff array in 'req'. 
		bool rslt = req->mergeBlocks();
		aos_assert_r(rslt, false);
		return true;
	}

};
template<class T>
AosMergeSorterMgr<T>* AosMergeSorterMgr<T>::smMergeSorterMgr = NULL;
#endif
