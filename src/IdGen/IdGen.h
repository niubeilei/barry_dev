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
// 10/04/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IdGen_IdGen_h
#define AOS_IdGen_IdGen_h

#include "IdGen/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/SyncServerReqCb.h"


class AosIdGen : virtual public AosSyncServerReqCb
{
	OmnDefineRCObject;

public:
	enum
	{
		eBlocksize = 100000,
		eMaxRangeIdx = 30,
		eMaxIdValue = 256,
		eMaxRanges = 64,
		eFlagSize = 30,
		eSizeOffset = 50,
		eDftBlocksize = 5000000,
	};

private:
	int				mNumRanges;
	int 			mCrtLen;
	int				mRangeMin[eMaxRanges];	
	int				mRangeMax[eMaxRanges];	
	int				mCrtRangeIdx[eMaxRangeIdx];
	int				mBase;
	AosIdGenDefPtr	mDef;
	OmnString		mCrtId;
	int				mBlocksize;
	int				mMaxLen;

public:
	AosIdGen(const OmnString &idname);
	~AosIdGen();

	// SynObj interface
	virtual bool    syncServerCb(const AosSyncServerReqType, void *data);

	static u64 			strToU64(const OmnString &id, const u64 dft);
	static OmnString	u64ToStr(const u64 &docid);
	bool		getNextId(OmnString &word, const bool flag = true);
	OmnString	getNextId();
	u64			getNextU64(const u64 &dft);
	bool		isValidId(const OmnString &id);
	OmnString 	getNextIdBlock(const int num);
	bool		setBlock(const OmnString &start, const int num);
	bool 		loadBlock();
	bool		setCrtId(const OmnString &id);
	u64			convertDocIdToU64(const u64 &docid, const u64 &dft);

private:
	bool		start(const OmnString &name);
};

#endif
