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
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_BuffArray_h
#define AOS_Util_BuffArray_h

#include "DataRecord/Ptrs.h"
#include "DataTypes/DataColOpr.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/StrValueInfo.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/CompareFun.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"

#include <vector>
using namespace std;


class AosBuffArray : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eInitSize = 10000,
		eExtendStepSize = 5,
		eMaxRcdLen = 10000,
	};

	OmnMutexPtr			mLock;
	OmnMutex *			mLockRaw;
	AosCompareFunPtr 	mComp;
	AosCompareFun * 	mCompRaw;
	AosBuffPtr			mBuff;
	AosBuff *			mBuffRaw;

	bool				mStable;
	bool				mWithDocid;
	bool				mIsSorted;
	i64					mNumRcds;
	i64					mMemCap;
	int					mRcdLen;
	int					mKeyLen;

	char *				mCrtPos;
	i64					mItr;
	i64					mRangeItr;	
	AosStrValueInfo		mFieldInfo;

	bool				mAutoExtend;
	int					mExtendCounter;

public:
	AosBuffArray(const int record_len);
	AosBuffArray(const AosCompareFunPtr &comp, const bool with_docid, const bool stable, const i64 &buff_len);
	AosBuffArray(const AosBuffPtr &buff,const AosCompareFunPtr &comp, const bool stable);
	AosBuffArray(const AosXmlTagPtr &conf, AosRundata *rdata);
	AosBuffArray(const AosBuffArray &rhs);
	//AosBuffArray(const AosBuffArray &rhs, bool flag); // for test
	~AosBuffArray();

	AosCompareFunPtr getCompareFunc() const {return mComp;} 
	void	setCompareFunc(const AosCompareFunPtr &func) {mComp = func; mCompRaw = mComp.getPtr();} 

	bool	isStable() const {return mStable;}
	bool	isWithDocid(){return mWithDocid;}

	bool	isSorted() const {return mIsSorted;}
	void	setSorted(const bool sorted) {mIsSorted = sorted;}

	bool    sort();
	bool	sort(const AosDataColOpr::E &opr);

	int		getRecordLen() const {return mRcdLen;}
	int 	getKeyLen() const {return mKeyLen;}

	i64		size() const {return mNumRcds * mRcdLen;}
	i64		dataLength() const {return mNumRcds * mRcdLen;}
	i64		getNumEntries() const {return mNumRcds;}
	void	setNumEntries(const i64 &idx) 
	{
		mNumRcds = idx;
		mCrtPos = mBuffRaw->data() + idx * mRcdLen;
	}

	char*	first() {return mBuffRaw->data();}
	char*	last() {return mCrtPos;}
	AosBuffPtr  getBuff()
	{
		aos_assert_r(mBuffRaw, 0);
		mBuffRaw->setDataLen(mNumRcds * mRcdLen);
		return mBuff;
	}
	char*	getEntry(const i64 &idx)
	{
		if (idx >= mNumRcds) return 0;
		return &mBuffRaw->data()[idx * mRcdLen];
	}
	bool	getEntry(const i64 &idx, char* &data, int &len)
	{
		len = 0;
		if (idx >= mNumRcds) return 0;
		len = mRcdLen;
		return &mBuffRaw->data()[idx * mRcdLen];
	}

	bool	appendEntry(
				const AosValueRslt &value, 
				AosRundata *rdata);
	bool	appendEntry(
				const AosDataRecordObjPtr &record, 
				AosRundata *rdata);
	bool 	appendEntry(
				AosDataRecordObj *record, 
				AosRundata *rdata);
	bool	appendEntry(
				const AosQueryRsltObjPtr &query_rslt,
				AosRundata *rdata);
	bool	appendEntry(
				const char *data,
				const int len,
				AosRundata *rdata);
	bool	appendEntry(
				const u64 &data,
				const u64 &docid,
				AosRundata *rdata);
	bool	appendEntry(
				const char *data, 
				const int len,
				const u64 &docid,
				AosRundata *rdata);

	bool	setBuff(const AosBuffPtr &buff);

	bool	clear(); 
	bool 	nextValue(char** data);
	bool 	nextValue(const AosDataRecordObjPtr &record);
	bool	nextValue(AosValueRslt &value, const bool copyflag);
	bool	hasMoreData();

	bool	firstRecordInRange(const i64 &idx, const AosDataRecordObjPtr &record);
	bool	nextRecordInRange(const AosDataRecordObjPtr &record);
	
	bool	copyRecordTo(
				const i64 &rcd_idx,
				const AosDataRecordObjPtr &record,
				AosRundata *rdata);
	bool	copyRecordTo(
				const i64 &rcd_idx,
				const AosValueRslt &value,
				AosRundata *rdata);
	
	i64		findBoundary(
				const i64 &start_pos, 
				const char *key,
				const bool reverse, 
				const bool inclusive);
	
	i64		findBoundary(const char *key, const bool inclusive)
	{
		return findBoundary(0, key, false, inclusive);
	}
	
	bool	handleNullData(char *record, AosRundata *rdata);
	void	resetRecordLoop() {mItr = 0;} 
	void	moveTo(const i64 &pos) {mItr = pos;} 
	void	setRecordLoop(const i64 &idx);
	void	resetRangeLoop() {mRangeItr = 0;}
	bool	removeLastEntry();
	bool	removeNumEntries(const i64 &num);
	AosBuffArrayPtr clone() const;

	bool	serializeFromXml(const AosXmlTagPtr &xml);
	bool	serializeToXml(AosXmlTagPtr &xml);
	bool	serializeFromBuff(const AosBuffPtr &buff);
	bool	serializeToBuff(const AosBuffPtr &buff);

	// Chen Ding, 10/29/2012
	/*AosBuffPtr merge(
			const AosBuffArrayPtr &lhs, 
			const AosBuffArrayPtr &rhs,
			const AosDataColOpr::E opr,
			AosRundata *rdata);*/

	// felicia, 2012/11/02
	void	addToBuff(
				const AosBuffPtr &buff,
				const char *crt,
				const i64 &length)
	{
		buff->addBuff(crt, length);
	}

	void	copy(
				char *new_crt, 
				char *crt, 
				const i64 &length)
	{
		memcpy(new_crt, crt, length);	
	}

	char*	move(
				char *crt,
				const i64 &length)
	{
		char * to = crt + length;
		return to;
	}

	i64		distance(
				const char *crt,
				const char *last)
	{
		return (last - crt);
	}

	void	setReverse(const bool reverse){mCompRaw->setReverse(reverse);}
	bool	mergeFrom(
				const AosBuffArrayPtr &buff_array,
				const u64 &startidx,
				const u64 &len = 0);
	bool	shrinkFromHead(const u64 &num_remove);
	void	setAutoExtend(const bool flag,const int start_size)
	{
		aos_assert(start_size > 0);
		mAutoExtend = flag;
		if(flag == false)return;

		if(mNumRcds)
		{
			aos_assert(start_size >= mRcdLen );
		}
		autoExtend(start_size, false);
		calculateKeyLen();
	}

private:
	void	initMemberData();
	bool	expandMemoryPriv(const u64 &crt_size,const u64 &expect_size);
	bool	initBuff(const i64 &buff_len);
	bool	config(const AosXmlTagPtr &conf, AosRundata *rdata);
	
   	bool	moveback(const i64 &idx);

	bool	copyRecordTo(
				const i64 &rcd_idx,
				const char * data,
				const int len,
				const u64 &docid,
				AosRundata *rdata);

	
	bool inline calculateKeyLen()
	{
		//felicia, 2014/08/12
		return true;
		mKeyLen = mRcdLen;
		if (mFieldInfo.set_trailing_null) mKeyLen--;
		if (mWithDocid) 
		{
			mKeyLen -= sizeof(u64);
			if (mFieldInfo.set_field_null) mKeyLen--;
		}
	
		if (mKeyLen <= 0)
		{
			return false;
		}
		return true;
	}

public:
	// Chen Ding, 2013/02/11
	bool	recordExist(const char *data);
	bool	removeRecord(const char *data);
	const char *findEntry(const char *data);
	bool	merge(
				AosRundata *rdata, 
				const AosBuffArrayPtr &buff_array,
				const AosDataColOpr::E &opr = AosDataColOpr::eIndex);

	//felicia, 2015/08/28 for streaming merge data
	bool	mergeData();
	

private:
	AosBuffPtr sort(
				AosRundata *rdata, 
				const AosBuffArrayPtr &rhs,
				const AosDataColOpr::E &opr);
	bool	autoExtendLocked(const int target_size, const bool more_room = true);
	bool	autoExtend(const int target_size, const bool more_room = true)
	{
		mLockRaw->lock();
		bool rslt = autoExtendLocked(target_size, more_room);
		mLockRaw->unlock();
		return rslt;
	}
};
#endif

