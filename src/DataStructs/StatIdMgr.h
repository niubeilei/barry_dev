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
#ifndef AOS_DataStructs_StatIdMgr_h
#define AOS_DataStructs_StatIdMgr_h

#include "DataStructs/Ptrs.h"
#include "Util/TransId.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"

class AosStatIdMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eDocidOff = sizeof(int64_t),
		eDocidLen = sizeof(u64)
	};

private:
	AosBuffPtr				mBuff;
	u64 *					mDistBlockEntries;
	u32						mNumDistBlockEntries;
	u64						mDocsPerDistBlock;

public:
	AosStatIdMgr(const AosBuffPtr &buff, const u64 &docs_per_distblock);
	~AosStatIdMgr();

	bool createOneEmptyDocid();
	
	u32 getNumDistBlockEntries() {return mNumDistBlockEntries;}

	AosBuffPtr getCtrlBuff(){return mBuff;}

	u64 getEntry(const u32 &idx);

	bool addEntryByStatId(const u64 &stat_id, const u64 &binary_docid);

	bool addEntryByStatId(
			vector<u64> &vt_statid,
			vector<u64> &vt_binary_docid);

	bool isCheck(const u32 &idx);
	void reset(const AosBuffPtr &buff);

	u64 getDocsPerDistBlock(){return mDocsPerDistBlock;}
	u64 getEntryByStatId(const u64 &stat_id);

private:
	bool addEntry(const u32 &idx, const u64 &binary_docid);
};
#endif
