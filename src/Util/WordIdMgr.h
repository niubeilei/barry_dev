////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2015/01/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_WordIdMgr_h
#define AOS_Util_WordIdMgr_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "Util/String.h"

class AosRundata;

class AosWordIdMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, u32, Omn_Str_hash, compare_str> map_t;
	typedef map_t::iterator itr_t;

	OmnMutex *	mLock;
	OmnString	mObjid;
	u64			mDocid;
	map_t		mMap;

public:
	AosWordIdMgr(AosRundata *rdata, const OmnString &objid);
	AosWordIdMgr(AosRundata *rdata, const u64 objid);
	~AosWordIdMgr();

	u32 getWordId(AosRundata *rdata, 
					const OmnString &word, 
					const bool create_flag);

private:
	bool loadMapByObjid(AosRundata *rdata);
	bool loadMapByDocid(AosRundata *rdata);
};
#endif
