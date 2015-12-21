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
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. It has a companion IIL that keeps the
// same set of Docids but sorted based on docids. 
//
// Modification History:
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/BuffArray.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util1/MemMgr.h"
#include "Util/Sort.h"

#include <stdlib.h>

static OmnMutex sgLock;

AosBuffArray::AosBuffArray(const int record_len)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mComp(0),
mCompRaw(0),
mBuff(0),
mBuffRaw(0),
mStable(true),
mWithDocid(false),
mIsSorted(false),
mNumRcds(0),
mMemCap(0),
mRcdLen(record_len),
mKeyLen(record_len),
mCrtPos(0),
mItr(0),
mRangeItr(0),
mAutoExtend(false),
mExtendCounter(0)
{
}


AosBuffArray::AosBuffArray(
		const AosCompareFunPtr &comp, 
		const bool with_docid,
		const bool stable,
		const i64 &buff_len)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mComp(comp),
mCompRaw(mComp.getPtr()),
mBuff(0),
mBuffRaw(0),
mStable(stable),
mWithDocid(with_docid),
mIsSorted(false),
mNumRcds(0),
mMemCap(0),
mRcdLen((*comp).size),
mKeyLen((*comp).size),
mCrtPos(0),
mItr(0),
mRangeItr(0),
mAutoExtend(false),
mExtendCounter(0)
{
	if (!calculateKeyLen())
	{
		OmnThrowException("invalid key length");
		return;
	}
	if (buff_len > 0)
	{
		initBuff(buff_len);
	}
}


AosBuffArray::AosBuffArray(
		const AosBuffPtr &buff,
		const AosCompareFunPtr &comp, 
		const bool stable)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mComp(comp),
mCompRaw(mComp.getPtr()),
mBuff(buff),
mBuffRaw(mBuff.getPtr()),
mStable(stable),
mWithDocid(false),
mIsSorted(false),
mNumRcds(0),
mMemCap(0),
mRcdLen((*comp).size),
mKeyLen((*comp).size),
mCrtPos(0),
mItr(0),
mRangeItr(0),
mAutoExtend(false),
mExtendCounter(0)
{
	if (!calculateKeyLen())
	{
		OmnThrowException("invalid key length");
		return;
	}

	if ((*comp).size <= 0)
	{
		OmnThrowException("(*comp).size is 0");
		return;
	}

	mNumRcds = buff->dataLen() / (*comp).size;
	mMemCap = buff->dataLen() / (*comp).size;
	mCrtPos = buff->data() + mNumRcds * mRcdLen;
}


AosBuffArray::AosBuffArray(const AosXmlTagPtr &conf, AosRundata* rdata)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mComp(0),
mCompRaw(0),
mBuff(0),
mBuffRaw(0),
mStable(true),
mWithDocid(false),
mIsSorted(false),
mNumRcds(0),
mMemCap(0),
mRcdLen(0),
mKeyLen(0),
mCrtPos(0),
mItr(0),
mRangeItr(0),
mAutoExtend(false),
mExtendCounter(0)
{
	if (!config(conf, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosBuffArray::AosBuffArray(const AosBuffArray &rhs)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mComp(rhs.mComp),
mCompRaw(mComp.getPtr()),
mBuff(0),
mBuffRaw(0),
mStable(rhs.mStable),
mWithDocid(rhs.mWithDocid),
mIsSorted(false),
mNumRcds(0),
mMemCap(0),
mRcdLen(rhs.mRcdLen),
mKeyLen(rhs.mKeyLen),
mCrtPos(0),
mItr(0),
mRangeItr(0),
mFieldInfo(rhs.mFieldInfo),
mAutoExtend(rhs.mAutoExtend),
mExtendCounter(0)
{
}


// for test
/*
AosBuffArray::AosBuffArray(const AosBuffArray &rhs, bool flag)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mComp(rhs.mComp),
mCompRaw(mComp.getPtr()),
mBuff(rhs.mBuff->clone(AosMemoryCheckerArgsBegin)),
mBuffRaw(mBuff.getPtr()),
mStable(rhs.mStable),
mWithDocid(rhs.mWithDocid),
mIsSorted(false),
mNumRcds(rhs.mNumRcds),
mMemCap(rhs.mMemCap),
mRcdLen(rhs.mRcdLen),
mKeyLen(rhs.mKeyLen),
mCrtPos(mBuffRaw->data() + mBuffRaw->dataLen()),
mItr(rhs.mItr),
mRangeItr(rhs.mRangeItr),
mFieldInfo(rhs.mFieldInfo),
mAutoExtend(rhs.mAutoExtend),
mExtendCounter(rhs.mExtendCounter)
{
//	OmnScreen << "@@@@@@@@@@@@@@@@@@@@@@@@@ ::::: " << rhs.mBuff->data() - rhs.mCrtPos << endl;
}
*/

AosBuffArray::~AosBuffArray()
{
	mCrtPos = NULL;
}


bool
AosBuffArray::config(const AosXmlTagPtr &def, AosRundata* rdata)
{
	aos_assert_rr(def, rdata, false);
	mStable = def->getAttrBool(AOSTAG_STABLE, true);
	AosXmlTagPtr tag = def->getFirstChild(AOSTAG_COMPARE_FUNC);
	if (!tag)
	{
		AosSetErrorU(rdata, "missing_compare_func") << ": " << def->toString() << enderr;
		return false;
	}

	mComp = AosCompareFun::getCompareFunc(tag);
	if (!mComp)
	{
		AosSetErrorU(rdata, "failed_creating_comp_func") << ": " << def->toString() << enderr;
		return false;
	}
	mCompRaw = mComp.getPtr();

	if (!mFieldInfo.config(def, rdata)) return false;
	mWithDocid = def->getAttrBool(AOSTAG_WITH_DOCID, false);

	mRcdLen = mCompRaw->size;
	mKeyLen = mRcdLen;
	if (mFieldInfo.set_trailing_null) mKeyLen--;
	if (mWithDocid) 
	{
		mKeyLen -= sizeof(u64);
		if (mFieldInfo.set_field_null) mKeyLen--;
	}

	if (mKeyLen <= 0)
	{
		AosSetErrorU(rdata, "invalid_record_len") << ": " << def->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosBuffArray::initBuff(const i64 &buff_len)
{
	i64 newsize = buff_len / mRcdLen;
	if (newsize == 0) newsize = 4000;
	
	try
	{
		mBuff = OmnNew AosBuff(newsize * mRcdLen, 0 AosMemoryCheckerArgs);
		mBuffRaw = mBuff.getPtr();
	}
	catch(...)
	{
		OmnAlarm << "Failed to expand memory: " << newsize * mRcdLen << enderr;
		return false;
	}
	
	mCrtPos = mBuffRaw->data();
	mMemCap = newsize;
	return true;
}


bool
AosBuffArray::setBuff(const AosBuffPtr &buff)
{
	// This function replace mBuff with buff,
	// and the entry of buff should have the 
	// same structure with the mBuff.
	// Brian Zhang 07/18/2012
	aos_assert_r(buff, false);

	// Chen Ding, 10/21/2012: There is no need to check whether it contains data.
	// aos_assert_r(buff->dataLen() > 0, false);
	
	mLockRaw->lock();
	mBuff = buff;
	mBuffRaw = mBuff.getPtr();
	
	// Chen Ding, 10/21/2012
	int size = (mComp) ? mCompRaw->size : mRcdLen;
	aos_assert_rl(size > 0, mLockRaw, false);
	mMemCap = buff->buffLen() / size;

	mItr = 0;
	if (mCompRaw)
	{
		if ((*mCompRaw).size <= 0)
		{
			OmnAlarm << "Invalid size" << enderr;
			mLockRaw->unlock();
			return false;
		}
		mNumRcds = buff->dataLen() / (*mCompRaw).size;
	}
	else
	{
		mNumRcds = buff->dataLen() / mRcdLen;
	}
	mLockRaw->unlock();
	return true;
}


bool
AosBuffArray::appendEntry(
		const AosQueryRsltObjPtr &query_rslt,
		AosRundata *rdata)
{
	u64 docid;
	OmnString key;
	bool finished = false;
	bool rslt = true;
	while (query_rslt->nextDocidValue(docid, key, finished, rdata))
	{
		if (finished) break;
		aos_assert_r(key != "", false);

		autoExtend(key.length() + 1);
		rslt = appendEntry(key.data(), key.length(), docid, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosBuffArray::appendEntry(
		const AosDataRecordObjPtr &record, 
		AosRundata *rdata)
{
	const char *data = record->getData(rdata);
	autoExtend(strlen(data) + 1);
	return appendEntry(data, strlen(data), record->getDocid(), rdata);
}


bool
AosBuffArray::appendEntry(
		AosDataRecordObj *record, 
		AosRundata *rdata)
{
	const char *data = record->getData(rdata);
	i64 rcd_len = record->getRecordLen();
	aos_assert_r(rcd_len > 0, false);

	autoExtend(rcd_len);

	bool rslt = true;
	//Linda, 2014/01/22
	int num_entry = record->getNumRecord();
	vector<int> record_lens;
	if (num_entry == 1) 
	{
		record_lens.push_back(rcd_len);
	}
	else
	{
		rslt = record->getRecordLens(record_lens);	
		aos_assert_r(rslt, false);
	}
	aos_assert_r(record_lens.size() == (u32)num_entry, false);

	int off = 0;
	for (u32 i = 0; i < record_lens.size(); i++)
	{
		aos_assert_r(off + record_lens[i] <= rcd_len, false);
		rslt = appendEntry(&data[off], record_lens[i], rdata);
		aos_assert_r(rslt, false);
		off += record_lens[i];
	}
	return true;
}


/*
bool
AosBuffArray::appendEntry(
		const AosValueRslt &value, 
		AosRundata *rdata)
{
	//const char *data = value.getCharStr(len);
	OmnString str = value.getStr();
	const char *data = str.data();
	int len = str.length();

	//ken 2012/08/02
	if (data[0] == 0)
	{
		OmnAlarm << "the entry is empty" << enderr;
	}

	return appendEntry(data, len, value.getDocid(), rdata);
}
*/


bool
AosBuffArray::appendEntry(
		const char *data,
		const int len,
		AosRundata *rdata)
{
	aos_assert_r(data, false);

	autoExtend(len + 1);
	aos_assert_r(len <= mRcdLen, false);

	mLockRaw->lock();
	if (mNumRcds + 1 >= mMemCap)
	{
		bool rslt = expandMemoryPriv(mNumRcds, mNumRcds + 1);
		aos_assert_rl(rslt, mLockRaw, false);
	}

	memcpy(mCrtPos, data, len);
	mCrtPos += mRcdLen;
	mNumRcds++;
	mLockRaw->unlock();
	return true;
}


bool
AosBuffArray::appendEntry(
		const char *data,
		const int len,
		const u64 &docid,
		AosRundata *rdata)
{
	// This function does the following:
	//  1. Retrieve the entry [name, value] by 'name'. 
	//  2. If the entry does not exist in the iil, add the entry [name, value]
	//  3. If the entry is found, add 'value' to the entry's value.
	// This function should always success. 
	//
	// After adding, if the entry has too many entries, create an IIL transaction
	// and sends it to the IILMgr to process it. It then clear some of the 
	// old entries from memory.

	mLockRaw->lock();
	autoExtendLocked(len + 1);
	if (mNumRcds + 1 >= mMemCap)
	{
		bool rslt = expandMemoryPriv(mNumRcds, mNumRcds + 1);
		aos_assert_rl(rslt, mLockRaw, false);
	}
	

	char *record = mCrtPos;
	
	if (!data || len <= 0)
	{
		if (!AosDataNullPolicy::handleNullData(record, mRcdLen, mFieldInfo, rdata))
		{
			mLockRaw->unlock();
			return false;
		}
	}
	else
	{
		if (len > mKeyLen)
		{
			if (!AosDataTooLongPolicy::handleDataTooLong(
					record, mKeyLen, data, len, mFieldInfo, rdata))
			{
				mLockRaw->unlock();
				return false;
			}

			if (!record) 
			{
				// It is ignored silently.
				mLockRaw->unlock();
				return true;
			}
		}
		else
		{
			memcpy(mCrtPos, data, mKeyLen);
		}
	}

	if (mWithDocid)
	{
		if (mFieldInfo.set_field_null) 
		{
			record[mKeyLen] = 0;
			*(u64*)&record[mKeyLen + 1] = docid;
		}
		else
		{
			*(u64*)&record[mKeyLen] = docid;
		}
	}

	if (mFieldInfo.set_trailing_null)
	{
		mCrtPos[mRcdLen - 1] = 0;
	}

	mCrtPos += mRcdLen;
	mNumRcds++;
	mLockRaw->unlock();
	return true;
}


bool
AosBuffArray::appendEntry(
		const u64 &data,
		const u64 &docid,
		AosRundata *rdata)
{
	// This function does the following:
	//  1. Retrieve the entry [name, value] by 'name'. 
	//  2. If the entry does not exist in the iil, add the entry [name, value]
	//  3. If the entry is found, add 'value' to the entry's value.
	// This function should always success. 
	//
	// After adding, if the entry has too many entries, create an IIL transaction
	// and sends it to the IILMgr to process it. It then clear some of the 
	// old entries from memory.

	mLockRaw->lock();
	if (mNumRcds + 1 >= mMemCap)
	{
		bool rslt = expandMemoryPriv(mNumRcds, mNumRcds + 1);
		aos_assert_rl(rslt, mLockRaw, false);
	}
	
	// ice, 2014/04/18
	char *record = mCrtPos;
	*(u64*)record = data;
	*(u64*)(record + sizeof(u64)) = docid;

	/*
	if (mKeyLen < (int)sizeof(u64))
	{
		mLockRaw->unlock();
		return false;
	}

	char *record = mCrtPos;
	*(u64*)record = data;
	if (mWithDocid)
	{
		if (mFieldInfo.set_field_null) 
		{
			record[mKeyLen] = 0;
			*(u64*)&record[mKeyLen + 1] = docid;
		}
		else
		{
			*(u64*)&record[mKeyLen] = docid;
		}
	}

	if (mFieldInfo.set_trailing_null)
	{
		mCrtPos[mRcdLen - 1] = 0;
	}
	*/

	mCrtPos += mRcdLen;
	mNumRcds++;
	mLockRaw->unlock();
	return true;
}


//Added by wanghaijun for inserting data:2012/12/11
bool 
AosBuffArray::moveback(const i64 &idx)
{
	aos_assert_r(idx >= 0 && idx <= mNumRcds, false);
   	char *src = &mBuffRaw->data()[idx * mRcdLen];
   	char *dest = &mBuffRaw->data()[(idx + 1) * mRcdLen];
   	i64 last_pos = getNumEntries();
   	i64 len = (last_pos - idx) * mRcdLen;
   	aos_assert_r(len >= 0, false);
	
   	if (idx < last_pos)
   	{
     	memmove(dest, src, len);
   	}

   	//copy the last data to mCrtPos, and mNumRcds add one
   	//memcpy(mCrtPos, &mBuffRaw->data()[last_pos * mRcdLen], mRcdLen);
   	mCrtPos += mRcdLen;
   	return true;
}


bool
AosBuffArray::expandMemoryPriv(const u64 &crt_size, const u64 &expect_size)
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 
	u64 newsize = mMemCap + mMemCap;
	if (newsize == 0) newsize = 4000;
	if (newsize <= expect_size) newsize = expect_size + mMemCap;
	AosBuffPtr buff;
	
	try
	{
		buff = OmnNew AosBuff(newsize * mRcdLen + 100, 0 AosMemoryCheckerArgs);
	}

	catch(...)
	{
		OmnAlarm << "Failed to expand memory: " << newsize * mRcdLen << enderr;
		return false;
	}
	
	if (crt_size > 0)
	{
		aos_assert_r(mMemCap > 0, false);
		memcpy(buff->data(), mBuffRaw->data(), crt_size * mRcdLen);
		buff->setDataLen(crt_size * mRcdLen);
		mCrtPos = buff->data() + crt_size * mRcdLen;
	}
	else
	{
		mCrtPos = buff->data();
	}
	mMemCap = newsize;
	mBuff = buff;
	mBuffRaw = mBuff.getPtr();
	return true;
}


bool 
AosBuffArray::sort()
{
	mLockRaw->lock();
	if (!mBuff)
	{
		mLockRaw->unlock();
		return true;
	}
	u64 t1 = OmnGetSecond();
	if (!mStable)
	{
		char *tmpbuff = OmnNew char[mCompRaw->size+10];
		AosSort::sort(mBuffRaw->data(), mCrtPos, mCompRaw, tmpbuff);
		OmnDelete [] tmpbuff;
	}
	else
	{
		char *tmpbuff = OmnNew char[mCompRaw->size+10];
		AosSort::sort(mBuffRaw->data(), mCrtPos, mCompRaw, tmpbuff);
		OmnDelete [] tmpbuff;
	}
	mIsSorted = true;
	int t2 = OmnGetSecond();
	if (t2 - t1 > 20)
	{
		OmnScreen << "Sort time: " << t2 - t1 << " Entries: " 
			<< mNumRcds << " Entry size: " << mRcdLen << endl;
	}
	mLockRaw->unlock();
	return true;
}


bool
AosBuffArray::clear()
{
	mLockRaw->lock();
	mNumRcds = 0;
	mMemCap = 0,
	mBuff = OmnNew AosBuff(eInitSize * mRcdLen, 0 AosMemoryCheckerArgs);
	mBuffRaw = mBuff.getPtr();
	mCrtPos = mBuffRaw->data();
	mItr = 0,
	mLockRaw->unlock();
	mAutoExtend = false;
	mExtendCounter = 0;
	return true;
}


bool
AosBuffArray::hasMoreData()
{
	mLockRaw->lock();
	if (mNumRcds > 0 && mItr < mNumRcds)
	{
		mLockRaw->unlock();
		return true;
	}
	mLockRaw->unlock();
	return false;
}


bool 	
AosBuffArray::nextValue(const AosDataRecordObjPtr &record)
{
	aos_assert_r(record, false);
	
	mLockRaw->lock();
	if (mItr < 0 || mItr >= mNumRcds)
	{
		mLockRaw->unlock();
		return false;
	}
	char *data = mBuffRaw->data() + mItr * mRcdLen;
	mItr++;
	// Chen Ding, 2013/11/29
	// bool rslt = record->setData(data, mRcdLen, 0, false);
	
	// jimodb-1301
	int status;
	bool rslt = record->setData(data, mRcdLen, 0, status);

	if (mWithDocid)
	{
		if (mFieldInfo.set_trailing_null)
		{
			record->setDocid(*(u64*)&data[mRcdLen - sizeof(u64) - 1]);
		}
		else
		{
			record->setDocid(*(u64*)&data[mRcdLen - sizeof(u64)]);
		}
	}
	else
	{
		record->setDocid(0);
	}
	mLockRaw->unlock();
	return rslt;
}


/*
bool	
AosBuffArray::nextValue(AosValueRslt &value, const bool copyflag)
{
	mLockRaw->lock();
	if (mItr < 0 || mItr >= mNumRcds)
	{
		mLockRaw->unlock();
		return false;
	}
	char *data = mBuffRaw->data() + mItr * mRcdLen;
	mItr++;
	bool rslt = value.setCharStr1(data, mKeyLen, copyflag);

	if (mWithDocid)
	{
		if (mFieldInfo.set_trailing_null)
		{
			value.setDocid(*(u64*)&data[mRcdLen - sizeof(u64) - 1]);
		}
		else
		{
			value.setDocid(*(u64*)&data[mRcdLen - sizeof(u64)]);
		}
	}
	else
	{
		value.setDocid(0);
	}
	mLockRaw->unlock();
	return rslt;
}
*/
	

void
AosBuffArray::setRecordLoop(const i64 &idx)
{
	mLockRaw->lock();
	if (idx >= 0 && idx < mNumRcds)
	{
		mItr = idx;
	}	
	mLockRaw->unlock();
}


bool
AosBuffArray::nextValue(char** data)
{
	mLockRaw->lock();
	if (mItr < 0 || mItr >= mNumRcds)
	{
		mLockRaw->unlock();
		return false;
	}

	*data = mBuffRaw->data() + mItr * mRcdLen;
	mItr++ ;
	mLockRaw->unlock();
	return true;
}


bool 
AosBuffArray::removeLastEntry()
{
	mLockRaw->lock();
	aos_assert_rl(mItr != 0, mLockRaw, false);
	mItr--;
	mLockRaw->unlock();
	return true;
}

bool 
AosBuffArray::removeNumEntries(const i64 &num)
{
	mLockRaw->lock();

	aos_assert_rl(num <= mNumRcds, mLockRaw, false);

	char* start = first();
	i64 size = mRcdLen * num;
	i64 size_left = mRcdLen * (mNumRcds - num);

	aos_assert_rl(mCrtPos >= start + size, mLockRaw, false);
	aos_assert_rl(mBuffRaw->dataLen() >= size, mLockRaw, false);
	
	if (size_left > 0)
	{
		memmove(start, start + size , size_left);
	}
	mBuffRaw->setDataLen( size_left);
	
	mNumRcds -= num;
	mCrtPos -= size;
		
	if (mItr < size)
	{
		mItr = 0;
	}
	else
	{
		mItr -= size;
	}
	mLockRaw->unlock();
	return true;
}

/*
bool 
AosBuffArray::copyRecordTo(
		const i64 &rcd_idx, 
		const AosValueRslt &value, 
		AosRundata *rdata)
{
	int len;
	const char *data = value.getCharStr(len);
	u64 docid = value.getDocid();
	return copyRecordTo(rcd_idx, data, len, docid, rdata); 	
}


bool 
AosBuffArray::copyRecordTo(
		const i64 &rcd_idx, 
		const AosDataRecordObjPtr &record,
		AosRundata *rdata)
{
	return copyRecordTo(rcd_idx, record->getData(rdata), record->getRecordLen(), record->getDocid(), rdata);
}



bool 
AosBuffArray::copyRecordTo(
		const i64 &rcd_idx, 
		const char *data, 
		const int len,
		const u64 &docid,
		AosRundata *rdata)
{
	mLockRaw->lock();
	if (rcd_idx >= mNumRcds) 
	{
		mLockRaw->unlock();
		return appendEntry(data, len, docid, rdata);
	}

	char *record = &mBuffRaw->data()[rcd_idx * mRcdLen];
	if (!data || len <= 0)
	{
		bool rslt = AosDataNullPolicy::handleNullData(record, mRcdLen, mFieldInfo, rdata);
		mLockRaw->unlock();
		return rslt;
	}
	
	memset(record, 0, mRcdLen);

	if (len > mKeyLen)
	{
		if (!AosDataTooLongPolicy::handleDataTooLong(
				record, mKeyLen, data, len, mFieldInfo, rdata))
		{
			mLockRaw->unlock();
			return false;
		}

		if (!record) 
		{
			// It is ignored silently.
			mLockRaw->unlock();
			return true;
		}
	}
	else
	{
		memcpy(&mBuffRaw->data()[rcd_idx * mRcdLen], data, len);
	}

	if (mWithDocid)
	{
		if (mFieldInfo.set_field_null) 
		{
			record[mKeyLen] = 0;
			*(u64*)&record[mKeyLen + 1] = docid;
		}
		else
		{
			*(u64*)&record[mKeyLen] = docid;
		}
	}

	if (mFieldInfo.set_trailing_null)
	{
		mCrtPos[mRcdLen - 1] = 0;
	}
	
	mLockRaw->unlock();
	return true;
}

*/

bool 
AosBuffArray::firstRecordInRange(
		const i64 &idx,
		const AosDataRecordObjPtr &record) 
{
	aos_assert_r(record, false);
	mLockRaw->lock();
	if (idx >= mNumRcds) 
	{
		mLockRaw->unlock();
		return false;
	}

	mRangeItr = idx;
	
	char *buff = &mBuffRaw->data()[idx * mRcdLen];
	// Chen Ding, 2013/11/29
	// bool rslt = record->setData(buff, mRcdLen, 0, false);
	
	// jimodb-1301
	int status;
	bool rslt = record->setData(buff, mRcdLen, 0, status);
	
	if (mWithDocid)
	{
		if (mFieldInfo.set_field_null) 
		{
			record->setDocid(*(u64*)&buff[mKeyLen + 1]);
		}
		else
		{
			record->setDocid(*(u64*)&buff[mKeyLen]);
		}
	}

	mLockRaw->unlock();
	return rslt;
}

	
bool 
AosBuffArray::nextRecordInRange(const AosDataRecordObjPtr &record) 
{
	aos_assert_r(record, false);
	mLockRaw->lock();
	mRangeItr++;
	
	char *buff = &mBuffRaw->data()[mRangeItr * mRcdLen];
	if (mRangeItr >= mNumRcds) 
	{
		mLockRaw->unlock();
		return false;
	}

	// Chen Ding, 2013/11/29
	// bool rslt = record->setData(buff, mRcdLen, 0, false);
	
	// jimodb-1301
	int status;
	bool rslt = record->setData(buff, mRcdLen, 0, status);
	
	if (mWithDocid)
	{
		if (mFieldInfo.set_field_null) 
		{
			record->setDocid(*(u64*)&buff[mKeyLen + 1]);
		}
		else
		{
			record->setDocid(*(u64*)&buff[mKeyLen]);
		}
	}
	
	mLockRaw->unlock();
	return rslt;
}


bool
AosBuffArray::handleNullData(char *record, AosRundata *rdata)
{
	return AosDataNullPolicy::handleNullData(record, mRcdLen, mFieldInfo, rdata);
}


AosBuffArrayPtr
AosBuffArray::clone() const
{
	try
	{
		//return OmnNew AosBuffArray(*this, true);
		return OmnNew AosBuffArray(*this);
	}

	catch (...)
	{
		OmnAlarm << "failed creating object" << enderr;
		return 0;
	}
}

/*
AosBuffPtr
AosBuffArray::merge(
		const AosBuffArrayPtr &lhs, 
		const AosBuffArrayPtr &rhs,
		const AosDataColOpr::E opr,
		AosRundata *rdata)
{
	// This function merges the two buff arrays 'lhs' and 'rhs' into
	// a new one. Both 'lhs' and 'rhs' must be in the same structure
	// and sorted by the same keys.
	aos_assert_rr(lhs, rdata, 0);
	aos_assert_rr(rhs, rdata, 0);
	
	char *lhs_crt = lhs->first();
	char *rhs_crt = rhs->first();
	char *lhs_last = lhs->last();
	char *rhs_last = lhs->last();
	AosCompareFunPtr lhs_comp = lhs->getCompareFunc();
	AosCompareFunPtr rhs_comp = rhs->getCompareFunc();

	aos_assert_rr(lhs_comp, rdata, 0);
	aos_assert_rr(rhs_comp, rdata, 0);
	aos_assert_rr(lhs_comp->size == rhs_comp->size, rdata, 0);
	aos_assert_rr(lhs->isStable() == rhs->isStable(), rdata, 0);
	aos_assert_rr(lhs->mStable == rhs->mStable, rdata, 0);
	aos_assert_rr(lhs->mKeyLen == rhs->mKeyLen, rdata, 0);
	aos_assert_rr(lhs->mWithDocid == rhs->mWithDocid, rdata, 0);

	u64 total_rcds = lhs->mNumRcds + rhs->mNumRcds;
	u64 record_len = lhs_comp->size;

	u64 total_size = total_rcds * record_len;
	AosBuffPtr new_buff = OmnNew AosBuff(total_size AosMemoryCheckerArgs);
	if (total_rcds <= 0) return new_buff;

	aos_assert_rr(lhs_crt, rdata, 0);
	aos_assert_rr(rhs_crt, rdata, 0);
	aos_assert_rr(lhs_last, rdata, 0);
	aos_assert_rr(rhs_last, rdata, 0);

	if (lhs->mNumRcds <= 0)
	{
		new_buff->addBuff(rhs_crt, rhs->mNumRcds * record_len);
		return new_buff;
	}

	if (rhs->mNumRcds <= 0)
	{
		new_buff->addBuff(rhs_crt, rhs->mNumRcds * record_len);
		return new_buff;
	}

	// Now it is ready to merge the two buffs. Both are not empty.
	u64 num_rcds = 0;
	char *new_crt = new_buff->data();
	while (num_rcds < total_rcds)
	{
		u64 rr = lhs_comp->cmp(lhs_crt, rhs_crt);
		if (rr < 0)
		{
			// lhs_crt < rhs_crt. Need to put lhs_crt to new_buff and 
			// advance lhs_crt.
			memcpy(new_crt, lhs_crt, record_len);
			new_crt += record_len;
			lhs_crt += record_len;
			num_rcds++;
			if (lhs_crt >= lhs_last)
			{
				aos_assert_rr(lhs_crt == lhs_last, rdata, 0);
				memcpy(new_crt, rhs_crt, record_len * (total_rcds - num_rcds));
				new_buff->setDataLen(total_rcds * record_len);
				return new_buff;
			}
		}
		else if (rr > 0)
		{
			// lhs_crt > rhs_crt. Need to put lhs_crt to new_buff and 
			// advance lhs_crt.
			memcpy(new_crt, rhs_crt, record_len);
			new_crt += record_len;
			rhs_crt += record_len;
			num_rcds++;
			if (rhs_crt >= rhs_last)
			{
				aos_assert_rr(rhs_crt == rhs_last, rdata, 0);
				memcpy(new_crt, lhs_crt, record_len * (total_rcds - num_rcds));
				new_buff->setDataLen(total_rcds * record_len);
				return new_buff;
			}
		}
		else
		{
			// lhs_crt == rhs_crt
			switch (opr)
			{
			case AosDataColOpr::eIndex:
				 memcpy(new_crt, lhs_crt, record_len); num_rcds++;
				 memcpy(new_crt, rhs_crt, record_len); num_rcds++;
				 lhs_crt += record_len;
				 rhs_crt += record_len;
				 if (lhs_crt >= lhs_last && rhs_crt != rhs_last)
				 {
					 aos_assert_rr(lhs_crt == lhs_last, rdata, 0);
					 memcpy(new_crt, rhs_crt, record_len * (total_rcds - num_rcds));
					 return new_buff;
				 }

				 if (rhs_crt >= rhs_last && lhs_crt != lhs_last)
				 {
					 aos_assert_rr(rhs_crt == rhs_last, rdata, 0);
					 memcpy(new_crt, lhs_crt, record_len * (total_rcds - num_rcds));
					 return new_buff;
				 }

			case AosDataColOpr::eSum:
				 // To be done
				 break;

			case AosDataColOpr::eSetValue:
				 // To be done
				 break;

			default:
				 OmnAlarm << "UNrecognized opr: " << opr << enderr;
				 return 0;
			}
		}
	}

	new_buff->setDataLen(total_rcds * record_len);
	return new_buff;
}
*/


// Chen Ding, 2014/02/16
// Added the parameter 'inclusive'
i64 
AosBuffArray::findBoundary(
		const i64 &start_pos, 
		const char *key,
		const bool reverse, 
		const bool inclusive)
{
	aos_assert_r(start_pos >= 0, -1);
	aos_assert_r(start_pos < mNumRcds, -1);
	char* low = mBuffRaw->data() + start_pos * mRcdLen;

	// low must be no smaller than the first entry
	aos_assert_r(mCompRaw->cmp(low, mBuffRaw->data()) >= 0, false);

	// low must be no bigger than the last entry
	aos_assert_r(mCompRaw->cmp(low, mCrtPos - mRcdLen) <= 0, -1);

	// Use the binrary search to find the right position
	i64 half;
	char* mid;
	i64 len = (mNumRcds - start_pos) * mRcdLen;
	
	int forward = 0;
	int backward = 0;
	if (!reverse)
	{
	 	forward = mRcdLen;
	 	backward = 0;
	}
	else
	{
	 	//forward = 0;
	 	//backward = mRcdLen;
	 	forward = mRcdLen;
	 	backward = 0;
	}
	
	// It returns the first index that does not hold
	// the condition. More specifically, if the condition matches 
	// no entries, it returns 0. If all entries match the 
	// condition, it returns mNumRcds. 
	while (len)
	{
		half = (len/mRcdLen) / 2;
		mid = low;
		mid += half * mRcdLen;
		int rslt = mCompRaw->cmp(mid, key);
		if ((inclusive && rslt <= 0) || (!inclusive && rslt < 0))
		{
			low = mid + forward;
			len -= (half + 1) * mRcdLen;
		}
		else
		{
			len = half * mRcdLen - backward;
		}
	}
	i64 pos = (low - mBuffRaw->data()) / mRcdLen;
	aos_assert_r(pos <= mNumRcds, -1);
	return pos;
}


// Chen Ding, 2013/02/11
bool 
AosBuffArray::recordExist(const char *data)
{
	aos_assert_r(data, false);
	mLockRaw->lock();
	i64 pos = findBoundary(0, data);			 
	if (pos < 0) 
	{
		mLockRaw->unlock();
		return false;
	}

	aos_assert_rl(pos < mNumRcds, mLockRaw, false);
    const char *record = &mBuffRaw->data()[pos * mRcdLen];
	int rslt = mCompRaw->cmp(record, data);
	mLockRaw->unlock();
	return rslt == 0;
}


// Chen Ding, 2013/02/11
bool 
AosBuffArray::removeRecord(const char *data)
{
	if (mNumRcds <= 0) return false;
	aos_assert_r(data, false);
	mLockRaw->lock();
	char *record = (char *)findEntry(data);			 
	if (!record) 
	{
		// The entry does not exist.
		mLockRaw->unlock();
		return false;
	}

	if (record == mCrtPos - mRcdLen)
	{
		mNumRcds--;
		mCrtPos -= mRcdLen;
		mLockRaw->unlock();
		return true;
	}

	i64 len = mCrtPos - record - mRcdLen;
	memmove(record, record + mRcdLen, len);
	mNumRcds--;
	mCrtPos -= mRcdLen;
	mLockRaw->unlock();
	return true;
}


const char *
AosBuffArray::findEntry(const char *key)
{
	aos_assert_r(key, 0);
	if (mNumRcds <= 0) return 0;

	// Use the binrary search to find the right position
	const char* mid;
	const char *raw_data = mBuffRaw->data();
	i64 start_idx = 0;
	i64 end_idx = mNumRcds - 1;
	i64 idx;
	while (start_idx < end_idx)
	{
		idx = start_idx + (end_idx - start_idx) / 2;
		mid = &raw_data[idx * mRcdLen];
		int rslt = mCompRaw->cmp(mid, key);
		if (rslt < 0)
		{
			start_idx = idx + 1;
		}
		else if (rslt > 0)
		{
			end_idx = idx - 1;
		}
		else
		{
			return mid;
		}
	}

	if (start_idx == end_idx)
	{
		mid = &raw_data[start_idx * mRcdLen];
		if (mCompRaw->cmp(mid, key) == 0)
		{
			return mid;
		}
	}

	return 0;
}


AosBuffPtr 
AosBuffArray::sort(
		AosRundata *rdata, 
		const AosBuffArrayPtr &rhs,
		const AosDataColOpr::E &opr)
{
	// This function merges the two buff arrays 'lhs' and 'rhs' into
	// a new one. Both 'lhs' and 'rhs' must be in the same structure
	// and sorted by the same keys.

	aos_assert_r(rhs, 0);

	char *lhs_crt = first();
	char *rhs_crt = rhs->first();
	char *lhs_last = last();
	char *rhs_last = rhs->last();

	aos_assert_r(mCompRaw, 0);
	aos_assert_r(mRcdLen == mCompRaw->size, 0);
	aos_assert_r(rhs->getRecordLen() == mCompRaw->size, 0);

	i64 lhs_rcds = mNumRcds;
	i64 rhs_rcds = rhs->getNumEntries();
	i64 total_rcds = lhs_rcds + rhs_rcds;
	int record_len = mCompRaw->size;

	if (rhs_rcds <= 0) return 0;

	i64 total_size = total_rcds * record_len;
	AosBuffPtr new_buff = OmnNew AosBuff(total_size AosMemoryCheckerArgs);
	if (total_rcds <= 0) return new_buff;

	aos_assert_r(lhs_crt, 0);
	aos_assert_r(rhs_crt, 0);
	aos_assert_r(lhs_last, 0);
	aos_assert_r(rhs_last, 0);

	if (lhs_rcds <= 0)
	{
		rhs->copy(new_buff->data(), rhs_crt, rhs_rcds * record_len);
		new_buff->setDataLen(rhs_rcds * record_len);
		return new_buff;
	}
	
	// Now it is ready to merge the two buffs. Both are not empty.
	i64 num_rcds = 0;
	char *new_crt = new_buff->data();
	while (num_rcds < total_rcds)
	{
		// Check whether lhs has run out. If yes, copy the remaining of rhs.
		if (lhs_last == lhs_crt)
		{
			i64 rhs_distance = rhs_last - rhs_crt;
			memcpy(new_crt, rhs_crt, rhs_distance);
			num_rcds += rhs_distance/record_len;
			new_buff->setDataLen(total_size);
			return new_buff;
		}

		// Check whether rhs has run out. If yes, copy the remaining of lhs.
		if (rhs_last - rhs_crt == 0)
		{
			// rhs runs out. Copy the remaining of rhs.
			i64 lhs_distance = lhs_last - lhs_crt;
			memcpy(new_crt, lhs_crt, lhs_distance);
			num_rcds += lhs_distance/record_len;
			new_buff->setDataLen(total_size);
			return new_buff;
		}

		int rr = mCompRaw->cmp(lhs_crt, rhs_crt);
		if (rr < 0)
		{
			// lhs_crt < rhs_crt. Need to put lhs_crt to new_buff and 
			// advance lhs_crt.
			memcpy(new_crt, lhs_crt, record_len);
			new_crt += record_len;
			lhs_crt += record_len;
			aos_assert_rr(lhs_crt <= lhs_last, rdata, 0);
			num_rcds++;
		}
		else if (rr > 0)
		{
			// lhs_crt > rhs_crt. Need to put lhs_crt to new_buff and 
			// advance lhs_crt.
			memcpy(new_crt, rhs_crt, record_len);
			new_crt += record_len;
			rhs_crt += record_len;
			aos_assert_rr(rhs_crt <= rhs_last, rdata, 0);
			num_rcds++;
		}
		else
		{
			u64 lhs_docid, rhs_docid;
			switch (opr)
			{
				case AosDataColOpr::eIndex:
					lhs_docid = *(u64 *)&lhs_crt[record_len - sizeof(u64)];
					rhs_docid = *(u64 *)&rhs_crt[record_len - sizeof(u64)];
					if (lhs_docid <= rhs_docid)
					{
						memcpy(new_crt, lhs_crt, record_len);
						lhs_crt += record_len;
						aos_assert_rr(lhs_crt <= lhs_last, rdata, 0);
					//	new_crt += record_len;
					//	memcpy(new_crt, rhs_crt, record_len);
					}
					else
					{
						memcpy(new_crt, rhs_crt, record_len);
						rhs_crt += record_len;
						aos_assert_rr(rhs_crt <= rhs_last, rdata, 0);
					//	new_crt += record_len;
					//	memcpy(new_crt, lhs_crt, record_len);
					}
					new_crt += record_len;
					num_rcds++;
					break;

				case AosDataColOpr::eNormal:
					// To be done
					// the two array has merged
					*(u64 *)&lhs_crt[record_len - sizeof(u64)] += *(u64 *)&rhs_crt[record_len - sizeof(u64)]; 
					memcpy(new_crt, lhs_crt, record_len); 
					new_crt += record_len;
					lhs_crt += record_len;
					rhs_crt += record_len;
					num_rcds++;
					break;

				case AosDataColOpr::eNoUpdate:
					memcpy(new_crt, lhs_crt, record_len); 
					new_crt += record_len;
					lhs_crt += record_len;
					rhs_crt += record_len;
					num_rcds++;
					break;

				case AosDataColOpr::eSetValue:
					memcpy(new_crt, rhs_crt, record_len); 
					new_crt += record_len;
					lhs_crt += record_len;
					rhs_crt += record_len;
					num_rcds++;
					break;

				case AosDataColOpr::eMaxValue:
					lhs_docid = *(u64 *)&lhs_crt[record_len - sizeof(u64)];
					rhs_docid = *(u64 *)&rhs_crt[record_len - sizeof(u64)];
					if (lhs_docid >= rhs_docid)
					{
						memcpy(new_crt, lhs_crt, record_len); 
					}
					else
					{
						memcpy(new_crt, rhs_crt, record_len); 
					}
					new_crt += record_len;
					lhs_crt += record_len;
					rhs_crt += record_len;
					num_rcds++;
					break;

				case AosDataColOpr::eMinValue:
					lhs_docid = *(u64 *)&lhs_crt[record_len - sizeof(u64)];
					rhs_docid = *(u64 *)&rhs_crt[record_len - sizeof(u64)];
					if (lhs_docid <= rhs_docid)
					{
						memcpy(new_crt, lhs_crt, record_len); 
					}
					else
					{
						memcpy(new_crt, rhs_crt, record_len); 
					}
					new_crt += record_len;
					lhs_crt += record_len;
					rhs_crt += record_len;
					num_rcds++;
					break;

				default:
					OmnAlarm << "UNrecognized opr: " << opr << enderr;
					return 0;
			}
		}
	}
	new_buff->setDataLen(total_size);
	return new_buff;
}


bool 
AosBuffArray::merge(
		AosRundata *rdata, 
		const AosBuffArrayPtr &buff_array,
		const AosDataColOpr::E &opr)
{
	// This function merges 'buff_array' to this class. 
	aos_assert_rr(mCompRaw->size == buff_array->mCompRaw->size, rdata, false);
	i64 num_records = buff_array->mNumRcds;
	if (num_records <= 0) return true;

	AosBuffPtr buff = sort(rdata, buff_array, opr);
	i64 bufflen = buff->dataLen();	
	aos_assert_rr(bufflen % mCompRaw->size == 0, rdata, false);
	aos_assert_rr(bufflen / mCompRaw->size >= mNumRcds, rdata, false);
	mNumRcds = bufflen/mCompRaw->size;
	mMemCap = buff->buffLen() / mCompRaw->size;
	mBuff = buff;
	mBuffRaw = mBuff.getPtr();
	mCrtPos = buff->data() + mNumRcds * mRcdLen; 
	return true;
}


bool  
AosBuffArray::sort(const AosDataColOpr::E &opr)
{
	if (mNumRcds <= 0 || !mBuffRaw) return true;
	if (!mIsSorted)
	{
		sort();	
	}
	if (!mCompRaw->hasMerge()) return true;

	bool rslt = true;
	rslt = mergeData();
	/*
	switch (opr)
	{
		case AosDataColOpr::eNormal:
			rslt = mergeDataNorm();
			break;

		case AosDataColOpr::eNoUpdate:
			rslt = mergeDataNoUpdate();
			break;

		case AosDataColOpr::eSetValue:
			rslt = mergeDataSet();
			break;

		case AosDataColOpr::eMaxValue:
			rslt = mergeDataUpdateMax();
			break;

		case AosDataColOpr::eMinValue:
			rslt = mergeDataUpdateMin();
			break;

		default:
			OmnAlarm << "UNrecognized opr: " << opr << enderr;
			return false;
	}
	*/
	aos_assert_r(rslt, false);
	return rslt;
}


bool
AosBuffArray::mergeData()
{
	aos_assert_r(mBuffRaw, false);
	char * entry = mBuffRaw->data();
	char * crt_entry = &entry[mCompRaw->size];
	aos_assert_r(entry, false);

	char *data = NULL;
	int  len = 0;
	u32 idx = 0;
	u32 num_entries = mNumRcds;
	for (u32 i=1; i<num_entries; i++)
	{
		if (mCompRaw->cmp(entry, crt_entry) == 0)
		{
			mCompRaw->mergeData(entry, crt_entry, data, len);
		}
		else
		{
			idx++;
			entry = &entry[mCompRaw->size];
			if (entry != crt_entry)
			{
				memcpy(entry, crt_entry, mCompRaw->size);
			}
		}
		crt_entry = &crt_entry[mCompRaw->size];
	}
	mBuffRaw->setDataLen((idx + 1) * mCompRaw->size);	
	setNumEntries(idx + 1);
	return true;
}

/*
bool
AosBuffArray::mergeDataNorm()
{
	aos_assert_r(mBuffRaw, false);
	char * entry = mBuffRaw->data();
	char * crt_entry = &entry[mCompRaw->size];
	aos_assert_r(entry, false);

	u32 idx = 0;
	u32 num_entries = mNumRcds;
	for (u32 i=1; i<num_entries; i++)
	{
		if (mCompRaw->cmp(entry, crt_entry) == 0)
		{
			// *(u64 *)&entry[mCompRaw->size - sizeof(u64)] += *(u64 *)&crt_entry[mCompRaw->size - sizeof(u64)];
			mCompRaw->agr1(entry, crt_entry);
		}
		else
		{
			idx++;
			entry = &entry[mCompRaw->size];

			if (entry != crt_entry)
			{
				memcpy(entry, crt_entry, mCompRaw->size);
			//	mCompRaw->agr1(entry, crt_entry);
			//  mCompRaw->agr2(entry, crt_entry, mDataType);
				// *(u64 *)&entry[mCompRaw->size - sizeof(u64)] = *(u64 *)&crt_entry[mCompRaw->size - sizeof(u64)];
			}
		}
		crt_entry = &crt_entry[mCompRaw->size];
	}
	mBuffRaw->setDataLen((idx + 1) * mCompRaw->size);	
	setNumEntries(idx + 1);
	return true;
}


bool
AosBuffArray::mergeDataNoUpdate()
{
	aos_assert_r(mBuffRaw, false);
	char * entry = mBuffRaw->data();
	char * crt_entry = &entry[mCompRaw->size];
	aos_assert_r(entry, false);

	u32 idx = 0;
	u32 num_entries = getNumEntries();
	for (u32 i=1; i<num_entries; i++)
	{
		if (mCompRaw->cmp(entry, crt_entry) != 0)
		{
			idx++;
			entry = &entry[mCompRaw->size];

			if (entry != crt_entry)
			{
				memcpy(entry, crt_entry, mCompRaw->size);
				*(u64 *)&entry[mCompRaw->size - sizeof(u64)] = *(u64 *)&crt_entry[mCompRaw->size - sizeof(u64)];
			}
		}
		crt_entry = &crt_entry[mCompRaw->size];
	}
	
	mBuffRaw->setDataLen((idx + 1) * mCompRaw->size);	
	setNumEntries(idx + 1);
	return true;
}


bool
AosBuffArray::mergeDataSet()
{
	aos_assert_r(mBuffRaw, false);
	char * entry = mBuffRaw->data();
	char * crt_entry = &entry[mCompRaw->size];
	aos_assert_r(entry, false);

	u32 idx = 0;
	u32 num_entries = getNumEntries();
	for (u32 i=1; i<num_entries; i++)
	{
		if (mCompRaw->cmp(entry, crt_entry) != 0)
		{
			idx++;
			entry = &entry[mCompRaw->size];

			if (entry != crt_entry)
			{
				memcpy(entry, crt_entry, mCompRaw->size);
				*(u64 *)&entry[mCompRaw->size - sizeof(u64)] = *(u64 *)&crt_entry[mCompRaw->size - sizeof(u64)];
			}
		}
		else
		{
			*(u64 *)&entry[mCompRaw->size - sizeof(u64)] = *(u64 *)&crt_entry[mCompRaw->size - sizeof(u64)];
		}
		crt_entry = &crt_entry[mCompRaw->size];
	}
	
	mBuff->setDataLen((idx + 1) * mCompRaw->size);	
	setNumEntries(idx + 1);
	return true;
}


bool
AosBuffArray::mergeDataUpdateMax()
{
	aos_assert_r(mBuff, false);
	char * entry = mBuff->data();
	char * crt_entry = &entry[mComp->size];
	aos_assert_r(entry, false);

	u32 idx = 0;
	u32 num_entries = getNumEntries();
	for (u32 i=1; i<num_entries; i++)
	{
		if (mComp->cmp(entry, crt_entry) == 0)
		{
			u64 entry_docid = *(u64 *)&entry[mComp->size - sizeof(u64)];
			u64 crt_docid = *(u64 *)&crt_entry[mComp->size - sizeof(u64)];
			if (entry_docid < crt_docid)
			{
				*(u64 *)&entry[mComp->size - sizeof(u64)] = *(u64 *)&crt_entry[mComp->size - sizeof(u64)];
			}
		}
		else
		{
			idx++;
			entry = &entry[mComp->size];

			if (entry != crt_entry)
			{
				memcpy(entry, crt_entry, mComp->size);
				*(u64 *)&entry[mComp->size - sizeof(u64)] = *(u64 *)&crt_entry[mComp->size - sizeof(u64)];
			}
		}
		crt_entry = &crt_entry[mComp->size];
	}
	
	mBuff->setDataLen((idx + 1) * mComp->size);	
	setNumEntries(idx + 1);
	return true;
}


bool
AosBuffArray::mergeDataUpdateMin()
{
	aos_assert_r(mBuff, false);
	char * entry = mBuff->data();
	char * crt_entry = &entry[mComp->size];
	aos_assert_r(entry, false);

	u32 idx = 0;
	u32 num_entries = getNumEntries();
	for (u32 i=1; i<num_entries; i++)
	{
		if (mComp->cmp(entry, crt_entry) == 0)
		{
			u64 entry_docid = *(u64 *)&entry[mComp->size - sizeof(u64)];
			u64 crt_docid = *(u64 *)&crt_entry[mComp->size - sizeof(u64)];
			if (entry_docid > crt_docid)
			{
				*(u64 *)&entry[mComp->size - sizeof(u64)] = *(u64 *)&crt_entry[mComp->size - sizeof(u64)];
			}
		}
		else
		{
			idx++;
			entry = &entry[mComp->size];

			if (entry != crt_entry)
			{
				memcpy(entry, crt_entry, mComp->size);
				*(u64 *)&entry[mComp->size - sizeof(u64)] = *(u64 *)&crt_entry[mComp->size - sizeof(u64)];
			}
		}
		crt_entry = &crt_entry[mComp->size];
	}
	
	mBuff->setDataLen((idx + 1) * mComp->size);	
	setNumEntries(idx + 1);
	return true;
}

*/

bool 
AosBuffArray::serializeFromXml(const AosXmlTagPtr &xml)
{
	clear();
	
	mStable = xml->getAttrBool("stable");
	mAutoExtend = xml->getAttrBool("autoextend");
	mRcdLen = xml->getAttrInt("rcdlen", 0);
	mKeyLen = xml->getAttrInt("keylen", 0);
	mNumRcds = xml->getAttrI64("numrcds", 0);
	mItr = 0;
	mRangeItr = 0;
	mMemCap = mNumRcds * 2;
	if (mMemCap < eInitSize)
	{
		mMemCap = eInitSize;
	}
	
	mWithDocid = xml->getAttrBool("withdocid");
	mIsSorted = xml->getAttrBool("issorted");
	if (mNumRcds)
	{
		mBuff = xml->getNodeTextBinaryCopy("buff" AosMemoryCheckerArgs);
		mBuffRaw = mBuff.getPtr();
	}
	mCrtPos = mBuffRaw->data() + mNumRcds * mRcdLen;
	return true;
}

bool 
AosBuffArray::serializeToXml(AosXmlTagPtr &xml)
{
	OmnString str;
	str << "<buff_array ";

	if (mStable) str << "stable=\"" << mStable << "\" ";
	if (mAutoExtend) str << "autoextend=\"" << mAutoExtend<< "\" ";
	if (mRcdLen) str << "rcdlen=\"" << mRcdLen << "\" ";
	if (mKeyLen) str << "keylen=\"" << mKeyLen << "\" ";
	if (mNumRcds) str << "numrcds=\"" << mNumRcds << "\" ";

	if (mWithDocid) str << "withdocid=\"" << mWithDocid << "\" ";
	if (mIsSorted) str << "issorted=\"" << mIsSorted << "\" ";
		
	str << " >";

	// Notice: the data length in mBuff is not correct, use mRcdLen * mNumRcds
	u64 len = mRcdLen * mNumRcds;
	if (len)
	{
//	str << "<buff><![BDATA[" << mBuffRaw->dataLen() << ":";                                                            
//	str.append(mBuffRaw->data(), mBuffRaw->dataLen());
//	str << "]]></buff>";
		str << "<buff><![BDATA[" << len << ":";                                                            
		str.append(mBuffRaw->data(), len);
		str << "]]></buff>";
	}	
	str << "</buff_array>";

	AosXmlParser parser;
	xml = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	return true;
	
}

bool 
AosBuffArray::mergeFrom(
		const AosBuffArrayPtr &buff_array,
		const u64 &startidx,
		const u64 &len)
{
	int rcd_len = buff_array->getRecordLen();
	if (mRcdLen < rcd_len)
	{
		aos_assert_r(mAutoExtend, false);
		autoExtend(rcd_len, false);
	}
	else if (mRcdLen > rcd_len)
	{
		aos_assert_r(buff_array->mAutoExtend, false);
		buff_array->autoExtend(mRcdLen, false);
	}
	

	i64 num_docs = buff_array->getNumEntries();
	aos_assert_r(num_docs > (i64)startidx, false);
	if ((i64)startidx >= num_docs) return true;
	
	i64 num_add = (i64)len;
	if (num_add == 0) num_add = num_docs - startidx;
	if (num_add > (num_docs - startidx))
	{
		num_add = num_docs - startidx;
	}

	mLockRaw->lock();
	mCrtPos = mBuffRaw->data() + mNumRcds * mRcdLen;

	if (mNumRcds + num_add >= mMemCap)
	{
		bool rslt = expandMemoryPriv(mNumRcds, mNumRcds + num_add);
		aos_assert_rl(rslt, mLockRaw, false);
	}

	mNumRcds += num_add;

	memcpy(mCrtPos, buff_array->getEntry(startidx), num_add * mRcdLen);

	// mNumRcds += num_add;
	mCrtPos = mBuffRaw->data() + mNumRcds * mRcdLen;

	mLockRaw->unlock();
	return true;
}

bool 
AosBuffArray::shrinkFromHead(const u64 &num_remove)
{
	if (num_remove == 0) return true;
	aos_assert_r(mNumRcds >= (i64)num_remove, false);
	mNumRcds -= num_remove;

	mLockRaw->lock();
	char* start_pos = mBuffRaw->data();
	if (mNumRcds > 0)
	{
		memmove(start_pos, start_pos + num_remove * mRcdLen, mNumRcds * mRcdLen);
	}
	mCrtPos -= num_remove * mRcdLen;

	mLockRaw->unlock();

	return true;
}

bool
AosBuffArray::autoExtendLocked(const int target_size, const bool more_room)
{
	if (!mAutoExtend) return true;
	if (mRcdLen >= target_size) return true;
	// target_size is bigger now
	// calculate the new size
	mExtendCounter ++;
	int new_len = target_size;
	if (more_room)
	{
		new_len += mExtendCounter * eExtendStepSize;
	}

	aos_assert_r(new_len < eMaxRcdLen, false);

	AosBuffPtr buff;
	try
	{
		buff = OmnNew AosBuff(mMemCap * new_len + 100, 0 AosMemoryCheckerArgs);
	}

	catch(...)
	{
		OmnAlarm << "Failed to expand memory: " << mMemCap * new_len + 100 << enderr;
		return false;
	}
	
	if (mNumRcds > 0)
	{
		buff->setDataLen(mNumRcds * new_len);
		mCrtPos = buff->data();
		char* orig_data = mBuffRaw->data();
		for (i64 i = 0; i < mNumRcds; i++)
		{
			memcpy(mCrtPos, orig_data, mRcdLen);
			mCrtPos += new_len;
			orig_data += mRcdLen;
		}
	}
	else
	{
		mCrtPos = buff->data();
	}
	mCrtPos = buff->data() + mNumRcds * new_len;
	mBuff = buff;
	mBuffRaw = mBuff.getPtr();
	mRcdLen = new_len;
	mCompRaw->size = mRcdLen;

	calculateKeyLen();

	return true;
}

bool 
AosBuffArray::serializeFromBuff(const AosBuffPtr &buff)
{
	mStable = buff->getBool(false);
	mAutoExtend = buff->getBool(false);
	mCrtPos = 0;
	mBuff = buff->getAosBuff(true AosMemoryCheckerArgs);
	mBuffRaw = mBuff.getPtr();
	mRcdLen = buff->getInt(0);
	mKeyLen = buff->getInt(0);
	mNumRcds = buff->getI64(0);
	mItr = buff->getI64(0);
	mRangeItr = buff->getI64(0);	
	mMemCap = buff->getI64(0);
	mWithDocid = buff->getBool(false);
	mIsSorted = buff->getBool(0);
//?????????????????	AosStrValueInfo		mFieldInfo;
	mAutoExtend = buff->getBool(false);
	mExtendCounter = buff->getInt(0);
	
	return true;
}

bool 
AosBuffArray::serializeToBuff(const AosBuffPtr &buff)
{
	buff->setBool(mStable);
	buff->setBool(mAutoExtend);
	buff->setAosBuff(mBuff);
	buff->setInt(mRcdLen);
	buff->setInt(mKeyLen);
	buff->setI64(mNumRcds);
	buff->setI64(mItr);
	buff->setI64(mRangeItr);	
	buff->setI64(mMemCap);
	buff->setBool(mWithDocid);
	buff->setBool(mIsSorted);
//????????????????	AosStrValueInfo		mFieldInfo;
	buff->setBool(mAutoExtend);
	buff->setInt(mExtendCounter);
	
	return true;	
}




