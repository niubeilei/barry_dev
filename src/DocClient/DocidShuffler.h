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
// 09/20/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocClient_DocidShuffler_h
#define AOS_DocClient_DocidShuffler_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocClient/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Opr.h"

class AosDocidShuffler : virtual public OmnRCObject 
{
	OmnDefineRCObject;
	
	enum
	{
		eDftNumThreads = 1
	};

public:
	class ShuffleRunner : public OmnThrdShellProc
	{
		  OmnDefineRCObject;

	private:
		AosDocidShufflerPtr				mShuffler;
		AosBitmapObjPtr			mBitmap;
		OmnString						mScannerId;
		int64_t							mStartIdx;
		int64_t							mPsize;
		AosDocClientCallerPtr			mCaller;
		int								mNumPhysicals;
		AosRundataPtr 					mRundata;

		vector<AosBitmapObjPtr>	mBitmaps;

	public:
		ShuffleRunner(
			const AosDocidShufflerPtr &shuffle,
			const AosBitmapObjPtr &bitmap,
			const OmnString &scanner_id,
			const int64_t &start_idx,
			const int64_t &psize,
			const AosDocClientCallerPtr &caller,
			const int num_physicals,
			const AosRundataPtr &rdata);

		~ShuffleRunner();

		virtual bool    run();
		virtual bool    procFinished();
	};

private:
	OmnString		mScannerId;
	int				mNumPhysicals;
	vector<int>		mTotalNumDataMsgs;	
	OmnMutexPtr     mLock;

public:
	AosDocidShuffler(const OmnString &scannerid, const AosRundataPtr &rdata);

	~AosDocidShuffler();

	bool	sendStart(
			const u32 &queue_size,
			const OmnString &read_policy,
			const u64 &block_size,
			const OmnString &batch_type,
			const AosXmlTagPtr &field_names,
			const AosRundataPtr &rdata);
	
	bool	sendFinished(
			const AosRundataPtr &rdata);

	bool	shuffle(
			const AosBitmapObjPtr &bitmap,
			const int num_thrds,
			const AosDocClientCallerPtr &caller,
			const AosRundataPtr &rdata);

	void 	updateTotalMsgs(const int serverid, const int vv);
};
#endif

