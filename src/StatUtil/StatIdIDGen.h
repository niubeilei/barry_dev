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
// 2014/08/11 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StatUtil_StatIdIDGen_h
#define AOS_StatUtil_StatIdIDGen_h

#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Thread/Mutex.h"
#include "DataStructs/Ptrs.h"
//#include "DataStructs/StatIdIDGen.h"

class AosStatIdIDGen :  virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr         mLock;
	OmnString			mKey;
	vector<u64> 		mCrtDocids;
	vector<int>			mNumDocids;
	u32			 		mDocidNums;
	int 				mNumVirtuals;

public:
	AosStatIdIDGen(const OmnString &stat_definition_key, const u32 nums);
	~AosStatIdIDGen();

	u64		nextDocid(const AosRundataPtr &rdata, const u32 cube_id);
	void 	returnDocid(const u64 &docid, const AosRundataPtr &rdata);

private:
	bool	retrieveDocids(const AosRundataPtr &rdata, const u32 cube_id);
};
#endif
