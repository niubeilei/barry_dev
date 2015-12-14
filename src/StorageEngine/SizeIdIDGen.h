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
//
// Modification History:
// 03/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_SizeIdIDGen_h
#define Aos_StorageEngine_SizeIdIDGen_h

#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosSizeIdIDGen : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eInitSizeId = 10000,
		eIncSizeId = 1,
		eInitDocids = 10000,
		eIncDocids = 10,
		eDocTypeOffsetSize = 56,
		eSizeidOffsetSize  = 28,
		eMaxDocid   = 0xffffffffffffffULL,
		eDocidsMask = 0xfffffff,
		eSizeIdMask = 0xfffffff
	};

	struct DocidAllocator
	{
		u64	mSizeid;
		u64	mRecordDocid;
		u64 mCrtDocid;
		int mNumDocids;
	};

	OmnMutexPtr     	mLock;

	u32 				mSiteid;
	u64 				mCrtSizeid;
	int					mSizeidNum;

	map<u64, DocidAllocator>	mDocidAllocator;

public:
	AosSizeIdIDGen(const u32 siteid);
	~AosSizeIdIDGen();
	
	static bool			config(const AosXmlTagPtr &def);
	
	static inline u64	getSizeIdByDocid(const u64 &docid)
	{
		return (docid >> eSizeidOffsetSize) & eSizeIdMask; 
	}
	static inline u64	getLocalIdByDocid(const u64 &docid)
	{
		u64 id = docid << 8;
		id = id >> 8;
		return id; 
	}

	u64					createSizeId(
							const u64 &record_docid,
							const AosRundataPtr &rdata);
	u64 				getCrtSizeId(
							const u64 &record_docid,
							const AosRundataPtr &rdata);
	u64					getNextDocid(
							const u64 &record_docid,
							const AosRundataPtr &rdata);

	bool				getDocids(
							const u64 &sizeid,
							u64 &start_docid,
							int &num_docids,
							bool &overflow,
							const AosRundataPtr &rdata);

private:
	u64 				getCrtSizeIdPriv(
							const u64 &record_docid,
							const AosRundataPtr &rdata);
	bool				getDocidsPriv(
							const u64 &sizeid,
							u64 &start_docid,
							int &num_docids,
							bool &overflow,
							const AosRundataPtr &rdata);
	u64					getNextSizeId(
							const u64 &record_docid,
							const AosRundataPtr &rdata);
	
	u64					getNewSizeIds(const AosRundataPtr &rdata);

};

#endif

