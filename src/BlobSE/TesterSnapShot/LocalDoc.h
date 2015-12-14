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
// 2015/04/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_Tester_LocalDoc_h
#define Aos_BlobSE_Tester_LocalDoc_h 

#include "Util/Ptrs.h"
#include "Util/String.h"

class AosLocalDoc
{
private:
	u64		mDocid;
	bool	mDeletedFlag;
	u32		mSeedId;
	u32		mRepeat;

public:
	AosLocalDoc(const u64 docid); 
	AosLocalDoc(
			const u64 docid,
			const bool delete_flag,
			const u32 seed_id,
			const u32 repeat);
	~AosLocalDoc();

	void 	resetData();
	void 	markDeleted();
	u32 	getDocSize();
	OmnString getPattern();
	AosBuffPtr getData();
	bool 	isSameData(const AosBuffPtr &buff);

	u64		getDocid(){ return mDocid; }
	bool	isDeleted(){ return mDeletedFlag; };
	u32		getSeedId(){ return mSeedId; };
	u32		getRepeat(){ return mRepeat; };

};

#endif
