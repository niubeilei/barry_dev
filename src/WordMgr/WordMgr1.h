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
#ifndef AOS_WordMgr1_WordMgr1_h
#define AOS_WordMgr1_WordMgr1_h

#include "FmtMgr/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEInterfaces/WordMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/File.h"
#include "SEUtil/SeTypes.h" 
#include <map>
using namespace std;

OmnDefineSingletonClass(AosWordMgr1Singleton,
						AosWordMgr1,
						AosWordMgr1Self,
						OmnSingletonObjId::eWordMgr1,
						"WordMgr1");

class AosWordMgr1 : virtual public OmnThreadedObj,
					virtual public AosWordMgrObj
{
	OmnDefineRCObject;

	struct Record
	{
		OmnString 	word;
		OmnString	wordId;
		Record *	next;
	};

public:
	enum
	{
		eMarkSize = 10000
	};

private:
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	// OmnThreadPtr	mThread;
	int				mNumMarked;
	u32				mBucketMarks[eMarkSize];
	OmnFilePtr		mWordIdFile;
	OmnFilePtr		mIndexFile;
	u32				mWordIdFileSize;
	OmnString		mWordIdName;
	u32				mNumBuckets;
	bool			mStarted;
	map<u32, AosWordIdHashPtr> mWordHash;
	u32				mTableSize;
	bool			mIsStopping;

public:
	AosWordMgr1();
	~AosWordMgr1();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	u32		getTotalWords(const u32 vid);
	u32		getWordHashSize(const u32 vid);
	u32		getMaxConflictNum(const u32 vid);
	u32		getMaxNumElems(const u32 vid);
	u32		getMaxBucketLen(const u32 vid);

	int		getTablesize() const {return mTableSize;}
	u64		addWord(const AosRundataPtr &rdata, const char *word, const int len, const u64 id);
	u64		getWordId(const OmnString &word, const bool addFlag);
	u64		getWordId(const u8 *word, const bool addFlag)
			{return getWordId(word, strlen((char *)word), addFlag);}
	u64		getWordId(const u8 *word, 
				const int len, 
				const bool addFlag = true);
	char *	getWord(const u64 &wordId, u32 &len);
	bool 	updateToFile(const u32 bktkey);
	bool	markBucket(const u64 wordId);
	bool 	updateIndexRecord(const u32 bktkey);
	bool 	appendIndexRecord(const u32 bktkey);
	bool 	writeBucketToFile(
				char *bucket, 
				const u16 size, 
				const u16 numElems, 
				const u32 bktkey,
				const bool appendFlag);
	bool	getId(const u64 &wordId, u64 &id);
	bool	setId(const u64 &wordId, const u64 id);
	bool	saveId(const u64 &wordId, const u64 id);
	bool	verifyDb(const int bktid);
	//bool	createHashtable();		// Ketty 2013/03/12

	// Ketty 2012/08/08
	int		getVirtualIdByWordId(const u64 wordid);

	// Ketty 2012/11/29
	AosWordIdHashPtr getWordHash(const int virtual_id);

    // Singleton class interface
    static AosWordMgr1 *    	getSelf();
    virtual bool            start();
    virtual bool            stop();
    virtual bool			config(const AosXmlTagPtr &def);

	u64 	recoverWord(
				const char *word,
				const int wordlen);

	// Chen Ding, 11/21/2010
	bool addWord(
		const AosRundataPtr &rdata,
		const char *word, 
		const int len, 
		const u64 &wordid,
		const u64 id);

private:
	bool 	readIndexFile();
	bool	initTablesize();
	AosFmtPtr createFmt();
	void	sendFmt(const AosRundataPtr &rdata, const AosFmtPtr &fmt);
};

#endif
