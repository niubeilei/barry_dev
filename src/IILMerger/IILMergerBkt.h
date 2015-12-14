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
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMerger_IILMergerBkt_h
#define AOS_IILMerger_IILMergerBkt_h

#include "IILMerger/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/Ptrs.h"
#include "Util/BuffArray.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Porting/Sleep.h"

#include <list>
using namespace std;

class AosIILMergerBkt : virtual public OmnRCObject
{
	OmnDefineRCObject;

/*public:
	enum
	{
		eMaxEntries = 50000
	};
*/
private:
	u32							mMaxMemEntries;
	u32							mMaxEntries;
	AosIILMerger				*mMerger;
	char* 						mLow;
	char* 						mHigh;
	OmnFilePtr 					mFile;
	u32 						mTotalEntries;
	u32 						mTotalMemEntries;
	u32							mTimestamp;
	char*  						mData;
	AosCompareFunPtr 			mComp;
	AosIILMergerHandlerPtr      mHandler;
	OmnMutexPtr                 mLock;
	OmnMutexPtr                 mRegionLock;
	AosIILMergerPtr				mChildMerger;
	bool						mIsLocked;
	list<AosIILMergerBktPtr>::iterator  mBktItr;
	char* 	checkSort(
				char *begin, 
				const int length);

	bool	overFlow(
				const int num,
				char *begin,
				const AosRundataPtr &rdata);
	int		compare(const char* lhs,const char* rhs);
public:
	AosIILMergerBkt(
			AosIILMerger *merger, 
			const u32 maxmementries,
			const u32 maxentries);
	~AosIILMergerBkt();

	bool		 sanitycheck(char *begin, int length);

	AosIILMergerPtr  getChildMerger(){return mChildMerger;}
	char*		 getLow(){return mLow;}
	char* 		 getHigh(){return mHigh;}
	int 		 size(){return mTotalEntries;}
	int 		 getMaxSize(){return mMaxEntries;}
	int 		 getMaxMemEntries(){return mMaxMemEntries;}
	int			 getTimestamp(){return mTimestamp;}
	void		 setTimestamp(const u32 &time){mTimestamp = time;}
	void 		 setLow(const char* low)
				 {
					 mRegionLock->lock();
					 memcpy(mLow, low,(*mComp).size);
					 mRegionLock->unlock();
				 }
	void 		 setHigh(const char* high)
				 {
					 mRegionLock->lock();
					 memcpy(mHigh, high, (*mComp).size);
					 mRegionLock->unlock();
				 }
	bool 		 append(char *begin, const int num, const AosRundataPtr &rdata);
	char* 	     findBoundary(char *begin, const int length);
	void 		 setBktItr(list<AosIILMergerBktPtr>::iterator &itr){mBktItr = itr;}
	bool		 splitBranch(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata);
	bool		 mergeAllData(const AosRundataPtr &rdata);
	AosBuffPtr   load();
	u64 		getTotalSize();
	void		lock(){mLock->lock();mIsLocked = true;}
	void		unlock(){mIsLocked = false; mLock->unlock();}
	bool		tryLock()
				{
//static int total, failed;
					bool rslt = false;
					//int i = 20;
					if (!(rslt = mLock->trylock()))
					{
						OmnUsSleep(10);
						rslt = mLock->trylock();
					}
/*if (!rslt)
	failed++;
total++;
if ((total%500)==0)
	OmnScreen << "try lock status" << failed << ", " << total << endl;
	*/
					return rslt;
				}
	bool 		isLocked(){return mIsLocked;}
};

struct Comp
{
	//bool operator()(AosIILMergerBktPtr &lh, AosIILMergerBktPtr &rh)
	bool operator()(list<AosIILMergerBktPtr>::iterator &lh, list<AosIILMergerBktPtr>::iterator &rh)
	{
		int lh_size = (*lh)->size();
		int lh_time = (*lh)->getTimestamp();
		int rh_size = (*rh)->size();
		int rh_time = (*rh)->getTimestamp();
		int time = OmnGetSecond();
		if ((time - lh_time == 0) && (time - rh_time == 0)) return lh_size < rh_size;
		if (time - lh_time == 0) return true;
		if (time - rh_time == 0) return false;
		return lh_size/(time-rh_time) < rh_size/(time-lh_time);
	}
};
#endif

