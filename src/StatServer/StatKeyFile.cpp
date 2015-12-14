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
// Vector Key Files are used to save keys. The file structure is:
// 		number of keys	u32
// 		[key offset]	u32	
// 		[key offset]	u32
// 		...
// 		[key offset]	u32		// These define the index block
// 		key				variable	
// 		key				variable	
// 		...
//
// The index block size is configurable and defaults to 1MB. Keys
// are immutable. 
//
// There can be multiple key files. One key file may potentially
// have millions of keys.
//
// Modification History:
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StatServer/StatKeyFile.h"

#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Debug/Debug.h"

static int globalPrintCounts = 0;
static int maxPrint = 100;

AosStatKeyFile::AosStatKeyFile(
		AosRundata *rdata, 
		const u64 cube_id,
		const u64 file_id, 
		const u64 start_sdocid,
		const u32 num_rows_per_file, 
		const AosRlbFileType::E file_type)
:
mIsGood(false),
mCubeId(cube_id),
mFileId(file_id),
mFileSize(-1),
mStartSdocid(start_sdocid),
mNumKeys(0),
mNumSdocidsPerFile(num_rows_per_file),
//mCacheSize(eCacheSize),
//mCachedStartSdocid(-1),
mIsDirty(false),
mIndexArray(0),
mCacheBuffRaw(0),
mCachedStartOff(0),
//mCachedStartSdocid(-1),
//mCachedKeyNum(0),
mCachedBytes(eCacheBytes),
mFileType(file_type),
mReliableFileRaw(0)
{
	initCounters();

	if (file_id > 0)
	{
		mIsGood = readMetadata(rdata);
	}
	else
	{
		mIsGood = initializeVectorKeyFile(rdata);
	}

	aos_assert(mIsGood);
}

void
AosStatKeyFile::initCounters()
{
	mReadTime1 = 0;
	mReadTime2 = 0;
	mReadTime3 = 0;

	mReadNum1 = 0;
	mReadNum2 = 0;
	mReadNum3 = 0;
}

void
AosStatKeyFile::outputCounters()
{
	OmnScreen << "(Statistics counters : StatKeyFile ) readKeys --- Time1 : "
		        << mReadTime1 << ", Num: " << mReadNum1 << endl;

	OmnScreen << "(Statistics counters : StatKeyFile ) readFromCache --- Time2 : "
		        << mReadTime2 << ", Num: " << mReadNum2 << endl;

	OmnScreen << "(Statistics counters : StatKeyFile ) mReliableFileRaw->readToBuff --- Time3 : "
		        << mReadTime3 << ", Num: " << mReadNum3 << endl;
}

AosStatKeyFile::~AosStatKeyFile()
{
}


bool
AosStatKeyFile::readMetadata(AosRundata *rdata)
{
	// This is a private function. It shall be called only once.
	// It initializes the class by reading the metadata from
	// the file.
	// File format is:
	// 		start_sdocid		u64
	// 		num_keys			u32
	// 		index block			u32 * keys_per_file
	// 		key block			variable
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	mReliableFile = vfsMgr->openRlbFile(mFileId, mFileType, rdata);
	aos_assert_r(mReliableFile, false);

	mReliableFileRaw = mReliableFile.getPtr();
	aos_assert_rr(mReliableFileRaw, rdata, false);
	mFileSize = mReliableFileRaw->getFileCrtSize();

	int buff_size = sizeof(u64) + sizeof(u32) + mNumSdocidsPerFile *sizeof(u32);
	mIndexBuff = OmnNew AosBuff(buff_size AosMemoryCheckerArgs);
	char *data = mIndexBuff->data();
	bool rslt = mReliableFile->readToBuff(eIndexStartPos, buff_size, data, rdata);
	aos_assert_rr(rslt, rdata, false);

	mStartSdocid = *(u64 *)data;
	mNumKeys = *(u32 *)&data[sizeof(u64)];
	mIndexArray = (u32 *)&mIndexBuff->data()[sizeof(u64) + sizeof(u32)];
	
	if(mStartSdocid == 0 && mNumKeys ==0 )	mNumKeys = 1;		// Temp.

	mNumKeys = mNumSdocidsPerFile;
	
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosStatKeyFile::initializeVectorKeyFile(AosRundata *rdata)
{
	// It initializes a vector key file. It creates the index
	// block, initializes it, and saves it.
	//
	// File format is:
	// 		start_sdocid		u64
	// 		num_keys			u32
	// 		index block			u32 * keys_per_file
	// 		key block			variable
	//
	// mFileSize must be set to the proper file size.
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	mReliableFile = vfsMgr->createRlbFile(rdata, mCubeId, "vctfile", 0, mFileType, false);
	aos_assert_rr(mReliableFile, rdata, false);
	mReliableFileRaw = mReliableFile.getPtr();
	mFileId = mReliableFileRaw->getFileId(); 
	
	//mNumKeys = 0;
	mNumKeys = mNumSdocidsPerFile;

	int buff_size = sizeof(u64) + sizeof(u32) + mNumSdocidsPerFile *sizeof(u32);
	mIndexBuff = OmnNew AosBuff(buff_size AosMemoryCheckerArgs);
	char *data = mIndexBuff->data();
	memset(data, 0, buff_size);

	*(u64 *)data = mStartSdocid;
	*(u32 *)&data[sizeof(u64)] = 0;
	mIndexArray = (u32 *)&data[sizeof(u64) + sizeof(u32)];

	bool rslt = mReliableFile->put(eIndexStartPos, data, buff_size, true, rdata);
	aos_assert_rr(rslt, rdata, false);
	
	// Ketty 2014/09/09
	mFileSize = mReliableFileRaw->getFileCrtSize();
	
	if(mStartSdocid == 0 && mNumKeys ==0 )	mNumKeys = 1;		// Temp.
	OmnTagFuncInfo << endl;
	return true;
}

/*
bool
AosStatKeyFile::readKeys(
		AosRundata *rdata, 
		const u64 sdocid, 
		int &num_keys, 
		AosBuffPtr &buff)
{
	// This function reads 'num_keys' starting from 'sdocid'. 
	// Upon success, it reads in up to 'num_keys'. The actual
	// number of keys being read is returned through 'num_keys'.
	//
	// Keys are encoded as [int (for length), contents (variable)]
	aos_assert_rr(num_keys > 0, rdata, false);	
	aos_assert_rr(sdocid >= mStartSdocid, rdata, false);

	int offset = sdocid - mStartSdocid;
	int num_to_read = num_keys;
	if (offset + num_to_read > mNumKeys)
	{
		num_to_read = mNumKeys - offset;
	}

	u32 start_pos = mIndexArray[offset];
	u32 end_pos = 0;
	if (offset + num_to_read == mNumKeys)
	{
		end_pos = mFileSize;
	}
	else
	{
		end_pos = mIndexArray[offset + num_to_read];
	}

	u32 size = end_pos - start_pos;
	buff = OmnNew AosBuff(size);
	char *data = buff->data();

	return mReliableFileRaw->readToBuff(start_pos, size, data, rdata);
}
*/

bool 
AosStatKeyFile::readKey(
		AosRundata *rdata, 
		const u64 sdocid, 
		OmnString &key)
{
	// This function reads in one key. The class has a cache.
	
	// Check whether it is in the cache.
	aos_assert_rr(sdocid >= mStartSdocid && sdocid < mStartSdocid + mNumSdocidsPerFile, rdata, false);
	u32 local_sdocid = sdocid - mStartSdocid;
	int len;
	u32 startKeyOff = mIndexArray[sdocid - mStartSdocid];

	if (local_sdocid >= mNumKeys || startKeyOff == 0)
	{
		// The key does not exist.
		key = "";
		return true;
	}

	if (isInCache(sdocid)) 
		return readFromCache(rdata, sdocid, key, len);

	// Read the cache. The cache will start at 'sdocid'. 
	// The cache size is 'mCacheSize'
	//int num_reads = mNumKeys;
	//int num_reads = eCacheKeyNum;
	u64 tStart, tEnd;

	tStart = OmnGetTimestamp();
	bool rslt = readKeys(rdata, sdocid);
	tEnd = OmnGetTimestamp();
	mReadTime1 += tEnd - tStart;
	mReadNum1++;

	aos_assert_rr(rslt, rdata, false);
	//aos_assert_rr(num_reads > 0, rdata, false);

	OmnTagFuncInfo << endl;
	//outputCounters();
	return readFromCache(rdata, sdocid, key, len);
}


bool
AosStatKeyFile::readKeys(
		AosRundata *rdata, 
		const u64 sdocid) 
{
	// This function reads in a number of keys from the file
	// starting at 'sdocid' and reads 'num_keys'.
	u32 offset = sdocid - mStartSdocid;
	u64 tStart, tEnd;
	bool rslt;

	int start_pos = mIndexArray[offset];
	int end_pos = start_pos + mCachedBytes; 
	int read_size = mCachedBytes;
	if (end_pos > mFileSize)
	{
		end_pos = mFileSize;
		read_size = end_pos - start_pos; 
	}

	if(!mCacheBuff)
	{
		mCacheBuff = OmnNew AosBuff(read_size AosMemoryCheckerArgs);
		mCacheBuffRaw = mCacheBuff.getPtr();
	}
	else
	{
		if(read_size > mCacheBuff->buffLen())
		{
			mCacheBuff->expandMemory1(read_size);
			mCacheBuffRaw = mCacheBuff.getPtr();
		}
	}

	mCachedStartOff = start_pos;
	//mCachedStartSdocid = sdocid; 
	//mCachedKeyNum = num_to_read;

	char *data = mCacheBuff->data();

	tStart = OmnGetTimestamp();
	rslt = mReliableFileRaw->readToBuff(start_pos, read_size, data, rdata);
	tEnd = OmnGetTimestamp();
	mReadTime3 += tEnd - tStart;
	mReadNum3++;                 

	aos_assert_rr(rslt, rdata, -1);
	mCacheBuff->setDataLen(read_size);

	//if (!mCacheBuff) 
	//{
	//	mCacheBuff = OmnNew AosBuffArrayVar();
	//	mCacheBuffRaw = mCacheBuff.getPtr();
	//}
	//mCacheBuff->setStrBuff(rdata, bb);
	OmnTagFuncInfo << endl;
	return true;
}

/*
bool
AosStatKeyFile::readFromCache(
		AosRundata *rdata, 
		const u64 sdocid,
		OmnString &key, 
		int &len)
{
	u64 tStart, tEnd;
	tStart = OmnGetTimestamp();
	// This function reads the key 'sdocid' from the cache.
	aos_assert_rr(mCacheBuffRaw, rdata, false);
	int idx_off = sdocid - mStartSdocid;
	aos_assert_r((u32)idx_off < mNumKeys, false);
	
	int key_off = mIndexArray[idx_off];
	//aos_assert_r(key_off > 0, false);
	if (key_off <= 0) return true;

	int cache_off = key_off - mCachedStartOff; 
	aos_assert_r(cache_off >= 0, false);
	
	mCacheBuffRaw->setCrtIdx(cache_off);
	key = mCacheBuffRaw->getOmnStr("");

	//char *dd = mCacheBuffRaw->getEntry(rdata, offset, len);
	//OmnString sss(dd, len);
	//key = sss;
	OmnTagFuncInfo << endl;
	tEnd = OmnGetTimestamp();
	mReadTime2 += tEnd - tStart;
	mReadNum2++;

	return true;
}*/

bool
AosStatKeyFile::readFromCache(
		AosRundata *rdata, 
		const u64 sdocid,
		OmnString &key, 
		int &len)
{
	u64 tStart, tEnd;
	tStart = OmnGetTimestamp();

	// This function reads the key 'sdocid' from the cache.
	aos_assert_rr(mCacheBuffRaw, rdata, false);
	int idx_off = sdocid - mStartSdocid;
	aos_assert_r((u32)idx_off < mNumKeys, false);
	
	int key_off = mIndexArray[idx_off];
	aos_assert_r(key_off >= 0, false);

	if (key_off == 0) 
		return true;

	int cache_off = key_off - mCachedStartOff; 
	aos_assert_r(cache_off >= 0, false);
	
	mCacheBuffRaw->setCrtIdx(cache_off);
	key = mCacheBuffRaw->getOmnStr("");

	//char *dd = mCacheBuffRaw->getEntry(rdata, offset, len);
	//OmnString sss(dd, len);
	//key = sss;
	OmnTagFuncInfo << endl;
	tEnd = OmnGetTimestamp();
	mReadTime2 += tEnd - tStart;
	mReadNum2++;

	return true;
}


bool
AosStatKeyFile::isInCache(const u64 sdocid) const
{
	if (!mCacheBuffRaw) return false;
	//return (sdocid >= mStartSdocid && sdocid < mStartSdocid + mNumKeys);
	//return (sdocid >= mCachedStartSdocid && 
	//		sdocid < mCachedStartSdocid + mCachedKeyNum);
	u32 startKeyOff = mIndexArray[sdocid - mStartSdocid];

	//the sdocid doesn't exist
	if (startKeyOff == 0)
		return false;

	//verify if the start offset is in the cache range
	if ( startKeyOff < mCachedStartOff  || 
			startKeyOff + sizeof(u32) > mCachedStartOff + mCachedBytes )
	{
		return false;
	}

	//verify if the sdocid data is in the cache range
	mCacheBuffRaw->setCrtIdx(startKeyOff - mCachedStartOff);
	int len = mCacheBuffRaw->getInt(0); 
	//aos_assert_r( len > 0, false);
	// Pay  2015-11-23 JIMODB-1233
	if(len <= 0)
		return false;

	if (startKeyOff + sizeof(u32) + len > mCachedStartOff + mCachedBytes)
	{
		return false;
	}

	return true;
}


bool 
AosStatKeyFile::appendKey(
		AosRundata *rdata, 
		const u64 start_sdocid,
		const OmnString &key)
{
	// This function appends one key to the end of the file.
	// The file is in the following format:
	//	
	//	num_keys			(at Position eNumKeysPos)
	//	index[0]			(start at Position eIndexArrayPos)
	//	index[1]
	//	...
	//	key_length + key	(the first key)
	//	key_length + key	(the second key)
	//	...
	aos_assert_r(mReliableFileRaw, false);

	// AosBuffPtr bb = OmnNew AosBuff();
	aos_assert_rr(start_sdocid >= mStartSdocid, rdata, false);
	u64 sd = start_sdocid - mStartSdocid;
	aos_assert_rr(sd < mNumSdocidsPerFile, rdata, false);

	// Ketty Temp.
	//aos_assert_rr(mIndexArray[sd] == 0, rdata, false);
	if(mIndexArray[sd] != 0)
	{
		if(globalPrintCounts < maxPrint)
		{
			OmnScreen << "Ketty Stat Print. Save Key failed. maybe this sdocid has saved."
				<< "sdocid:" << start_sdocid << "; "
				<< "key:" << key << "; "
				<< endl;
			globalPrintCounts++;
			return true;
		}
	}

	mIndexArray[sd] = mFileSize;
	//mNumKeys++;

	// save the num keys.
	bool rslt = mReliableFileRaw->setU32(eNumKeysPos, mNumKeys, false, rdata);
	aos_assert_rr(rslt, rdata, false);

	// Save the indexes
	i64 start_pos = eIndexArrayPos + sd*sizeof(u32);
	u32 index_size = sizeof(u32);
	rslt = mReliableFileRaw->put(start_pos, 
			(char *)&mIndexArray[sd], index_size, false, rdata);
	aos_assert_rr(rslt, rdata, false);

	// Save the body
	//rslt = mReliableFileRaw->append(key.data(), key.length(), true);
	rslt = mReliableFileRaw->setInt(mFileSize, key.length(), false, rdata);
	aos_assert_rr(rslt, rdata, false);
	rslt = mReliableFileRaw->put(mFileSize + sizeof(int), key.data(), key.length(), true, rdata);
	aos_assert_rr(rslt, rdata, false);

	mFileSize += (sizeof(int) + key.length());
	
	//OmnScreen << "Append Key:" << start_sdocid
	//	<< "; key:" << key
	//	<< endl;
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosStatKeyFile::appendKeys(
		AosRundata *rdata, 
		const u64 start_sdocid,
		const int num_keys,
		AosBuff *keys)
{
	// This function appends the entries to the key file.
	// 'keys' is in the following format:
	// 		[contents]
	// 		[contents]
	// 		...
	// 		[contents]
	// The contents are constructed by using AosBuff::setCharStr(data, len, false);

	// Ketty 2014/09/09
	OmnNotImplementedYet;
	return false;

	AosBuffPtr bb = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	aos_assert_rr(start_sdocid >= mStartSdocid, rdata, false);
	u64 sd = start_sdocid - mStartSdocid;
	aos_assert_rr(sd + num_keys < mNumSdocidsPerFile, rdata, false);

	i64 pos = mFileSize;
	int key_len;
	for (int i=0; i<num_keys; i++)
	{
		aos_assert_rr(mIndexArray[sd+i] == 0, rdata, false);
		mIndexArray[sd+i] = pos;
		keys->getCharStr(key_len);
		pos = mFileSize + keys->getCrtIdx();
	}
	
	// Save the indexes
	i64 start_pos = eIndexStartPos + sd*sizeof(u32);
	u32 index_size = sizeof(u32) * num_keys;
	bool rslt = mReliableFileRaw->write(start_pos, 
			(char *)&mIndexArray[sd], index_size, rdata);
	aos_assert_rr(rslt, rdata, false);

	// Save the body
	rslt = mReliableFileRaw->append(keys->data(), keys->dataLen(), true);
	aos_assert_rr(rslt, rdata, false);

	mFileSize += keys->dataLen();

	OmnTagFuncInfo << endl;
	return true;
}


