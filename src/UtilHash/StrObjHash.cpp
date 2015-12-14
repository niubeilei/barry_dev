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
// 09/04/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilHash/StrObjHash.h"

#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/HashUtil.h"
#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/SystemId.h"


static bool sgPrintFlag = false;

AosStrObjHash::AosStrObjHash()
:
mLock(OmnNew OmnMutex()),
mNumNodes(0),
mMapMaxSize(eMaxMapSize),
mBucketSize(-1),
mKeyBitmap(eDefaultKeyBitmap),
mReset(false),
mIsPersis(false)
{
}


AosStrObjHash::AosStrObjHash(
		const AosHashedObjPtr &dft_obj,
		const u32 mapsize,
		AosErrmsgId::E &errid, 
		OmnString &errmsg,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mDftObj(dft_obj),
mNumNodes(0),
mMapMaxSize(mapsize),
mBucketSize(0),
mBucketIdBits(0),
mKeyBitmap(0),
mIsPersis(false)
{
	bool rslt = init(errid, errmsg, rdata);
	aos_assert(rslt);
}


AosStrObjHash::AosStrObjHash(
		const AosHashedObjPtr &dft_obj,
		//const OmnString &fname,
		const AosReliableFilePtr &file,		// Ketty 2012/08/16
		const u32 mapsize,
		const u64 &key_bitmap, 
		const int bucket_size,
		const bool reset,
		AosErrmsgId::E &errid, 
		OmnString &errmsg,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mDftObj(dft_obj),
mNumNodes(0),
mFile(file),
mMapMaxSize(mapsize),
mBucketSize(bucket_size),
mBucketIdBits(key_bitmap),
mKeyBitmap(eDefaultKeyBitmap),
//mFilename(fname),
mReset(reset),
mIsPersis(false)
{
	/*
	if (fname == "")
	{
		errid = AosErrmsgId::eMissingFilename;
		errmsg = "The File name is empty!";
		OmnThrowException(errmsg);
	}
	*/
	mIsPersis = true;
	bool rslt = init(errid, errmsg, rdata);
	aos_assert(rslt);
}
	
AosStrObjHash::~AosStrObjHash()
{
	MapItr_t itr = mMap.begin();
	for(; itr!=mMap.end(); ++itr)
	{
		itr->second = 0;
	}
}


bool
AosStrObjHash::init(AosErrmsgId::E &errid, OmnString &errmsg, const AosRundataPtr &rdata)
{
	if (!mDftObj)
	{
		errid = AosErrmsgId::eInvalidParm;
		errmsg = "Default object is null";
		OmnThrowException(errmsg);
	}
	mDataReader = mDftObj->clone();

	if (!mIsPersis)  return true;

	if (mBucketSize <= 0)
	{
		errid = AosErrmsgId::eInvalidParm;
		errmsg = "Bucket size invalid: ";
		errmsg << mBucketSize;
		OmnThrowException(errmsg);
	}

	mKeyBitmap = OmnHashUtil::getBitmask(mBucketIdBits);
	
	// Ketty 2012/08/16
	aos_assert_r(mFile && mFile->isGood(), false);
	if (mReset)
	{
		return resetFile(rdata);	
	}
	/*
	if (!openFile())
	{
		errid = AosErrmsgId::eFailedOpenFile;
		errmsg = "Failed open file: ";
		errmsg << mFilename;
		OmnThrowException(errmsg);
	}
	*/
	return true;
}


AosHashedObjPtr 
AosStrObjHash::find(const OmnString &key, const AosRundataPtr &rdata)
{
	// Brian Zhang 09/17/2011
	aos_assert_r(key != "", 0);
	
	mLock->lock();
	AosStrObjHashSanityCheckLocked;
	AosHashedObjPtr data;
	MapItr_t itr = mMap.find(key);
	if (itr != mMap.end())
	{
		// This means that there are entries that match 'key' in cache hash_map. 
		data = itr->second;
		if (mIsPersis && !data->isExist())
		{
			// This means that the entry is not exist in the hash file.
			moveNode(data);
			data = 0;
			mLock->unlock();
			return data;
		}
		moveNode(data);
		AosStrObjHashSanityCheckLocked;
		mLock->unlock();
		return data;
	}
	if (!mIsPersis) 
	{
		AosStrObjHashSanityCheckLocked;
		mLock->unlock();
		return data;
	}

	bool rslt = readObjPriv(key, data, rdata);
	if (!rslt)
	{
		// It does not exist in the file. 
		aos_assert_rl(mDftObj, mLock, 0);
		AosHashedObjPtr nn = mDftObj->clone();
		aos_assert_rl(nn, mLock, 0);
		nn->setExist(false);
		nn->setKeyStr(key);
		addNode(nn);
		mMap[key] = nn;
		AosStrObjHashSanityCheckLocked;
		mLock->unlock();
		data = 0;
		return data;
	}

	// Read from file
	data->setExist(true);
	addNode(data);
	mMap[key] = data;
	AosStrObjHashSanityCheckLocked;
	mLock->unlock();
	return data;
} 


bool
AosStrObjHash::readObjPriv(
		const OmnString &key, 
		AosHashedObjPtr &node, 
		const AosRundataPtr &rdata)
{
	// This function reads the entry 'key' into memory. If the entry
	// does not exist, it will create an Invalid node. Otherwise, 
	// it creates the node.
	u64 offset = getOffset(key.data(), key.length());
	// Ketty 2012/08/16
	//if (!mFile) openFile();
	aos_assert_r(mFile, false);

	bool rslt = mDataReader->readObj(mFile, offset, key, mBucketSize, node, rdata);
	if (!rslt)
	{
		// There is no entry stored in the file. 
		return false;
	}
	return true;
}


bool 
AosStrObjHash::addDataPublic(
		const OmnString &key, 
		const AosHashedObjPtr &data,
		const bool override,
		const AosRundataPtr &rdata)
{
	if (mIsPersis) 
	{
		return addPersisDataPriv(key, data, override, rdata);
	}
	else
	{
		return addCacheDataPriv(key, data, override);
	}
}


bool 
AosStrObjHash::addCacheDataPriv(
		const OmnString &key, 
		const AosHashedObjPtr &data,
		const bool override)
{
	mLock->lock();
	aos_assert_rl(key != "", mLock, false);
	aos_assert_rl(data, mLock, false);
	aos_assert_rl(key == data->getKeyStr(), mLock, false);
	AosStrObjHashSanityCheckLocked;
	MapItr_t itr = mMap.find(key);
	if (itr != mMap.end())
	{
		AosHashedObjPtr cacheNode = itr->second;
		aos_assert_rl(cacheNode, mLock, false);
		if (override)
		{
			data->replaceNode(cacheNode);
		}
		AosStrObjHashSanityCheckLocked;
		mLock->unlock();
		return true;
	}
	addNode(data);
	mMap[key] = data;
	AosStrObjHashSanityCheckLocked;
	mLock->unlock();
	return true;
}


bool 
AosStrObjHash::addPersisDataPriv(
		const OmnString &key, 
		const AosHashedObjPtr &data,
		const bool override,
		const AosRundataPtr &rdata)
{
	// This function adds 'data' into the map and hash file.
	mLock->lock();

	u64 ss1; if (sgPrintFlag) ss1 = OmnGetTimestamp();

	aos_assert_rl(key != "", mLock, false);
	aos_assert_rl(data, mLock, false);
	aos_assert_rl(key == data->getKeyStr(), mLock, false);
	AosStrObjHashSanityCheckLocked;

	u64 ss11; if (sgPrintFlag) ss11 = OmnGetTimestamp();
	MapItr_t itr = mMap.find(key);
	u64 ss2; if (sgPrintFlag) ss2 = OmnGetTimestamp();

	if (itr != mMap.end())
	{
		// This means that the data exists in the map.
		AosHashedObjPtr cacheNode = itr->second;
		aos_assert_rl(cacheNode, mLock, false);
		if (!cacheNode->isExist())
		{
			bool addRslt = addObjPriv(key, data, rdata);
			aos_assert_rl(addRslt, mLock, false);
			data->setExist(true);
			addNode(data);
			cacheNode->removeFromLinkedList();
			mNumNodes--;
			mMap[key] = data;
			AosStrObjHashSanityCheckLocked;
			mLock->unlock();
			return true;
		}

		// The data is exist in the file, but they are not the same. 
		// So, we need modify this entry according to the 'override' falg.
		if (override)
		{
			bool modifyRslt =  modifyEntryPriv(key, cacheNode, data, rdata);
			aos_assert_rl(modifyRslt, mLock, false);
			data->setExist(true);
		}

		AosStrObjHashSanityCheckLocked;
		mLock->unlock();
		return true;
	}

	// The data does not exist in cache. We need to know 
	// whether the data exist in the hash file.
	aos_assert_rl(data, mLock, false);
	AosHashedObjPtr dbData;
	bool readRslt = readObjPriv(key, dbData, rdata);
	u64 ss3; if (sgPrintFlag) ss3 = OmnGetTimestamp();

	if (readRslt) 
	{
		aos_assert_rl(dbData, mLock, false);
		if (override) 
		{
			bool modifyRslt =  modifyEntryPriv(key, dbData, data, rdata);
			aos_assert_rl(modifyRslt, mLock, false);
			data->setExist(true);
			addNode(data);
			mMap[key] = data;
			AosStrObjHashSanityCheckLocked;
			mLock->unlock();
			u64 ss4; if (sgPrintFlag) ss4 = OmnGetTimestamp();
			if (sgPrintFlag) OmnScreen << "read TimeInfo: " 
				<< ss4 - ss3 << ":" << ss3 - ss2 << ":" << ss2 - ss1 << endl;
			return true;
		}
		dbData->setExist(true);
		addNode(dbData);
		mMap[key] = dbData;
		AosStrObjHashSanityCheckLocked;
		mLock->unlock();
		return true;
	}
	// The data does not exist in the hash file and add it.
	bool addRslt = addObjPriv(key, data, rdata);

	u64 ss4; if (sgPrintFlag) ss4 = OmnGetTimestamp();

	aos_assert_rl(addRslt, mLock, false);
	data->setExist(true);
	addNode(data);
	mMap[key] = data;
	AosStrObjHashSanityCheckLocked;

	u64 ss5; if (sgPrintFlag) ss5 = OmnGetTimestamp();
	if (sgPrintFlag) 
	{
		OmnScreen << "add TimeInfo: " << ss5 - ss4 << ":" << ss4 - ss3 
			<< ":" << ss3 - ss2 << ":" << ss2 - ss11 << ":" 
			<< ss11 - ss1  << ":" << ss5 - ss1 << endl;
	}

	mLock->unlock();
	return true;
}


bool
AosStrObjHash::addObjPriv(const OmnString &key, const AosHashedObjPtr &data, const AosRundataPtr &rdata)
{
	aos_assert_r(key != "", false);
	aos_assert_r(data, false);
	aos_assert_r(key == data->getKeyStr(), false);

	u64 offset = getOffset(key.data(), key.length());
	// Ketty 2012/08/16
	//if (!mFile) openFile();
	aos_assert_r(mFile, false);

	bool rslt = data->addObj(mFile, offset, mBucketSize, rdata);
	
	aos_assert_r(rslt, false);
	return true;
}


bool
AosStrObjHash::modifyEntryPriv(
		const OmnString &key, 
		const AosHashedObjPtr &oldobj, 
		const AosHashedObjPtr &newobj,
		const AosRundataPtr &rdata)
{
	aos_assert_r(oldobj, false);
	aos_assert_r(newobj, false);
	aos_assert_r(oldobj->isKeySame(newobj), false);
	if (oldobj->isSame(newobj)) return true;

	u64 offset = getOffset(key.data(), key.length());
	// Ketty 2012/08/16
	//if (!mFile) openFile();
	aos_assert_r(mFile, false);

	bool rslt = newobj->modifyContentsToFile(mFile, offset, mBucketSize, rdata);
	aos_assert_r(rslt, false);
	if (!oldobj->getPrev() || !oldobj->getNext()) return true;
	newobj->replaceNode(oldobj);
	return true;
}


bool 
AosStrObjHash::erase(const OmnString &key, const AosRundataPtr &rdata)
{
	mLock->lock();
	AosStrObjHashSanityCheckLocked;
	MapItr_t itr = mMap.find(key);
	if (itr == mMap.end())
	{
		// Not found
		if (mIsPersis)
		{
			AosHashedObjPtr node;
			bool readRslt = readObjPriv(key, node, rdata);
			if (!readRslt)
			{
				AosStrObjHashSanityCheckLocked;
				mLock->unlock();
				return true;
			}
			aos_assert_rl(node, mLock, false);
			bool deleteRslt = deleteObjPriv(key, node, rdata);
			aos_assert_rl(deleteRslt, mLock, false);
			AosStrObjHashSanityCheckLocked;
			mLock->unlock();
			return true;
		}
	}
	
	AosHashedObjPtr node = itr->second;
	if (mNumNodes == 1)
	{
		// There is only one node. 
		aos_assert_rl(mHead == node, mLock, false);
		mHead = 0;
		mNumNodes = 0;
		mMap.erase(key);
		node->resetLinks();
		if (mIsPersis)
		{
			bool rslt = deleteObjPriv(key, node, rdata);
			aos_assert_rl(rslt, mLock, false);
		}
		AosStrObjHashSanityCheckLocked;
		mLock->unlock();
		return true;
	}

	if (mHead == node)
	{
		// It is to remove the head, pointing the head to the next one
		mHead = node->getNext();
		aos_assert_rl(mHead, mLock, false);
		aos_assert_rl(mHead != node, mLock, false);
	}

	// Remove the node from the list
	node->removeFromLinkedList();
	mMap.erase(key);
	mNumNodes--;
	if (mIsPersis)
	{
		bool rslt = deleteObjPriv(key, node, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	AosStrObjHashSanityCheckLocked;
	mLock->unlock();
	return true;
}


bool
AosStrObjHash::deleteObjPriv(
		const OmnString &key,
		const AosHashedObjPtr &data,
		const AosRundataPtr &rdata)
{
	
	aos_assert_r(key != "", false);
	aos_assert_r(data, false);
	u64 offset = getOffset(key.data(), key.length());
	// Ketty 2012/08/16
	//if (!mFile) openFile();
	aos_assert_r(mFile, false);

	bool rslt = data->deleteObj(mFile, offset, key, mBucketSize, rdata);
if (!rslt)
	rslt = data->deleteObj(mFile, offset, key, mBucketSize, rdata);

	aos_assert_r(rslt, false);

	return true;
}


bool
AosStrObjHash::modifyDataPublic(
		const OmnString &key, 
		const AosHashedObjPtr &obj,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	AosStrObjHashSanityCheckLocked
	aos_assert_rl(key != "", mLock, false);
	aos_assert_rl(key == obj->getKeyStr(), mLock, false);
	bool rslt = false;

	if (mIsPersis)
	{
		rslt = deleteObjPriv(key, obj, rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	MapItr_t itr = mMap.find(key);
	if (itr != mMap.end())
	{
		AosHashedObjPtr cacheNode = itr->second;
		mMap.erase(key);
		if (mNumNodes == 1)
		{
			aos_assert_rl(mHead == cacheNode, mLock, false);
			mHead = 0;
			mNumNodes = 0;
			cacheNode->resetLinks();
		}
		else
		{
			if (mHead == cacheNode)
			{
				mHead = cacheNode->getNext();
				aos_assert_rl(mHead, mLock, false);
	        	aos_assert_rl(mHead != cacheNode, mLock, false);
			}
			cacheNode->removeFromLinkedList();
			mNumNodes--;
		}
	}

	if (mIsPersis)
	{
		rslt = addObjPriv(key, obj, rdata);
		aos_assert_rl(rslt, mLock, false);
		obj->setExist(true);
	}
	addNode(obj);
	mMap[key] = obj;
	AosStrObjHashSanityCheckLocked;	
	mLock->unlock();
	return true;
}


bool
AosStrObjHash::sanityCheck()
{
	aos_assert_r(mNumNodes >= 0, false);
	if (mNumNodes == 0)
	{
		aos_assert_r(!mHead, false);
		return true;
	}

	AosHashedObjPtr crt = mHead;
	for (u32 i=0; i<mNumNodes; i++)
	{
		if (i == mNumNodes-1)
		{
			aos_assert_r(crt->getNext() == mHead, false);
			continue;
		}
		aos_assert_r(crt->getNext() != crt, false);
		aos_assert_r(crt->getNext() != mHead, false);
		crt = crt->getNext();
	}

	crt = mHead;
	for (u32 i=0; i<mNumNodes; i++)
	{
		if (i == mNumNodes-1)
		{
			aos_assert_r(crt->getPrev() == mHead, false);
			continue;
		}
		aos_assert_r(crt->getPrev() != crt, false);
		aos_assert_r(crt->getPrev() != mHead, false);
		crt = crt->getPrev();
	}

	return true;
}


// Ketty 2012/08/16
/*
bool
AosStrObjHash::openFile()
{
	if (mFile && mFile->isGood())
	{
		return true;
	}
	aos_assert_r(mFilename != "", false);
	mFile = OmnNew OmnFile(mFilename, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!mFile->isGood())
	{
		mFile = OmnNew OmnFile(mFilename, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(mFile->isGood(), false);
	}
	if (mReset)
	{
		return resetFile();	
	}

	// Chen Ding, 08/14/2012
	u64 filesize = mFile->readBinaryU64(AosStrObjHash::eFileSizePos, 0);
	if (filesize == 0)
	{
		filesize = (mKeyBitmap + 1) * mBucketSize + eBucketStart;
		mFile->setU64(AosStrObjHash::eFileSizePos, filesize, true);
	}
	return true;
}
*/

bool
AosStrObjHash::resetFile(const AosRundataPtr &rdata)
{
	// Ketty 2012/08/16
	//if (!mFile || !mFile->isGood()) openFile();
	aos_assert_r(mFile && mFile->isGood(), false);
	// char mem[10];
	// AosBuff buff(mem, 10, 0, 0 AosMemoryCheckerArgs);
	// buff.setU32(0);
	// u64 pos = eBucketStart;

	bool rslt = OmnHashUtil::verifyBitmask(mKeyBitmap);
	aos_assert_r(rslt, false);
	mFile->writelock();
	// for(u32 i = 0; i <= mKeyBitmap; i++)
	// {
	// 	mFile->put(pos, mem, 10, false);
	// 	pos += mBucketSize;
	// }
	u64 fileSize = (mKeyBitmap + 1) * mBucketSize + eBucketStart;
	//AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	//rdata->setSiteid(AOS_SYS_SITEID);
	mFile->setU64(eFileSizePos, fileSize, true, rdata.getPtr());

	mFile->unlock();
	return true;
}


bool
AosStrObjHash::setPrintFlag(const bool flag)
{
	sgPrintFlag = flag;
	return true;
}
