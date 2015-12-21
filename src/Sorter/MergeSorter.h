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
// 10/30/2012 Created by Felicia
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Sorter_MergeSorter_h
#define Aos_Sorter_MergeSorter_h

#include "DataSort/Ptrs.h"
#include "DataSort/DataSort.h"
#include "DataRecord/Ptrs.h"
#include "DataTypes/DataColOpr.h"
#include "Rundata/Ptrs.h"
#include "Sorter/Ptrs.h"
#include "Util/BuffArray.h"
#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Porting/Sleep.h"
#include "XmlUtil/Ptrs.h"

using namespace std;


template <class Type>
class AosMergeSorter : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	Type            		mLhs;
	Type                    mRhs;
	int 					mLevel;
	AosDataColOpr::E		mOperator;
	//AosCompareFunPtr		mCmp;
	AosCompareFun *			mCmpRaw;
	AosSortCallerPtr		mCaller;
	void*					mData;
	u64						mTimeStamp;
	int 					mPriority;
	AosRundataPtr           mRundata;

public:
	AosMergeSorter(
		const Type &lhs,
		const Type &rhs,
		const AosDataColOpr::E opr,
		AosCompareFun * cmp,
		const AosSortCallerPtr &caller,
		const int level,
		void * data,
		const AosRundataPtr &rdata)
	:
	mLhs(lhs),
	mRhs(rhs),
	mLevel(level),
	mOperator(opr),
	mCmpRaw(cmp),
	mCaller(caller),
	mData(data),
	mTimeStamp(OmnGetSecond()),
	mPriority(0),
	mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
	{
	}
	
	~AosMergeSorter()
	{
	}

	void setPriority(int priority) { mPriority = priority; }
	int getPriority() { return mPriority; }

	/*bool operator < (const OmnSPtr<AosMergeSorter<Type> > &rhs) const
	{
		int rhs_level = rhs->getLevel();
		u64 rhs_time = rhs->getTimeStamp();
		if(mLevel == rhs_level)
		{
			return mTimeStamp < rhs_time;
		}
		
		return mLevel > rhs_level;
	}
	*/
	Type& getHls() {return mLhs;}
	Type& getRls() {return mRhs;}

	int getLevel() {return mLevel;}
	u64 getTimeStamp(){return mTimeStamp;}

	//bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	//AosBuffPtr sort(const AosRundataPtr &rdata);
	AosBuffPtr sort(const AosRundataPtr &rdata)
	{
		// This function merges the two buff arrays 'lhs' and 'rhs' into
		// a new one. Both 'lhs' and 'rhs' must be in the same structure
		// and sorted by the same keys.

		aos_assert_r(mLhs, 0);
		aos_assert_r(mRhs, 0);
//static int sortnum = 0;
		if (!mLhs->isSorted())
		{
			mLhs->sort(mOperator);
			//bool rslt = sanitycheck(mLhs->first(), mLhs->last() - mLhs->first());
			//aos_assert_r(rslt, NULL);
//sortnum++;
//OmnScreen << "ssssssssss  sortnum : " << sortnum << endl;		
		}
		if (!mRhs->isSorted())
		{
			mRhs->sort(mOperator);
			//bool rslt = sanitycheck(mRhs->first(), mRhs->last() - mRhs->first());
			//aos_assert_r(rslt, NULL);
//sortnum++;
//OmnScreen << "ssssssssss  sortnum : " << sortnum << endl;		
		}
	
		char *lhs_crt = mLhs->first();
		char *rhs_crt = mRhs->first();
		char *lhs_last = mLhs->last();
		char *rhs_last = mRhs->last();

		aos_assert_r(mCmpRaw, 0);
		aos_assert_r(mLhs->size() % mCmpRaw->size == 0, 0);
		aos_assert_r(mRhs->size() % mCmpRaw->size == 0, 0);

		int lhs_rcds = mLhs->size()/(mCmpRaw->size);
		int rhs_rcds = mRhs->size()/(mCmpRaw->size);
		int total_rcds = lhs_rcds + rhs_rcds;
		int record_len = mCmpRaw->size;

		u64 total_size = total_rcds * record_len;
		AosBuffPtr new_buff = OmnNew AosBuff(total_size AosMemoryCheckerArgs);
		if (total_rcds <= 0) return new_buff;

		aos_assert_r(lhs_crt, 0);
		aos_assert_r(rhs_crt, 0);
		aos_assert_r(lhs_last, 0);
		aos_assert_r(rhs_last, 0);

		if (lhs_rcds <= 0)
		{
			mRhs->copy(new_buff->data(), rhs_crt, rhs_rcds * record_len);
			new_buff->setDataLen(rhs_rcds * record_len);
			aos_assert_r(rhs_rcds <= total_rcds, 0);
			return new_buff;
		}

		if (rhs_rcds <= 0)
		{
			mLhs->copy(new_buff->data(), lhs_crt, lhs_rcds * record_len);
			new_buff->setDataLen(lhs_rcds * record_len);
			aos_assert_r(lhs_rcds <= total_rcds, 0);
			return new_buff;
		}
/*OmnScreen << " ============================= " << endl;
OmnScreen << " lsh_crt : " << lhs_crt << endl;
OmnScreen << " lhs_rcds : " << lhs_rcds << endl;
OmnScreen << " rhs_rcds : " << rhs_rcds << endl;
OmnScreen << " rhs_crt : " << rhs_crt << endl;
OmnScreen << " total_size : " << total_size << endl;
OmnScreen << " mLevel : " << mLevel << endl;
OmnScreen << " ============================= " << endl;
*/
		// Now it is ready to merge the two buffs. Both are not empty.
		int num_rcds = 0;
		char *new_crt = new_buff->data();
	//	u64 lhs_docid = 0;
	//	u64 rhs_docid = 0;
		char *data = NULL;
		int  len = 0;
		while (num_rcds < total_rcds)
		{
			int rr = mCmpRaw->cmp(lhs_crt, rhs_crt);
			if (rr < 0)
			{
				// lhs_crt < rhs_crt. Need to put lhs_crt to new_buff and 
				// advance lhs_crt.
				mLhs->copy(new_crt, lhs_crt, record_len);
				new_crt += record_len;
				lhs_crt = mLhs->move(lhs_crt, record_len);
				num_rcds++;
				if (mLhs->distance(lhs_crt, lhs_last)/record_len == 0)
				{
					//OmnScreen << "111111 " << " num_rcds : " << num_rcds << " total_rcds : " << total_rcds << endl;
					aos_assert_r(lhs_crt == lhs_last, 0);
					int rhs_distance = mRhs->distance(rhs_crt, rhs_last);
					mRhs->copy(new_crt, rhs_crt, rhs_distance);
					num_rcds += rhs_distance/record_len;
					new_buff->setDataLen(num_rcds * record_len);
					aos_assert_r(num_rcds <= total_rcds, 0);
					return new_buff;
				}
			}
			else if (rr > 0)
			{
				// lhs_crt > rhs_crt. Need to put lhs_crt to new_buff and 
				// advance lhs_crt.
				mRhs->copy(new_crt, rhs_crt, record_len);
				new_crt += record_len;
				rhs_crt = mRhs->move(rhs_crt, record_len);
				num_rcds++;
				if (mRhs->distance(rhs_crt, rhs_last)/record_len == 0)
				{
					//OmnScreen << "222222" << " num_rcds : " << num_rcds << " total_rcds : " << total_rcds <<endl;
					aos_assert_r(rhs_crt == rhs_last, 0);
					int lhs_distance = mLhs->distance(lhs_crt, lhs_last);
					mLhs->copy(new_crt, lhs_crt, lhs_distance);
					num_rcds += lhs_distance/record_len;
					new_buff->setDataLen(num_rcds * record_len);
					aos_assert_r(num_rcds <= total_rcds, 0);
					return new_buff;
				}
			}
			else
			{
				memcpy(new_crt, lhs_crt, record_len); 
				mCmpRaw->mergeData(new_crt, rhs_crt, data, len);
				lhs_crt = mLhs->move(lhs_crt, record_len);
				rhs_crt = mRhs->move(rhs_crt, record_len);
				new_crt += record_len; 
				num_rcds++;
								
				int lhs_distance = mLhs->distance(lhs_crt, lhs_last);
				int rhs_distance = mRhs->distance(rhs_crt, rhs_last);

				if (lhs_distance/record_len == 0 && rhs_distance/record_len > 0)
				{
					aos_assert_r(lhs_crt == lhs_last, 0);
					mRhs->copy(new_crt, rhs_crt, rhs_distance);
					num_rcds += rhs_distance/record_len;
					new_buff->setDataLen(num_rcds * record_len);
					aos_assert_r(num_rcds <= total_rcds, 0);
					return new_buff;
				}

				if (lhs_distance/record_len > 0 && rhs_distance/record_len == 0)
				{
					aos_assert_r(rhs_crt == rhs_last, 0);
					mLhs->copy(new_crt, lhs_crt, lhs_distance);
					num_rcds += lhs_distance/record_len; 
					new_buff->setDataLen(num_rcds * record_len);
					aos_assert_r(num_rcds <= total_rcds, 0);
					return new_buff;
				}

				if (lhs_distance/record_len == 0 && rhs_distance/record_len == 0)
				{
					aos_assert_r(rhs_crt == rhs_last, 0);
					aos_assert_r(lhs_crt == lhs_last, 0);
					new_buff->setDataLen(num_rcds * record_len);
					aos_assert_r(num_rcds <= total_rcds, 0);
					return new_buff;
				}
			}
		}

		new_buff->setDataLen(num_rcds * record_len);
		aos_assert_r(num_rcds <= total_rcds, 0);

		//bool rslt = sanitycheck(new_buff->data(), new_buff->dataLen());
		//aos_assert_r(rslt, NULL);

		//OmnScreen << "555555" << " num_rcds : " << num_rcds << " total_rcds : " << total_rcds <<endl;
		return new_buff;
	}

	bool createErrorFile(const AosBuffPtr &buff)
	{
		AosBuffPtr lhs_buff = mLhs->getBuff();
		if (!lhs_buff) return true;
		AosBuffPtr rhs_buff = mRhs->getBuff();
		if (!rhs_buff) return true;
		
		static int id = 0;
		OmnString fname = AosGetBaseDirname();
		OmnString fname1 = fname;
		fname1 << "/sorterror_" << mLevel << "_" << id << "_" << (u64)(unsigned long)this;
		OmnFilePtr	file = OmnNew OmnFile(fname1, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(file->isGood(), false);
		bool rslt = file->append(buff->data(), buff->dataLen(), true);
		aos_assert_r(rslt, false);
		id++;

		OmnString fname2 = fname;
		fname2 << "/sorterror_lhs_" << mLevel << "_" << id << "_" << (u64)(unsigned long)this;
		file = OmnNew OmnFile(fname2, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(file->isGood(), false);
		rslt = file->append(lhs_buff->data(), lhs_buff->dataLen(), true);
		aos_assert_r(rslt, false);
		id++;
		
		OmnString fname3 = fname;
		fname3 << "/sorterror_rhs_" << mLevel << "_" << id << "_" << (u64)(unsigned long)this;
		file = OmnNew OmnFile(fname3, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(file->isGood(), false);
		rslt = file->append(rhs_buff->data(), rhs_buff->dataLen(), true);
		aos_assert_r(rslt, false);
		id++;
		return true;
	}

	bool mergeBlocks()
	{
		// it's reading to mergeSorter, and it will return a buff
		// judge the buff's dataLen whether higher the maxsize
		// yes, to save to file
		// no, to mergeSorter the next level

		// mergeSorter
		int64_t total_size = mLhs->size() + mRhs->size();
	
		bool errorFlag = false;
		OmnString errorMsg;
		if (mLhs->size() % mCmpRaw->size != 0)
		{
			errorMsg << " mLhs size : " << mLhs->size();
			errorFlag = true;
		}

		if (mRhs->size() % mCmpRaw->size != 0)
		{
			errorMsg << " mRhs size : " << mRhs->size();
			errorFlag = true;
		}
		//AosMergeSorterPtr mergeSorter = OmnNew AosMergeSorter(mLhs, mRhs, mOperator);	
		//aos_assert_r(mergeSorter, false);
		AosBuffPtr buff = sort(mRundata);
		//OmnMutex *lock = OmnNew OmnMutex();
		//lock->lock();
		int64_t bufflen = buff->dataLen();	
		//aos_assert_r(bufflen <= total_size, false);
		//aos_assert_r(bufflen % mCmpRaw->size == 0, false);
		if (bufflen > total_size || bufflen % mCmpRaw->size != 0)
		{
			errorMsg << "  sortbuff size : " << bufflen;
			errorFlag = true;
		}
		
		if (errorFlag)
		{
			errorMsg << " mCmpSize : " << mCmpRaw->size;
			OmnAlarm << errorMsg << enderr;
			createErrorFile(buff);
		}

		int64_t merged_size = total_size - bufflen;
		aos_assert_r(merged_size % mCmpRaw->size == 0, false);
//OmnScreen << "@@@@ " << mCaller.getPtr() << "  buff:" << buff.getPtr() << endl;
		if (merged_size > 0)
		{
			mCaller->reduceCacheSize(merged_size, true);
		}
		if (bufflen <= 0) return true;

		bool rslt = mCaller->mergeSortFinish(buff, mLevel, mData, mRundata);
		aos_assert_r(rslt, false);
		mCaller->mergeSortFinish(this, mLevel);
		mCaller = 0;
		return true;
	}

	bool sanitycheck(char *begin, int length)
	{
		return true;
		aos_assert_r(length%mCmpRaw->size == 0, false);
		int size = length/mCmpRaw->size;
		for(int i=0; i<size; i++)
		{
			//aos_assert_r(begin[0] != 0, false);
			if (i>0)
			{
				if(!(mCmpRaw->cmp(begin, begin-mCmpRaw->size) >= 0))
				{
					OmnAlarm << "error" << enderr;
					return false;
				}
			}
			begin += mCmpRaw->size;
		}

		return true;
		for (int i=0; i<length; i++)
		{
			aos_assert_r(begin+i*mCmpRaw->size != 0, false);
			aos_assert_r(strcmp(begin+i*mCmpRaw->size, "0") >= 0, false);
			aos_assert_r(strcmp(begin+i*mCmpRaw->size, "{") <= 0, false);
		}
		return true;
	}

	bool printf_docid(char *begin, int pox, int len)
	{
	aos_assert_r(len%mCmpRaw->size == 0, false);
		int size = len/mCmpRaw->size;
		for(int i=0; i<size; i++)
		{
OmnScreen << "@@andyandy@@  docid is " << *((u64*)(begin+pox)) << endl;
			begin += mCmpRaw->size;
		}
		return true;
	};

};

#endif

