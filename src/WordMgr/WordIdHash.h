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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_WordMgr_WordIdHash_h
#define AOS_WordMgr_WordIdHash_h

#include "Database/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h" 
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "SEUtil/SeTypes.h" 


class AosWordIdHash : public OmnRCObject
{
	OmnDefineRCObject;

	struct Element
	{
		char *		mWord;
		char *		mWordId;
		Element	*	mNext;
	};

	struct BktRecord
	{
		unsigned short	mLen;
		Element **		mElements;
	};

	struct Header
	{
		u32		bktid;
		u32		bktsize;
		u16		num_entries;
		u32		offset;
		u32		capacity;
	};

	enum
	{
		eFileSize = 1000000000		// 1G
	};

public:
	enum
	{
		eDataFileFlagOffset = 0,
		eDataFileFlagSize = 30,
		eDataFileSizeOffset = 50,
		eDataFileInitSize = 100,

		eIdxFileRcdSize = 20,
		eIdxFileInitSize = 100,
		eIdxFileFlagOffset = 0,
		eIdxFileFlagSize = 30,

		eIdSize = sizeof(u64),

		eMaxPtrSize = 16,
		eMaxBktSize = 0xffff,
		eMaxTableSizeBits = 20,
		eMaxTableSize = 0xfffff,
		eMaxConflictNum = 1024,
		eMaxBucketLen = 65000,
		eNumRcdsPerRead = 1000,
		eDftBlockSize = 1000,		// 1k
		eCapacityInc = 1000,		// 1k increments
		eInvalidWordId = 0,
		eLinearSearchSize = 10,
		eNoEMask = 0xfff,
		eWordIdLen = 5,
		eWordIdBitmap = 0x3fffffff,	// low 30 bits
		eMaxDataSize = 2000000000	// 2000M
	};

	enum PosType
	{
		ePtr,
		eWord
	};

private:
	OmnString		mHashName;
	OmnMutexPtr		mLock;
	u32				mVirtualId;
	bool			mHasVirtual;
	bool			mError;
	bool			mAddFlag;
	char **			mBuckets;
	u16 *			mNumElems;
	u32				mTableSize;
	u32				mWordBktIdx;
	u32				mWordEtrIdx;
	u32				mWordIdBktIdx;
	u32				mWordIdEtrIdx;
	int				mFileSize;
	//OmnFilePtr		mDataFile;
	//OmnFilePtr		mIdxFile;
	AosReliableFilePtr mDataFile;
	AosReliableFilePtr mIdxFile;
	int				mCrtDbReadIdx;
	//OmnString		mDirname;
	u32				mMaxConflictNum;
	u32				mMaxNumElems;
	u32				mMaxBucketLen;
	u32				mTotalWords;
	//static OmnFilePtr		smBackFile;			// Chen Ding, 2012/10/11
	
	AosRundataPtr	mRdata;		// Ketty 2012/11/01
	bool			mStopped;	// Ketty 2013/03/12

	OmnFilePtr		mBackFile;	// Ken Lee, 2014/02/10

public:
	AosWordIdHash(const u32 tablesize);
	AosWordIdHash(const u32 tablesize, const OmnString &name);
	AosWordIdHash(
		const u32 virtual_id,
		//const OmnString &dirname, 
		const u32 tablesize, 
		const OmnString &fname); 
	~AosWordIdHash();

	static bool config(const AosXmlTagPtr &def);

	bool 	init();
	u64		getWordId(const char *word, const bool addFlag)
			{
				return getWordId(word, strlen(word), addFlag);
			}
	
	//u64		addWord(const char *word, const int len, const u64 &ptr);
	u64		addWord(const char *word, const int len, const u64 id);
	u64		getWordId(const char *word, const int len, const bool addFlag);
	char *	getWord(const u64 &wordId, u32 &len);
	u32		getTotalWords() const {return mTotalWords;}
	u32		getTablesize() const {return mTableSize;}
	u32		getMaxConflictNum() const {return mMaxConflictNum;}
	u32		getMaxNumElems() const {return mMaxNumElems;}
	u32		getMaxBucketLen() const {return mMaxBucketLen;}
	u64 	addEntry(
				const char *word, 
				const int wordLen,
				const int idx, 
				const u32 hashkey);
	bool	isWordIdNew() const {return mAddFlag;}
	u32		getBucketKey(const char *wordId);
	u32		getBucketSize(const u32 bktkey);
	char *	getBucket(const u32 bktkey);
	u16		getNumElems(const u32 bktkey);
	bool	setId(const u64 &wordId, const u64 &id);
	bool	getId(const u64 &wordId, u64 &id);
	bool	resetBuckets();
	void	resetWordLoop(){mWordBktIdx=0; mWordEtrIdx=0;}
	char *	nextWord(int &len);
	void	resetWordIdLoop(){mWordIdBktIdx=0; mWordIdEtrIdx=0;}
	u64		nextWordId();
	//bool 	updateBktToFile(const u32 bktid);
	bool 	updateBktToFile(const u64 &wordid);
	bool 	updateBktToFileByBktid(const u32 bktid);
	//bool 	createHashtable();		// Ketty 2013/3/12
	bool 	createHashtableNew();
	bool	verifyDb(const int bktid);
	bool	saveId(const u64 &wordId, const u64 id);

	// Chen Ding, 11/21/2010
	bool addWord(
			const char *word, 
			const int wordLen,
			const u64 &wordid,
			const u64 id);

	// Ken Lee, 2014/02/10
	bool	rebuild();
	bool	recover();

	u64 	recoverWord(
				const char *word,
				const int wordlen);

	// Ketty 2013/03/12
	bool	start();
	bool 	stop();
	bool	isStopped(){ return mStopped; };

private:
	bool 	integrityCheck(const u32 bktid, const u64 wid);
	bool 	integrityCheck1(
				char *new_bucket, 
				char *old_bucket, 
				const u16 numElems, 
				const u32 newHashkey,
				const u16 conflictNum, 
				const char *word);
	bool 	integrityCheck2(
				const char *word, 
				const int wordLen, 
				const int idx,
				char *bucket, 
				const u32 bktId);

	u64 	getWordId(
				const char *word, 
				int &idx,
				const int wordLen, 
				const u32 hashkey);
	char *	getPtrPriv(const u64 &wordId, u32 &pos, u32 &, const PosType);
	char *	allocateBucket(const int size);
	bool	releaseBucket(const char *);
	bool 	updateHeader(const Header &header);
	u32 	getNewOffset();
	bool	readHeader(const int bktid, Header &header);
	bool	openDataFile();
	bool	openIdxFileNew(bool &isNewTable);
	bool	openIdxFile(bool &isNewTable);

	// Chen Ding, 11/21/2010
	bool addEntry(
			const char *word, 
			const int wordLen,
			const u32 newHashkey, 
			const u64 &wordid);
	int getHeaderSize() const;

	u64		getWordIdPriv(
				const char *word,
				const int len,
				const bool addFlag);
	bool	setIdPriv(
				const u64 &wordId,
				const u64 &id);
};

#endif
