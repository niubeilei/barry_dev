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
//
// Modification History:
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatServer_StatKeyFile_h
#define Aos_StatServer_StatKeyFile_h

#include "Vector2D/Ptrs.h" 
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/RlbFileType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/BuffArrayVar.h"


class AosStatKeyFile : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eIndexStartPos = 100,
		eStartSdocidPos = eIndexStartPos,
		eNumKeysPos = eIndexStartPos + sizeof(u64),
		eIndexArrayPos = eNumKeysPos + sizeof(u32),

		//eCacheSize = 10000
		eCacheKeyNum = 1000,
		eCacheBytes = 10000,
	};

	typedef hash_map<const u64, AosBuffPtr, u64_hash, u32_cmp> map1_t;
	typedef hash_map<const u64, AosBuffPtr, u64_hash, u32_cmp>::iterator itr1_t;
	typedef hash_map<const u64, i64 *, u64_hash, u32_cmp> map2_t;
	typedef hash_map<const u64, i64 *, u64_hash, u32_cmp>::iterator itr2_t;

	bool		mIsGood;
	u64			mCubeId;
	u64			mFileId;
	i64			mFileSize;
	u64			mStartSdocid;
	//i64			mCrtIndexPos;	// It points to the first unused entry
	u32			mNumKeys;
	u32			mNumSdocidsPerFile;
	//u32			mCacheSize;
	//i64			mCachedStartSdocid;	
	bool		mIsDirty;
	AosBuffPtr	mIndexBuff;
	u32 *		mIndexArray;

	u64                 mReadTime1;
	u64                 mReadNum1;

	u64                 mReadTime2;
	u64                 mReadNum2;

	u64                 mReadTime3;   
	u64                 mReadNum3;

	//AosBuffArrayVarPtr	mCacheBuff;
	//AosBuffArrayVar	*	mCacheBuffRaw;
	AosBuffPtr	mCacheBuff;
	AosBuff*	mCacheBuffRaw;
	u64			mCachedStartOff;
	//i64			mCachedStartSdocid;
	//u32		mCachedKeyNum;
	u32			mCachedBytes;

	AosRlbFileType::E mFileType;
	AosReliableFile *  mReliableFileRaw;
	AosReliableFilePtr mReliableFile;

public:
	AosStatKeyFile(AosRundata *rdata, 
				const u64 cube_id,
				const u64 file_id, 
	            const u64 start_sdocid,
				const u32 num_rows_per_file, 
				const AosRlbFileType::E file_type);
	
	//yang
	AosStatKeyFile(){ mCachedBytes = eCacheBytes; };

	~AosStatKeyFile();

	inline bool isGood() const {return mIsGood;}
	inline u64 getFileId() const {return mFileId;}

	virtual bool readKey(AosRundata *rdata,
						const u64 sdocid, 
						OmnString &key);

	virtual bool readKeys(AosRundata *rdata,
						const u64 sdocid); 
						//AosBuffArrayVar *buff);

	virtual bool appendKey(AosRundata *rdata,
						const u64 start_sdocid,
						const OmnString &key);

	virtual bool appendKeys(AosRundata *rdata,
						const u64 start_sdocid,
						const int num_keys,
						AosBuff *keys);

	void outputCounters();
	void initCounters();

private:
	bool readMetadata(AosRundata *rdata);
	bool isInCache(const u64 sdocid) const;
	bool init(AosRundata *rdata);
	bool initializeVectorKeyFile(AosRundata *rdata);

	bool readFromCache(AosRundata *rdata, 
						const u64 sdocid,
						OmnString &key, 
						int &len);

};
#endif


