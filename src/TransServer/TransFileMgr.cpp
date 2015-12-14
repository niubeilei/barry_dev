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
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransServer/TransFileMgr.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "TransServer/TransServer.h"
#include "TransServer/TransFileReq.h"
#include "TransServer/TransFile.h"
#include "TransBasic/AppMsg.h"
#include "TransBasic/Trans.h"
#include "TransUtil/TransBitmap.h"

AosTransFileMgr::AosTransFileMgr(
		const AosTransServerPtr &trans_svr,
		const OmnString &dir_name,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mReadLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mTransSvr(trans_svr),
mCrtCleanSeq(-1),
mNextReadId(0),
mStarted(false),
mShowLog(show_log)
{
	bool rslt = init(dir_name);
	aos_assert(rslt);
}

AosTransFileMgr::~AosTransFileMgr()
{
}

bool
AosTransFileMgr::init(const OmnString &dir_name)
{
	//OmnString bitmap_fname = "trans_bm";
	//mTidBitmap = OmnNew AosTransBitmap(
	//		dir_name, bitmap_fname, mShowLog);

	OmnThreadedObjPtr this_ptr(this, false);
	mCleanThrd = OmnNew OmnThread(this_ptr, "TransCleanThrd",
			eSaveThrdId, true, true, __FILE__, __LINE__);
	
	mReadThrd = OmnNew OmnThread(this_ptr, "TransReadThrd",
			eReadThrdId, true, true, __FILE__, __LINE__);
	mReadThrd->start();
	
	OmnString trans_fname = "trans_log";
	mTransFile = OmnNew AosTransFile(dir_name, trans_fname, eMaxTransFileSize, mShowLog); 
	mTransFile->start();

	// init max recv file.
	OmnString max_recv_fname = dir_name;
	max_recv_fname << "/max_recv_tid";
	mMaxRecvFile = OmnNew OmnFile(max_recv_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!mMaxRecvFile || !mMaxRecvFile->isGood())
	{
		mMaxRecvFile = OmnNew OmnFile(max_recv_fname, OmnFile::eCreate AosMemoryCheckerArgs); 
	}
	aos_assert_r(mMaxRecvFile && mMaxRecvFile->isGood(), false);

	OmnString finish_log_fname = "Trans_finish";
	finish_log_fname << "_log";
	mFinishLog = OmnNew AosLogFile(dir_name, finish_log_fname, eMaxTransFileSize); 
	mFinishLog->start();
	
	bool rslt = initFinishTids();
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosTransFileMgr::initFinishTids()
{
	if(mFinishLog->empty())	return true;
	
	int file_seqno = mFinishLog->begSeq();
	while(file_seqno >=0)
	{
		AosBuffPtr cont;
		mFinishLog->readEachFile(file_seqno, cont);
		aos_assert_r(cont, false);
		if(mShowLog)
		{
			OmnScreen << "init mFinishTids"
				<< "; read log file. seq:" << file_seqno
				<< "; file_len:" << cont->dataLen()
				<< endl;
		}
		
		while(cont->getCrtIdx() < cont->dataLen())
		{
			AosTransId tid = AosTransId::serializeFrom(cont);
			if(tid == AosTransId::Invalid)	break;
				
			mFinishTids.insert(tid);
		}
		
		file_seqno = mFinishLog->getNextSeqno(file_seqno);
	}
	return true;
}


void
AosTransFileMgr::start()
{
	mStarted = true;
	
	//OmnScreen << "finish later." << endl;
	//if(mCleanThrd->getStatus() != OmnThrdStatus::eActive)
	//{
	//	mCleanThrd->start();
	//}
}


void
AosTransFileMgr::stop()
{
	//mTidBitmap->saveToFile();
	cleanFinishLogs();
	saveMaxRecvTid();
	mCleanThrd->stop();
	mReadThrd->stop();
}

bool
AosTransFileMgr::threadFunc(
		OmnThrdStatus::E &state, 
		const OmnThreadPtr &thread)
{
	u32 tid = thread->getLogicId();
	if(tid == eSaveThrdId)
	{
		return cleanThrdFunc(state, thread);
	}

	if(tid == eReadThrdId)
	{
		return readFileThrdFunc(state, thread);
	}

	OmnShouldNeverComeHere;
	return false;
}

bool
AosTransFileMgr::cleanThrdFunc(
		OmnThrdStatus::E &state, 
		const OmnThreadPtr &thread)
{
	bool rslt;
	AosTransFileReqPtr clean_req = OmnNew AosCleanTFReq();
	while(state == OmnThrdStatus::eActive)
    {
		OmnSleep(eSaveThrdSleep);
		//mTidBitmap->saveToFile();
	
		//while(!AosSvrIsBusy())
		while(1)
		{
			clean_req->reset();
			rslt = addCleanReq(clean_req);
			aos_assert_r(rslt, false);
			if(!clean_req->isReadFinish())	continue;
			
			rslt = cleanReqReadFinish(clean_req); 
			break;
		}
	}
	return true;
}


bool
AosTransFileMgr::addCleanReq(const AosTransFileReqPtr &req)
{
	aos_assert_r(req, false);
	
	mReadLock->lock();
	if(mReqs.size() !=0 || mCrtReadRcd.size() != 0)
	{
		// means in the queue. has some read file req. 
		// so this clean req can ignore.
		mReadLock->unlock();
		return true;
	}
	
	bool rslt = addReadReqLocked(req);
	aos_assert_rl(rslt, mReadLock, false);
	mReadLock->unlock();
	
	req->wait();
	return true;
}


bool
AosTransFileMgr::addReadReqLocked(const AosTransFileReqPtr &req)
{
	aos_assert_r(req, false);
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; add file req"
			<< "; file_req:" << req->toString()
			<< endl;
	}

	mReqs.push(req);
	mCondVar->signal();
	return true;
}


bool 	
AosTransFileMgr::readUnfinishTrans(
		AosBuffPtr &trans_buff,
		int &read_id,
		bool &finish)
{
	AosTransFileReqPtr read_req = OmnNew AosReadUnfinishTFReq(read_id);
	bool rslt = addNormReadReq(read_req);
	aos_assert_r(rslt, false);

	read_id = read_req->getReadId();
	aos_assert_r(read_id >=0, false);
	finish = read_req->isReadFinish();
	trans_buff = read_req->getTransBuffs();
	if(trans_buff)	trans_buff->setCrtIdx(0);
	
	if(mShowLog)
	{
		OmnScreen << "TransFileMgr; readUnfinishTrans finish"
			<< "; req:" << read_id 
			<< "; finish: " << finish
			<< endl;
	}

	return true;
}


bool
AosTransFileMgr::addNormReadReq(const AosTransFileReqPtr &req)
{
	aos_assert_r(req, false);

	mReadLock->lock();	
	bool rslt = addReadReqLocked(req);
	aos_assert_rl(rslt, mReadLock, false);
	mReadLock->unlock();

	req->wait();
	return true;
}


bool
AosTransFileMgr::readFileThrdFunc(
		OmnThrdStatus::E &state, 
		const OmnThreadPtr &thread)
{
	OmnScreen << "----------TransFileMgr Clean Thread" << endl;
	
	bool timeout, rslt;
	while(state == OmnThrdStatus::eActive)
    {
		mReadLock->lock();
		if(mReqs.empty())
		{
			timeout = false;               
			mCondVar->timedWait(mReadLock, timeout, 60);
			mReadLock->unlock();
			continue;
		}
	
		AosTransFileReqPtr req = mReqs.front();
		mReqs.pop();
		
		rslt = procReadReq(req); 
		aos_assert_rl(rslt, mReadLock, false);	
		mReadLock->unlock();	
	}
	return true;
}


bool
AosTransFileMgr::procReadReq(const AosTransFileReqPtr &req)
{
	bool rslt, is_stale = false;
	int next_seq;
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr; procReadReq"
			<< "; req:" << req->toString() 
			<< endl;
	}

	AosTransFileReq::Type tp = req->getType();
	if(tp == AosTransFileReq::eClean)
	{
		next_seq = mCrtCleanSeq == -1 ?  mTransFile->begSeq() :
			mTransFile->getNextSeqno(mCrtCleanSeq);
	}
	else
	{
		AosReadRcdPtr read_rcd = getReadRcd(req);
		aos_assert_r(read_rcd, false);

		rslt = getNextReadPos(read_rcd, next_seq, is_stale);
		aos_assert_r(rslt, false);
	
		// Ketty 2014/09/12
		//read_rcd->mCrtFileSeq = next_seq;
		if(next_seq != -1)
		{
			read_rcd->mCrtFileSeq = next_seq;
			read_rcd->mIsStaleFile = is_stale;
		}
	}

	if(next_seq == -1)
	{
		// means read finish.
		aos_assert_r(!is_stale, false);
		//reqReadFinish(req);
		req->setReadFinish();
	}
	else
	{
		rslt = readTransPriv(req, next_seq, is_stale);	
		aos_assert_r(rslt, false);
	}

	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr; procReadReq finish"
			<< "; req:" << req->toString() 
			<< "; req.finish: " << req->isReadFinish()
			<< "; next_seq:" << next_seq
			<< endl;
	}
	req->wakeup();
	return true;
}


AosReadRcdPtr
AosTransFileMgr::getReadRcd(const AosTransFileReqPtr &req)
{
	aos_assert_r(req->getType() != AosTransFileReq::eClean, 0);

	AosReadRcdPtr read_rcd;
	int read_id = req->getReadId();	
	if(read_id != -1)
	{
		map<u32, AosReadRcdPtr>::iterator itr = mCrtReadRcd.find(read_id);
		aos_assert_r(itr != mCrtReadRcd.end(), 0);

		read_rcd = itr->second;
		return read_rcd;
	}

	// means this read is first read.
	read_id = mNextReadId++;
	req->setReadId(read_id);
	
	read_rcd = OmnNew AosReadRcd(read_id);
	mCrtReadRcd.insert(make_pair(read_id, read_rcd));
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr; add new read rcd"
			<< "; read_id:" << read_id 
			<< endl;
	}

	return read_rcd;
}


bool
AosTransFileMgr::getNextReadPos(const AosReadRcdPtr &read_rcd, int &new_seq, bool &new_is_stale)
{
	aos_assert_r(read_rcd, false);

	int crt_seq = read_rcd->mCrtFileSeq;
	bool crt_is_stale = read_rcd->mIsStaleFile;
	if(crt_seq == -1)
	{
		// means It's first read.
		if(mCrtCleanSeq == -1)
		{
			new_seq = mTransFile->begSeq();
			new_is_stale = false;
			return true;
		}

		new_seq = mTransFile->staleBegSeq();
		if(new_seq != -1)
		{
			new_is_stale = true;
			return true;
		}
		
		// maybe no stale file.
		new_seq = mTransFile->getNextSeqno(mCrtCleanSeq);
		new_is_stale = false;
		return true;
	}

	if(!crt_is_stale)
	{
		new_seq = mTransFile->getNextSeqno(crt_seq);
		new_is_stale = false;
		return true;
	}

	new_seq = mTransFile->getNextStaleSeqno(crt_seq);	
	if(new_seq != -1)
	{
		new_is_stale = true;
		return true;	
	}

	// maybe no stale file.
	aos_assert_r(mCrtCleanSeq != -1, false);
	new_seq = mTransFile->getNextSeqno(mCrtCleanSeq);
	new_is_stale = false;
	return true;
}


bool
AosTransFileMgr::readTransPriv(
		const AosTransFileReqPtr &req,
		const int crt_seq,
		const bool is_stale)
{
	aos_assert_r(req, false);
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr; readTransFromFile"
			<< "; crt_seq:" << crt_seq
			<< "; is_stale:" << is_stale
			<< "; req:" << req->toString() 
			<< "; mCrtCleanSeq:" << mCrtCleanSeq
			<< endl;
	}

	AosBuffPtr cont;
	bool rslt, finish = false;
	if(is_stale)
	{
		cont = readBuffFromStaleFile(crt_seq);
		aos_assert_r(cont, false);
		
		rslt = readUnfinishFromBuff(req, cont, false);
		aos_assert_r(rslt, false);
		return true;
	}

	cont = readBuffFromNormFile(crt_seq, finish);
	aos_assert_r(cont, false);

	rslt = readNormFileFinish(req, crt_seq);
	aos_assert_r(rslt, false);

	rslt = readUnfinishFromBuff(req, cont, true);
	aos_assert_r(rslt, false);

	//if(finish)	reqReadFinish(req);
	if(finish) req->setReadFinish();
	return true;
}


AosBuffPtr
AosTransFileMgr::readBuffFromStaleFile(const int stale_seq)
{
	aos_assert_r(stale_seq >=0, 0);
	AosBuffPtr cont;
	mTransFile->readStaleFile(stale_seq, cont);
	aos_assert_r(cont, 0);	
	return cont;
}


AosBuffPtr
AosTransFileMgr::readBuffFromNormFile(
		const int file_seq,
		bool &finish)
{
	aos_assert_r(file_seq >= 0, 0);
	bool rslt;
	int active_seq = mTransFile->activeSeq();	// the activeSeq maybe -1.
	int end_seq = mTransFile->endSeq();	// the end_seq maybe -1.
	AosBuffPtr cont;
	if(file_seq == active_seq || file_seq == end_seq)
	{
		mTransFile->startNextActive();

		rslt = mTransFile->readNormFile(file_seq, cont);
		aos_assert_r(rslt && cont, 0);

		finish = true;
		mTransFile->setStaleBoundSeq(file_seq);
		return cont;
	}

	finish = false;
	rslt = mTransFile->readNormFile(file_seq, cont);
	aos_assert_r(rslt && cont, 0);
	return cont;
}


bool
AosTransFileMgr::readUnfinishFromBuff(
		const AosTransFileReqPtr &req,
		const AosBuffPtr &cont,
		const bool save_to_stale)
{
	// if !save_to_stale, means this buff is read from stale file.
	// then can't clean the mFinishTids.
	
	vector<AosTransId> clean_tids;
	AosBuffPtr unfinish_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);

	u32 trans_size;
	bool finished;
	mLock->lock();
	while(1)
	{
		trans_size = AosTrans::getNextTransSize(cont);
		if(!trans_size)
		{
			printReadedTrans(AosTransId::Invalid, false);
			break;
		}

		// getTransIdFromBuff don't change cont's mCrtIdx.
		AosTransId trans_id = AosTrans::getTransIdFromBuff(cont); 
		aos_assert_r(trans_id != AosTransId::Invalid, false);
		finished = isFinishPriv(trans_id);
		printReadedTrans(trans_id, finished);
		
		if(finished)
		{
			clean_tids.push_back(trans_id);
		}
		else
		{
			unfinish_buff->setU32(trans_size);
			unfinish_buff->setBuff(cont->data() + cont->getCrtIdx(), trans_size);
		}

		cont->setCrtIdx(cont->getCrtIdx() + trans_size);
	}
	mLock->unlock();

	if(unfinish_buff->dataLen() == 0)	return true;
	
	req->addTransBuffs(unfinish_buff);
	if(save_to_stale)
	{
		bool rslt = mTransFile->saveToStaleFile(unfinish_buff);
		aos_assert_r(rslt, false);
	}

	cleanFinishTids(clean_tids);
	return true;
}

bool
AosTransFileMgr::cleanFinishTids(vector<AosTransId> & clean_tids)
{
	setitr_t itr;
	mLock->lock();
	for(u32 i=0; i<clean_tids.size(); i++)
	{
		AosTransId trans_id = clean_tids[i];
		itr = mFinishTids.find(trans_id);
		if(itr == mFinishTids.end()) continue;
		
		mFinishTids.erase(itr);
		printCleanedTid(trans_id);
	}
	mLock->unlock();
	printCleanedTid(AosTransId::Invalid);
	return true;
}


bool
AosTransFileMgr::readNormFileFinish(
		const AosTransFileReqPtr &req,
		const int file_seq)
{
	aos_assert_r(file_seq >=0, false);
	mCrtCleanSeq = file_seq;
	if(req->getType() == AosTransFileReq::eClean)	return true;	
	
	int read_id = req->getReadId();
	
	map<u32, AosReadRcdPtr>::iterator itr = mCrtReadRcd.begin();
	AosReadRcdPtr read_rcd;
	int crt_seq = -1;
	bool is_stale;
	int stale_end_seq = mTransFile->staleEndSeq();
	for(; itr != mCrtReadRcd.end(); itr++)
	{
		if((u32)read_id == itr->first)	continue;

		read_rcd = itr->second;
		crt_seq = read_rcd->mCrtFileSeq;
		is_stale = read_rcd->mIsStaleFile;

		if(is_stale)
		{
			// so the unfinish trans can save to new stale file.
			// next read_rcd can read the new stale file.
			if(crt_seq == stale_end_seq) mTransFile->startNextStale();
			continue;
		}
	
		aos_assert_r(crt_seq < file_seq, false);
		if(crt_seq == -1)	continue;
		mTransFile->startNextStale();
		read_rcd->mCrtFileSeq = stale_end_seq;
		read_rcd->mIsStaleFile = true;
		if(mShowLog)
		{
			OmnScreen << "TransServer; FileMgr; readNormFile finish."
				<< "; ReadRcd change file seq"
				<< "; crt_read_id:" << read_id
				<< "; change_read_id:" << read_rcd->mReadId
				<< "; old_seq:" << crt_seq
				<< "; new_seq is in stale." << stale_end_seq
				<< endl;
		}
	}
	return true;
}


bool
AosTransFileMgr::cleanReqReadFinish(const AosTransFileReqPtr &req)
{
	aos_assert_r(req && mStarted, false);
	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr"
			<< "; clean req read finish"
			<< "; req:" << req->toString()
			<< "; " << endl;
	}

	// check.
	map<u32, AosReadRcdPtr>::iterator itr;
	for(itr = mCrtReadRcd.begin(); itr != mCrtReadRcd.end(); itr++)
	{
		int crt_seq = (itr->second)->mCrtFileSeq;
		aos_assert_r(crt_seq == -1, false);
	}

	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr; mergeStaleFiles;"
			<< endl;
	}
	saveMaxRecvTid();
	mTransFile->mergeStaleFiles();
	cleanFinishLogs();
	mCrtCleanSeq = -1;
	return true;
}


bool
AosTransFileMgr::normReqReadFinish(const int &read_id)
{
	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr"
			<< "; req read finish"
			<< "; read_id:" << read_id
			<< "; " << endl;
	}

	map<u32, AosReadRcdPtr>::iterator itr = mCrtReadRcd.find(read_id);
	aos_assert_r(itr != mCrtReadRcd.end(), false);
	mCrtReadRcd.erase(itr);
	
	if(!mCrtReadRcd.empty())
	{
		// also has someone read.
		if(mShowLog)
		{
			int crt_seq;
			bool is_stale;
			for(itr = mCrtReadRcd.begin(); itr != mCrtReadRcd.end(); itr++)
			{
				crt_seq = (itr->second)->mCrtFileSeq;
				is_stale = (itr->second)->mIsStaleFile;
				OmnScreen << "TransServer; FileMgr"
					<< "; req read finish; read_id:" << read_id
					<< "; but read_id:" << itr->first << " not finish yet."
					<< "; crt_seq:" << crt_seq
					<< "; is_stale:" << is_stale
					<< "; " << endl;
			}
		}
		return true;
	}
	if(!mStarted)	return true;

	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr; mergeStaleFiles;"
			<< "; start:" << mStarted << endl;
	}
	
	saveMaxRecvTid();
	mTransFile->mergeStaleFiles();
	cleanFinishLogs();
	mCrtCleanSeq = -1;
	return true;
}

/*
bool
AosTransFileMgr::reqReadFinish(const AosTransFileReqPtr &req)
{
	aos_assert_r(req, false);
	req->setReadFinish();
	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr"
			<< "; req read finish"
			<< "; req:" << req->toString()
			<< "; " << endl;
	}

	map<u32, AosReadRcdPtr>::iterator itr;
	int crt_seq;
	bool is_stale;
	if(req->getType() != AosTransFileReq::eClean)
	{
		int read_id = req->getReadId();	

		itr = mCrtReadRcd.find(read_id);
		aos_assert_r(itr != mCrtReadRcd.end(), false);
		mCrtReadRcd.erase(itr);
		
		if(!mCrtReadRcd.empty())
		{
			// also has someone read.
			if(mShowLog)
			{
				for(itr = mCrtReadRcd.begin(); itr != mCrtReadRcd.end(); itr++)
				{
					crt_seq = (itr->second)->mCrtFileSeq;
					is_stale = (itr->second)->mIsStaleFile;
					OmnScreen << "TransServer; FileMgr"
						<< "; req read finish; read_id:" << read_id
						<< "; but read_id:" << itr->first << " not finish yet."
						<< "; crt_seq:" << crt_seq
						<< "; is_stale:" << is_stale
						<< "; " << endl;
				}
			}
			return true;
		}
	}
	else
	{
		// check.
		for(itr = mCrtReadRcd.begin(); itr != mCrtReadRcd.end(); itr++)
		{
			crt_seq = (itr->second)->mCrtFileSeq;
			is_stale = (itr->second)->mIsStaleFile;
			aos_assert_r(crt_seq == -1, false);
		}
	}

	if(mShowLog)
	{
		OmnScreen << "TransServer; FileMgr; mergeStaleFiles;"
			<< "; start:" << mStarted << endl;
	}
	
	if(!mStarted)	return true;

	saveMaxRecvTid();
	mTransFile->mergeStaleFiles();
	cleanFinishLogs();
	mCrtCleanSeq = -1;
	return true;
}
*/

bool
AosTransFileMgr::mergeNewerFiles()
{
	bool rslt = mTransFile->mergeNewerFiles();
	aos_assert_r(rslt, false);

	return true;
}


bool
AosTransFileMgr::saveToNormFile(const AosTransPtr &trans)
{
	//OmnScreen << "save trans. finish later.";
	return true;

	aos_assert_r(trans && trans->isNeedSave(), false);	
	
	AosBuffPtr data = trans->getConnBuff();
	aos_assert_r(data, false);

	u32 data_len = data->dataLen();
	AosBuffPtr trans_buff = OmnNew AosBuff(data_len + 20 AosMemoryCheckerArgs);
	trans_buff->setU32(data_len);
	trans_buff->setBuff(data);
	
	if(mShowLog)
	{
		OmnScreen << "save Trans to norm file;"
			<< "; trans_id:"<< trans->getTransId().toString()
			<< "; data_len:" << data_len
			<< endl;
	}
	bool rslt = mTransFile->saveToNormFile(trans_buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTransFileMgr::saveToNewerFile(const AosTransPtr &trans)
{
	//OmnScreen << "save trans. finish later.";
	return true;

	aos_assert_r(trans && trans->isNeedSave(), false);	
	
	bool rslt = mTransFile->saveToNewerFile(trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTransFileMgr::saveToNormFile(const AosBuffPtr &trans_buff)
{
	//OmnScreen << "save trans. finish later.";
	return true;
	
	aos_assert_r(trans_buff, false);

	bool rslt = mTransFile->saveToNormFile(trans_buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTransFileMgr::getMaxRecvTid(map<u64, u64> & max_recv_seq)
{
	bool rslt;
	int read_seq = -1;
	u32 file_len = mMaxRecvFile->getLength();
	if(file_len != 0)
	{
		AosBuffPtr buff;
		rslt = mMaxRecvFile->readToBuff(buff, file_len + 1);
		aos_assert_r(rslt && buff, false);
		
		int crt_seq = buff->getInt(-1);
		aos_assert_r(crt_seq >= 0, false);
		while(buff->getCrtIdx() < buff->dataLen())
		{
			u64 key = buff->getU64(0);
			u64 seq = buff->getU64(0);
			max_recv_seq[key] = seq;
		}
		
		if(crt_seq > read_seq)	read_seq = crt_seq;
	}
	
	if(read_seq == -1)
	{
		read_seq = mTransFile->begSeq();
		if(read_seq == -1)	return true;
	}
	
	// read trans file.
	bool finish;
	vector<AosTransId> v_tids;
	while(1)
	{
		AosBuffPtr cont = readBuffFromNormFile(read_seq, finish);
		if(!cont)
		{
			aos_assert_r(finish, false);
			break;
		}

		v_tids.clear();
		readAllTransIds(cont, v_tids);
		for(u32 i=0; i<v_tids.size(); i++)
		{
			AosTransId trans_id = v_tids[i];
			u64 key = trans_id.getCltKey(); 
			u64 seq = trans_id.getSeqno();
			if(seq > max_recv_seq[key])	max_recv_seq[key] = seq;
		}
		
		if(finish)	break;
		read_seq = mTransFile->getNextSeqno(read_seq);
	}
	return true;
}


bool
AosTransFileMgr::readAllTransIds(
		const AosBuffPtr &cont,
		vector<AosTransId> &v_tids)
{
	aos_assert_r(cont, false);	
	
	u32 trans_size;
	while(1)
	{
		trans_size = AosTrans::getNextTransSize(cont);
		if(!trans_size) break;
	
		AosTransId trans_id = AosTrans::getTransIdFromBuff(cont); 
		v_tids.push_back(trans_id);
		
		cont->setCrtIdx(cont->getCrtIdx() + trans_size);
	}
	return true;
}


bool
AosTransFileMgr::saveMaxRecvTid()
{
	aos_assert_r(mMaxRecvFile, false);

	int end_seq = mTransFile->endSeq();	
	if(end_seq == -1)	return true;

	AosBuffPtr buff = OmnNew AosBuff(500, 0 AosMemoryCheckerArgs);
	buff->setInt(end_seq);
	mTransSvr->serializeMaxRecvSeq(buff);

	bool rslt = mMaxRecvFile->put(0, buff->data(), buff->dataLen(), true);
	aos_assert_r(rslt, false);
	return true;
}


//=============================
bool
AosTransFileMgr::cleanFinishLogs()
{
	mLock->lock();
	OmnScreen << "clean Finish logs;" << endl;	
	mFinishLog->startNextActive();
	int end_seq = mFinishLog->endSeq();

	AosBuffPtr ids_buff = OmnNew AosBuff(500, 0 AosMemoryCheckerArgs);
	u32 max_id_per_file = eMaxTransFileSize / sizeof(AosTransId);
	int remain_num = mFinishTids.size();

	setitr_t itr = mFinishTids.begin();
	AosTransId trans_id;
	while(1)
	{
		if(remain_num <=0)	break;	
		u32 save_num = max_id_per_file < (u32)remain_num ? max_id_per_file : remain_num;

		for(u32 i=0; i<save_num; i++, itr++)
		{
			aos_assert_r(itr != mFinishTids.end(), false);
			trans_id = *itr;	
			trans_id.serializeTo(ids_buff);
		}
		mFinishLog->addData(ids_buff->data(), ids_buff->dataLen());
		remain_num -= save_num;	
		ids_buff->reset();
	}
	
	if(end_seq == -1)
	{
		mLock->unlock();
		return true;	
	}
	
	int beg_seq = mFinishLog->begSeq();
	aos_assert_r(beg_seq <= end_seq, false);
	for(int seq=beg_seq; seq<=end_seq; seq++)
	{
		mFinishLog->deleteFile(seq);
	}

	mLock->unlock();
	return true;
}


bool
AosTransFileMgr::isFinish(AosTransId &trans_id)
{
	// this func is only called by TransServer.
	// when trans is resend. and need resp, need save.
	
	bool finish = false;
	mLock->lock();
	finish = isFinishPriv(trans_id);
	if(finish)
	{
		mLock->unlock();
		return finish;
	}

	//check mTmpFinishTid.
	u64 key = trans_id.getCltKey(); 
	mapitr_t itr = mTmpFinishTid.find(key);
	if(itr == mTmpFinishTid.end())
	{
		mLock->unlock();
		return finish;
	}
	
	set_t & tid_set = itr->second;

	setitr_t s_itr = tid_set.find(trans_id);
	finish = (s_itr != tid_set.end());
	mLock->unlock();

	return finish;
}

bool
AosTransFileMgr::isFinishPriv(const AosTransId &trans_id)
{
	setitr_t itr = mFinishTids.find(trans_id);
	bool exist = (itr != mFinishTids.end());
	return exist;
}


void
AosTransFileMgr::resetTransSvr(const AosTransServerPtr &trans_svr)
{
	mTransSvr = trans_svr;
}

/*
void
AosTransFileMgr::cleanTransIds(vector<AosTransId> &v_tids)
{
	//mTidBitmap->removeFinishTid(trans_id);
	if(v_tids.size() == 0)	return;

	AosBuffPtr ids_buff = OmnNew AosBuff(500, 0 AosMemoryCheckerArgs);
	ids_buff->setU32(v_tids.size());
	ids_buff->setU8(eCleanTransId);

	setitr_t itr;
	mLock->lock();
	for(u32 i=0; i<v_tids.size(); i++)
	{
		AosTransId trans_id = v_tids[i];
		itr = mFinishTids.find(trans_id);
		if(itr != mFinishTids.end()) mFinishTids.erase(itr);
		trans_id.serializeTo(ids_buff);
		
		if(mShowLog)
		{
			OmnScreen << "TransServer; clean trans id:" << trans_id.toString() 
				<< endl;
		}
	}

	mFinishLog->addMergeData(ids_buff->data(), ids_buff->dataLen());
	mLock->unlock();
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; append clean finish ids to file."
			<< endl;
	}
}
*/


bool
AosTransFileMgr::finishTrans(vector<AosTransId> &v_tids)
{
	// add to the bitmap.
	//mTidBitmap->addFinishTids(trans_ids);
	if(v_tids.size() == 0)	return true;

	AosBuffPtr ids_buff = OmnNew AosBuff(500, 0 AosMemoryCheckerArgs);
	//ids_buff->setU32(v_tids.size());
	//ids_buff->setU8(eFinishTransId);

	mLock->lock();
	for(u32 i=0; i<v_tids.size(); i++)
	{
		AosTransId tid = v_tids[i];
		if(tid == AosTransId::Invalid)	continue;

		mFinishTids.insert(tid);
		tid.serializeTo(ids_buff);
		
		addToTmpFinishTids(tid);
		
		printFinishedTid(tid);
	}
	printFinishedTid(AosTransId::Invalid);
	
	mFinishLog->addData(ids_buff->data(), ids_buff->dataLen());
	int act_seq = mFinishLog->activeSeq();
	mLock->unlock();
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; append finish ids to file."
			<< "; crt_act_seq:" << act_seq
			<< endl;
	}
	return true;
}


bool
AosTransFileMgr::addToTmpFinishTids(AosTransId &tid)
{
	u64 key = tid.getCltKey(); 

	set_t emp_tid_set; 
	pair<mapitr_t, bool> pr;
	pr = mTmpFinishTid.insert(make_pair(key, emp_tid_set));
	if(pr.second)
	{
		// maybe this is a new entry.
	}
	
	set_t & tid_set = (pr.first)->second;
	if(tid_set.size() >= eMaxTmpTids)
	{
		setitr_t s_itr = tid_set.begin();
		tid_set.erase(s_itr);
	}
	
	tid_set.insert(tid);
	return true;
}


void
AosTransFileMgr::printReadedTrans(
		const AosTransId &trans_id,
		const bool finish)
{
	if(!mShowLog)	return;
	
	static int r_num = 0;
	static OmnString ss = "TransServer; read from file; ";
	if(trans_id == AosTransId::Invalid)
	{
		// means print remain.
		if(!r_num)	return;
		
		OmnScreen << ss << endl;
		r_num = 0;
		ss = "TransServer; read from file; ";
		return;
	}

	r_num++;
	ss << "trans_id:" << trans_id.toString()
		<< "; finish:" << finish << ". ";
	if(r_num < 5)	return;
	
	OmnScreen << ss << endl;
	r_num = 0;
	ss = "TransServer; read from file; ";
}


void
AosTransFileMgr::printCleanedTid(const AosTransId &tid)
{
	if(!mShowLog)	return;
	
	static int c_num = 0;
	static OmnString ss = "TransServer; clean trans id; ";
	if(tid == AosTransId::Invalid)
	{
		// means print remain.
		if(!c_num)	return;
		
		OmnScreen << ss << endl;
		c_num = 0;
		ss = "TransServer; clean trans id; ";
		return;
	}

	c_num++;
	ss << tid.toString() << "; ";
	if(c_num < 10)	return;
	
	OmnScreen << ss << endl;
	c_num = 0;
	ss = "TransServer; clean trans id; ";
}


void
AosTransFileMgr::printFinishedTid(const AosTransId &tid)
{
	if(!mShowLog)	return;
	
	static int f_num = 0;
	static OmnString ss = "TransServer; finish trans; ";
	if(tid == AosTransId::Invalid)
	{
		// means print remain.
		if(!f_num)	return;
		
		OmnScreen << ss << endl;
		f_num = 0;
		ss = "TransServer; finish trans; ";
		return;
	}

	f_num++;
	ss << tid.toString() << "; ";
	if(f_num < 10)	return;
	
	OmnScreen << ss << endl;
	f_num = 0;
	ss = "TransServer; finish trans; ";
}


/*
bool
AosTransFileMgr::initFinishTids()
{
	if(mFinishLog->empty())	return true;
	
	bool rslt;
	int file_seqno = mFinishLog->begSeq();
	while(file_seqno >=0)
	{
		AosBuffPtr cont;
		mFinishLog->readEachFile(file_seqno, cont);
		aos_assert_r(cont, false);
		if(mShowLog)
		{
			OmnScreen << "init mFinishTids"
				<< "; read log file. seq:" << file_seqno
				<< "; file_len:" << cont->dataLen()
				<< endl;
		}
		rslt = initFinishTids(cont); 
		aos_assert_r(rslt, false);
		file_seqno = mFinishLog->getNextSeqno(file_seqno);
	}
	return true;
}


bool
AosTransFileMgr::initFinishTids(const AosBuffPtr &cont)
{
	aos_assert_r(cont, false);
	cont->setCrtIdx(0);
	while(cont->getCrtIdx() < cont->dataLen())
	{
		u32 id_num = cont->getU32(0); 
		u8 is_clean = (cont->getU8(0) == eCleanTransId);

		setitr_t itr;
		while(id_num--)
		{
			AosTransId tid = AosTransId::serializeFrom(cont);
			if(tid == AosTransId::Invalid)	break;
			
			if(!is_clean)	
			{
				mFinishTids.insert(tid);
				continue;
			}
			itr = mFinishTids.find(tid);
			if(itr != mFinishTids.end()) mFinishTids.erase(itr);
		}
	}
	return true;
}
*/


/*
bool
AosTransFileMgr::addReadReq(const AosTransFileReqPtr &req)
{
	aos_assert_r(req, false);
	
	mReadLock->lock();
	if(req->getType() == AosTransFileReq::eClean)
	{
		if(mReqs.size() !=0 || mCrtReadRcd.size() != 0)
		{
			// means in the queue. has some read file req. 
			// so this clean req can ignore.
			mReadLock->unlock();
			return true;
		}
	}

	if(mShowLog)
	{
		OmnScreen << "TransServer; add file req"
			<< "; file_req:" << req->toString()
			<< endl;
	}

	mReqs.push(req);
	mCondVar->signal();
	mReadLock->unlock();
	
	req->wait();
	return true;
}
*/


