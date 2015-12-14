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
// 2014/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StIILCache_h
#define AosSengTorUtil_StIILCache_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorUtil/Ptrs.h"
#include "SengTorUtil/StStrIIL.h"
#include "SengTorUtil/StU64IIL.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "Util/HashMap.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "Thrift/AosJimoAPI.h"


class AosStIILCache : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftMinIILNameLen = 5,
		eDftMaxIILNameLen = 50
	};

	typedef hash_map<const OmnString, AosStIILPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosStIILPtr, Omn_Str_hash, compare_str>::iterator mapitr_t;
	typedef hash_map<const u64, AosStIILPtr, u64_hash, u64_cmp> umap_t;
	typedef hash_map<const u64, AosStIILPtr, u64_hash, u64_cmp>::iterator umapitr_t;

	OmnMutexPtr	mLock;
	map_t		mIILNameMap;
	umap_t		mIILIDMap;
	u32			mMinIILNameLen;
	u32			mMaxIILNameLen;
	vector<AosStIILPtr> mIILArray;

public:
	AosStIILCache();
	~AosStIILCache();

	AosStIILPtr pickIIL(
			shared_ptr<AosJimoAPIClient> &thriftClient,
			JmoCallData &call_data);

	bool createIIL(
			shared_ptr<AosJimoAPIClient> &thriftClient,
			JmoCallData &call_data);

	i64 getNumIILs() {return mIILNameMap.size() + mIILIDMap.size();}
	AosStIILPtr getIILByIILName(OmnString &iilname);
	AosStIILPtr getIILByIILID(u64 &iilid);
	bool deleteIIL(OmnString &iilname);

private:
};
#endif

