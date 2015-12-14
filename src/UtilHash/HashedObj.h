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
// 09/04/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_UtilHash_HashedObj_h
#define AOS_UtilHash_HashedObj_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "UtilHash/Ptrs.h"

class AosHashedObj : virtual public OmnRCObject 
{
public:
	enum Type
	{
		eInvalid,

		eUnknown,
		eU64,
		eSeqnoFnameEntry,
		eBuff,

		eMax
	};

	enum
	{
		eLengthSize = sizeof(u32),
		ePoisonSize = sizeof(u32),
		eContinueSize = sizeof(u64),
		eEmptyStrSize = sizeof(int),
		eFrontPoisonOffset = sizeof(u32),

		eEmptyRecordSize = eLengthSize + ePoisonSize + eEmptyStrSize
			+ eContinueSize + ePoisonSize + eLengthSize,

		eMaxNumRecords = 10000, 
		eFrontPoison = 345543234,
		eEndPoison = 988907655,
		eBuffInitSize = 500,
		eBuffIncSize = 500
	};


protected:
	OmnMutexPtr			mLock;
	AosHashedObjPtr		mPrev;
	AosHashedObjPtr		mNext;
	OmnString			mKey;
	Type				mType;

private:
	bool				mExist;

public:
	AosHashedObj(const Type type = eUnknown)
	:
	mLock(OmnNew OmnMutex()),
	mType(type),
	mExist(false)
	{
	}

	AosHashedObj(
			const OmnString &key,
			const Type type)
	:
	mLock(OmnNew OmnMutex()),
	mKey(key),
	mType(type),
	mExist(false)
	{
	}

	~AosHashedObj(){}

	virtual bool isSame(const AosHashedObjPtr &rhs) const = 0;
	
	virtual int  doesRecordMatch(
			AosBuff &buff,
			const OmnString &key_in_buff,
			const OmnString &expected_key,
			AosHashedObjPtr &obj) = 0;

	virtual int setContentsToBuff(AosBuff &buff) = 0;

	virtual AosHashedObjPtr clone() = 0;
	virtual bool reset() = 0;

	inline void removeFromLinkedList()
	{
		// It assumes this tag is in a linked list. It removes itself
		// from the linked list.
		if (mNext) mNext->mPrev = mPrev;
		if (mPrev) mPrev->mNext = mNext;
	}
	
	inline bool insertToLinkedList(const AosHashedObjPtr &node)
	{
		// It inserts itself to the front of 'node'
		aos_assert_r(node, false);
		mPrev = node->mPrev;
		if (mPrev) mPrev->mNext = this;
		mNext = node;
		node->mPrev = this;
		return true;
	}

	inline void replaceNode(const AosHashedObjPtr &node)
	{
		// It replaces the node 'node'
		mNext = node->mNext;
		mNext->mPrev = this; 

		mPrev = node->mPrev;
		mPrev->mNext = this;

		node->mNext = 0;
		node->mPrev = 0;
	}

	inline void setLinksToSelf()
	{
		mNext = this;
		mPrev = this;
	}

	inline void resetLinks()
	{
		mNext = 0;
		mPrev = 0;
	}

	AosHashedObjPtr getPrev() const {return mPrev;}
	AosHashedObjPtr getNext() const {return mNext;}
	OmnString getKeyStr() const {return mKey;}
	void setKeyStr(const OmnString &key) { mKey = key;}
	bool isKeySame(const AosHashedObjPtr &rhs) const
	{
		aos_assert_r(rhs, false);
		return mKey == rhs->mKey;
	}

	Type getType() const {return mType;}
	bool isExist() const {return mExist;}
	void setExist(bool flag) { mExist = flag;}
	bool readObj(
	        //const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
	        const u64 &offset,
	        const OmnString &key,
	        const int bucket_size,
	        AosHashedObjPtr &obj,
			const AosRundataPtr &rdata);

	bool addObj(
	        //const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
			const u64 &offset,
			const int bucket_size,
			const AosRundataPtr &rdata);

	bool deleteObj(
	        //const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
			const u64 &offset,
			const OmnString &key,
			const int bucket_size,
			const AosRundataPtr &rdata);

	bool modifyContentsToFile(
	        //const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
        	const u64 &offset,
        	const int bucket_size,
			const AosRundataPtr &rdata);
	
protected:
	bool readToBuff(
			AosBuff &buff, 
			bool &isEmpty,
	        //const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
			const u64 &offset,
			const int bucket_size,
			const AosRundataPtr &rdata);

	AosBuffPtr getInsertBucket(
	        //const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
			const u64 &offset, 
			const int len,
			const int bucket_size,
			u64 &insertPos,
			const AosRundataPtr &rdata);

	bool insertData(
			const AosBuffPtr &buff, 
			const int len,
	        //const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
			const u64 &offset,
			const AosRundataPtr &rdata);

	AosBuffPtr allocateNewBucket(
	        //const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
 	        u64 &offset,
			const int bucket_len,
	        const int bucket_size,
			const AosRundataPtr &rdata);

	inline u32 getEndSizeOffset()
	{
		// Bucket format is:
		// length          (u32)
		// front-poison    (u32)
		// contents        (variable)
		// empty string	   (sizeof(int))
		// continue_field  (u64)
		// end-poison      (u32)
		// length          (u32)
		return eLengthSize + ePoisonSize + eContinueSize + eEmptyStrSize;
	}

	inline u32 getContinueOffset()
	{
		return eLengthSize + ePoisonSize + eContinueSize;
	}

 	bool isEmptyBucket(
            const AosBuff &buff,
            const u32 datalen,
			const int &bucket_size)
    {
		aos_assert_r(bucket_size >= 0, true);
		u32 buff_datalen = buff.dataLen();
        if (buff_datalen == 0 || buff_datalen > (u32)bucket_size) return true;
        if (datalen == 0 || datalen > (u32)bucket_size) return true;
        if (buff.getU32(eFrontPoisonOffset, 0) != eFrontPoison) return true;
        if (datalen < eEmptyRecordSize) return true;
        if (buff.getU32(datalen - eLengthSize - ePoisonSize, 0) != eEndPoison) return true;
        if (buff.getU32(datalen - eLengthSize, 0) != datalen) return true;
        return false;
    }


	inline u32 getRemainingSpace(const int bucket_len, const int max_bucket_size)
	{
		return 0;
 	}

	bool modifyRecord(
			const u64 &crt_offset,
			const OmnString &key,
			AosBuff &oldbuff, 
			AosBuff &newbuff, 
			AosBuff &buff, 
			const int pos, 
	        //const OmnFilePtr &file,		// Ketty 2012/11/29
			const AosReliableFilePtr &file, 
			const int bucket_size,
			const AosRundataPtr &rdata);
};
#endif
