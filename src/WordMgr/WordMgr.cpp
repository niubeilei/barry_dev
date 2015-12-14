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
//	This class is responsible for creating new WordIds. 
//	It manages two files:
//		WordID file
//		WordIdIdx file
//	The WordID file contains all the word definition. It is the file
//	based on which the WordID hash table is to be constructed. 
//	The WordIdIdx file is used to tell which bucket is stored where.
//	
//
// Modification History:
// 10/04/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "WordMgr/WordMgr1.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Debug.h"
//#include "FmtMgr/FmtMgr.h"
#include "SEInterfaces/FmtMgrObj.h"
#include "FmtMgr/Fmt.h"
#include "IdGen/IdGen.h"
#include "Porting/Sleep.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/SeTypes.h"
#include "StorageMgr/VirtualFileSys.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "WordMgr/WordIdHash.h"
#include "XmlUtil/XmlTag.h"

//extern bool sgIsBigIILServer;		Ketty 2012/10/22	

OmnSingletonImpl(AosWordMgr1Singleton,
                 AosWordMgr1,
                 AosWordMgr1Self,
                "AosWordMgr1");


AosWordMgr1::AosWordMgr1()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mNumMarked(0),
mNumBuckets(0),
mStarted(false),
mIsStopping(false)
{
}


AosWordMgr1::~AosWordMgr1()
{
}


bool
AosWordMgr1::start()
{
	// OmnThreadedObjPtr thisPtr(this, false);
    // mThread = OmnNew OmnThread(thisPtr, "WordMgrThrd", 0, true, true);
    //mThread->start();		// Ketty 2012/08/01
	return true;
}


bool
AosWordMgr1::stop()
{
	mIsStopping = true;
	if (mNumMarked == 0) return 0;

	int guard = 0;
	while (guard < 10 && mNumMarked)
	{
		OmnSleep(1);
		guard++;
	}

	aos_assert_r(mNumMarked == 0, false);
    return true;
}


bool
AosWordMgr1::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr wordmgrconfig = config->getFirstChild(AOSCONFIG_WORDMGR);
	aos_assert_r(wordmgrconfig, false);
	mWordIdName = wordmgrconfig->getAttrStr(AOSCONFIG_WORDID_HASHNAME);

	bool exist;
	mTableSize = wordmgrconfig->getAttrU64(AOSCONFIG_WORDID_TABLESIZE, 0, exist);
	aos_assert_r(mTableSize > 0, false);

	// Ken Lee, 2014/02/11
	AosWordIdHash::config(wordmgrconfig);

	initTablesize();
	
	/*
	 * Linda, 2013/08/16
	// 2. init mWordHash
	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	for(u32 i=0; i < total_vids.size(); i++)
	{
		u32 cube_id = total_vids[i];
		AosWordIdHashPtr wordHash = OmnNew AosWordIdHash(
				cube_id, mTableSize, mWordIdName);
		
		mWordHash.insert(make_pair(cube_id, wordHash));
	}	
	*/

	return true;
}


bool
AosWordMgr1::initTablesize()
{
	// Move from WordIdHash. Ketty 2012/08/01
	u32 tsize = 1;
	for (int i=0; i<AosWordIdHash::eMaxTableSizeBits; i++)         
	{
		tsize = (tsize << 1) + 1;
		if (tsize >= mTableSize)
		{
			mTableSize = tsize;
			break;
		}
	}
	if (mTableSize == 0) mTableSize = AosWordIdHash::eMaxTableSize;
	return true;
}

// This thread will wake up every eRefreshFreq number of seconds.
// When it wakes up, it checks whether there are anything in the
// cached hash table. If yes, it updates the new wordIDs into 
// the WordID file, then it updates the db to indicate that the
// WordID file has been changed. Any server that uses WordID
// file may refresh its contents. Note that this class also 
// uses the WordID file, so will it update its WordID hash table.
bool
AosWordMgr1::threadFunc(
        OmnThrdStatus::E &state,
        const OmnThreadPtr &thread)
{
	//u32 bktkey;
    while (state == OmnThrdStatus::eActive)
    {
		// Ketty 2012/08/01	
		/*
		mLock->lock();
		if (mNumMarked == 0)
		{
			// There is no more cached entries. Start a timer to 
			// wake this thread up later. 
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		// There are something to be processed. Get the first entry
		// from the list, and then release the lock.
		bktkey = mBucketMarks[mNumMarked-1];
		mNumMarked--;
		mLock->unlock();
		mWordHash->updateBktToFile(bktkey);
		*/
	}
	return true;
}


bool
AosWordMgr1::markBucket(const u64 wordId) 
{
	// This function is used to mark the buckets that need to 
	// be updated to WordID file. It is a sorted array of 
	// bucket keys. Its size is 'eMarkSize'. When the array
	// reaches its eMarkSize, it will call the function 
	// 'updateToFile(...)' to flush the dirty buckets into
	// disk. Otherwise, it searches the array to 
	// see whether the bucket was already marked. If yes, it
	// does nothing. Otherwise, it inserts the bucket ID
	// in the right position in the array and returns.

	// 'wordId' is in 64-bits format. Only the lower 5 bytes
	// are used. It will convert 'wordId' into bucket ID
	// which is byte[4], byte[3], byte[2], byte[1], then 
	// shift right 2 bits, and then the least 20 bits. 

	OmnNotImplementedYet;
	return false;
	/*
	u32 bktId = ((u32)(wordId >> 10)) & mTableSize;
	if (mNumMarked == 0)
	{
		// No marks yet. 
		mNumMarked = 1;
		mBucketMarks[0] = bktId;
		mCondVar->signal();
		return true;
	}

	// It uses the binary search algorithm.
	int left = 0;
	int right = mNumMarked-1;
	int idx;
	while (left <= right)
	{
		idx = left + ((right - left) >> 1);
		if (mBucketMarks[idx] == bktId) 
		{
			// The bucket is already in the array.
			return true;
		}

		if (bktId < mBucketMarks[left])
		{
			// It means the bucket is not in the array and it
			// should be inserted in front of 'left'.
			idx = left;
			break;
		}

		if (bktId > mBucketMarks[right])
		{
			// It means the bucket is not in the array and
			// it should be inserted after 'right'.
			idx = right+1;
			break;
		}
		
		if (bktId < mBucketMarks[idx]) 
		{
			if (left == right)
			{
				// This means we need to insert it in front
				// of 'idx'.
				break;
			}
			right = idx-1;
		}
		else 
		{
			if (left == right)
			{
				idx++;
				break;
			}
			left = idx+1;
		}
	}

	// It means it did not find the bucket.	It should insert 
	// the bucket in front of 'idx'.
	if (mNumMarked == eMarkSize) 
	{
		// The marks overflowed. Need to update to file.
//		int bb = mNumMarked-1;
//		mNumMarked--;
//		mLock->unlock();
		
		//ken 2011/10/28
//		u32 bktid = mBucketMarks[bb];
		OmnScreen << "Bucket full" << endl;
		mWordHash->updateBktToFile(bktId);
		return true;
		//mWordHash->updateBktToFile(bb);
//		mLock->lock();
	}

	aos_assert_r(mNumMarked < eMarkSize, false);
	// Need to move the elements that are after 'idx'
	if (idx == mNumMarked)
	{
		mBucketMarks[idx] = bktId;
		mNumMarked++;
	}
	else
	{
		memmove(&mBucketMarks[idx+1], &mBucketMarks[idx], ((mNumMarked-idx) << 2));
		mNumMarked++;
		mBucketMarks[idx] = bktId;
	}

	mCondVar->signal();
	return true;
	*/
}


u64
AosWordMgr1::addWord(
		const AosRundataPtr &rdata,
		const char *word, 
		const int len, 
		const u64 id)
{
	int vid = AosGetCubeId(word, len);
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, 0);

	u64 wordid = wordHash->addWord(word, len, id);
	//markBucket(wordid);
	wordHash->updateBktToFile(wordid);

	OmnString str_word(word, len);

	//mLock->unlock();

//OmnString word_str(word, len);
//OmnScreen << "**********AddWord; vid:" << vid
//		<< "; iilid:" << id
//		<< "; word:" << word_str
//		<< "; wordId:" << wordid
//		<< endl;

	AosFmtPtr fmt = createFmt();
	sendFmt(rdata, fmt);
	return wordid;
}


u64 
AosWordMgr1::recoverWord(
		const char *word,
		const int wordlen)
{
	int vid = AosGetCubeId(word, wordlen);
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, 0);

	u64 wordid = wordHash->recoverWord(word, wordlen);
	return wordid;
}


u64
AosWordMgr1::getWordId(const OmnString &word, const bool addFlag)
{
	return getWordId((const u8*)word.data(), word.length(), addFlag);
}

	
// This function retrieves the WordId for the word. If no WordID
// has not been created for the word, it will create one and 
// insert it into the hash table. 
// 
// !!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!
// The memory returned shall not be kept by the caller. The caller
// should never modify the memory. 
//
// Returns
// The function should always return a wordId unless it fails. The
// only possibility of failing is db operations.
u64
AosWordMgr1::getWordId(const u8 *word, const int len, const bool addFlag)
{
	// Ketty 2013/01/04
	int vid = AosGetCubeId((char *)word, len);
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, 0);

	u64 wordId = wordHash->getWordId((char *)word, len, addFlag);

//OmnString str_word((char*)word, len);
//OmnScreen << "******* get wordid:"
//		<< "; word:" << str_word 
//		<< "; wordId:" << wordId
//		<< "; virtualId: " << vid
//		<< endl;
	
	if (wordId != AOS_INVWID) 
	{
		if (wordHash->isWordIdNew())
		//if (mWordHash->isWordIdNew())
		{
			//markBucket(wordId);
			wordHash->updateBktToFile(wordId);	//Ketty 2012/08/01
		}
		//mLock->unlock();
		return wordId;
	}

	if (!addFlag) 
	{
		//mLock->unlock();
		return AOS_INVWID;
	}

	// This is an error
	//mLock->unlock();
	OmnAlarm << "Failed to retrieve WordID" << enderr;
	return AOS_INVWID;
}


bool
AosWordMgr1::getId(const u64 &wordId, u64 &id)
{
	int vid = getVirtualIdByWordId(wordId);
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, false);

	bool rslt = wordHash->getId(wordId, id);
	return rslt;
}


bool
AosWordMgr1::setId(const u64 &wordId, const u64 id)
{
	int vid = getVirtualIdByWordId(wordId);
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, false);
	
	bool rslt = wordHash->setId(wordId, id);
	return rslt;
}


bool    
AosWordMgr1::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool    
AosWordMgr1::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


char *	
AosWordMgr1::getWord(const u64 &wordId, u32 &len)
{
	// Ketty 2012/08/01
	int vid = getVirtualIdByWordId(wordId);
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, 0);

	return wordHash->getWord(wordId, len);
}


bool	
AosWordMgr1::saveId(const u64 &wordId, const u64 id)
{
	// Ketty 2012/08/01
	int vid = getVirtualIdByWordId(wordId);
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, false);

	return wordHash->saveId(wordId, id);
}


u32		
//AosWordMgr1::getWordHashSize() const
AosWordMgr1::getWordHashSize(const u32 vid) 
{
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, 0);
	
	return wordHash->getTablesize();
}


u32		
//AosWordMgr1::getMaxConflictNum() const
AosWordMgr1::getMaxConflictNum(const u32 vid) 
{
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, 0);
	
	return wordHash->getMaxConflictNum();
}


u32		
//AosWordMgr1::getMaxNumElems() const
AosWordMgr1::getMaxNumElems(const u32 vid) 
{
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, 0);
	
	return wordHash->getMaxNumElems();
}


u32		
//AosWordMgr1::getMaxBucketLen() const
AosWordMgr1::getMaxBucketLen(const u32 vid) 
{
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, 0);

	return wordHash->getMaxBucketLen();
}


u32		
//AosWordMgr1::getTotalWords() const
AosWordMgr1::getTotalWords(const u32 vid) 
{
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, 0);
	
	return wordHash->getTotalWords();
}


// Chen Ding, 11/21/2010
bool
AosWordMgr1::addWord(
		const AosRundataPtr &rdata,
		const char *word, 
		const int len, 
		const u64 &wordid,
		const u64 id)
{
	int vid = getVirtualIdByWordId(wordid);
	AosWordIdHashPtr wordHash = getWordHash(vid); 
	aos_assert_r(wordHash, false);

//OmnString word_str(word, len);
//OmnScreen << "**********AddWord; vid:" << vid
//		<< "; iilid:" << id
//		<< "; word:" << word_str
//		<< endl;
	
	bool rslt = wordHash->addWord(word, len, wordid, id);
	aos_assert_r(rslt, AOS_INVWID);

	AosFmtPtr fmt = createFmt();
	sendFmt(rdata, fmt);
	return true;
}


// Ketty 2012/08/08
int
AosWordMgr1::getVirtualIdByWordId(const u64 wordid)
{
	u32 haskKey = (u32)(wordid >> 10);
	int vid = AosGetCubeId(haskKey);
	aos_assert_r(vid >=0, -1);
	return vid;
}


AosWordIdHashPtr
AosWordMgr1::getWordHash(const int virtual_id)
{
	// Ketty 2012/11/29 
	aos_assert_r(virtual_id >=0, 0);
	AosWordIdHashPtr word_hash;
	
	mLock->lock();
	map<u32, AosWordIdHashPtr>::iterator itr = mWordHash.find(virtual_id);
	if(itr != mWordHash.end())
	{
		word_hash = itr->second;
		aos_assert_rl(!word_hash->isStopped(), mLock, 0);
		mLock->unlock();
		return word_hash;
	}

	// Linda, 2013/08/16
	// init mWordHash
	word_hash = OmnNew AosWordIdHash(
			virtual_id, mTableSize, mWordIdName);

	mWordHash.insert(make_pair(virtual_id, word_hash));
	mLock->unlock();
	return word_hash;
}


AosFmtPtr
AosWordMgr1::createFmt()
{
	if(!AosIsSelfCubeSvr())	return 0;
	
	//Linda, 2013/08/22 LINDAFMT
	// fmt create 
	AosFmtPtr fmt = OmnNew AosFmt();
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, 0);
	fmt_mgr->setFmt(fmt);
	return fmt;
}


void
AosWordMgr1::sendFmt(
		const AosRundataPtr &rdata,
		const AosFmtPtr &fmt)
{
	if(!AosIsSelfCubeSvr())	return;
	
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert(fmt_mgr);
	fmt_mgr->addToGlobalFmt(rdata, fmt);
	fmt_mgr->setFmt(0);
}

