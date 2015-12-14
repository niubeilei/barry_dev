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
#ifndef AOS_UtilHash_StrObjHash_h
#define AOS_UtilHash_StrObjHash_h

#include "ErrorMgr/ErrmsgId.h"
#include "ReliableFile/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "Util/UtUtil.h"
#include "Util/File.h"
#include "UtilHash/HashedObj.h"
#include "UtilHash/Ptrs.h"
#include <ext/hash_map>
using namespace std;

//#define AosStrObjHashSanityCheck aos_assert_r(sanityCheck(), false);
//#define AosStrObjHashSanityCheckLocked aos_assert_rl(sanityCheck(), mLock, false);
#define AosStrObjHashSanityCheck ;
#define AosStrObjHashSanityCheckLocked ;

class AosStrObjHash : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxMapSize = 3,
		eBucketStart = 100,
		eFileSizePos = 10,

		eDefaultKeyBitmap = 0xfffff
	};

private:
	typedef hash_map<const OmnString, AosHashedObjPtr, Omn_Str_hash, compare_str> Map_t;
	typedef hash_map<const OmnString, AosHashedObjPtr, Omn_Str_hash, compare_str>::iterator MapItr_t;

	OmnMutexPtr		mLock;
	Map_t			mMap;
	AosHashedObjPtr	mHead;
	AosHashedObjPtr	mDftObj;
	AosHashedObjPtr	mDataReader;
	u32				mNumNodes;
	//OmnFilePtr		mFile;
	AosReliableFilePtr	mFile;		// Ketty 2012/08/16
	u32				mMapMaxSize;
	int				mBucketSize;
	int				mBucketIdBits;
	u64				mKeyBitmap;
	OmnString		mFilename;
	bool			mReset;
	bool			mIsPersis;

public:
	AosStrObjHash();

	AosStrObjHash(
			const AosHashedObjPtr &dft_obj,
			const u32 mapsize,
			AosErrmsgId::E &errid,
			OmnString &errmsg,
			const AosRundataPtr &rdata);
	
	AosStrObjHash(
			const AosHashedObjPtr &dft_obj,
			//const OmnString &fname,		// Ketty 2012/08/16 
			const AosReliableFilePtr &file, 
			const u32 mapsize,
			const u64 &key_bitmap, 
			const int bucket_size, 
			const bool reset,
			AosErrmsgId::E &errid,
			OmnString &errmsg,
			const AosRundataPtr &rdata);

	~AosStrObjHash();

	u32		getNumNodes() const {return mMap.size();};
	// Brian Zhang 21/09/2011 Test for selogmgr will delete after
	Map_t getMap() const {return mMap;}
	
	AosHashedObjPtr find(const OmnString &key, const AosRundataPtr &rdata);
	bool erase(const OmnString &key, const AosRundataPtr &rdata);
	bool addDataPublic(const OmnString &key, const AosHashedObjPtr &obj, const bool override, const AosRundataPtr &rdata);
	bool modifyDataPublic(const OmnString &key, const AosHashedObjPtr &obj, const AosRundataPtr &rdata);

	AosHashedObjPtr getHead() const {return mHead;}
	static bool setPrintFlag(const bool b);

private:
	bool moveNode(const AosHashedObjPtr &node)
	{
		AosStrObjHashSanityCheck;
		if (mHead == node) return true;
		aos_assert_r(mNumNodes > 1, false);

		node->removeFromLinkedList();
		node->insertToLinkedList(mHead);
		mHead = node;
		AosStrObjHashSanityCheck;
		return true;
	}

	bool addNode(const AosHashedObjPtr &node)
	{
		// It adds the node 'node' to the front of the list. 
		AosStrObjHashSanityCheck;
		aos_assert_r(node, false);
		if(mNumNodes == 0)
		{
			// This is the first node to add:
			aos_assert_r(!mHead, false);
			mHead = node;
			node->setLinksToSelf();
			mNumNodes = 1;
		}
		else if (mNumNodes < mMapMaxSize)
		{
			// Insert it to the front:
			node->insertToLinkedList(mHead);
			mHead = node;
			mNumNodes++;
		}
		else
		{
			// It needs to replace the last with 'node'.
			AosHashedObjPtr lastone = mHead->getPrev();
			aos_assert_r(lastone, false);
			mMap.erase(lastone->getKeyStr());
			node->replaceNode(lastone);
			mHead = node;
		}

		AosStrObjHashSanityCheck;
		return true;
	}

	bool sanityCheck();

	inline u64 getHashkey(const char *word, const int len)
	{
		u64 key = AosStrHashFunc(word, len);
		return key & mKeyBitmap;
	}

	inline u64 getOffset(const char *word, const int len)
	{
		u64 key = AosStrHashFunc(word, len);
		return (key & mKeyBitmap) * mBucketSize + eBucketStart;
	}


private:
	bool init(AosErrmsgId::E &errid, OmnString &errmsg, const AosRundataPtr &rdata);
	bool openFile();
	bool resetFile(const AosRundataPtr &rdata);
	bool addPersisDataPriv(
			const OmnString &key, 
			const AosHashedObjPtr &obj, 
			const bool override,
			const AosRundataPtr &rdata);

	bool addCacheDataPriv(
			const OmnString &key, 
			const AosHashedObjPtr &obj, 
			const bool override);

	bool modifyEntryPriv(
			const OmnString &key, 
			const AosHashedObjPtr &oldobj, 
			const AosHashedObjPtr &newobj,
			const AosRundataPtr &rdata);
	bool addObjPriv(const OmnString &key, const AosHashedObjPtr &data, const AosRundataPtr &rdata);
	bool readObjPriv(
			const OmnString &key, 
			AosHashedObjPtr &node,
			const AosRundataPtr &rdata);
	bool deleteObjPriv(const OmnString &key, const AosHashedObjPtr &data, const AosRundataPtr &rdata);
	bool isValidHashBitmap(const u32 &bitmap) ;
};
#endif
