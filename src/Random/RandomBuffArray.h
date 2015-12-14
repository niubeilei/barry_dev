////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/04/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Random_RandomBuffArray_h
#define AOS_Random_RandomBuffArray_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/Opr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


#include <vector>
using namespace std;

class AosRandomBuffArray: virtual public OmnRCObject 
{
	OmnDefineRCObject;
public:
	enum
	{
		eInitDocid = 10000,
		eDftNumSeeds = 1000,
		eSeedMinLen = 5,
		eSeedMaxLen = 15,
		eBuffLength = 1000000
	};

private:
	AosRundataPtr	mRundata;
	u64				mCrtDocid;
	int				mRecordLength;
	int				mNumSeeds;
	vector<OmnString>	mSeeds;
	vector<u64>		*mDocids;
	vector<int>		mSeqno;
	int				mSeedMinLen;
	int				mSeedMaxLen;

	// for loop
	int				mCrtSeedPos;
	u64				mCrtDocidPos;	
	u64				mTotalSize;
	
public:
	AosRandomBuffArray(const int record_len);
	~AosRandomBuffArray();

	AosBuffPtr	createNRecordToBuff(const u64 &num_rec,AosRundata *rdata);
	int			getNumSeeds()const{return mNumSeeds;}
	OmnString	getSeed(const int idx,int &record_len);
	OmnString	getRecordsBySeeds(const int idx,vector<u64>* &vector_ptr);

	bool hasMore() const
	{
		if (mCrtSeedPos >= mNumSeeds) return false;
		return (mCrtDocidPos < mDocids[mCrtSeedPos].size());
	}

	bool nextValue(OmnString &value, u64 &docid)
	{
		value = "";
		docid = 0;
		if (mCrtSeedPos >= mNumSeeds) return false;
		aos_assert_r(mCrtDocidPos < mDocids[mCrtSeedPos].size(), false);

		docid = mDocids[mCrtSeedPos][mCrtDocidPos];
		value = constructEntry(mSeeds[mCrtSeedPos], mCrtDocidPos, docid);
		
		// find next
		while(1)
		{
			// Chen Ding, 2013/06/27
			// if (mCrtDocidPos < mDocids[mCrtSeedPos].size()-1)
			if (mDocids[mCrtSeedPos].size() > 0 && mCrtDocidPos < mDocids[mCrtSeedPos].size()-1)
			{
				mCrtDocidPos++;
				return true;
			}

			mCrtSeedPos++;
			mCrtDocidPos = 0;
			if(mCrtSeedPos >= mNumSeeds)
			{
				return true;
			}

			// not the last entry
			if (mDocids[mCrtSeedPos].size() > 0)
			{
				return true;
			}
		}
		return true;
	}

	bool resetRecordLoop()
	{
		mCrtSeedPos = 0;
		mCrtDocidPos = 0;	

		if(mTotalSize == 0)
		{
			mCrtSeedPos = mNumSeeds;
			return true;
		}

		// find first record
		while(mCrtSeedPos < mNumSeeds)
		{
			if(mDocids[mCrtSeedPos].size() > 0)
			{
				return true;
			}
			mCrtSeedPos++;
		}
		return true;
	}

	int64_t size() const
	{
		return mTotalSize;
	}

	// Chen Ding, 2013/09/21
	bool query(			AosRundata *rdata, 
						OmnString &start_value,
						OmnString &end_value, 
						AosBitmapObjPtr &bitmap);

	bool query( 		AosRundata *rdata,
						const AosOpr	&opr1,
						const OmnString &value1,
						const AosOpr	&opr2,
						const OmnString &value2,
						AosBitmapObjPtr &bitmap);
	bool query(
						AosRundata *rdata,
						const AosOpr	&opr1,
						const OmnString &value1,
						const AosOpr	&opr2,
						const OmnString &value2,
						const u64		&start_idx,
						const u64		&page_size,
						vector<OmnString>  &values,
						vector<u64>     &docids);

private:
	bool 			init();
	u64				getNewDocid();

	bool getStartPosGT(	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getStartPosGE(	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getStartPosEQ(	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getStartPosLT(	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getStartPosLE(	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getStartPosAN(	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getStartPosNE(	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getEndPosGT( 	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getEndPosGE( 	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getEndPosEQ( 	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getEndPosLT( 	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getEndPosLE( 	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getEndPosNE( 	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getEndPosAN( 	AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx, 
						int &entry_idx);
	bool getStartPosGreater(
						AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx);
	bool getStartPosLess(
						AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx);
	bool getEndPosGreater(
						AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx);
	bool getEndPosLess(
						AosRundata *rdata, 
						const OmnString &value,
						int &seed_idx);
	OmnString constructEntry(
						const OmnString &seed, 
						const int seqno, 
						const u64 docid);
	bool getStartPos( 	AosRundata *rdata, 
						const AosOpr	&opr,
						const OmnString &value,
						int &start_seed_idx, 
						int &start_entry_idx);
	bool getEndPos( 	AosRundata *rdata, 
						const AosOpr	&opr,
						const OmnString &value,
						int &start_seed_idx, 
						int &start_entry_idx);
};
#endif
