////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/27/2012 Created by Ice Yu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Sorter_MergeFileSorter_h
#define Aos_Sorter_MergeFileSorter_h


#include "Rundata/Ptrs.h"
#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/NetFileObj.h"
#include "Sorter/DataSource.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <queue>

class AosMergeFileSorter : public  OmnRCObject
{
public:
	AosCompareFun					*mCmpRaw;

private:
	OmnDefineRCObject;	

	enum {
		eDefBuffSize = 10000000
	};

	vector<OmnSPtr<DataSource> > 	mDataSources;
	vector<DataSource*> 			mDataSourcesRaw;
	u32								mDataSourceNum;
	vector<int> 					mLosterTree;
	OmnCondVarPtr					mCondVar;
	OmnMutexPtr						mBuffQueueLock;						
	queue<AosBuffPtr> 				mBuffQueue;
	bool 							mFinished;
	bool							mNeedMerge;

	int								mProcTotal;
	int 							mMergeTotal;
	u64								mTime;		
	AosBuffPtr						mTmpBuff;


public:

	AosMergeFileSorter(const bool regflag);
	AosMergeFileSorter(
			int buffsize,
			AosCompareFun * cmp,
			vector<AosNetFileObjPtr> &files,
			const AosRundataPtr &rdata);
	~AosMergeFileSorter();

	virtual bool sort();

	static bool sanitycheck_var(
			AosCompareFun * cmp,
			char *pos, 
			const int length);

 	static bool sanitycheck(
			AosCompareFun * cmp,
			char *pos, 
			const int length);

	static bool sanitycheck(
			const AosBuffPtr &tmpbuff,
			const AosBuffPtr &buff,
			const AosMergeFileSorterPtr &sort);

	virtual void callBack(const u64&, const int64_t&, const bool&){return ;}
	bool 	nextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	AosBuffPtr nextBuff();

private:
	bool buildLosterTree();

};
#endif

