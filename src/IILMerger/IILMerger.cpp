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
// 1. This class maintaines the fixed numbers of buckets to cache iiltrans 
// that is a sorted  array of AosIILEntry. 
// 2. Every bucket has boundary of low and hight, the entries will append
// to the matched bucket by the range. 
// 3. The bucket can automaticly ajust range.  The mechanism of mergering 
// and spliting bucket by the aging and total entries  of the bucket. 
// If the bucket is too old. it will be merged to it's left bucket or the
// right bucket, and the bucket will free. If the bucket has full, 
// the entries will be proccesed, and the bucket will free. when the entrie 
// has not in all the buckets range .it will get the  free buckets 
// to split the range.
// 
// Modification History:
// 04/09/2012 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "IILMerger/IILMerger.h"

#include "IILMerger/IILMergerMgr.h"
#include "SEInterfaces/JobMgrObj.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include "Thread/ThreadPool.h"


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("iilmerger", __FILE__, __LINE__);


AosIILMerger::AosIILMerger(
		const AosIILMergerHandlerPtr &handler, 
		const int maxbkt,
		const u32 maxmementries, 
		const u32 maxentries, 
		const u32 layer,
		AosIILMerger *pmerger)
:
mHandler(handler),
mLock(OmnNew OmnMutex()),
mTotalLock(OmnNew OmnMutex()),
mMaxBucket(maxbkt),
mTotalClient(0),
mNumSyncs(-1),
mTotalFinished(0),
mFinishedEntries(0),
mComp(mHandler->getComp()),
mMaxMemEntries(maxmementries),
mMaxEntries(maxentries),
mLayer(layer),
mFileId(0),
mAddTotal(0),
mAppendTotal(0),
mProcTotal(0),
mParentMerger(pmerger),
mData(0)
{
}


AosIILMerger::~AosIILMerger()
{
	if (mFile)
	{
	    mFile->deleteFile();
	}

	// Chen Ding, 09/03/2012
	if (mData) OmnDelete [] mData;
	mData = 0;
}


bool 
AosIILMerger::addDataSync(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	// When a sender sent some data, receiver will receive a request
	// and it will call this function. 
	
	// ken 2012/08/01
	int size = buff->dataLen() / mComp->size;
	char * data = buff->data(); 
	for(int i=0; i<size; i++)
	{
		if(data[0] == 0)
		{
			OmnAlarm << "the entry is empty" << enderr;
		}
		data += mComp->size;
	}
	
	mTotalLock->lock();
	mAddTotal += size;
	mTotalLock->unlock();

	// This is the entrance function for this class. 
	vector<OmnThrdShellProcPtr> runners;
	bool rslt = addDataPriv(buff, runners, true, rdata);

	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->procSync(runners);
}


bool 
AosIILMerger::addDataAsync(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	mTotalLock->lock();
	mAddTotal += buff->dataLen()/mComp->size;
	mTotalLock->unlock();

	// This is the entrance function for this class. 
	vector<OmnThrdShellProcPtr> runners;
	bool rslt = addDataPriv(buff, runners, false, rdata);

	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->procSync(runners);
}


bool 
AosIILMerger::addDataPriv(
		const AosBuffPtr &buff, 
		vector<OmnThrdShellProcPtr> &runners,
		const bool needmem,
		const AosRundataPtr &rdata)
{
	// this function is to append the data to merger or bucket
	// first,append the data to the merger memory
	// if the merger memory is overflow
	// then split the data and the merger memory's data to the buckets
	aos_assert_r(mComp, false);
	AosBuffArray array(buff, mComp, mHandler->getStable()); 
	if (array.getNumEntries() <= 0) return true;

	char* crt = array.first();
	aos_assert_r(crt, false);
	bool rslt;

	mLock->lock();
	int num = array.getNumEntries();
	sanitycheck(crt, num);
	if (mBuckets.size() <= 0)
	{
		if (!needmem)
		{
			rslt = splitRange(crt, num);
			aos_assert_rl(rslt, mLock, false);
		}
		else
		{
			if (!mData)
			{
				mData = (char*)OmnNew char[(*mComp).size * mMaxEntries];
				mAppendTotal = 0;
			}
			if (mAppendTotal + num < mMaxEntries)
			{
				// append to merger memory
				memcpy(&mData[mAppendTotal*(*mComp).size], crt, num*(*mComp).size);
				mAppendTotal += num;
				sanitycheck(mData, mAppendTotal);
				mLock->unlock();
				return true;
			}
			else
			{
				// merger memory overflow, splitRang
				AosBuffPtr newbuff = OmnNew AosBuff((*mComp).size*(mAppendTotal + num), 0 AosMemoryCheckerArgs);		
				int pos = 0;
				if (mAppendTotal)
				{
					memcpy(&newbuff->data()[pos], mData, mAppendTotal * (*mComp).size);
					pos += mAppendTotal * mComp->size;
				}
				OmnDelete [] mData;
				mData = 0;
				memcpy(&newbuff->data()[pos], crt, num * (*mComp).size);
				newbuff->setDataLen((*mComp).size*(mAppendTotal + num));
				mAppendTotal = 0;
				AosBuffArray newarray(newbuff, mComp, mHandler->getStable());
				newarray.sort();
				sanitycheck(newarray.first(), newbuff->dataLen()/mComp->size);
				rslt = splitRange(newarray.first(), newbuff->dataLen()/mComp->size);
				aos_assert_rl(rslt, mLock, false);
				rslt = appendData(newarray, runners, rdata);
				aos_assert_rl(rslt, mLock, false);
				mLock->unlock();
				//aos_assert_r(rslt, false);
				//rslt = addDataPriv(newbuff, runners, needmem, rdata);
				return rslt;
			}
		}
	}

	rslt = appendData(array, runners, rdata);
	mLock->unlock();

	return rslt;
}

char* 
AosIILMerger::findBoundary(
		char *begin, 
		char *high,
		const int length)
{
	char* low = begin;
	int half;
	char* mid;
	int len = length;
	while(len)
	{
		half = (len/(*mComp).size)/2;
		mid = low;
		mid += half*(*mComp).size;
		if (compare(mid, high) <= 0)
		{
			low = mid;
			low += (*mComp).size;
			len -= (half+1)*(*mComp).size;
		}
		else
		{
			len = half*(*mComp).size;
		}
	}
	return low;
}

bool
AosIILMerger::appendData(
		AosBuffArray &array, 
		vector<OmnThrdShellProcPtr> &runners,
		const AosRundataPtr &rdata)
{
	char* crt = array.first();
	aos_assert_r(crt, false);
	list<AosIILMergerBktPtr>::iterator bkt = mBuckets.begin();
	aos_assert_rr(bkt != mBuckets.end(), rdata, false);
	int index = 0;
	//Loop on all the buckets and appand entries.
	for (; crt && bkt != mBuckets.end();)
	{
		index++;
		aos_assert_r(*bkt, false);
		aos_assert_r(crt && compare(crt ,array.first())>= 0 && compare(crt ,array.last()-(*mComp).size) <= 0, false);
		char *last = crt;
		if (compare(crt, (*bkt)->getHigh()) > 0)
		{
			// The records cannot be put in 'bkt'.
			++bkt;
		}
		else
		{
			// This means there are some entries for 'bkt'.
			// Check whether the last record is no bigger than the bkt high.
			// If yes, all the records 'belong' to 'bkt'.
			int num = 0;
			int length = array.last() - crt;
			if (compare(array.last() - mComp->size, (*bkt)->getHigh()) > 0)
			{
				last = findBoundary(crt, (*bkt)->getHigh(), length);
				aos_assert_rr(last, rdata, false);
				num = (last - crt)/mComp->size;
			}
			else
			{
				num = length / mComp->size;
				last = 0;
			}

			// 'last' points to the first record that is bigger than 'bkt's high.
			// If no such record, 'last' is 0.
			// Next we need to find all (except the last one) buckets whose low 
			// and high are the same as 'bkt's high. These buckets are called 
			// 'Equal Range Buckets'. If the last bucket belongs to 'Equal
			// Range Buckets', its low must be the same as 'bkt's high, but
			// its high may or may not be the same as 'bkt's high.
			vector<AosIILMergerBktPtr> buckets;
			char *lastentrie = crt + (num - 1) * mComp->size;
			list<AosIILMergerBktPtr>::iterator nextbkt = bkt;
			bool rslt = findBuckets(lastentrie, num, buckets, nextbkt);
			aos_assert_r(rslt, false);
			int bktnum = buckets.size();
			aos_assert_r(bktnum >= 1, false);
			int remain_num = (array.last() - crt) / mComp->size;
			if (num > bktnum && bktnum > 1)
			{
				rslt = splitData(crt, array, num, buckets, runners, rdata);
				aos_assert_r(rslt, false);
				bkt = nextbkt;

				// If there are still records, need to back off one bucket.
				if (remain_num > num)
				{
					--bkt;
				}
			}
			else
			{
				// This means either num <= bktnum or there is only one bucket.
				// In this case, we only need to use just one bucket.
				OmnThrdShellProcPtr runner = OmnNew BucketAppend(this, (*bkt).getPtr(), array.getBuff(), crt, num, rdata);
				runners.push_back(runner);
				++bkt;
			}
		}

		crt = last;
	}

	// there is no bucket can put into the data
	// put the data into the last bucket
	if (crt)
	{
		bkt = --(mBuckets.end());
		aos_assert_r(compare(crt, (*bkt)->getHigh()) > 0, false);
		int length = array.last() - crt;
		int num = length / mComp->size;
		OmnThrdShellProcPtr runner = OmnNew BucketAppend(this, (*bkt).getPtr(), array.getBuff(), crt, num, rdata);         
		runners.push_back(runner);
	}
	return true; 
}

bool
AosIILMerger::splitData(
		char *crt,
		AosBuffArray &array,
		const int num,
		const vector<AosIILMergerBktPtr> &buckets,
		vector<OmnThrdShellProcPtr> &runners,
		const AosRundataPtr &rdata)
{
	int bktnum = buckets.size();
	aos_assert_r(bktnum > 1, false);
	int prenum = num / bktnum;
	int hasnum = 0;
	for(int i=0; i<bktnum; )
	{
		if (!crt) 
		{
			break;
		}
		aos_assert_r(hasnum <= num, false);
		if (hasnum == num) break;
		char *last = crt + (prenum-1) * mComp->size;
		if (compare(crt, buckets[i]->getLow()) >= 0 &&
			compare(last, buckets[i]->getHigh()) <= 0)
		{
			OmnThrdShellProcPtr runner = OmnNew BucketAppend(this, buckets[i].getPtr(), array.getBuff(), crt, prenum, rdata);
			runners.push_back(runner);
			hasnum += prenum;	
			crt = last + mComp->size;
			i++;
			continue;
		}
	
		aos_assert_r(compare(last, buckets[i]->getHigh()) <= 0, false);

		if (compare(crt, buckets[i]->getLow()) < 0)
		{
			if (i == 0)
			{
				OmnThrdShellProcPtr runner = OmnNew BucketAppend(this, buckets[i].getPtr(), array.getBuff(), crt, prenum, rdata);
				runners.push_back(runner);
				hasnum += prenum;	
				crt = last + mComp->size;
				i++;
				continue;
			}
			int restnum = num - hasnum;
			if (restnum < bktnum)
			{
				prenum = restnum;
			}
			else
			{
				prenum = restnum/bktnum;
			}
			i = 0;
			continue;
		}
	}

	if (hasnum < num)
	{
		aos_assert_r(crt && compare(crt ,array.first())>= 0 && compare(crt ,array.last()-(*mComp).size) <= 0, false);
		aos_assert_r(compare(crt, buckets[bktnum-1]->getLow()) >= 0, false);
		int restnum = num - hasnum;
		char *last = crt + (restnum-1) * mComp->size;
		aos_assert_r(compare(last, buckets[bktnum-1]->getHigh()) <=0, false);
		
		OmnThrdShellProcPtr runner = OmnNew BucketAppend(this, buckets[bktnum-1].getPtr(), array.getBuff(), crt, restnum, rdata);
		runners.push_back(runner);
	}
	return true;
}

bool
AosIILMerger::findBuckets(
		char *crt, 
		const int num,
		vector<AosIILMergerBktPtr> &buckets,
		list<AosIILMergerBktPtr>::iterator &pos)
{
	// This function finds all the buckets whose low and high (except the last one)
	// are the same as 'crt'. For the last one, its low must be the same as 'crt'
	// but its high can be bigger than 'crt'.
	list<AosIILMergerBktPtr>::iterator nextpos;
	if (compare(crt, (*pos)->getHigh()) < 0 || num == 1)
	{
		buckets.push_back(*pos);
		return true;
	}
	
	aos_assert_r(compare(crt, (*pos)->getHigh()) == 0, false);
	buckets.push_back(*pos);
	nextpos = pos;
	++nextpos;
	for(; nextpos != mBuckets.end(); ++nextpos)
	{
		if(compare(crt, (*nextpos)->getLow()) == 0 &&
		   compare((*pos)->getHigh(), (*nextpos)->getLow())== 0)
		{
			pos = nextpos;
			buckets.push_back(*pos);
		}
		else
		{
			break;
		}
	}

	pos = nextpos;
	return true;
}

bool
AosIILMerger::splitRange(const char *start, const int length)
{
	aos_assert_r(length, false);
	int num = mMaxBucket;
	aos_assert_r(length > num, false);
	int inc = (length/num) * (*mComp).size;
	aos_assert_r(inc, false);
	char* low = (char *)start;
	for (int i=0; i<num; i++)
	{
		aos_assert_r(compare(low, start) >= 0 && low-start+1 <= length*(*mComp).size, false);	
		aos_assert_r(low+inc <= start+length*(*mComp).size, false);
		AosIILMergerBktPtr bkt = OmnNew AosIILMergerBkt(this, mMaxMemEntries, mMaxEntries);
		aos_assert_r(low, false);
		aos_assert_r(low+inc-(*mComp).size, false);
		bkt->setLow(low);
		bkt->setHigh(low+inc-(*mComp).size);
		mBuckets.push_back(bkt);
		list<AosIILMergerBktPtr>::iterator itr = mBuckets.end();
		bkt->setBktItr(--itr);
		low += inc;
	}
	return true;
}

int
AosIILMerger::compare(const char* lhs, const char* rhs)
{
	aos_assert_r(mComp, false);
	return mComp->cmp(lhs, rhs);
}

bool 
AosIILMerger::sanitycheck(char *begin, int length)
{
	return true;
	for (int i=0; i<length; i++)
	{
		aos_assert_r(begin+i*(*mComp).size != 0, false);
		aos_assert_r(strcmp(begin+i*(*mComp).size, "0") >= 0, false);
		aos_assert_r(strcmp(begin+i*(*mComp).size, "{") <= 0, false);
		//aos_assert_r((compare(begin+i*(*mComp).size, mHigh) <= 0) && (compare(begin+i*(*mComp).size, mLow) >= 0), false);
	}
	return true;
}

bool
AosIILMerger::mergeAllData(const AosRundataPtr &rdata)
{
	mTotalLock->lock();

	if (mFinishedEntries > 0)
	{
		aos_assert_rl(mFinishedEntries >= (u64)mAddTotal, mTotalLock, false);
		if (mFinishedEntries != (u64)mAddTotal || mAddTotal != mAppendTotal)
		{
			OmnScreen << " mergerLayer == " << mLayer << " mAddTotal == " 
				<< mAddTotal << " mAppendTotal = " << mAppendTotal <<endl;
			mTotalLock->unlock();
			OmnSleep(1);
			OmnThrdShellProcPtr runner = OmnNew MergeAllData(this, rdata);
			
			aos_assert_r(sgThreadPool, false);
			return sgThreadPool->proc(runner);
		}
		else
		{
			mTotalLock->unlock();
		}
	}
	else if(mAddTotal != mAppendTotal)
	{
		OmnScreen << "merger:" << this << "  mergerLayer == " << mLayer << " mAddTotal == " 
			<< mAddTotal << " mAppendTotal = " << mAppendTotal <<endl;
		mTotalLock->unlock();
		OmnSleep(1);
		OmnThrdShellProcPtr runner = OmnNew MergeAllData(this, rdata);
			
		aos_assert_r(sgThreadPool, false);
		return sgThreadPool->proc(runner);
	}
	else
	{
		mTotalLock->unlock();
	}

	if (mBuckets.size() <= 0)
	{
		if (mAppendTotal == 0)
		{
			mLock->unlock();
			return true;
		}

		AosBuffPtr newbuff;
		if (mData)
		{
			OmnString fname = createFileName();
			fname << (u64)(unsigned long)this << ".txt";
			mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs); 
			aos_assert_rl(mFile->isGood(), mLock, false);
			mFile->append(mData, mAppendTotal * mComp->size, true);
			newbuff = OmnNew AosBuff(mAppendTotal * mComp->size, 0 AosMemoryCheckerArgs);		
			memcpy(&newbuff->data()[0], mData, mAppendTotal * mComp->size);
			newbuff->setDataLen(mAppendTotal * mComp->size);
			OmnDelete [] mData;
			mData = 0;
		}
		
		if (newbuff)
		{
			bool rslt = mergeData(newbuff, rdata);
			aos_assert_rl(rslt, mLock, false);
		}
		mLock->unlock();
		return true;
	}
	
	vector<OmnThrdShellProcPtr> runners;
	int size = 4; // total memory / bucketedsize
	vector<AosIILMergerBkt*> bkts[size];
	list<AosIILMergerBktPtr>::iterator itr = mBuckets.begin();
	for (int i=0; itr != mBuckets.end(); ++itr, i++)	
	{
		if(mLayer == 0)
		{
			bkts[i%size].push_back((*itr).getPtr());
		}
		else
		{
			(*itr)->lock();
			(*itr)->mergeAllData(rdata);
			(*itr)->unlock();
		}
	}
	mLock->unlock();

	if (mLayer == 0)
	{
		for (int i=0; i<size; i++)
		{
			OmnThrdShellProcPtr runner = OmnNew BktMergeData(bkts[i], rdata);
			runners.push_back(runner);
		}			

		aos_assert_r(sgThreadPool, false);
		return sgThreadPool->procAsync(runners);
	}
	return true;
}


u64 
AosIILMerger::getNumEntries()
{
	u64 total = 0;
	list<AosIILMergerBktPtr>::iterator itr = mBuckets.begin();
	for (; itr != mBuckets.end(); ++itr)	
	{
		total += (*itr)->getTotalSize();
	}
	return total;
}

bool
AosIILMerger::procFinished()
{
	aos_assert_r(mFinishedEntries == (u64)mAddTotal, false);
	aos_assert_r(mAddTotal == mAppendTotal, false);
	mTotalLock->lock();
	while(1)
	{
		//u64 total = getNumEntries();
		OmnScreen << "mergerLayer = " << mLayer << "  proctotal = " << mProcTotal << "  addtotal = " << mAddTotal << endl;
		aos_assert_rl(mProcTotal <= mAddTotal, mTotalLock, false);
		if (mProcTotal == mAddTotal)
		{
			mTotalLock->unlock();
			break;
		}
		mTotalLock->unlock();
		OmnSleep(1);
	}
	return true;
}

/*bool 
AosIILMerger::mergeData(const u32 totalentries, const AosRundataPtr &rdata)
{
	updateProcTotal(totalentries);
	return true;
}
*/
bool 
AosIILMerger::mergeData(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	// ken 2012/08/01
	int size = buff->dataLen() / mComp->size;
	char * data = buff->data(); 
	for(int i=0; i<size; i++)
	{
		if(data[0] == 0)
		{
			OmnAlarm << "the entry is empty" << enderr;
		}
		data += mComp->size;
	}
	
	bool rslt = true;//mHandler->proc(buff, rdata);
	aos_assert_r(rslt, false);
	
	int num = buff->dataLen() / (*mComp).size;
	char * entries = buff->data();
	for (int i=0; i<num; i++)
	{
		extern void procset(string &name);
		string s = entries;
		procset(s);
		entries += (*mComp).size;
	}
	
	updateProcTotal(buff->dataLen()/mComp->size);
	return true;
}


// Chen Ding, 06/23/2012
bool
AosIILMerger::isFinished()const
{
	mTotalLock->lock();
	aos_assert_rl(mNumSyncs >= mTotalClient, mLock, false);
	if (mNumSyncs != mTotalClient) 
	{
		mTotalLock->unlock();
		return false;
	}
	if (mNumSyncs != mTotalFinished) 
	{
		mTotalLock->unlock();
		return false;
	}
	mTotalLock->unlock();
	return true;
}

bool
AosIILMerger::mergeFinshed(
		const OmnString &tableid, 
		const u64 &jobdocid,
		const int &jobServerId,
		const OmnString &tokenid,
		const AosRundataPtr &rdata)
{
	OmnThrdShellProcPtr runner = OmnNew MergeData(this, tableid, jobdocid, jobServerId, tokenid, rdata);

	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->proc(runner);
}

bool
AosIILMerger::toProc(
		const u32 start,
		const OmnString &tableid, 
		const int &jobServerId,
		const u64 &jobdocid,
		const OmnString &tokenid,
		const AosRundataPtr &rdata)
{
	mTotalLock->lock();
	if (mFinishedEntries != (u64)mAddTotal || mAddTotal != mAppendTotal)
	{
		OmnScreen << " mAddTotal == " << mAddTotal << " mAppendTotal == " << mAppendTotal << endl;
		mTotalLock->unlock();
		OmnSleep(1);
		OmnThrdShellProcPtr runner = OmnNew ToProc(this, start, tableid, jobServerId, jobdocid, tokenid, rdata);
	
		aos_assert_r(sgThreadPool, false);
		return sgThreadPool->proc(runner);
	}
	mTotalLock->unlock();

	if (procFinished())
	{
		bool rslt = AosIILMergerMgr::getSelf()->deleteMerger(tableid);	
		aos_assert_r(rslt, false);

		u32 e = OmnGetSecond();
		OmnScreen << "mmmmmmmm : mergerData end, e:" << e << ", spend:" << (e - start) << endl;
		
		extern void procFinished();
		procFinished();	

/*		if (tokenid != "")
		{
			AosJobMgrObjPtr jobmgr = AosJobMgrObj->getJobMgr();
			aos_assert_r(jobmgr, false);
			return jobmgr->mergeFinished(jobServerId, jobdocid, tokenid, rdata);
		}
*/
	}
	return true;
}
	
bool 
AosIILMerger::clear()
{
	mLock->lock();
	mAddTotal = 0;
	mAppendTotal = 0;
	mBuckets.clear();
	mLock->unlock();
	return true;
}

bool 
AosIILMerger::serializeTo(
		const AosBuffPtr&, 
		const AosRundataPtr&)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosIILMerger::serializeFrom(
		const AosBuffPtr&, 
		const AosRundataPtr&)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosIILMerger::BucketAppend::run()
{
	bool rslt;
	AosCompareFunPtr  comp = mMerger->getComp();
	mBkt->lock();
//OmnScreen << "BucketAppend " << mMerger << "," << mBuff->data() << ", nextcrt = " << (mBuff->data()+comp->size) << ", num = " << mNum  
//	<< ", low = " << mBkt->getLow() << ", high=" << mBkt->getHigh() << endl;
	if (mMerger->compare(mCrt, mBkt->getLow()) < 0)
	{
		if (!mCrt)
		{
			OmnAlarm << " mLow is null !" << mCrt << enderr;
			mBkt->unlock();
			return false;
		}
		mBkt->setLow(mCrt);
	}

	if (mMerger->compare(mCrt+(mNum-1)*(comp->size), mBkt->getHigh()) > 0)
	{
		if (!(mCrt+(mNum-1)*(comp->size)))
		{
			OmnAlarm << " mHigh is null !" << (mCrt+(mNum-1)*(comp->size)) << enderr;
			mBkt->unlock();
			return false;
		}
		mBkt->setHigh(mCrt+(mNum-1)*(comp->size));
	}

	if (mMerger->compare(mCrt, mBkt->getLow()) >= 0 &&
		mMerger->compare(mCrt+(mNum-1)*(comp->size), mBkt->getHigh())<=0)
	{
		rslt = mBkt->append(mCrt, mNum, mRundata);
		mBkt->unlock();
		aos_assert_r(rslt, false);
		return rslt;
	}
	mBkt->unlock();
	OmnAlarm << " ======= Nerver come here ! =======" << enderr;
	return false;
}

bool
AosIILMerger::BucketAppend::procFinished()
{
	return true;	
}

bool
AosIILMerger::ToProc::run()
{
	aos_assert_r(mMerger, false);
	return mMerger->toProc(mStart, mTableid, mJobServerId, mJobDocid, mTokenid, mRundata);
}

bool
AosIILMerger::ToProc::procFinished()
{
	return true;
}

bool
AosIILMerger::MergeData::run()
{
	if (mMerger)
	{
		u32 s = OmnGetSecond();
		OmnScreen << "mmmmmmmm : mergerData , s:" << s << endl;
		bool rslt  = mMerger->mergeAllData(mRundata);
		aos_assert_r(rslt, false);
		rslt = mMerger->toProc(s, mTableid, mJobServerId, mJobDocid, mTokenid, mRundata);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool 
AosIILMerger::MergeData::procFinished()
{
	return true;
}

bool 
AosIILMerger::BktMergeData::run()
{
	aos_assert_r(!mBkts.empty(), false);

	bool rslt;
	for (size_t i=0; i<mBkts.size(); i++)
	{
		mBkts[i]->lock();
		rslt = mBkts[i]->mergeAllData(mRundata);
		mBkts[i]->unlock();
		aos_assert_r(rslt, false);
	}

	return true;
}

bool
AosIILMerger::BktMergeData::procFinished()
{
	return true;
}

bool
AosIILMerger::MergeAllData::run()
{
	aos_assert_r(mMerger, false);
	return mMerger->mergeAllData(mRundata);
}

bool
AosIILMerger::MergeAllData::procFinished()
{
	return true;
}
