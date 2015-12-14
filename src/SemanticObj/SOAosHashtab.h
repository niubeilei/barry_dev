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
// 12/10/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticObj_SOAosHashtab_h
#define Aos_SemanticObj_SOAosHashtab_h

#include "SemanticObj/Ptrs.h"
#include "SemanticObj/ItfHashtab.h"
#include "Thread/Ptrs.h"

struct aos_hashtable;

class AosHashtabSO : public AosHashtabInterface 
{
private:
	struct aos_hashtable 	*mTable;
	OmnMutexPtr	mLock;

	u16		   *mBucketLens;
	u16			mTableSize;

	u32			mNumQueries;
	u32			mQueryTime;
	u32			mMaxQueryTime;

	u32			mNumAdds;
	u32			mAddTime;
	u32			mMaxAddTime;

	u32			mNumDeletes;
	u32			mDeleteTime;
	u32			mMaxDeleteTime;

public:
	AosHashtabSO(struct aos_hashtable *);
	~AosHashtabSO();

	// 
	// Mandatory Interface
	//
	virtual u32 	getNoe() const;
	virtual u32		getTableSize() const;

	// 
	// Optional Interface
	//
	virtual u32		maxAllowedElem() const;
	virtual u32		averageBucketLen() const;
	virtual u32		bucketUsagePct() const;
	virtual u32		maxBucketLen() const;
	virtual u32		averageQueryTime() const;
	virtual u32 	maxQueryTime() const;
	virtual u32		averageAddTime() const;
	virtual u32		maxAddTime() const;
	virtual u32		averageDeleteTime() const;
	virtual u32		maxDeleteTime() const;
	virtual u32		totalQueries() const;
	virtual u32		totalAdditions() const;
	virtual u32		totalDeletions() const;
};

#endif

