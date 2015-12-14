////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// 1. There is a file that stores the index of the hash table. There is
//    a record for each backet. The record contains four integers:
//    [bucketsize, num entries, offset, capacity]
//
//    where 'bucketsize' is the size of the bucket, 'num entries' is the
//    number of entries in the bucket, 'offset' is the offset in the
//    hash table data file (see below), and 'capacity' is the disk space
//    allocated in the hash table data file for the bucket.
//
// 2. There is a data file for the hash table. Whenever a bucket is created,
//    a space is allocated in the data file for the bucket. This space is 
//    used to store the contents of the bucket. Note that the size of a bucket
//    may grow, which can grow out of the space allocated for the bucket. 
//    When this happens, a new space is allocated for the bucket. The index
//    record (see above) should be updated accordingly. The old space is
//    market as not used. In the current implementation, the old space
//    is not reused. 
//
// 3. The first x number of bytes in the data file is reserved for admin.
//    There is an integer in the reserved area that is used to indicate 
//    the current size of the file. Whenever a new space is allocated for
//    a bucket, this value is updated accordingly. Whenever the data file
//    is opened, it reads the current file size. 
//
// Modification History:
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "WordMgr/WordIdHash.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "API/AosApiG.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
//#include "ReliableFile/ReplicMgrClt.h"
#include "ReliableFile/ReliableFile.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/SeTypes.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgr/FileKeys.h"
#include "StorageMgr/FileId.h"
#include "StorageMgr/SystemId.h"
#include "Thread/Mutex.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/HashUtil.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"


static bool sgSanityCheck = false;
//static bool sgInit = false;
const OmnString sgDataFileFlag = "dkarakdcv163356adf8dgod5623dc5";
const OmnString sgIdxFileFlag  = "dkarakdcv163xyzadf8dgod5623dc6";

//OmnFilePtr AosWordIdHash::smBackFile;
static bool sgRebuildFlag = true;
static bool sgRecoverFlag = true;


AosWordIdHash::AosWordIdHash(const u32 tablesize)
:
mLock(OmnNew OmnMutex()),
mVirtualId(0),
mTableSize(tablesize),
mWordIdBktIdx(0),
mWordIdEtrIdx(0),
mMaxConflictNum(0),
mMaxNumElems(0),
mMaxBucketLen(0),
mTotalWords(0),
mStopped(true)
{
	// Ketty 2013/03/12
	//init();
	start();
}


// IMPORTANT: tablesize must be in the form: 
//	000...1111...
AosWordIdHash::AosWordIdHash(
	const u32 virtual_id,
	//const OmnString &dirname,
	const u32 tablesize, 
	const OmnString &name) 
:
mHashName(name),
mLock(OmnNew OmnMutex()),
mVirtualId(virtual_id),
mHasVirtual(true),			
mTableSize(tablesize),
mWordIdBktIdx(0),
mWordIdEtrIdx(0),
//mDirname(dirname),		// Ketty 2012/08/01
mMaxConflictNum(0),
mMaxNumElems(0),
mMaxBucketLen(0),
mTotalWords(0),
mStopped(true)
{
	// Ketty 2013/03/12
	//init();
	start();
}


AosWordIdHash::AosWordIdHash(
	const u32 tablesize, 
	const OmnString &name) 
:
mHashName(name),
mLock(OmnNew OmnMutex()),
mVirtualId(0),
mHasVirtual(false),			
mTableSize(tablesize),
mWordIdBktIdx(0),
mWordIdEtrIdx(0),
//mDirname(dirname),		// Ketty 2012/08/01
mMaxConflictNum(0),
mMaxNumElems(0),
mMaxBucketLen(0),
mTotalWords(0)
{
	// This func is called only from client.
	// for IgnoreWord. so, don't has virtual_id.
	
	// Ketty 2013/03/12
	//init();
	start();
}


AosWordIdHash::~AosWordIdHash()
{
}


bool
AosWordIdHash::config(const AosXmlTagPtr &def)
{
	if (!def) return true;
	sgRebuildFlag = def->getAttrBool("rebuild_flag", true);
	sgRecoverFlag = def->getAttrBool("recover_flag", true);
	return true;
}


bool
AosWordIdHash::start()
{
	mStopped = false;
	bool rslt = init();
	aos_assert_r(rslt, false);	
	
	rslt = createHashtableNew();
	if (!rslt)
	{
		rslt = recover();
		aos_assert_r(rslt, false);
	}

//OmnScreen << "ktttt; wordIdHash: start:"
//		<< "virtual_id:" << mVirtualId
//		<< "; mFileSize:" << mFileSize
//		<< endl;

	return true;
}


bool
AosWordIdHash::stop()
{
	mLock->lock();

	mStopped = true;

	mBuckets = 0;
	mNumElems = 0;
	mWordBktIdx = 0;
	mWordEtrIdx = 0;
	mWordIdBktIdx = 0;
	mWordIdEtrIdx = 0;
	mFileSize = 0;
	mDataFile = 0;
	mIdxFile = 0;
	mCrtDbReadIdx = 0;
	mMaxConflictNum = 0;
	mMaxNumElems = 0;
	mMaxBucketLen = 0;
	mTotalWords = 0;

	mBackFile = 0;	// Ken Lee, 2014/02/10

	mLock->unlock();
	
	return true;
}


bool
AosWordIdHash::init()
{
	mRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRdata->setSiteid(AOS_SYS_SITEID);
	
	// Make sure the tablesize is correct
	aos_assert_r(eIdSize <= eMaxPtrSize, false);

	u32 tsize = 1;
	for (int i=0; i<eMaxTableSizeBits; i++)
	{
		tsize = (tsize << 1) + 1;
		if (tsize >= mTableSize)
		{
			mTableSize = tsize;
			break;
		}
	}
	if (mTableSize == 0) mTableSize = eMaxTableSize;

	mBuckets = OmnNew char*[mTableSize+1];
	if (!mBuckets)
	{
		OmnAlarm << "Failed to allocate memory for buckets: "
			<< mTableSize << enderr;
	}
 	
	mNumElems = OmnNew u16[mTableSize+1];
	if (!mNumElems)
	{
		OmnAlarm << "Failed to allocate memory for mNumElems: "
			<< mTableSize << enderr;
	}

	memset(mBuckets, 0, sizeof(void*) * (mTableSize+1));
	memset(mNumElems, 0, sizeof(u16) * (mTableSize+1));

	/*if (!sgInit)
	{
		sgInit = true;
		smBackFile = OmnNew OmnFile("wordid_bak", OmnFile::eReadWrite AosMemoryCheckerArgs);
		if (!smBackFile->isGood())
		{
			smBackFile = OmnNew OmnFile("wordid_bak", OmnFile::eCreate AosMemoryCheckerArgs);
		}
		aos_assert_r(smBackFile->isGood(), false);
	}*/

	OmnString full_dir = OmnApp::getAppBaseDir();
	u32 len = full_dir.length();
	if (full_dir.data()[len-1] != '/') full_dir << "/";
	OmnString fname = full_dir;
	fname << "wordid_bak_" << mVirtualId;
	mBackFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!mBackFile->isGood())
	{
		mBackFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	}
	aos_assert_r(mBackFile->isGood(), false);

	u64 datasize = mBackFile->getFileCrtSize();
	if (datasize == 0 && sgRebuildFlag)
	{
		bool rslt = rebuild();
		aos_assert_r(rslt, false);
	}

	return true;
}


// Ketty 2013/03/12
/*
bool
AosWordIdHash::createHashtable()
{
	// The hashtable is defined in the database. Each bucket 
	// is defined by a record. This function loops over
	// the records to create the table. It reads a number
	// of records each time, process the records. 
	
	// 1. Read in the hash table information.
	mLock->lock();
	bool isNewTable;
	if (!mDataFile) 
	{
		bool rslt = openDataFile();
		aos_assert_rl(rslt, mLock, false);
	}

	if (!mIdxFile) 
	{
		bool rslt = openIdxFileNew(isNewTable);
		aos_assert_rl(rslt, mLock, false);
	}

	if (isNewTable) 
	{
		mLock->unlock();
		return true;
	}

	OmnRslt rslt;
	mCrtDbReadIdx = 0;
	char buff[eIdxFileRcdSize+1];
	for (u32 bktid=0; bktid<=mTableSize; bktid++)
	{
		// read from hard disk to buff
		//if ((bktid & 0xfff) == 0) OmnScreen << "Create bucket: " << bktid << endl;
		u32 idx_offset = bktid * eIdxFileRcdSize + eIdxFileInitSize;
		if (bktid == mTableSize)		
		{
			int size_read = mIdxFile->readToBuff(idx_offset, eIdxFileRcdSize, buff, mRdata);
			if (size_read != eIdxFileRcdSize)
			{
				//The last entry may not exist, we need to add it
				memset(buff, 0, eIdxFileRcdSize);
				aos_assert_rl(mIdxFile->put(idx_offset, 
										   buff, 
										   eIdxFileRcdSize, 
										   true, mRdata),mLock,false);
				// Ketty 2012/12/12
				u32 crt_tid = OmnGetCurrentThreadId();
				AosReplicMgrClt::getSelf()->sendReplicLog(mRdata, crt_tid, mVirtualId);        
			}
		}
		else
		{
			u32 ss = mIdxFile->readToBuff(idx_offset, eIdxFileRcdSize, buff, mRdata);
			aos_assert_rl(ss == eIdxFileRcdSize, mLock, false);
		}
		
		// Get data from buff.
		// 	Bucket Size			(4 bytes)
		// 	Num Elements		(2 bytes)
		// 	Offset 				(4 bytes)
		u32 bktsize = *(u32 *)&buff[0];
		u16 numElems = *(u16 *)&buff[4];
		u32 offset = *(u32 *)&buff[6];

		aos_assert_rl(bktsize >= 0 && bktsize < eMaxBktSize, mLock, false);
		aos_assert_rl(offset >= 0, mLock, false);
		
		if (numElems > 0)
		{
			// It is not an empty bucket
			char *mem = allocateBucket(bktsize);
			aos_assert_rl(mem, mLock, false);

			aos_assert_rl(mDataFile->readToBuff(offset, bktsize, mem, mRdata), mLock, false);
			mBuckets[bktid] = mem;
			mNumElems[bktid] = numElems;

			// Need to reset the pointers
			u16 *index = (u16 *)mem;
			for (u16 i=0; i<numElems; i++)
			{
				int idx = index[i+1] - eIdSize;
				u64 pp = *(u64*)&mem[idx];
				aos_assert_rl(pp, mLock, false);
			}
		}
	}
	mLock->unlock();
	return true;
}
*/

bool
AosWordIdHash::readHeader(
		const int bktid, 
		AosWordIdHash::Header &header) 
{
	aos_assert_r(mIdxFile, false);
	u32 offset = bktid * eIdxFileRcdSize + eIdxFileInitSize;
	char buff[eIdxFileRcdSize+1];

	// Ketty 2012/11/09
	bool rslt = mIdxFile->readToBuff(offset, eIdxFileRcdSize, buff, mRdata.getPtr());
	aos_assert_r(rslt, false);
	//aos_assert_r(mIdxFile->readToBuff(offset, eIdxFileRcdSize, buff, mRdata) 
	//		== eIdxFileRcdSize, false);
	header.bktid = bktid;
	header.bktsize = *(u32 *)&buff[0];
	header.num_entries = *(u16 *)&buff[4];
	header.offset = *(u32 *)&buff[6];
	header.capacity = *(u32 *)&buff[10];

//OmnScreen << "ktttt; wordIdHash: readHeader:"
//		<< "virtual_id:" << mVirtualId
//		<< "; bktid:" << bktid
//		<< "; header bktsize:" << header.bktsize
//		<< "; num_entris:" << header.num_entries
//		<< "; bktsize:" << header.bktsize
//		<< "; head_offset:" << offset 
//		<< "; body_offset:" << header.offset
//		<< "; capacity:" << header.capacity 
//		<< endl;

	return true;
}


u64
AosWordIdHash::addWord(
		const char *word, 
		const int wordLen,
		const u64 id)
{
	u64 wordid = getWordId(word, wordLen, true);
	aos_assert_r(wordid != AOS_INVWID, AOS_INVWID);
	setId(wordid, id);
	OmnString str(word, wordLen);
	str << "," << id << "\n";
	mBackFile->lock();
	mBackFile->goToFileEnd();
	mBackFile->append(str);
	mBackFile->unlock();
	return wordid;
}


u64 
AosWordIdHash::recoverWord(
		const char *word,
		const int wordlen)
{
	if (!sgRecoverFlag) return true;

	aos_assert_r(mBackFile, false);
	mBackFile->lock();
	mBackFile->seek(0);
	bool finished = false;
	OmnString str;
	OmnString wd(word, wordlen); 
	u64 id = 0;
	while(!finished)
	{
		str = mBackFile->getLine(finished);
		if (str == "") continue;
		vector<OmnString> strs;
		AosStrSplit::splitStrByChar(str, ",", strs, 2); 
		if (strs[0] == wd)
		{
			aos_assert_r(strs[1] != "", false);
			id = atoll(strs[1].data());
			break;
		} 
	}
	mBackFile->goToFileEnd();
	mBackFile->unlock();

	if (id != 0)
	{
		u64 wordid = addWord(word, wordlen, id); 
		aos_assert_r(wordid != 0, false);
		updateBktToFile(wordid);	
	}
	return id;
}


bool
AosWordIdHash::recover()
{
	if (!sgRecoverFlag) return true;

	mLock->lock();
	aos_assert_rl(mBackFile, mLock, false);

	if (!mDataFile)
	{
		if (mHasVirtual)
		{
			u64 file_id = ((u64)mVirtualId << 32) + AOSFILEID_WORDIDHASH;
			AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
			aos_assert_r(vfsMgr, false);
			mDataFile = vfsMgr->openRlbFile(mRdata.getPtr(), mVirtualId, file_id, 
				"word", eFileSize, true, AosRlbFileType::eLog);
		}
	}
	aos_assert_rl(mDataFile, mLock, false);
	mDataFile->resetFile();
	mDataFile = 0;

	if (!mIdxFile)
	{
		if (mHasVirtual)
		{
			u64 file_id = ((u64)mVirtualId << 32) + AOSFILEID_WORDIDHASH_IDX;
			AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
			aos_assert_rl(vfsMgr, mLock, false);
		
			mIdxFile = vfsMgr->openRlbFile(mRdata.getPtr(), mVirtualId, file_id, 
				"word_idx", eFileSize, true, AosRlbFileType::eLog, true);
		}
	}
	aos_assert_rl(mIdxFile, mLock, false);
	mIdxFile->resetFile();
	mIdxFile = 0;

	bool rslt = openDataFile();
	aos_assert_rl(rslt, mLock, false);
	bool isNewTable = false;
	rslt = openIdxFileNew(isNewTable);
	aos_assert_rl(rslt, mLock, false);
	aos_assert_rl(isNewTable, mLock, false);

	if (mBuckets)
	{
		for(u32 i=0; i<=mTableSize; i++)
		{
			if (mBuckets[i])
			{
				//Jackie 2014/09/16
	//			releaseBucket(mBuckets[i]);
				mBuckets[i] = 0;
			}
		}
	}
	else
	{
		mBuckets = OmnNew char*[mTableSize+1];
	}
	memset(mBuckets, 0, sizeof(char*) * (mTableSize+1));

	if (!mNumElems)
	{
		mNumElems = OmnNew u16[mTableSize+1];
	}
	memset(mNumElems, 0, sizeof(u16) * (mTableSize+1));
	mTotalWords = 0;

	mBackFile->lock();
	mBackFile->seek(0);
	u64 id = 0;
	OmnString str, word;
	bool finished = false;
	map<OmnString, u64> mp;
	vector<OmnString> strs;
	while (!finished)
	{
		str = mBackFile->getLine(finished);
		if (str == "") continue;

		strs.clear();
		AosStrSplit::splitStrByChar(str, ",", strs, 2); 
		if (strs[0] == "" || strs[1] == "")
		{
			continue;
		}

		word = strs[0];
		id = atoll(strs[1].data());

		if (id != 0 && mp.find(word) == mp.end())
		{
			mp[word] = id;
		}
	}

	map<OmnString, u64>::iterator itr = mp.begin();
	while (itr != mp.end())
	{
		word = itr->first;
		id = itr->second;
		if (id != 0)
		{
			u64 wordid = getWordIdPriv(word.data(), word.length(), true);
			if (wordid == AOS_INVWID)
			{
				OmnAlarm << "error" << enderr;
				continue;
			}
			setIdPriv(wordid, id);
		}
		itr++;
	}

	mBackFile->goToFileEnd();
	mBackFile->unlock();
	
	mLock->unlock();

	for(u32 i=0; i<=mTableSize; i++)
	{
		if (mBuckets[i])
		{
			updateBktToFileByBktid(i);
		}
	}

	return true;
}


bool
AosWordIdHash::rebuild()
{
	if (!sgRebuildFlag) return true;

	mLock->lock();
	bool rslt = true;
	bool isNewTable = false;
	if (!mDataFile) 
	{
		rslt = openDataFile();
		aos_assert_rl(rslt, mLock, false);
	}

	if (!mIdxFile) 
	{
		rslt = openIdxFileNew(isNewTable);
		aos_assert_rl(rslt, mLock, false);
	}

	if (isNewTable) 
	{
		mLock->unlock();
		return true;
	}

	const int size = getHeaderSize();
	char *head_buff = OmnNew char[size];
	rslt = mIdxFile->readToBuff(0, size, head_buff, mRdata.getPtr());
	aos_assert_rl(rslt, mLock, false);

	u64 datasize = mDataFile->getFileCrtSize();
	char *data_buff = 0;
	if (datasize < eMaxDataSize)
	{
		try
		{
			data_buff = OmnNew char[datasize];
		}

		catch (...)
		{
			OmnAlarm << "Run out of memory: " << datasize << enderr;
			datasize = 0;
		}

		rslt = mDataFile->readToBuff(0, datasize, data_buff, mRdata.getPtr());
		aos_assert_rl(rslt, mLock, false);
	}

	mBackFile->lock();
	mBackFile->resetFile();
	mBackFile->goToFileEnd();
	for (u32 bktid=0; bktid<=mTableSize; bktid++)
	{
		// read from hard disk to buff
		//if ((bktid & 0xfff) == 0) OmnScreen << "Create bucket: " << bktid << endl;
		
		// Get data from buff.
		// 	Bucket Size			(4 bytes)
		// 	Num Elements		(2 bytes)
		// 	Offset 				(4 bytes)
		int idx_offset = bktid * eIdxFileRcdSize + eIdxFileInitSize;
		u32 bktsize = *(u32 *)&head_buff[idx_offset];
		u16 numElems = *(u16 *)&head_buff[idx_offset+4];
		u32 offset = *(u32 *)&head_buff[idx_offset+6];

		if (bktsize < 0 || bktsize >= eMaxBktSize || offset < 0)
		{
			OmnAlarm << "bktsize:" << bktsize
				<< "offset:" << offset << enderr;
			continue;
		}

		if (numElems <= 0)
		{
			continue;
		}
			
		// It is not an empty bucket
		char *mem = allocateBucket(bktsize);
		if (!mem)
		{
			OmnAlarm << "error" << enderr;
			continue;
		}

		if (data_buff)
		{
			if (offset + bktsize > datasize)
			{
				OmnAlarm << "offset:" << offset 
					<< ",bktsize:" << bktsize
					<< ",datasize:" << datasize << enderr;
				continue;
			}
			memcpy(mem, &data_buff[offset], bktsize);
		}
		else
		{
			int nn = mDataFile->readToBuff(offset, bktsize, mem, mRdata.getPtr());
			if (nn <= 0 || nn != (int)bktsize)
			{
				OmnAlarm << "nn:" << nn << enderr;
				continue;
			}
		}

		u16 *index = (u16 *)mem;
		if (index[numElems] != bktsize)
		{
			OmnAlarm << "error" << enderr;
		}
		OmnString str;
		for (u16 i=0; i<numElems; i++)
		{
			if (index[i+1] > bktsize)
			{
				OmnAlarm << "Invalid data: " << index[numElems]
					<< ":" << bktsize << ":" << numElems << enderr;
				break;
			}

			int idx = index[i] + eWordIdLen;
			int len = index[i+1] - idx - eIdSize;
			if (len <= 0)
			{
				continue;
			}
			OmnString word(&mem[idx], len);

			idx = index[i+1] - eIdSize;
			u64 id = *(u64*)&mem[idx];

			str << word << "," << id << "\n";
		}
		
		mBackFile->append(str);
	}

	mBackFile->flushFileContents();
	mBackFile->unlock();
	
	delete [] head_buff;
	delete [] data_buff;

	mLock->unlock();

	return true;
}


u64
AosWordIdHash::getWordId(
		const char *word, 
		const int wordLen,
		const bool addFlag)
{
	mLock->lock();
	u64 wordid = getWordIdPriv(word, wordLen, addFlag);
	mLock->unlock();
	return wordid;
}


u64
AosWordIdHash::getWordIdPriv(
		const char *word, 
		const int wordLen,
		const bool addFlag)
{
	u32 hashkey = AosGetHashKey(word, wordLen);

	int idx;
	u64 wid = getWordId(word, idx, wordLen, hashkey);
	if (wid != AOS_INVWID) 
	{
		// Found the wordId
		return wid;
	}

	// Not found. Check whether to add
	if (!addFlag) 
	{
		return AOS_INVWID;
	}

	// Need to add
	mAddFlag = true;
	u64 the_wid = addEntry(word, wordLen, idx, hashkey);

	aos_assert_r(integrityCheck(hashkey & mTableSize, AOS_INVWID), AOS_INVWID);
	if (the_wid == AOS_INVWID)
	{
		OmnAlarm << "Failed to create WordID!" << enderr;
		mError = true;
		mAddFlag = false;
	}

	return the_wid;
}


u64
AosWordIdHash::getWordId(
		const char *word, 
		int &idx,
		const int wordLen, 
		const u32 hashkey)
{
	// This function searches the table for the WordID for
	// 'word'. 'word' is 0 terminated. If 'addFlag' is true, 
	// if the wordID is not found, it will add one. 
	//
	// IMPORTANT: Normally one should not set 'addFlag' to true.
	// The only occasion it is set to true is when the hash table
	// is used by AosWordMgr because it needs to update the table
	// for new words. 
	//
	// Return:
	// 1. If the word is found, it returns the WordId, and set 
	// 	  'mAddFlag' to false, indicating that no WordID being created.
	// 2. If the word is not found, if 'addFlag' is false, it 
	//    returns null, and 'mAddFlag' is set to false. 
	// 3. If the word is not found, and if 'addFlag' is true, it
	//    will create the WordID and returns it. 'mAddFlag' will be
	//    set to 'true', indicating that the new word was just
	//    created. 
	// 4. If any error, it returns null, and 'mError' is set to true.

	// 1. Calculate the hask key

	mError = false;
	mAddFlag = false;
	u32 bktkey = hashkey & mTableSize;
	aos_assert_r(integrityCheck(bktkey, AOS_INVWID), AOS_INVWID);

	// 2. Look up the table.
	char *bucket = mBuckets[bktkey];
	if (!bucket) 
	{
		idx = 0;
		return AOS_INVWID;
	}
	
	// The bucket contains something. Need to search the bucket.
	// Contents in the bucket are in the form:
	//	index[0]:	the offset for the first entry (2 bytes)
	//	index[1]:	the offset for the second entry (2 bytes)
	//	index[n-1]:	the offset for the last entry (2 bytes)
	//	index[n]:	this is used to determine the last entry length
	//  entry 0: 	it is in the form [WordId, Word].
	//  entry 1: 	it is in the form [WordId, Word].
	//	...
	//  entry n-1: 	it is in the form [WordId, Word].
	//  WordId is five bytes long. The first four bytes 
	//  is the 30-bit extended WordID (the high 30 bits).
	//  The remaining 2 bits and the next byte is the conflict 
	//  number.

	int numElems = mNumElems[bktkey];
	u16 *index = (u16 *)bucket;
	u32 wordid;
	idx = -1;
	int left = 0;
	int right = numElems-1;
	u32 *ww;
	u16 hh;
	while (left <= right)
	{
		idx = ((right - left) >> 1) + left;
		u16 offset = index[idx];
		ww = (u32 *)&bucket[offset];
		wordid = (*ww) >> 2;
		if (wordid < hashkey)
		{
			// The searched for should be on the right side
			if (left == right)
			{
				// The searched is not in and hashkey is larger
				// than the one at 'idx'. 
				idx++;
				break;
			}

			left = idx+1;
			continue;
		}
		else if (wordid > hashkey)
		{
			// The searched for should be on the left side
			if (left == right)
			{
				// The searched is not in and hashkey is
				// smaller than the one at 'idx'.
				break;
			}
			right = idx-1;
			continue;
		}
		else 
		{
			// It matched. It is possible that the searched for could be 
			// on the left or right. Check whether this is the one we are
			// looking for. 
			hh = index[idx+1] - index[idx] - eWordIdLen - eIdSize;
			if (hh == wordLen &&
				strncmp(word, &bucket[offset+eWordIdLen], hh) == 0)
			{
				// Found it. 
				u32 *ww = (u32 *)&bucket[offset];
				u64 the_wid = ((u64)*ww << 8) + (u8)bucket[offset+4];
				mAddFlag = false;
				aos_assert_r(integrityCheck(bktkey, the_wid), AOS_INVWID);
				return the_wid;
			}

			// Did not match. Search the left side
			int crtIdx = idx;
			while (idx > 0)
			{
				idx--;
				offset = index[idx];
				ww = (u32 *)&bucket[offset];
				wordid = (*ww) >> 2;
				if (wordid != hashkey)
				{
					// Did not find it. 
					break;
				}

				hh = index[idx+1] - index[idx] - eWordIdLen - eIdSize;
				if (hh == wordLen &&
					strncmp(word, &bucket[offset+eWordIdLen], hh) == 0)
				{
					// Found it.
					u32 *ww = (u32 *)&bucket[offset];
					mAddFlag = false;
					u64 the_wid = ((u64)*ww << 8) + (u8)bucket[offset+4];
					aos_assert_r(integrityCheck(bktkey, the_wid), AOS_INVWID);
					return the_wid;
				}
			}

			// Did not find the entry on the left side. Try the 
			// right side.
			idx = crtIdx;

			while (idx < numElems-1)
			{
				idx++;
				offset = index[idx];
				ww = (u32 *)&bucket[offset];
				wordid = (*ww) >> 2;
				if (wordid != hashkey)
				{
					aos_assert_r(integrityCheck(bktkey, AOS_INVWID), AOS_INVWID);
					return AOS_INVWID;
				}

				hh = index[idx+1] - index[idx] - eWordIdLen - eIdSize;
				if (hh == wordLen &&
					strncmp(word, &bucket[offset+eWordIdLen], hh) == 0)
				{
					// Found it.
					u32 *ww = (u32 *)&bucket[offset];
					mAddFlag = false;
					u64 the_wid = ((u64)*ww << 8) + (u8)bucket[offset+4];
					aos_assert_r(integrityCheck(bktkey, the_wid), AOS_INVWID);
					return the_wid;
				}
			}

			// Did not find it. 
			// This means 'idx' is already the last one. The
			// searched is not in the list, and 'idx' should
			// be set to idx+1, which means the new entry
			// should be inserted after the current one.
			idx++;
			return AOS_INVWID;
		}

		// Did not find it
		break;
	}

	aos_assert_r(integrityCheck(bktkey, AOS_INVWID), AOS_INVWID);
	return AOS_INVWID;
}


u64
AosWordIdHash::addEntry(
		const char *word, 
		const int wordLen,
		const int idx, 
		const u32 newHashkey)
{
	// This function inserts a new entry into the corresponding bucket. 
	// 'word' is the word to be added; 'idx' points to the location
	// in the bucket where the new entry should be inserted either
	// in front of it or after it. If 'idx' is 'numElems', it means 
	// it should append the entry. 'newHashkey' is the 30-bit hash key
	// for 'word'. 
	// 
	// IMPORTANT: The caller should have locked the table.
	//
	// Return:
	// Upon success, it returns the new WordID. 
	// Failure: 
	// 1. Failing to allocate memory. In that case, it returns null.
	// 2. The 30-bit hashkey at 'idx' should not equal to 'newHashkey'.
	//    Otherwise, it returns null.

	// 1. Find the bucket.
	u32 bktId = newHashkey & mTableSize;
	char *bucket = mBuckets[bktId];
	u16 numElems = mNumElems[bktId];

	aos_assert_r(integrityCheck2(word, wordLen, idx, bucket, bktId), false);

	u16 *indexArray = 0;
	if (!bucket)
	{
		// It is an empty bucket. Its size should be:
		// index[0]: two bytes
		// index[1]: two bytes
		// wordId:	 five bytes
		// word:	 variable
		// ptr:		 variable
		//
		// The WordId should be the hashkey (30 bits) << 10 bits + conflict number
		// If the hashkey is 0, the conflict number is set to 1. Otherwise, 
		// it is set to 0.
		int newSize = 4 + eWordIdLen + eIdSize + wordLen;
		char *mem = allocateBucket(newSize);
		if (!mem)
		{
			OmnAlarm << "Failed to allocate memory!" << enderr;
			return AOS_INVWID;
		}
		indexArray = (u16 *)mem;
		indexArray[0] = 4;
		indexArray[1] = newSize;
		u32 *ww = (u32*)&mem[4];
		*ww = (newHashkey << 2);
		mem[8] = (*ww == 0)?1:0;	// Make sure wordId never be 0
		memcpy(&mem[9], word, wordLen);
		mBuckets[bktId] = mem;
		mNumElems[bktId] = 1;

		// Reset the ptr field
		memset(&mem[newSize - eIdSize], 0, eIdSize);
		aos_assert_r(integrityCheck(bktId, AOS_INVWID), AOS_INVWID);
	
		u64 the_wid = ((u64)*ww << 8) + (u8)mem[8];
		mTotalWords++;
		return the_wid;
	}

	// Need to determine whether to insert the entry in front of
	// 'idx' or after 'idx'. If hashkey is smaller than the hashkey
	// of 'idx', it should be in front of it. Otherwise, it should
	// be after it. Note that if 'idx' is 'mNumElems[bktkey]', 
	// it means to append the entry.

	// 1. Determine the conflict number and 'offset'.
	indexArray = (u16 *)bucket;

	aos_assert_r(numElems > 0, AOS_INVWID);
	// aos_assert_r(indexArray[0] == ((numElems+1) << 1), AOS_INVWID);
	if (indexArray[0] != ((numElems+1) << 1))
	{
		OmnAlarm << "Something is wrong: " << indexArray[0]
			<< ", " << numElems << enderr;
		return AOS_INVWID;
	}

	u16 offset = 0; 
	u16 conflictNum = 0;
	int newIdx = idx;
	if (idx < numElems)
	{
		// Need to determine whether it is in front or after
		// 'idx' at which the new entry is to be added. It
		// retrieves 'idx' hashkey. If 'newHashkey' is smaller
		// than it, it means the new entry should be inserted
		// in front of 'idx'. Otherwise, it should be after
		// 'idx'. Note that before calling this function, 
		// the caller should have ensured that the hashkey
		// will not be the same.
		offset = indexArray[idx];
		u32 *tmp = (u32 *)&bucket[offset];
		u32 hh = (*tmp) >> 2;
		if (newHashkey == hh)
		{
			// This should never happen
			OmnAlarm << "Should never happen. Bucket ID: " << bktId
				<< ". Word: " << word << enderr;
			return AOS_INVWID;
		}

		newIdx = idx;
		if (hh < newHashkey)
		{
			// The new word should be after of 'idx'
			newIdx = idx+1;
		}
	}

	// 'newIdx' is the index in front of which a new entry 
	// should be inserted. If 'newIdx' is not the first one,
	// it needs to check whether the previous entry's 30-bit
	// hashkey is the same. If yes, need to calculate the 
	// conflict number. Otherwise, the conflict number is 0.
	conflictNum = 0;
	if (newIdx > 0)
	{
		u16 offset = indexArray[newIdx-1];
		u32 *tmp = (u32 *)&bucket[offset];
		u32 prevHashkey = (*tmp) >> 2;
		if (prevHashkey == newHashkey)
		{
			// The hashkey matches. Need to calculate the 
			// conflict number.
			conflictNum = (((*tmp) & 0x03) << 8) + (u8)bucket[offset+4];
			conflictNum++;
			aos_assert_r(conflictNum < eMaxConflictNum, false);
			if (mMaxConflictNum < conflictNum)
			{
				mMaxConflictNum = conflictNum;
			}
		}
	}

	// Allocate the memory. Note that the new size should be 
	// the current size plus two bytes for the index, five 
	// bytes for the WordID and 'wordLen' for the word itself.
	u32 delta = wordLen + 2 + eWordIdLen + eIdSize;
   	int newSize = indexArray[numElems] + delta;
	char *mem = allocateBucket(newSize);
	u32 bktLen = indexArray[numElems] + delta; 
	aos_assert_r(bktLen < eMaxBucketLen, false);
	if (mMaxBucketLen < bktLen) 
	{
		mMaxBucketLen = bktLen;
		if (bktLen > 30000)
		{
			OmnAlarm << "Bucket too long: " << mHashName << ":" << bktLen << enderr;
		}
	}
	if (!mem)
	{
		OmnAlarm << "Failed to allocate memory!" << enderr;
		return AOS_INVWID;
	}

	// 2. Construct the index. First, copy the entries that
	// are in front of 'newIdx'. Then, set the new entry
	// at 'newIdx', and then copy the remaining.
	u16 *newIndex = (u16*)mem;
	if (newIdx == 0)
	{
		// It is to insert in front of the first entry
		newIndex[0] = ((numElems+2)<<1);
		for (int i=1; i<=numElems+1; i++)
			newIndex[i] = indexArray[i-1] + delta;
		u16 old_offset = indexArray[0];
		u16 new_offset = newIndex[1];
		memcpy(&mem[new_offset], &bucket[old_offset], 
			newSize - new_offset);
	}
	else
	{
		// It inserts at 'newIdx'. 
		// 1. First, copy all the contents that are in 
		// front of 'newIdx'.
		for (int i=0; i<=newIdx; i++)
			newIndex[i] = indexArray[i] + 2;

		u16 old_offset = indexArray[0];
		u16 new_offset = newIndex[0];
		memcpy(&mem[new_offset], &bucket[old_offset], 
			indexArray[newIdx] - old_offset);

		// 3. Copy the remaining.
		for (int i=newIdx+1; i<=numElems+1; i++)
			newIndex[i] = indexArray[i-1] + delta;
		old_offset = indexArray[newIdx]; 
		new_offset = newIndex[newIdx+1];
		if (indexArray[numElems] > old_offset)
		{
			memcpy(&mem[new_offset], &bucket[old_offset],
				indexArray[numElems] - old_offset);
		}
	}

	// 2. Set the current entry
	offset = newIndex[newIdx];
	u32 *ww = (u32 *)&mem[offset];
	*ww = (newHashkey << 2) + ((conflictNum >> 8) & 0x03);

	// Make sure wordId will never be 0
	if (*ww == 0 && conflictNum == 0)
		conflictNum = 1;
	mem[offset+4] = conflictNum;
	memcpy(&mem[offset+eWordIdLen], word, wordLen);

	numElems = ++mNumElems[bktId];
	mBuckets[bktId] = mem;

	// Reset the ptr
	for (int kk=0; kk<eIdSize; kk++)
		mem[offset + eWordIdLen + wordLen + kk] = 0;

	// Make sure everything is correct.
	aos_assert_r(integrityCheck1(mem, bucket, numElems-1, 
		newHashkey, conflictNum, word), false);

	if (bucket) releaseBucket(bucket);

	u64 the_wid = ((u64)*ww << 8) + (conflictNum & 0xff);

	mTotalWords++;
	return the_wid;
}


u32		
AosWordIdHash::getBucketKey(const char *wordId)
{
	// The input 'wordId' is the 40-bit WordID. 
	u32 *ww = (u32 *)wordId;
	*ww = ((*ww) >> 2) & mTableSize;
	return *ww;
}


u32		
AosWordIdHash::getBucketSize(const u32 bktkey)
{
	if (!mBuckets[bktkey]) return 0;
	if (mNumElems[bktkey] == 0) return 0;
	u16 *idxArray = (u16*)mBuckets[bktkey];
	return idxArray[mNumElems[bktkey]];
}


char *	
AosWordIdHash::getBucket(const u32 bktkey)
{
	return mBuckets[bktkey];
}


u16 
AosWordIdHash::getNumElems(const u32 bktkey)
{
	return mNumElems[bktkey];
}


bool
AosWordIdHash::integrityCheck(
		const u32 the_bktid, 
		const u64 wordid)
{
	// 1. If 'the_bktid' is not 0, it checks the integrity
	//    of the bucket 'the_bktid', and 'wordid' is one of 
	//    the entries in the bucket. 
	// 2. If 'the_bktid' is 0 but 'wordid' is valid, it
	//    checks the integrity of the bucket in which 
	//    'wordid' resides. 
	// 3. Otherwise, it does nothing.
	if (!sgSanityCheck) return true;
	u32 bktid = the_bktid;
	if (the_bktid > mTableSize)
	{
		if (wordid == AOS_INVWID) return true;
		bktid = ((u32)(wordid >> 10)) & mTableSize;
	}

	for (u32 i=0; i<=mTableSize; i++)
	{
		aos_assert_r((unsigned long)mBuckets[i] == 0 || 
			(unsigned long)mBuckets[i] > 10000, false);
	}

	char *bucket = mBuckets[bktid];
	if (!bucket) return true;
	u16 numElems = mNumElems[bktid];	
	if (numElems == 0)
	{
		OmnAlarm << "Something is wrong!" << enderr;
		return false;
	}

	u16 *indexArray = (u16 *)bucket;
	u16 offset = (u16)((numElems + 1) << 1);
	if (indexArray[0] != offset)
	{
		OmnAlarm << "Something is wrong: " << the_bktid << enderr;
		return false;
	}


	if (mMaxNumElems < numElems) mMaxNumElems = numElems;

	for (int i=1; i<numElems; i++)
		aos_assert_r((indexArray[i-1] < indexArray[i]), false);

	if (wordid == AOS_INVWID) return true;
	bool found = false;
	u64 prevWid = 0;
	for (int i=0; i<numElems; i++)
	{
		u16 offset = indexArray[i];
		aos_assert_r(offset < indexArray[i+1], false);
		u32 *ww = (u32 *)&bucket[offset];
		u64 hh = (((u64)*ww) << 8) + (u8)bucket[offset+4];
		if (hh == wordid)
		{
			found = true;
		}
		aos_assert_r(hh != AOS_INVWID, false);
		aos_assert_r(prevWid < hh, false);
	}

	aos_assert_r(found, false);

	return true;
}


bool
AosWordIdHash::integrityCheck1(
	char *new_bucket, 
	char *old_bucket, 
	const u16 numElems, 
	const u32 newHashkey,
	const u16 conflictNum, 
	const char *word)
{
	// 'numElems' is the number of elements in 'old_bucket'.
	// 'new_bucket' contains one more item.
	// 'newHashkey' is the 30-bit hash value.
	// 1. Make sure all entries in 'old_bucket' is also in 
	//    new_bucket.
	// 2. Make sure the new entry is in the new bucket and
	//    it is correct.
	if (!sgSanityCheck) return true;
	for (u32 i=0; i<=mTableSize; i++)
	{
		aos_assert_r((unsigned long)mBuckets[i] == 0 || 
			(unsigned long)mBuckets[i] > 10000, false);
	}

	// 1. Make sure all entries in 'old_bucket' is also in 
	//    new_bucket.
	u16 *idx1 = (u16*)old_bucket;
	u16 *idx2 = (u16*)new_bucket;
	char *word1, *word2;
	bool found = false;
	u16 wordLen1, wordLen2;
	for (int i=0; i<numElems; i++)
	{
		u16 offset1 = idx1[i];
		wordLen1 = idx1[i+1] - idx1[i] - eWordIdLen - eIdSize;
		u32 *ww1 = (u32 *)&old_bucket[offset1];
		u64 wordid1 = ((u64)(*ww1) << 8) + (u8)old_bucket[offset1+4];
		word1 = &old_bucket[offset1 + eWordIdLen];
		u32 *ptr1 = (u32 *)&old_bucket[offset1 + eWordIdLen + wordLen1];

		word2 = 0;
		found = false;
		u64 wordid2;
		u32 *ptr2;
		for (int k=0; k<numElems+1; k++)
		{
			u16 offset2 = idx2[k];
			u32 *ww2 = (u32 *)&new_bucket[offset2];
			wordid2 = ((u64)(*ww2) << 8) + (u8)new_bucket[offset2+4];
			if (wordid2 == wordid1)
			{
				found = true;
				wordLen2 = idx2[k+1] - idx2[k] - eWordIdLen - eIdSize;
				word2 = &new_bucket[offset2 + eWordIdLen];
				ptr2 = (u32 *)&new_bucket[offset2 + eWordIdLen + wordLen2];
				break;
			}
		}
		aos_assert_r(found, false);
		aos_assert_r(wordLen1 == wordLen2, false);
		aos_assert_r(word2 != 0, false);
		if (eIdSize > 0 && *ptr1 != *ptr2)
		{
			OmnAlarm << "Failed the integrity check!" << enderr;
			return false;
		}

		if (strncmp(word1, word2, wordLen1) != 0)
		{
			OmnAlarm << "Failed integrity check!" << enderr;
			return false;
		}
	}

	// 2. Make sure the new entry is in the new bucket and
	//    it is correct. Also make sure the ptr for the new
	//    entry is reset to 0.
	u64 newWordid = (((u64)newHashkey) << 2) + 
			((conflictNum >> 8) & 0x03);
	newWordid = (newWordid << 8) + (conflictNum & 0xff);
	u64 prevWordid = 0;
	found = false;
	for (int k=0; k<numElems+1; k++)
	{
		u16 offset2 = idx2[k];
		u32 *ww2 = (u32 *)&new_bucket[offset2];
		u64 wordid2 = ((u64)(*ww2) << 8) + (u8)new_bucket[offset2+4];
		if (wordid2 == newWordid)
		{
			found = true;
			word2 = &new_bucket[offset2 + eWordIdLen];
			wordLen2 = idx2[k+1] - idx2[k] - eWordIdLen - eIdSize;

			// Check whether the pointer is 0
			int stIdx = offset2 + eWordIdLen + wordLen2;
			for (int mm=0; mm<eIdSize; mm++)
				aos_assert_r(new_bucket[stIdx+mm] == 0, false);
		}
		aos_assert_r(prevWordid < wordid2, false);
	}

	aos_assert_r(found, false);
	aos_assert_r(strncmp(word, word2, wordLen2) == 0, false);

	// 3. Making sure all the words contained in the new bucket
	//    are unique. 
	for (int i=0; i<numElems+1; i++)
	{
		u32 offset = idx2[i];
		u32 wordLen = idx2[i+1] - idx2[i] - eWordIdLen - eIdSize;
		char *word1 = &new_bucket[offset+5];

		for (int k=0; k<numElems+1; k++)
		{
			u32 os = idx2[k];
			if (os != offset)
			{
				u32 wl = idx2[k+1] - idx2[k] - eWordIdLen - eIdSize;
				char *word2 = &new_bucket[os+eWordIdLen];
				if (wl == wordLen && strncmp(word1, word2, wordLen) == 0)
				{
					OmnAlarm << "Failed the integrity check!" << enderr;
					return false;
				}
			}
		}
	}

	return true;
}


bool
AosWordIdHash::setId(const u64 &wordId, const u64 &id)
{
	mLock->lock();
	bool rslt = setIdPriv(wordId, id);
	mLock->unlock();
	return rslt;
}


bool
AosWordIdHash::setIdPriv(const u64 &wordId, const u64 &id)
{
	// This function sets 'ptr' to the word 'wordId'. 
	// If 'wordId' is not already in the table, it is
	// an error. 
	u32 position, wordLen;
	char *bucket = getPtrPriv(wordId, position, wordLen, ePtr);
//OmnScreen << "setId:" 
//		<< "; mVirtualId:" << mVirtualId
//		<< "; wordId: " << wordId
//		<< "; position: " << position
//		<< endl;
	if (!bucket)
	{
		OmnAlarm << "Failed to set ptr: " << wordId << enderr;
		return false;
	}

	aos_assert_rl(eIdSize == (sizeof(id)), mLock, false);
	u64 *ff = (u64 *)&bucket[position];
	*ff = id;

	aos_assert_r(integrityCheck(
		((u32)(wordId >> 10)) & mTableSize, wordId), false);
	return true;
}


bool
AosWordIdHash::getId(const u64 &wordId, u64 &id)
{
	// This function gets 'ptr' to the word 'wordId'. 
	// If 'wordId' is not already in the table, it is
	// an error. 
	u32 position, wordLen;
	//ptr = 0;
	mLock->lock();
	char *bucket = getPtrPriv(wordId, position, wordLen, ePtr);
	if (!bucket)
	{
		mLock->unlock();
		OmnAlarm << "Failed to get ptr: " << wordId << enderr;
		return false;
	}

	//aos_assert_rl(eIdSize == sizeof(u64), mLock, false);
	id = *(u64 *)&bucket[position];
	mLock->unlock();
	return true;
}


char *
AosWordIdHash::getPtrPriv(
		const u64 &wordId, 
		u32 &position, 
		u32 &wordLen,
		const PosType flag)
{
	u32 hashkey = (u32)(wordId >> 10);
	u32 bktkey = hashkey & mTableSize;

	// 2. Look up the table.
	char *bucket = mBuckets[bktkey];
	if (!bucket) return 0;
	
	int numElems = mNumElems[bktkey];
	u16 *index = (u16 *)bucket;
	int idx = -1;
	int left = 0;
	int right = numElems-1;
	u32 *ww;
	while (left <= right)
	{
		idx = ((right - left) >> 1) + left;
		u16 offset = index[idx];
		ww = (u32 *)&bucket[offset];
		u32 hk = (*ww) >> 2;
		if (hk < hashkey)
		{
			left = idx+1;
		}
		else if (hk > hashkey)
		{
			right = idx-1;
		}
		else 
		{
			// It matched. It is possible that the searched for could be 
			// on the left or right. Check whether this is the one we are
			// looking for. 
			u32 *ww1 = (u32 *)&bucket[offset];
			u64 the_wid = ((u64)*ww1 << 8) + (u8)bucket[offset+4];
			if (the_wid == wordId)
			{
				aos_assert_r(idx >= 0 && idx < numElems, 0);
				if (flag == eWord)
				{
					position = index[idx] + eWordIdLen;
					wordLen = index[idx+1] - index[idx] - 
						eWordIdLen - eIdSize;
				}
				else
				{
					position = index[idx+1] - eIdSize;
				}
				return bucket;
			}

			// Did not match. Search the left side
			int crtIdx = idx;
			while (idx > 0)
			{
				idx--;
				offset = index[idx];
				u32 *ww = (u32 *)&bucket[offset];
				u32 hk = (*ww) >> 2;
				if (hk != hashkey)
				{
					// Not on the left side.
					break;
				}

				the_wid = ((u64)*ww << 8) + (u8)bucket[offset+4];
				if (the_wid == wordId)
				{
					// Found it. 
					aos_assert_r(idx >= 0 && idx < numElems, 0);
					position = index[idx+1] - eIdSize;
					if (flag == eWord)
					{
						position = index[idx] + eWordIdLen;
						wordLen = index[idx+1] - index[idx] - 
							eWordIdLen - eIdSize;
					}
					else
					{
						position = index[idx+1] - eIdSize;
					}
					return bucket;
				}
			}

			// Did not find the entry on the left side. Try the 
			// right side.
			idx = crtIdx;

			while (idx < numElems-1)
			{
				idx++;
				offset = index[idx];
				ww = (u32 *)&bucket[offset];
				hk = (*ww) >> 2;
				if (hk != hashkey) return 0;

				the_wid = ((u64)*ww << 8) + (u8)bucket[offset+4];
				if (the_wid == wordId)
				{
					// Found it. 
					aos_assert_r(idx >= 0 && idx < numElems, 0);
					position = index[idx+1] - eIdSize;
					if (flag == eWord)
					{
						position = index[idx] + eWordIdLen;
						wordLen = index[idx+1] - index[idx] - 
							eWordIdLen - eIdSize;
					}
					else
					{
						position = index[idx+1] - eIdSize;
					}
					return bucket;
				}
			}

			return 0;
		}
	}

	return 0;
}


bool
AosWordIdHash::resetBuckets()
{
	// This function resets all the buckets. It will 
	// release the memory. 
	// The caller should have locked the table.
	for (u32 i=0; i<=mTableSize; i++)
	{
		if (!mBuckets[i]) continue;
		releaseBucket(mBuckets[i]);
		mBuckets[i] = 0;
	}
	memset(mNumElems, 0, 2 * (mTableSize+1));
	return true;
}


char *
AosWordIdHash::allocateBucket(const int size)
{
	// Currently we simply new the memory. In the future, 
	// we should consider using slab to improve the performance.
	return OmnNew char[size];
}


bool
AosWordIdHash::releaseBucket(const char *bucket)
{
	// Currently we simply delete the bucket. In the future,
	// we should consider using slab to improve the performance.
	OmnDelete [] bucket;
	return true;
}


char *
AosWordIdHash::getWord(const u64 &wordId, u32 &wordLen)
{
	// This function retrieves the word identified by wordId.
	// If not found, it returns 0. Otherwise, it returns the
	// pointer and the word length.
	u32 position;
	mLock->lock();
	char *bucket = getPtrPriv(wordId, position, wordLen, eWord);
	if (!bucket)
	{
		mLock->unlock();
		return 0;
	}

	char *buff = &bucket[position];
	mLock->unlock();
	return buff;
}


u64 
AosWordIdHash::nextWordId()
{
	// This function retrieves the next wordId. The caller should
	// have called 'resetWordIdLoop()' before looping through this
	// function.
	mLock->lock();
	while (mWordIdBktIdx <= mTableSize) 
	{
		if (mWordIdEtrIdx < mNumElems[mWordIdBktIdx])
		{
			char *bucket = mBuckets[mWordIdBktIdx];
			u16 *index = (u16 *)bucket;
			u16 offset = index[mWordIdEtrIdx++];
			u64 *ww = (u64*)&bucket[offset];
			u64 wordId = ((*ww) << 8) + (u8)bucket[offset+4];
			mLock->unlock();
			return wordId;
		}
		mWordIdBktIdx++;
	}
		
	mLock->unlock();
	return AOS_INVWID;
}


char *
AosWordIdHash::nextWord(int &len)
{
	// This function retrieves the next word. The caller should
	// have called 'resetWordLoop()' before looping through this
	// function.
	len = 0;
	mLock->lock();
	while (mWordBktIdx <= mTableSize) 
	{
		if (mWordEtrIdx < mNumElems[mWordBktIdx])
		{
			char *bucket = mBuckets[mWordBktIdx];
			u16 *index = (u16 *)bucket;
			u16 offset = index[mWordEtrIdx];
			len = index[mWordEtrIdx+1] - index[mWordEtrIdx] - eWordIdLen - eIdSize;
			char *word = &bucket[offset+eWordIdLen];
			mWordEtrIdx++;
			mLock->unlock();
			return word;
		}
		mWordEtrIdx = 0;
		mWordBktIdx++;
	}
		
	mLock->unlock();
	return 0;
}


bool
AosWordIdHash::updateBktToFile(const u64 &wordid)
{
	u32 bktid = ((u32)(wordid >> 10)) & mTableSize;
	aos_assert_r(bktid <= mTableSize, false);

	return updateBktToFileByBktid(bktid);
}


bool
AosWordIdHash::updateBktToFileByBktid(const u32 bktid)
{
	// This function updates the bucket 'bktid' into WordID file.
	// WordID file is a sequence of records. Each record is
	// for a bucket. Each
	// record has some extra space. If the size of the modified
	// is not bigger than the record's capacity,
	// then the record is modified
	// directly in the same location. Otherwise, the old record
	// is marked as being invalid, and the new record is appended
	// to the WordID file.
	
	mLock->lock();
	char *bucket = mBuckets[bktid];
	Header header;
   	aos_assert_rl(readHeader(bktid, header), mLock, false);
	aos_assert_rl(bucket, mLock, false);

	// Update the header
	u16 *index = (u16*)bucket;
	header.num_entries = mNumElems[bktid];
	header.bktsize = index[header.num_entries];

	// 2. Update the WordID File. The record is in 
	// the position identified by 'offset'. It checks
	// whether the record is big enough. If yes, it 
	// updates the block. Otherwise, it marks the record
	// as being invalid and append a new record at the
	// end of the file; update the offset. 
	
	u32 offset = header.offset;
	u32 capacity = header.capacity;
	u32 bktsize = getBucketSize(bktid);
	if (bktsize > capacity)
	{
		// Not big enough. Mark the record.
		offset = getNewOffset();
		capacity += eCapacityInc;
		header.offset = offset;
		header.capacity = capacity;
		header.bktsize = bktsize;
		header.num_entries = mNumElems[bktid];
	}

//OmnScreen << "ktttt; wordIdHash: save Bkt:"
//		<< "virtual_id:" << mVirtualId
//		<< "; bktid:" << bktid
//		<< "; header bktsize:" << header.bktsize
//		<< "; data bktsize:" << index[header.num_entries]
//		<< "; num_entris:" << header.num_entries
//		<< "; bktsize:" << bktsize 
//		<< endl;

	// Update the bucket
	aos_assert_rl(mDataFile && mDataFile->isGood(), mLock, false);
	bool rslt = mDataFile->put(offset, bucket, bktsize, true, mRdata.getPtr());
	aos_assert_rl(rslt, mLock, false);

	updateHeader(header);

	// It's not need send. just docFileMgr need sendReplicLog
	//u32 crt_tid = OmnGetCurrentThreadId();
	//AosReplicMgrClt::getSelf()->sendReplicLog(mRdata, crt_tid, mVirtualId);

	mLock->unlock();
	return true;
}


bool
AosWordIdHash::openDataFile()
{
	// Ketty 2012/08/01
	if (mHasVirtual)
	{
		u64 file_id = ((u64)mVirtualId << 32) + AOSFILEID_WORDIDHASH;
		//mDataFile = AosStorageMgr::getSelf()->openSysRlbFile(mRdata, mVirtualId, file_id, 
		//				"word", eFileSize, true, AosReliableFile::eNormal, true);
	
		AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
		aos_assert_r(vfsMgr, false);
		mDataFile = vfsMgr->openRlbFile(mRdata.getPtr(), mVirtualId, file_id, 
						"word", eFileSize, true, AosRlbFileType::eLog);
	}
	else
	{
		// This is only happened  for IgnoreWord. so, don't has virtual_id.
		// Ketty 2012/11/01
		/*
		OmnString fname = mHashName;
		mDataFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
		if (!mDataFile->isGood())
		{
			mDataFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		}
		*/
	}
	
	/*
	OmnString fname = mDirname;
	if (fname.find('/', true) == fname.length()-1 || fname == "")   
	{
		fname << mHashName;
	}
	else
	{
		fname << "/" << mHashName;
	}

	if (OmnApp::getAppConfig()->getAttrStr("serverReadonly") == "true")
	{
		mDataFile = OmnNew OmnFile(fname, OmnFile::eReadOnly);
		if (!mDataFile->isGood())
		{
			OmnScreen << "The Server has not data, please run it in repairing!" << endl;
			OmnExitApp("server exitting");
		}
	}
	else
	{
		mDataFile = OmnNew OmnFile(fname, OmnFile::eReadWrite);
		if (!mDataFile->isGood())
		{
			mDataFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		}
	}
	*/
	aos_assert_r(mDataFile && mDataFile->isGood(), false);

	// Ketty 2012/11/05
	// Read the flag
	bool rslt = true;
	bool isValidFile = false;
	u64 crtFileSize = mDataFile->getFileCrtSize();	
	if (crtFileSize > eDataFileFlagOffset)
	{
		char flag[eDataFileFlagSize+1] = {0};
		//int bytesread = mDataFile->readToBuff(eDataFileFlagOffset, eDataFileFlagSize, flag, mRdata); 
		rslt = mDataFile->readToBuff(eDataFileFlagOffset, eDataFileFlagSize, flag, mRdata.getPtr());
		aos_assert_r(rslt, false);

		flag[eDataFileFlagSize] = 0;
		isValidFile = strcmp(flag, sgDataFileFlag.data()) == 0 ? true : false;
	}
		
	if (!isValidFile)
	{
		// It is not a valid data file. Reset it.
		rslt = mDataFile->put(
			eDataFileFlagOffset, sgDataFileFlag.data(),
			sgDataFileFlag.length(), false, mRdata.getPtr());
		aos_assert_r(rslt, false);

		mFileSize = eDataFileInitSize;

		// Ketty 2013/03/14 mFileSize not need save.
		//aos_assert_r(mDataFile->setInt(eDataFileSizeOffset, mFileSize, true, mRdata), false);
	
		// Ketty 2012/12/12
		// It's not need send. just docFileMgr need sendReplicLog
		//u32 crt_tid = OmnGetCurrentThreadId();
		//AosReplicMgrClt::getSelf()->sendReplicLog(mRdata, crt_tid, mVirtualId);        
		return true;
	}

	// It is a valid data file
	// Ketty 2012/11/01
	//mFileSize = mDataFile->readBinaryInt(eDataFileSizeOffset, -1);
	// Ketty 2013/03/14
	mFileSize = (crtFileSize / eDftBlockSize) * eDftBlockSize; 
	if (crtFileSize % eDftBlockSize > 0) mFileSize += eDftBlockSize;

	//mFileSize = mDataFile->readInt(eDataFileSizeOffset, -1, mRdata);
	aos_assert_r(mFileSize > 0, false);

	/*
	char flag[eDataFileFlagSize+1];
	int bytesread = mDataFile->readToBuff(eDataFileFlagOffset, eDataFileFlagSize, flag, mRdata); 
	flag[eDataFileFlagSize] = 0;
	if (bytesread > 0 && strcmp(flag, sgDataFileFlag.data()) == 0)
	{
		// It is a valid data file
		// Ketty 2012/11/01
		//mFileSize = mDataFile->readBinaryInt(eDataFileSizeOffset, -1);
		mFileSize = mDataFile->readInt(eDataFileSizeOffset, -1, mRdata);
		aos_assert_r(mFileSize > 0, false);
	}
	else
	{
		// It is not a valid data file. Reset it.
		aos_assert_r(mDataFile->put(eDataFileFlagOffset, sgDataFileFlag.data(), 
				sgDataFileFlag.length(), false, mRdata), false);
		mFileSize = eDataFileInitSize;
		aos_assert_r(mDataFile->setInt(eDataFileSizeOffset, mFileSize, true, mRdata), false);
	}
	*/
	// Ketty end.
	return true;
}


bool
AosWordIdHash::openIdxFile(bool &isNewTable)
{
	// Index file name: "mDirname/mHashName_idx"

	// Ketty 2012/08/02
	if (mHasVirtual)
	{
		u64 file_id = ((u64)mVirtualId << 32) + AOSFILEID_WORDIDHASH_IDX;
		//mIdxFile = AosStorageMgr::getSelf()->openSysRlbFile(mRdata, mVirtualId, file_id, 
		//				"word_idx", eFileSize, true, AosReliableFile::eNormal, true);
		AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
		aos_assert_r(vfsMgr, false);
		
		mIdxFile = vfsMgr->openRlbFile(mRdata.getPtr(), mVirtualId, file_id, 
						"word_idx", eFileSize, true, AosRlbFileType::eLog, true);
		/*
		mIdxFile = AosStorageMgr::getSelf()->openFileByStrKey(
				                mVirtualId, AOSFILEKEY_WORD_HASH_IDX, rdata AosMemoryCheckerArgs);
		if (!mIdxFile || !mIdxFile->isGood())
		{
			mIdxFile = AosStorageMgr::getSelf()->createFileByStrKey(
				mVirtualId, AOSFILEKEY_WORD_HASH_IDX, eFileSize, true, rdata AosMemoryCheckerArgs);
		}
		*/
	}
	else
	{
		// This is only happened  for IgnoreWord. so, don't has virtual_id.
		// Ketty 2012/11/01
		/*
		OmnString fname_idx = mHashName;
		fname_idx << "_idx";

		mIdxFile = OmnNew OmnFile(fname_idx, OmnFile::eReadWrite AosMemoryCheckerArgs);
		if (!mIdxFile->isGood())
		{
			mIdxFile = OmnNew OmnFile(fname_idx, OmnFile::eCreate AosMemoryCheckerArgs);
		}
		*/
	}
	
	/*
	OmnString fname = mDirname;
	if (fname.find('/', true) == fname.length()-1 || fname == "")   
	{
		fname << mHashName << "_idx";
	}
	else
	{
		fname << "/" << mHashName << "_idx";
	}

	if (OmnApp::getAppConfig()->getAttrStr("serverReadonly") == "true")
	{
		mIdxFile = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	}
	else
	{
		mIdxFile = OmnNew OmnFile(fname, OmnFile::eReadWrite);
		if (!mIdxFile->isGood())
		{
			mIdxFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		}
	}
	*/
	aos_assert_r(mIdxFile->isGood(), false);

	// Read the flag
	char flag[eDataFileFlagSize+1] = {0};
	int bytesread = mIdxFile->readToBuff(eIdxFileFlagOffset, eIdxFileFlagSize, flag, mRdata.getPtr());
	flag[eIdxFileFlagSize] = 0;

	bool rslt = true;
	if (bytesread <= 0 || strcmp(flag, sgIdxFileFlag.data()))
	{
		// It is a new table. Reset it.
		isNewTable = true;
		rslt = mIdxFile->put(
			eIdxFileFlagOffset, sgIdxFileFlag.data(),
			sgIdxFileFlag.length(), false, mRdata.getPtr());
		aos_assert_r(rslt, false);

		char buff[eIdxFileRcdSize+1] = {0};
		for (u32 i=0; i<=mTableSize; i++)
		{
			u32 offset = i * eIdxFileRcdSize + eIdxFileInitSize;
			rslt = mIdxFile->put(offset, buff, eIdxFileRcdSize, (i==mTableSize-1), mRdata.getPtr());
			aos_assert_r(rslt, false);
		}
		
		// It's not need send. just docFileMgr need sendReplicLog
		// Ketty 2012/12/12
		//u32 crt_tid = OmnGetCurrentThreadId();
		//AosReplicMgrClt::getSelf()->sendReplicLog(mRdata, crt_tid, mVirtualId);        
	}
	else
	{
		isNewTable = false;
	}
	return true;
}


u32 
AosWordIdHash::getNewOffset()
{
	// The new offset is the size of the WordId file. 
	u32 offset = mFileSize;
	mFileSize += eDftBlockSize;

	// Ketty 2013/03/14 mFileSize not need save.
	//aos_assert_r(mDataFile, offset);
	//aos_assert_r(mDataFile->setInt(eDataFileSizeOffset, mFileSize, false, mRdata), offset);

//OmnScreen << "ktttt; wordIdHash: getNewOffset:"
//		<< "virtual_id:" << mVirtualId
//		<< "; offset:" << offset
//		<< "; mFileSize:" << mFileSize
//		<< endl;
	return offset;
}


bool
AosWordIdHash::updateHeader(const AosWordIdHash::Header &header)
{
	aos_assert_r(mIdxFile, false);
	u32 offset = header.bktid * eIdxFileRcdSize + eIdxFileInitSize;
	char buff[eIdxFileRcdSize+1] = {0};
	*(u32 *)&buff[0] = header.bktsize;
	*(u16 *)&buff[4] = header.num_entries;
	*(u32 *)&buff[6] = header.offset;
	*(u32 *)&buff[10] = header.capacity;

	bool rslt = mIdxFile->put(offset, buff, eIdxFileRcdSize, true, mRdata.getPtr());
	aos_assert_r(rslt, false);

	return true;
}


bool	
AosWordIdHash::saveId(const u64 &wordId, const u64 id)
{
	bool rslt = setId(wordId, id);
	aos_assert_r(rslt, false);

	rslt = updateBktToFile(wordId);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosWordIdHash::integrityCheck2(
		const char *word, 
		const int wordLen, 
		const int idx,
		char *bucket, 
		const u32 bktId)
{
	// 1. Make sure the wordLen is correct.
	if (!sgSanityCheck) return true;
	aos_assert_r(word, false);
	aos_assert_r(wordLen > 0, false);

	// 2. Make sure 'word' does not contain nulls. 
	for (int i=0; i<wordLen; i++)
	{
		aos_assert_r(word[i], false);
	}

	// 3. Make sure 'idx' is [0, numElems]
	u16 numElems = mNumElems[bktId];
	aos_assert_r(idx >= 0 && idx <= numElems, false);
	return true;
}


bool
AosWordIdHash::addEntry(
		const char *word, 
		const int wordLen,
		const u32 newHashkey, 
		const u64 &wordid)
{
	// This function inserts a new entry into the corresponding bucket. 
	// 'word' is the word to be added; 'newHashkey' is the 30-bit hash 
	// key for 'word'. 'wordid' is the wordid for the word.
	// 
	// IMPORTANT: The caller should have locked the table.
	//

	// 1. Find the bucket.
	u32 bktId = newHashkey & mTableSize;
	char *bucket = mBuckets[bktId];
	u16 numElems = mNumElems[bktId];

	// aos_assert_r(integrityCheck2(word, wordLen, idx, 
	// 	bucket, bktId), false);

	u16 *indexArray = 0;
	if (!bucket)
	{
		// It is an empty bucket. Its size should be:
		// index[0]: two bytes
		// index[1]: two bytes
		// wordId:	 five bytes
		// word:	 variable
		// ptr:		 variable
		//
		int newSize = 4 + eWordIdLen + eIdSize + wordLen;
		char *mem = allocateBucket(newSize);
		if (!mem)
		{
			OmnAlarm << "Failed to allocate memory!" << enderr;
			return AOS_INVWID;
		}
		indexArray = (u16 *)mem;
		indexArray[0] = 4;
		indexArray[1] = newSize;
		u32 *ww = (u32*)&mem[4];
		*ww = (wordid >> 8);
		mem[8] = (char)wordid;
		memcpy(&mem[9], word, wordLen);
		mBuckets[bktId] = mem;
		mNumElems[bktId] = 1;

		// Reset the ptr field
		memset(&mem[newSize - eIdSize], 0, eIdSize);
		aos_assert_r(integrityCheck(bktId, AOS_INVWID), AOS_INVWID);
	
		mTotalWords++;
		return true;
	}

	// Need to insert the entry into the bucket. The bucket is sorted
	// based on wordid. 
	
	// 1. Find the location at which the entry is to be inserted. 
	indexArray = (u16 *)bucket;
	aos_assert_r(numElems > 0, AOS_INVWID);
	if (indexArray[0] != ((numElems+1) << 1))
	{
		OmnAlarm << "Something is wrong: " << indexArray[0]
			<< ", " << numElems << enderr;
		return AOS_INVWID;
	}

	int loc = numElems;
	for (int i=0; i<numElems; i++)
	{
		u16 offset = indexArray[i];
		u32 *tmp = (u32 *)&bucket[offset];
		u64 wid = ((u64)(*tmp) << 8) + (u8)bucket[offset+4];

		if (wid < wordid)
		{
			continue;
		}
		else if (wid > wordid)
		{
			// The new entry should be inserted in front of i
			loc = i;
			break;
		}
		else
		{
			// This should normally not happen. But it may be the 
			// case that one thread was trying to find the word but
			// not found, it sent a request. Before the response was
			// received, another thread was trying to do the same
			// thing. When the second response (for the same word)
			// came back, it will come to this point. We need to 
			// make sure the same wordid should always match the 
			// same word. Otherwise, it is an error.
			u16 len = indexArray[i+1] - indexArray[i] - eWordIdLen - eIdSize;
 			if (len != wordLen || strncmp(word, &bucket[offset+eWordIdLen], len))
			{
				OmnAlarm << "Same wordid but not the same word: " 
					<< word << ":" << &bucket[offset + eWordIdLen] << enderr;
				return false;
			}

			return true;
		}
	}

	// Allocate the memory. Note that the new size should be 
	// the current size plus two bytes for the index, five 
	// bytes for the WordID and 'wordLen' for the word itself.
	u32 delta = wordLen + 2 + eWordIdLen + eIdSize;
   	int newSize = indexArray[numElems] + delta;
	char *mem = allocateBucket(newSize);
	u32 bktLen = indexArray[numElems] + delta; 
	aos_assert_r(bktLen < eMaxBucketLen, false);
	aos_assert_r(mem, false);

	// 2. Construct the index. First, copy the entries that
	// are in front of 'newIdx'. Then, set the new entry,
	// and then copy the remaining.
	u16 *newIndex = (u16*)mem;
	if (loc == 0)
	{
		// It is to insert in front of the first entry
		newIndex[0] = ((numElems+2)<<1);
		for (int i=1; i<=numElems+1; i++)
			newIndex[i] = indexArray[i-1] + delta;
		u16 old_offset = indexArray[0];
		u16 new_offset = newIndex[1];
		memcpy(&mem[new_offset], &bucket[old_offset], 
			newSize - new_offset);
	}
	else
	{
		// It inserts at 'loc'. 
		// 1. First, copy all the contents that are in 
		// front of 'loc'.
		for (int i=0; i<=loc; i++)
			newIndex[i] = indexArray[i] + 2;

		u16 old_offset = indexArray[0];
		u16 new_offset = newIndex[0];
		memcpy(&mem[new_offset], &bucket[old_offset], 
			indexArray[loc] - old_offset);

		// 3. Copy the remaining.
		for (int i=loc+1; i<=numElems+1; i++)
			newIndex[i] = indexArray[i-1] + delta;
		old_offset = indexArray[loc]; 
		new_offset = newIndex[loc+1];

		if (indexArray[numElems] > old_offset)
		{
			memcpy(&mem[new_offset], &bucket[old_offset],
				indexArray[numElems] - old_offset);
		}
	}

	// 2. Set the current entry
	u64 offset = newIndex[loc];
	u32 *ww = (u32 *)&mem[offset];
	*ww = (wordid >> 8);
	mem[offset+4] = (char)wordid;

	memcpy(&mem[offset+eWordIdLen], word, wordLen);
	numElems = ++mNumElems[bktId];
	mBuckets[bktId] = mem;

	// Reset the ptr
	for (int kk=0; kk<eIdSize; kk++)
		mem[offset + eWordIdLen + wordLen + kk] = 0;

	// Make sure everything is correct.
	// aos_assert_r(integrityCheck1(mem, bucket, numElems-1, 
	// 	newHashkey, conflictNum, word), false);

	if (bucket) releaseBucket(bucket);
	mTotalWords++;
	return true;
}


bool
AosWordIdHash::addWord(
		const char *word, 
		const int wordLen,
		const u64 &wordid,
		const u64 id)
{
	u32 hashkey = AosGetHashKey(word, wordLen);
	bool rslt = addEntry(word, wordLen, hashkey, wordid);
	aos_assert_r(rslt, AOS_INVWID);
	setId(wordid, id);
	return true;
}


bool
AosWordIdHash::createHashtableNew()
{
	// The hashtable is defined in the database. Each bucket 
	// is defined by a record. This function loops over
	// the records to create the table. It reads a number
	// of records each time, process the records. 
	
	// 1. Read in the hash table information.
	mLock->lock();
	bool rslt = true;
	bool isNewTable = false;
	if (!mDataFile) 
	{
		rslt = openDataFile();
		aos_assert_rl(rslt, mLock, false);
	}

	if (!mIdxFile) 
	{
		rslt = openIdxFileNew(isNewTable);
		aos_assert_rl(rslt, mLock, false);
	}

	if (isNewTable) 
	{
		mLock->unlock();
		return true;
	}

	//OmnRslt rslt;
	mCrtDbReadIdx = 0;
	const int size = getHeaderSize();

	char *buff = OmnNew char[size];
	// Ketty 2012/11/12
	//int size_read = mIdxFile->readToBuff(0, size, buff, mRdata);
	//aos_assert_rl(size_read == size, mLock, false);
	rslt = mIdxFile->readToBuff(0, size, buff, mRdata.getPtr());
	aos_assert_rl(rslt, mLock, false);

	int total_elems = 0;
	// Read all the contents into memory
	u64 datasize = mDataFile->getFileCrtSize();
	char *datamem = 0;
	if (datasize < eMaxDataSize)
	{
		try
		{
			datamem = OmnNew char[datasize];
		}

		catch (...)
		{
			OmnAlarm << "Run out of memory: " << datasize << enderr;
			datasize = 0;
		}
		// Ketty 2012/11/12
		rslt = mDataFile->readToBuff(0, datasize, datamem, mRdata.getPtr());
		aos_assert_r(rslt, false);
		/*
		int nn = mDataFile->readToBuff(0, datasize, datamem, mRdata);
		if (nn > 0 && (u32)nn != datasize)
		{
			OmnAlarm << "File reading error: " << nn << ":" << datasize << enderr;
			delete [] datamem;
			datamem = 0;
		}
		*/
	}

	for (u32 bktid=0; bktid<=mTableSize; bktid++)
	{
		// read from hard disk to buff
		//if ((bktid & 0xfff) == 0) OmnScreen << "Create bucket: " << bktid << endl;
		
		// Get data from buff.
		// 	Bucket Size			(4 bytes)
		// 	Num Elements		(2 bytes)
		// 	Offset 				(4 bytes)
		int idx_offset = bktid * eIdxFileRcdSize + eIdxFileInitSize;
		u32 bktsize = *(u32 *)&buff[idx_offset];
		u16 numElems = *(u16 *)&buff[idx_offset+4];
		u32 offset = *(u32 *)&buff[idx_offset+6];

		aos_assert_rl(bktsize >= 0 && bktsize < eMaxBktSize, mLock, false);
		aos_assert_rl(offset >= 0, mLock, false);
		
		if (numElems > 0)
		{
			// It is not an empty bucket
			total_elems += numElems;
			char *mem = allocateBucket(bktsize);
			aos_assert_rl(mem, mLock, false);

			if (datamem)
			{
				aos_assert_rl(offset + bktsize <= datasize, mLock, false);
				memcpy(mem, &datamem[offset], bktsize);
			}
			else
			{
				int nn = mDataFile->readToBuff(offset, bktsize, mem, mRdata.getPtr());
				aos_assert_rl(nn > 0 && (u32)nn == bktsize, mLock, false);
			}
			mBuckets[bktid] = mem;
			mNumElems[bktid] = numElems;

			// OmnScreen << "Bucket size: " << bktid << ":" << numElems << ":" << bktsize << endl;
			// Need to reset the pointers
			u16 *index = (u16 *)mem;
			if (index[numElems] != bktsize)
			{
				aos_assert_rl(index[numElems] == bktsize, mLock, false);
			}
			for (u16 i=0; i<numElems; i++)
			{
				if (index[i+1] > bktsize)
				{
					aos_assert_rl(index[i+1] == bktsize, mLock, false);
					//OmnAlarm << "Invalid data: " << index[numElems] << ":" << bktsize << ":" << numElems << enderr;
					//index[i+1] = bktsize;
					//mNumElems[bktid] = i+1;
				}
				else
				{
					int idx = index[i+1] - eIdSize;
					u64 pp = *(u64*)&mem[idx];

					// In the current implementations, IILIDs are u32. The following
					// code ensures that the IIL is u32. Chen Ding, 2012/10/11
					u64 pp1;
					if ((pp1 = (pp >> 32)))
					{
						*(u64*)&mem[idx] = pp1;
					}
					//aos_assert_rl(pp, mLock, false);
					if (!pp)
					{
						int ii = index[i] + eWordIdLen;
						OmnString ss(&mem[ii], idx - ii);
						OmnAlarm << "iilid is 0, word:" << ss << ", i:" << i << ", numElems:" << numElems << enderr;
						mLock->unlock();
						return false;
					}
				}
			}
		}
	}
	delete [] buff;
	delete [] datamem;
	mLock->unlock();

	return true;
}


bool
AosWordIdHash::openIdxFileNew(bool &isNewTable)
{
	// Index file name: "mDirname/mHashName_idx"
	// Ketty 2012/08/02
	if (mHasVirtual)
	{
		u64 file_id = ((u64)mVirtualId << 32) + AOSFILEID_WORDIDHASH_IDX;
		//mIdxFile = AosStorageMgr::getSelf()->openSysRlbFile(mRdata, mVirtualId, file_id, 
		//				"word_idx", eFileSize, true, AosReliableFile::eNormal, true);
		AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
		aos_assert_r(vfsMgr, false);
		
		mIdxFile = vfsMgr->openRlbFile(mRdata.getPtr(), mVirtualId, file_id, 
			"word_idx", eFileSize, true, AosRlbFileType::eLog, true);
		/*
		mIdxFile = AosStorageMgr::getSelf()->openFileByStrKey(
				                mVirtualId, AOSFILEKEY_WORD_HASH_IDX, rdata AosMemoryCheckerArgs);
		if (!mIdxFile || !mIdxFile->isGood())
		{
			mIdxFile = AosStorageMgr::getSelf()->createFileByStrKey(
						                mVirtualId, AOSFILEKEY_WORD_HASH_IDX, eFileSize, true, rdata AosMemoryCheckerArgs);
		}
		*/
	}
	else
	{
		// This is only happened  for IgnoreWord. so, don't has virtual_id.
		// Ketty 2012/11/01
		/*
		OmnString fname_idx = mHashName;
		fname_idx << "_idx";

		mIdxFile = OmnNew OmnFile(fname_idx, OmnFile::eReadWrite AosMemoryCheckerArgs);
		if (!mIdxFile->isGood())
		{
			mIdxFile = OmnNew OmnFile(fname_idx, OmnFile::eCreate AosMemoryCheckerArgs);
		}
		*/
	}
	/*
	OmnString fname = mDirname;
	if (fname.find('/', true) == fname.length()-1 || fname == "")   
	{
		fname << mHashName << "_idx";
	}
	else
	{
		fname << "/" << mHashName << "_idx";
	}

	if (OmnApp::getAppConfig()->getAttrStr("serverReadonly") == "true")
	{
		mIdxFile = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	}
	else
	{
		mIdxFile = OmnNew OmnFile(fname, OmnFile::eReadWrite);
		if (!mIdxFile->isGood())
		{
			mIdxFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		}
	}
	*/
	aos_assert_r(mIdxFile && mIdxFile->isGood(), false);

	// Read the flag
	// Ketty 2012/11/05
	bool rslt = true;
	isNewTable = true;
	u64 crtFileSize = mIdxFile->getFileCrtSize();	
	if (crtFileSize > eIdxFileFlagOffset)
	{
		char flag[eDataFileFlagSize+1] = {0};
		//int bytesread = mIdxFile->readToBuff(eIdxFileFlagOffset, eIdxFileFlagSize, flag, mRdata);
		rslt = mIdxFile->readToBuff(eIdxFileFlagOffset, eIdxFileFlagSize, flag, mRdata.getPtr());
		aos_assert_r(rslt, false);

		flag[eIdxFileFlagSize] = 0;
		isNewTable = strcmp(flag, sgIdxFileFlag.data()) != 0 ? true : false;
	}

	if (isNewTable)
	{
		// It is a new table. Reset it.
		rslt = mIdxFile->put(
			eIdxFileFlagOffset, sgIdxFileFlag.data(),
			sgIdxFileFlag.length(), false, mRdata.getPtr());
		aos_assert_r(rslt, false);

		const int size = getHeaderSize();
		char *buff = OmnNew char[size];
		memset(buff, 0, size);

		u32 offset = eIdxFileInitSize;
		rslt = mIdxFile->put(offset, buff, size, true, mRdata.getPtr());
		aos_assert_r(rslt, false);

		OmnDelete [] buff;

		// It's not need send. just docFileMgr need sendReplicLog
		// Ketty 2012/12/12
		//u32 crt_tid = OmnGetCurrentThreadId();
		//AosReplicMgrClt::getSelf()->sendReplicLog(mRdata, crt_tid, mVirtualId);        
	}

	/*
	char flag[eDataFileFlagSize+1];
	int bytesread = mIdxFile->readToBuff(eIdxFileFlagOffset, eIdxFileFlagSize, flag, mRdata);
	flag[eIdxFileFlagSize] = 0;
	if (bytesread <= 0 || strcmp(flag, sgIdxFileFlag.data()))
	{
		// It is a new table. Reset it.
		isNewTable = true;
		aos_assert_r(mIdxFile->put(eIdxFileFlagOffset, sgIdxFileFlag.data(), 
				sgIdxFileFlag.length(), false, mRdata), false);

		const int size = getHeaderSize();
		char *buff = OmnNew char[size];
		memset(buff, 0, size);
		u32 offset = eIdxFileInitSize;
		aos_assert_r(mIdxFile->put(offset, buff, size, true, mRdata), false);
		OmnDelete [] buff;
	}
	else
	{
		isNewTable = false;
	}
	*/
	// Ketty end.
	return true;
}


int 
AosWordIdHash::getHeaderSize() const
{
	return eIdxFileRcdSize*(mTableSize+1) + eIdxFileInitSize;
}

