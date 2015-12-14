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
#ifndef AOS_DocClient_Docid_h
#define AOS_DocClient_Docid_h

#include "IdGen/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeTypes.h"

class OmnDocid : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eNumDocids = 100,
		eMaxVirtual = 1000000
	};

private:
	OmnMutexPtr			mLock;
	vector<u64>			mCrtDocids;
	vector<int>			mNumDocids;
	u64					mInitDocid;
	OmnString 			mDocidKey;
	int 				mNumVirtuals;
	int 				mCrtVirNum;
	u32 				mDocidNums;

public:
	OmnDocid(const u64 &init_did, const u32 nums, const OmnString &key);
	~OmnDocid();

	u64	nextDocid(const int &vid, const AosRundataPtr &rdata);
	void 	returnDocid(const u64 &docid, const AosRundataPtr &rdata);

private:
	//bool	verifyDocid(const u64 &docid, const AosRundataPtr &rdata);
	bool	retrieveDocids(const int &vid, const AosRundataPtr &rdata);
};
#endif
