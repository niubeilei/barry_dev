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
#if 0
#ifndef AOS_DataStructs_StatIdExtIDGen_h
#define AOS_DataStructs_StatIdExtIDGen_h

#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Thread/Mutex.h"
#include "DataStructs/Ptrs.h"

class AosStatIdExtIDGen : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	class AosExtStatIdMgr
	{
	private:
		int 		mDocsPerDistBlock;
		u64 		mCtrlDocid;
		int64_t		mCrtIdx;
		
	public:
		AosExtStatIdMgr(const AosXmlTagPtr &control_doc);
		~AosExtStatIdMgr();

		bool appendStatId(
			const AosRundataPtr &rdata,
			const int vid,
			const u64 &stat_id);

		int	getDocsPerDistBlock(){return mDocsPerDistBlock;}

	private:
		u64  getDocid(
			const AosRundataPtr &rdata, 
			const u64 &stat_id);

		bool addDocid(
			const AosRundataPtr &rdata, 
			const u64 &stat_id, 
			const u64 &binary_docid);
	};
private:
	OmnMutexPtr         mLock;
	OmnString			mKey;
	vector<u64>			mCrtDocids;
	vector<int>			mNumDocids;
	u32 				mDocidNums;
	int 				mNumVirtuals;
	vector<u64>			mGroupids;
	vector<u64>			mSanityCheck;
	AosExtStatIdMgr *	mExtStatIdMgr;
	int 				mDocsPerDistBlock;

public:
	AosStatIdExtIDGen(
			const AosXmlTagPtr &control_doc,
			const u32 nums);

	~AosStatIdExtIDGen();

	u64		nextDocid(const int vid, const AosRundataPtr &rdata);

	OmnString 	getIILKey(){return mKey;}

private:
	bool	retrieveDocids(const int vid, const AosRundataPtr &rdata);

	bool 	sanityCheck(const int vid, const u64 &docid);

	u64		getGroupId(const u64 &docid);

	void 	print(
				const AosRundataPtr &rdata,
				const int vid,
				const OmnString &key,
				const u32 incValue,
				const u32 initDocid);
};
#endif
#endif
