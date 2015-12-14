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
// 1. 
//   
//
// Modification History:
// 2014/10/10 Created by Felicia Peng
////////////////////////////////////////////////////////////////////////////
#include "DocServer/ReadWriteLock.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Security/SecurityMgr.h"
#include "Thread/CondVar.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"
#include "Timer/TimerMgr.h"
#include "DocTrans/ReadLockDocTrans.h"
#include "DocTrans/WriteLockDocTrans.h"
#include <algorithm>


AosReadWriteLock::AosReadWriteLock()
:
mLock(OmnNew OmnMutex()),
mNumReqs(0),
mCondVar(OmnNew OmnCondVar())
{
	for (int i=0; i<100; i++)
	{
		vector<AosRWLockPtr> *v = OmnNew vector<AosRWLockPtr>;
		mOmnNewCtnrs.push(v);
	}
	
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "ReadWriteLockThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosReadWriteLock::~AosReadWriteLock()
{
	vector<AosRWLockPtr> *v = 0;
	while(!mOmnNewCtnrs.empty())
	{
		v = mOmnNewCtnrs.front();
		mOmnNewCtnrs.pop();
		OmnDelete v;
	}
}


vector<AosRWLockPtr>*
AosReadWriteLock::getVector()
{
	vector<AosRWLockPtr>* v;
	if(mOmnNewCtnrs.empty())
	{
		v = OmnNew vector<AosRWLockPtr>;
		mOmnNewCtnrs.push(v);
	}
	else
	{
		v = mOmnNewCtnrs.front();
		mOmnNewCtnrs.pop();
		v->clear();
	}
	return v;
}


void
AosReadWriteLock::returnVector(vector<AosRWLockPtr>* v)
{
	mOmnNewCtnrs.push(v);
}


bool
AosReadWriteLock::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		sleep(1);

		//check wait locks();
		vector<Info> expired_trans = getWaiterExpires();
		for (u32 i=0; i<expired_trans.size(); i++)
		{
			OmnString type = expired_trans[i].type;
			OmnString lock_type = expired_trans[i].lock_type;
			OmnString start_time = expired_trans[i].start_time;
			OmnString wait_timer = expired_trans[i].wait_timer;
			OmnString user_id = expired_trans[i].user_id;
			AosTransPtr trans = expired_trans[i].trans;
			aos_assert_r(trans, false);

			if(type == "read_lock")
			{
				AosReadLockTrans * read_trans = dynamic_cast<AosReadLockTrans *>(trans.getPtr());
				aos_assert_r(read_trans, false);
				read_trans->waitLockExpired(user_id, start_time, wait_timer, lock_type);
			}
			else if(type == "write_lock")
			{
				//trans->waitLockExpired(user_id, start_time, wait_timer, lock_type);
				AosWriteLockTrans * write_trans = dynamic_cast<AosWriteLockTrans *>(trans.getPtr());
				aos_assert_r(write_trans, false);
				write_trans->waitLockExpired(user_id, start_time, wait_timer, lock_type);
			}
			else
			{
				OmnAlarm << "invalid type " << type << enderr;
			}

		}

		//check expired locks();
		bool rslt = checkExpiredLocks();
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosReadWriteLock::checkExpiredLocks()
{
	mLock->lock();
	AosDocid2LockPairItr_t itr = mReadWriteLock.begin();
	vector<u64> docids;
	vector<AosRWLockPtr>* read;
	AosRWLockPtr write;
	u32 crt_time = OmnGetSecond();
	while(itr != mReadWriteLock.end())
	{
		read = itr->second.first;
		write = itr->second.second;
		if (read && read->size() > 0)
		{
			aos_assert_rl(!write, mLock, false);
			vector<AosRWLockPtr>::iterator iter = read->begin();
			for (;iter != read->end();)
			{
				AosRWLockPtr dd = *iter;
				aos_assert_rl(dd, mLock, false);
				aos_assert_rl(dd->req_userid, mLock, false);
				if (dd->start_time + dd->hold_timer < crt_time)
				{
					iter = read->erase(iter);
				}
				else
				{
					++iter;
				}
			}

			if (read->size() == 0)
			{
				if (read) returnVector(read);
				docids.push_back(itr->first);
				//mReadWriteLock.erase(docid);
			}

			++itr;
			continue;
		}

		aos_assert_rl((!read || read->size() == 0), mLock, false);
		if (write && (write->start_time+write->hold_timer < crt_time))
		{
			itr->second.second = 0;
			if (read) returnVector(read);
			docids.push_back(itr->first);
			//mReadWriteLock.erase(docid);
			++itr;
			continue;
		}

		if (!write && (!read || read->size() == 0))
		{
			docids.push_back(itr->first);
		}

		++itr;
	}

	for(size_t i=0; i<docids.size(); i++)
	{
		itr = mReadWriteLock.find(docids[i]);
		aos_assert_rl(itr != mReadWriteLock.end(), mLock, false);
		mReadWriteLock.erase(docids[i]);
	}
	mLock->unlock();
	return true;
}

vector<AosReadWriteLock::Info>
AosReadWriteLock::getWaiterExpires()
{
	vector<Info> expired_reqs;
	mLock->lock();
	AosDocid2ReqPairItr_t itr = mReq.begin();
	bool rslt;
	while(itr != mReq.end())
	{
		deque<AosRWLockReqPtr>* que = itr->second;
		deque<AosRWLockReqPtr>::iterator qitr = que->begin();
		u32 crt_time = OmnGetSecond();
		while(qitr != que->end())
		{
			if ((*qitr)->wait_timer + (*qitr)->start_time < crt_time)
			{
				switch((*qitr)->reqtype)
				{
					case AosRWLockReq::eReadLock:
						rslt = getReadExpireInfo(expired_reqs, *qitr);
						aos_assert_rl(rslt, mLock, expired_reqs);
						break;
					case AosRWLockReq::eWriteLock:
						rslt = getWriteExpireInfo(expired_reqs, *qitr);
						aos_assert_rl(rslt, mLock, expired_reqs);
						break;
					default:
			 			OmnAlarm << "Unrecognized doc request type: " << (*qitr)->reqtype << enderr;
						break;
				}

				qitr = que->erase(qitr);
			}
			else
			{
				++qitr;
			}
			//++qitr;
		}
		++itr;
	}
	mLock->unlock();
	return expired_reqs;
}


bool
AosReadWriteLock::getReadExpireInfo(
		vector<Info> &expired_reqs,
		const AosRWLockReqPtr &req)
{
	u64 docid = req->docid;
	AosDocid2LockPairItr_t itr = mReadWriteLock.find(docid);
	aos_assert_r(itr != mReadWriteLock.end(), false);

	vector<AosRWLockPtr>* read = itr->second.first;
	aos_assert_r((!read || read->size() == 0), false);

	AosRWLockPtr write = itr->second.second;
	aos_assert_r(write, false);

	Info info;
	info.type = "read_lock";
	info.lock_type = "write_lock";
	info.wait_timer << write->wait_timer;
	info.hold_timer << write->hold_timer;
	info.start_time << write->start_time;
	info.user_id << write->req_userid;
	info.trans = req->trans;

	expired_reqs.push_back(info);
	return true;
}

bool
AosReadWriteLock::getWriteExpireInfo(
		vector<Info> &expired_reqs,
		const AosRWLockReqPtr &req)
{
	u64 docid = req->docid;
	AosDocid2LockPairItr_t itr = mReadWriteLock.find(docid);
	aos_assert_r(itr != mReadWriteLock.end(), false);

	vector<AosRWLockPtr>* read = itr->second.first;
	aos_assert_r(read, false);
	//aos_assert_r(read->size() > 0, false);
	
	AosRWLockPtr write = itr->second.second;
	//aos_assert_r(!write, false);

	Info info;
	info.type = "write_lock";
	if (read->size() == 0)
	{
		aos_assert_r(write, false);
		info.lock_type = "write_lock";
		info.wait_timer << write->wait_timer;
		info.hold_timer << write->hold_timer;
		info.start_time << write->start_time;
		info.user_id << write->req_userid;
		info.trans = req->trans;

		expired_reqs.push_back(info);
		return true;
	}
	
	aos_assert_r(read->size() > 0, false);
	vector<AosRWLockPtr>::iterator read_itr = read->begin();
	aos_assert_r(read_itr != read->end(), false);

	info.lock_type = "read_lock";
	info.wait_timer << (*read_itr)->wait_timer;
	info.hold_timer << (*read_itr)->hold_timer;
	info.start_time << (*read_itr)->start_time;
	info.user_id << (*read_itr)->req_userid;
	info.trans = req->trans;
	++read_itr;

	for(; read_itr != read->end(); ++read_itr)
	{
		info.wait_timer << ", " << (*read_itr)->wait_timer;
		info.hold_timer << ", " << (*read_itr)->hold_timer;
		info.start_time << ", " << (*read_itr)->start_time;
		info.user_id << ", " << (*read_itr)->req_userid;
	}

	expired_reqs.push_back(info);
	return true;
}


bool
AosReadWriteLock::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


AosRWLockReqPtr
AosReadWriteLock::getReqFromQueue(
		const u64 &docid,
		AosDocid2ReqPairItr_t &req_itr)
{
	AosRWLockReqPtr req = 0;
	
	req_itr = mReq.find(docid);
	if (req_itr == mReq.end())	return req;

	deque<AosRWLockReqPtr> *que = req_itr->second;
	if(que && !que->empty())
	{
		req = que->front();
		que->pop_front();
	}
	return req;
}

bool
AosReadWriteLock::unLockRead(
		const u64 &docid,
		const u64 &userid,
		AosRundata* rdata)
{
	//OmnScreen << "@@@@@@ to unlock read userid : " << userid << endl;
	mLock->lock();
	bool found = false;
	AosDocid2LockPairItr_t itr;
	bool rslt = findReadLock(docid, userid, found, itr, rdata);
	aos_assert_rl(rslt, mLock, false);

	if (!found)
	{
		rdata->setErrmsg("docid not found");
		mLock->unlock();
	//OmnScreen << "@@@@@@ to unlock readerror userid : " << userid << endl;
		return true;
	}
	
	aos_assert_rl(itr != mReadWriteLock.end(), mLock, false);
	aos_assert_rl(!(itr->second.second), mLock, false);
	vector<AosRWLockPtr> *read = itr->second.first;
	//OmnScreen << "@@@@@@ unlock read userid : " << userid << endl;

	if (!read || read->size() == 0)
	{
		/*if (writeWaiters is not empty)
		{
			convert the first write waiter to the write holder;
			AosXmlTagPtr doc = AosDocSvr::getSelf()->readDoc(...);
			trans->writeDocObtained(doc);
		}
		*/

		AosDocid2ReqPairItr_t req_itr;
		AosRWLockReqPtr req = getReqFromQueue(docid, req_itr);
			
		if (req)
		{
			aos_assert_rl(req->reqtype == AosRWLockReq::eWriteLock, mLock, false);

			//rslt = procWriteWaitReq(req, itr, rdata);
			rslt = procWriteWaitReq(req, docid, rdata);
			aos_assert_rl(rslt, mLock, false);
		}
	}
	mLock->unlock();
	return true;
}


// Request
bool
AosReadWriteLock::unLockWrite(
		const u64 &docid,
		const u64 &userid,
		const AosXmlTagPtr &newdoc,
		const AosTransId &trans_id,
		//bool &lock_expired,
		AosRundata* rdata)
{
	/*
	find the lock;
	if (lock expired)
	{
		lock_expired = true;
		return true;
	}

	remove the writer;
	if (waiters not empty)
	{
		pick the first waiter;
		if (it is a reader)
		{
			vector<...> readers = getAllReaders();
			AosXmlTagPtr doc = getDoc();
			for (u32 i=0; i<readers.size(); i++)
			{
				trans->readLockDocSuccess(...);
			}
		}
	}
	*/

	//OmnScreen << "@@@@@@ to unlock write userid : " << userid << endl;
	mLock->lock();
	bool found = false;
	AosDocid2LockPairItr_t itr;
	bool rslt = findWriteLock(docid, userid, found, itr, rdata);
	aos_assert_rl(rslt, mLock, false);

	if (!found)
	{
		//lock_expired = true;
		rdata->setErrmsg("docid not found");
		mLock->unlock();
	//OmnScreen << "@@@@@@ to unlock writeerror userid : " << userid << endl;
		return true;
	}

	u64 newdid = newdoc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rl(newdid, mLock, false);
	rslt = AosDocSvr::getSelf()->modifyObj(rdata, newdoc, newdid, trans_id, 0);
	aos_assert_rl(rslt, mLock, false);
	//OmnScreen << "@@@@@@ unlock write userid : " << userid << endl;

	AosDocid2ReqPairItr_t req_itr;
	AosRWLockReqPtr req = getReqFromQueue(docid, req_itr);
	if (!req)
	{
		mLock->unlock();
		return true;
	}

	switch(req->reqtype)
	{
		case AosRWLockReq::eReadLock:
			//rslt = procReadWaitReq(req, itr, req_itr, rdata);
			rslt = procReadWaitReq(req, docid, req_itr, rdata);
			aos_assert_rl(rslt, mLock, false);
			break;
		case AosRWLockReq::eWriteLock:
			//rslt = procWriteWaitReq(req, itr, rdata);
			rslt = procWriteWaitReq(req, docid, rdata);
			aos_assert_rl(rslt, mLock, false);
			break;
		default:
			 OmnAlarm << "Unrecognized doc request type: " << req->reqtype << enderr;
			break;
	}
	mLock->unlock();
	return true;
}


bool
AosReadWriteLock::readLock(
		const u64 &docid,
		const u32 &userid,
		const int waittimer,
		const int holdtimer,
		const AosTransPtr &trans,
		bool &wait,
		AosRundata *rdata)
{
	// This function tries to read lock the doc 'docid'. If the doc is 
	// write locked, it cannot read lock the doc. Otherwise, it checks
	// whether the requester has the right to read lock the doc. If not, 
	// it responds with an error code. Otherwise, it read locks the doc.

	//OmnScreen << "@@@@@@ to readlock userid : " << userid << endl;

	wait = false;
	mLock->lock();
	bool canlock = false;
	bool rslt = canReadLock(docid, userid, canlock, rdata);
	if (!rslt) 
	{
		mLock->unlock();
	//OmnScreen << "@@@@@@ to readlockerror userid : " << userid << endl;
		return false;
	}

	if (canlock)
	{
		//add read lock to readers;
		rslt = addReadLock(docid, userid, waittimer, holdtimer, rdata);	
		aos_assert_rl(rslt, mLock, false);
		mLock->unlock();
	//OmnScreen << "@@@@@@ readlock userid : " << userid << endl;
		return true;
	}

	// It cannot hold the read lock, which means a writer has
	// locked the doc for write. 
	//add to ReadWaiters(trans, crt_second + wait_timer);
	rslt = addReadWaitLock(docid, userid, waittimer, holdtimer, trans, rdata);
	aos_assert_rl(rslt, mLock, false);
	wait = true;
	mLock->unlock();
	//OmnScreen << "@@@@@@ wait readlock userid : " << userid << endl;
	return true;
}

bool
AosReadWriteLock::writeLock(
		const u64 &docid,
		const u32 &userid,
		const int waittimer,
		const int holdtimer,
		const AosTransPtr &trans,
		bool &wait,
		AosRundata *rdata)
{
	// This function tries to write lock the doc 'docid'. If the doc is 
	// read locked, it cannot write lock the doc. Otherwise, it checks
	// whether the requester has the right to write lock the doc. If not, 
	// it responds with an error code. Otherwise, it write locks the doc.
	
	//OmnScreen << "@@@@@@ to writelock userid : " << userid << endl;
	mLock->lock();
	bool canlock = false;
	bool rslt = canWriteLock(docid, userid, canlock, rdata);
	if (!rslt) 
	{
		mLock->unlock();
	//OmnScreen << "@@@@@@ to writelockerror userid : " << userid << endl;
		return false;
	}

	if (canlock)
	{
		//add read lock to readers;
		rslt = addWriteLock(docid, userid, waittimer, holdtimer, rdata);	
		aos_assert_rl(rslt, mLock, false);
		wait = false;
		mLock->unlock();
	//OmnScreen << "@@@@@@ writelock userid : " << userid << endl;
		return true;
	}

	// It cannot hold the read lock, which means a writer has
	// locked the doc for write. 
	//add to ReadWaiters(trans, crt_second + wait_timer);
	rslt = addWriteWaitLock(docid, userid, waittimer, holdtimer, trans, rdata);
	aos_assert_rl(rslt, mLock, false);
	wait = true;
	mLock->unlock();
	//OmnScreen << "@@@@@@ wait writelock userid : " << userid << endl;
	return true;
}


bool
AosReadWriteLock::canReadLock(
		const u64 &docid, 
		const u64 &userid,
		bool &canlock,
		AosRundata *rdata)
{
	canlock = false;
	AosDocid2LockPairItr_t itr = mReadWriteLock.find(docid);
	if(itr == mReadWriteLock.end())
	{
		canlock = true;
		return true;
	}

	AosRWLockPtr write = itr->second.second;
	if (!write)
	{
		vector<AosRWLockPtr> *read = itr->second.first;
		aos_assert_r(read, false);
		vector<AosRWLockPtr>::iterator iter = read->begin();
		for (;iter != read->end(); ++iter)
		{
			AosRWLockPtr dd = *iter;
			aos_assert_r(dd, false);
			aos_assert_r(dd->req_userid, false);
			if (dd->req_userid == userid)
			{
				rdata->setErrmsg("You have locked some doc!");
				return false;
			}
		}
		canlock = true;
		return true;
	}

	return true;
}

bool
AosReadWriteLock::canWriteLock(
		const u64 &docid, 
		const u64 &userid,
		bool &canlock,
		AosRundata *rdata)
{
	canlock = false;
	AosDocid2LockPairItr_t itr = mReadWriteLock.find(docid);
	if(itr == mReadWriteLock.end())
	{
		canlock = true;
		return true;
	}

	AosRWLockPtr write = itr->second.second;
	vector<AosRWLockPtr> *read = itr->second.first;
	if (!write && (!read || read->size() == 0))
	{
		if (read) returnVector(read);
		mReadWriteLock.erase(docid);
		canlock = true;
		return true;
	}

	return true;
}


bool
AosReadWriteLock::addReadLock(
		const u64 &docid,
		const u32 &userid,
		const int waittimer,
		const int holdtimer,
		AosRundata *rdata)
{
	aos_assert_r(docid, false);

	AosDocid2LockPairItr_t itr = mReadWriteLock.find(docid);

	u32 start_time = OmnGetSecond();
	AosRWLockPtr lock = OmnNew AosRWLock;
	lock->req_userid = userid;
	lock->start_time = start_time;
	lock->wait_timer = waittimer;
	lock->hold_timer = holdtimer;

	vector<AosRWLockPtr> *read = 0;
	if (itr != mReadWriteLock.end())
	{
		read = itr->second.first;
		aos_assert_r(read, false);
		aos_assert_r(!(itr->second.second), false);
	}
	else
	{
		//read = OmnNew vector<AosRWLockPtr>;
		read = getVector();
		aos_assert_r(read, false);
	}

	read->push_back(lock);
	aos_assert_r(read->size() != 0, false);

	if (itr == mReadWriteLock.end())
	{
		AosRWLockPtr write = 0;
		mReadWriteLock.insert(make_pair(docid, make_pair(read, write)));
	}

	return true;
}

bool
AosReadWriteLock::addReadWaitLock(
		const u64 &docid,
		const u32 &userid,
		const int waittimer,
		const int holdtimer,
		const AosTransPtr &trans,
		AosRundata *rdata)
{
	AosRWLockReqPtr req = OmnNew AosRWLockReq;

	AosRWLockReq::REQ reqtype = AosRWLockReq::eReadLock;
	aos_assert_r(reqtype > AosRWLockReq::eInvalid && reqtype < AosRWLockReq::eMax, false);
	
	u32 start_time = OmnGetSecond();

	req->reqtype = reqtype;
	req->docid = docid;
	req->req_userid = userid;
	req->trans = trans;
	req->start_time = start_time;
	req->wait_timer = waittimer;
	req->hold_timer = holdtimer;
	req->rdata = rdata;

	//mReq.push_back(req);
	AosDocid2ReqPairItr_t itr = mReq.find(docid);

	deque<AosRWLockReqPtr> * que = 0;
	if (itr == mReq.end())
	{
		que = OmnNew deque<AosRWLockReqPtr>;
	}
	else
	{
		que = itr->second;
	}
	aos_assert_r(que, false);
	que->push_back(req);
	
	if (itr == mReq.end())
	{
		mReq.insert(make_pair(docid, que));
	}
	return true;
}

bool
AosReadWriteLock::addWriteLock(
		const u64 &docid,
		const u32 &userid,
		const int waittimer,
		const int holdtimer,
		AosRundata *rdata)
{
	aos_assert_r(docid, false);

	AosDocid2LockPairItr_t itr = mReadWriteLock.find(docid);

	u32 start_time = OmnGetSecond();

	AosRWLockPtr lock = OmnNew AosRWLock;
	lock->req_userid = userid;
	lock->start_time = start_time;
	lock->wait_timer = waittimer;
	lock->hold_timer = holdtimer;

	vector<AosRWLockPtr> *read = 0;
	if (itr != mReadWriteLock.end())
	{
		read = itr->second.first;
	}
	else
	{
		//read = OmnNew vector<AosRWLockPtr>;
		read = getVector();
	}

	aos_assert_r(read, false);
	aos_assert_r(read->size() == 0, false);

	if (itr == mReadWriteLock.end())
	{
		mReadWriteLock.insert(make_pair(docid, make_pair(read, lock)));
	}
	else
	{
		itr->second.second = lock;
	}

	return true;
}

bool
AosReadWriteLock::addWriteWaitLock(
		const u64 &docid,
		const u32 &userid,
		const int waittimer,
		const int holdtimer,
		const AosTransPtr &trans,
		AosRundata *rdata)
{
	AosRWLockReqPtr req = OmnNew AosRWLockReq;

	AosRWLockReq::REQ reqtype = AosRWLockReq::eWriteLock;
	aos_assert_r(reqtype > AosRWLockReq::eInvalid && reqtype < AosRWLockReq::eMax, false);
	
	u32 start_time = OmnGetSecond();

	req->reqtype = reqtype;
	req->docid = docid;
	req->req_userid = userid;
	req->trans = trans;
	req->start_time = start_time;
	req->wait_timer = waittimer;
	req->hold_timer = holdtimer;
	req->rdata = rdata;

	//mReq.push_back(req);
	AosDocid2ReqPairItr_t itr = mReq.find(docid);

	deque<AosRWLockReqPtr> * que = 0;
	if (itr == mReq.end())
	{
		que = OmnNew deque<AosRWLockReqPtr>;
	}
	else
	{
		que = itr->second;
	}
	aos_assert_r(que, false);
	que->push_back(req);
	
	if(itr == mReq.end())
	{
		mReq.insert(make_pair(docid, que));
	}
	return true;
}

bool
AosReadWriteLock::findReadLock(
		const u64 &docid,
		const u64 &userid,
		bool &found,
		AosDocid2LockPairItr_t &itr,
		AosRundata* rdata)
{
	found = false;

	itr = mReadWriteLock.find(docid);
	if (itr == mReadWriteLock.end())
	{
//OmnScreen << "1111111111111111111" << endl;
		found = false;
		return true;
	}

	vector<AosRWLockPtr> *read = itr->second.first;
	AosRWLockPtr write = itr->second.second;

	if (!read || read->size() == 0)
	{
		found = false;
		aos_assert_r(write, false);
//OmnScreen << "222222222222222222222" << endl;
		return true;
	}

	aos_assert_r(!write, false);
	vector<AosRWLockPtr>::iterator iter = read->begin();
	for (;iter != read->end(); ++iter)
	{
		AosRWLockPtr dd = *iter;
		aos_assert_r(dd, false);
		aos_assert_r(dd->req_userid, false);
		if (dd->req_userid == userid)
		{
			iter = read->erase(iter);
			found = true;
//OmnScreen << "333333333333333333333333" << endl;
			//return true;
			break;
		}
	}

	if (found && read->size() == 0)
	{
		if (read) returnVector(read);
		mReadWriteLock.erase(docid);
	}

	return true;
}


bool
AosReadWriteLock::findWriteLock(
		const u64 &docid,
		const u64 &userid,
		bool &found,
		AosDocid2LockPairItr_t &itr,
		AosRundata* rdata)
{
	found = false;

	itr = mReadWriteLock.find(docid);
	if (itr == mReadWriteLock.end())
	{
//OmnScreen << "aaaaaaaaaaaaaaaaaaaaaaaa" << endl;	
		return true;
	}

	vector<AosRWLockPtr> *read = itr->second.first;
	AosRWLockPtr write = itr->second.second;
	if(!write) 
	{
//OmnScreen << "bbbbbbbbbbbbbbbbbbbbbbb" << endl;
		return true;
	}

	aos_assert_r(!read || read->size() == 0, false);
	if(write->req_userid == userid)
	{
		itr->second.second = 0;
		if (read) returnVector(read);
		mReadWriteLock.erase(docid);
		found = true;
//OmnScreen << "ccccccccccccccccccccccc" << endl;
		return true;
	}
	return true;
}


bool
AosReadWriteLock::procWriteWaitReq(
		const AosRWLockReqPtr &req, 
		const u64 &docid,
		//const AosDocid2LockPairItr_t &itr,
		AosRundata* rdata)
{
	/*
	AosRWLockPtr w = OmnNew AosRWLock;
	w->req_userid = req->req_userid;
	w->wait_timer = req->wait_timer;
	w->hold_timer = req->hold_timer;
	itr->second.second = w;
	*/

	bool rslt = addWriteLock(docid, req->req_userid, req->wait_timer, req->hold_timer, rdata);	
	aos_assert_r(rslt, false);
	//AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(req->docid, req->rdata);
	
	AosTransPtr trans = req->trans;
	aos_assert_r(trans, false);

	AosWriteLockTrans * write_trans = dynamic_cast<AosWriteLockTrans *>(trans.getPtr());
	aos_assert_r(write_trans, false);
	write_trans->writeDocObtained(req->rdata);
	return true;
}

bool
AosReadWriteLock::procReadWaitReq(
		const AosRWLockReqPtr &req, 
		const u64 &docid,
		//const AosDocid2LockPairItr_t &itr,
		const AosDocid2ReqPairItr_t &req_itr,
		AosRundata* rdata)
{
	vector<AosRWLockReqPtr> read_reqs;
	read_reqs.push_back(req);

	//vector<AosRWLockPtr> *read = itr->second.first;
	//aos_assert_r(read, false);

	deque<AosRWLockReqPtr>* que = req_itr->second;
	aos_assert_r(que, false);

	deque<AosRWLockReqPtr>::iterator qitr = que->begin();
	while (qitr != que->end())
	{
		if((*qitr)->reqtype == AosRWLockReq::eReadLock)
		{
			read_reqs.push_back(*qitr);
			qitr = que->erase(qitr);
			continue;
		}
		 
		++qitr;
	}

	//AosXmlTagPtr doc;
	AosTransPtr trans;
	bool rslt;
	for(size_t i=0; i<read_reqs.size(); i++)
	{
	/*
		AosRWLockPtr lock = OmnNew AosRWLock;
		lock->req_userid = read_reqs[i]->req_userid;
		lock->wait_timer = read_reqs[i]->wait_timer;
		lock->hold_timer = read_reqs[i]->hold_timer;
		read->push_back(lock);
*/
		rslt = addReadLock(docid, read_reqs[i]->req_userid, read_reqs[i]->wait_timer, read_reqs[i]->hold_timer, rdata);	
		aos_assert_r(rslt, false);
		//doc = AosDocSvr::getSelf()->getDoc(read_reqs[i]->docid, rdata);
		trans = read_reqs[i]->trans;
		aos_assert_r(trans, false);
		AosReadLockTrans * read_trans = dynamic_cast<AosReadLockTrans *>(trans.getPtr());
		aos_assert_r(read_trans, false);
		//read_trans->readLockDocSuccess(doc, rdata);
		read_trans->readLockDocSuccess(rdata);
	}
	return true;
}

