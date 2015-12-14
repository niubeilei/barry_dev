////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Each record is stored in the following format:
// 	<docsize>		4 bytes (including itself)
// 	<flag>			4 bytes (for error checking)
// 	<reserved>		eReservedRcdSize
// 	<body>			variable
// Modification History:
// 07/18/2010 Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DiskBlock_h
#define AOS_DocFileMgr_DiskBlock_h


#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "Alarm/Alarm.h"
#include "API/AosApiC.h"
#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/DfmUtil.h"
#include "ReliableFile/Ptrs.h"
#include "StorageMgrUtil/SignatureAlgo.h"
#include "Thread/Ptrs.h"
#include "Thread/CondVar.h"
#include "Util/RCObject.h"
#include "Debug/Debug.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/File.h"

#define AosDiskBlockCheck sanityCheck()

class AosDiskBlock2 : virtual public OmnRCObject
{
	OmnDefineRCObject;
	static bool smSanityCheck;
	
public:
	struct BlockId
	{
		u32 virtualId;
		u32 dfmId;
		u32 fileSeqno;
		u32 blockStart;
		u32 blockSize;

		bool operator < (const BlockId &rhs) const
		{
			return memcmp(this, &rhs, sizeof(BlockId)) < 0;
		}
		
		bool operator != (const BlockId &rhs) const
		{
			return memcmp(this, &rhs, sizeof(BlockId)) != 0;
		}
	};

public:
	enum
	{
		eMaxRecords = 2500,
		eGroupSize = 50,
		
		eFlagByte0 = 0x45,
		eFlagByte1 = 0x8d,
		eFlagByte2 = 0xac,
		eFlagByte3 = 0x7d,

		//eBlockFlagByte0 = 0x29,
		//eBlockFlagByte1 = 0xa3,
		//eBlockFlagByte2 = 0xd1,
		//eBlockFlagByte3 = 0x2c,
		
		//eReservedRcdSize = 20,
		eReservedRcdSize = 5,

		//eRcdDocsizeOffset 	= 0,		// Ketty 2013/01/26
		//eRcdFlagOffset 		= eRcdDocsizeOffset + 4,
		eRcdOrigLenOffset = 0,
		eRcdCompressLenOffset = eRcdOrigLenOffset + 4,
		eRcdFlagOffset 		= eRcdCompressLenOffset + 4,
		eRcdEntryidOffset	= eRcdFlagOffset + 4,
		eRcdReservedOffset 	= eRcdEntryidOffset + 4,
		eRcdBodyOffset 		= eRcdReservedOffset + eReservedRcdSize,
		eRcdHeaderSize		= eRcdBodyOffset,

		//eBlkReserved		= 100,
		eBlkReserved		= 20,
		eSignAlgo			= AosSignatureAlgo::eMD5,
		eSignLen 			= AosSignatureAlgo::eMD5Len,		// Ketty 2012/09/10
		
		eSignOffset			= 0,		// Ketty 2012/09/10
		eBlockSizeOffset 	= eSignOffset + eSignLen,
		//eBlockFlagOffset 	= eBlockSizeOffset + 4,		// Ketty 2012/09/10
		//eNumRcdsOffset      = eBlockFlagOffset + 4,
		eNumRcdsOffset      = eBlockSizeOffset + 4,
		eBuffSizeOffset     = eNumRcdsOffset + 4,
		eBuffStartOffset    = eBuffSizeOffset + 4,
		eBlkReservedOffset	= eBuffStartOffset + 4,
		eIdxStart			= eBlkReservedOffset + eBlkReserved,

		eBodyStart			= eIdxStart + sizeof(u32) * (eMaxRecords/eGroupSize+1),
	
	};

private:
	//u32				mVirtualId;
	//u32				mDocFileMgrId;
	BlockId			mBlockId;
	//u32				mBlocksize;
	//u64				mBlockStart;
	unsigned char *	mData;
	//OmnFilePtr		mFile;
	AosReliableFilePtr mFile;
	u32			  * mGroupIdx;
	bool 			mIsGood;
	//u32 			mSeqno;
	OmnMutexPtr     mLock;
	OmnCondVarPtr   mCondVar;
	int				mRefCount;
	AosDiskBlock2Ptr	mPrev;
	AosDiskBlock2Ptr	mNext;

public:
	AosDiskBlock2(
			const AosRundataPtr &rdata,
			const BlockId &block_id,
			//const u32 virtual_id,
			//const u32 docFileMgrId,
			//const OmnFilePtr &file,
			const AosReliableFilePtr &file,
			//const u32 seqno,
			//const u64 &offset,
			//const u32 blocksize,
			const bool flag);

	~AosDiskBlock2();

	// Ketty 2012/09/26
	//static BlockId  composeBlockId(const u32 virtual_id, const u32 dfm_id, 
	//		const u32 fseqno, const u32 foffset, const u32 block_size)
	//{
	//	u32 block_start = (foffset/block_size) * block_size; 
	//	
	//	BlockId id = {virtual_id, dfm_id, fseqno, block_start, block_size};
	//	return id;
	//}


	//bool readRcd(const u64 &offset, char *data, const u32 expected_size);
	bool readRcd(
			const u64 &offset, 
			AosBuffPtr &data_buff,
			u32 &orig_len,
			u32 &compress_len);
	u64  addRcd(const AosRundataPtr &rdata, 
			//const u32 docsize, 
			const char *data,
			const u32 orig_len,
			const u32 compress_len);
	bool modifyRcd(
			const AosRundataPtr &rdata,
			u64 &offset,
			const char *data, 
			bool &kickout,
			const u32 orig_len,
			const u32 compress_len);
	bool deleteRcd(const AosRundataPtr &rdata, const u64 &offset);
	bool reload(const AosRundataPtr &rdata,
				const BlockId &block_id,
				//const u32 virtual_id,
				//const u32 docFileMgrId,
				//const OmnFilePtr &file,
				const AosReliableFilePtr &file,
				//const u32 seqno,
				//const u64 &offset, 
				//const u32 blocksize,
				const bool createnew);

	bool saveToDiskPublic(const AosRundataPtr &rdata)
	{
	 	mLock->lock();
	 	bool rslt = saveToDiskPriv(rdata);
	 	mLock->unlock();
	 	return rslt;
	}
	int	 getReference() const {return mRefCount;}
	int  addReference() 
	{
		mLock->lock(); 
		int ref = mRefCount++; 
		mLock->unlock();
		return ref;
	}
	bool isIdle() const { return mRefCount == 0; }
	//u32  getVirtualId() const { return mVirtualId; }
	//u32  getDocFileMgrId() const { return mDocFileMgrId; }
	BlockId	getBlockId() const { return mBlockId; };
	u32  getSeqno() const { return mBlockId.fileSeqno; }
	u32  getBlockStart() const {return mBlockId.blockStart;}
	u32  getBlockSize() const {return mBlockId.blockSize;}
	OmnString toString() const
	{
		OmnString str;
		//str << "virtual_id: " << mVirtualId
		//	<< ", dfm_id:" << mDocFileMgrId
		str	<< ", BlockSize: " << getBlockSize() 
			<< ", BlockStart: " << getBlockStart() 
			<< ", Seqno: " << getSeqno() 
			<< ", RefCount: " << mRefCount;
		//if (mFile) str << ", Filename: " << mFile->getFileName();
		return str;
	}

	void removeReference() 
	{
		mLock->lock();
		if(mRefCount == 0)
		{
			OmnAlarm << "the Ref is already 0" << enderr;
			return;
		}
		mRefCount--;
		if(mRefCount == 0) mCondVar->signal();
		mLock->unlock();
	}

	inline void setPrev(const AosDiskBlock2Ptr &p) {mPrev = p;}
	inline void setNext(const AosDiskBlock2Ptr &n) {mNext = n;}
	inline AosDiskBlock2Ptr getPrev() const {return mPrev;}
	inline AosDiskBlock2Ptr getNext() const {return mNext;}
	bool removeFromList()
	{
		if (mPrev) mPrev->setNext(mNext);
		if (mNext) mNext->setPrev(mPrev);
		if(mPrev == this && mNext == this)	return true;	// Ketty 2012/02/21
	
		mPrev = 0;
		mNext = 0;
		return true;
	}

	bool addToList(const AosDiskBlock2Ptr &node)
	{
		// It adds itself in front of 'node'
		aos_assert_r(node, false);
		AosDiskBlock2Ptr thisptr(this, false);
		mPrev = node->getPrev();
		if (mPrev) mPrev->setNext(thisptr);
		mNext = node;
		node->setPrev(thisptr);
		return true;
	}

	// Ketty 2012/09/22
	bool 	isNeedKickOut(const AosRundataPtr &rdata, 
				const u64 offset,
				const u64 new_docsize, 
				bool &need_kickout);
	
	// Ketty 2014/02/21
	u64	getFileId();

private:
	bool saveToDiskPriv(const AosRundataPtr &rdata);
	bool canFitNewRcd(const u32 newsize)
	{
		mLock->lock();
		u32 buffsize = getBufferSize();
		mLock ->unlock();
		return (buffsize >= newsize);
	}

	bool blockMatch(const u32 seqno, u32 blockstart, const u32 blocksize)
	{
		return ( seqno == getSeqno() && blockstart == getBlockStart() && blocksize == getBlockSize());
	}


	bool createNewBlock();
	bool readBlock(const AosRundataPtr &rdata);
	bool moveBackward(const u32 crtpos, const u32 delta);
	bool moveForward( const u64 &offset, const u32 crtpos, const u32 delta, bool &kickedout);
	u32 getU32(const u32 crtpos) const
	{
		aos_assert_r(crtpos< getBlockSize(), false);
		u32 value = (mData[crtpos] << 24) +
			(mData[crtpos+1] << 16) +
			(mData[crtpos+2] << 8) +
			 mData[crtpos+3];
		return value;
	}

	bool setU32(const u32 crtpos, const u32 value)
	{
		aos_assert_r(crtpos< getBlockSize(), false);
		mData[crtpos]  = (unsigned char)(value >> 24);
		mData[crtpos+1] = (unsigned char)(value >> 16);
		mData[crtpos+2] = (unsigned char)(value >> 8);
		mData[crtpos+3] = (unsigned char)value;
		return true;
	}

	bool isValidRcd(const u32 crtpos) const
	{
		// A doc is valid if its flag is correct
		aos_assert_r(crtpos < getBlockSize(), false);
		u32 pp = crtpos + eRcdFlagOffset;
		aos_assert_r(mData[pp] == eFlagByte0 &&
				mData[pp+1] == eFlagByte1 &&
				mData[pp+2] == eFlagByte2 &&
				mData[pp+3] == eFlagByte3, false);
		return true;
	}

	// Ketty 2013/01/26
	u32 getDocsize(const u32 crtpos) const 
	{
		//return getU32(crtpos + eRcdDocsizeOffset);
		u32 orig_len = getRcdOrigLen(crtpos);
		u32 compress_len = getRcdCompressLen(crtpos);
		return compress_len ? compress_len : orig_len;
	}
	u32 getRcdOrigLen(const u32 crtpos) const  
	{
		return getU32(crtpos + eRcdOrigLenOffset);
	}
	u32 getRcdCompressLen(const u32 crtpos) const
	{
		return getU32(crtpos + eRcdCompressLenOffset);
	}

	u32 getRcdDisksize(const u32 crtpos) const
	{
		return getDocsize(crtpos) + eRcdHeaderSize;
	}

	u32 getBufferSize() const
	{
		return getU32(eBuffSizeOffset);
	}

	u32 getNextRcdStart(const u32 crtpos) const
	{
		if (getU32(crtpos + eRcdEntryidOffset)>>31 & 1) return crtpos;
		//if (getEntryId(crtpos) == 0xffffffff) return crtpos;
		return getRcdDisksize(crtpos) + crtpos;
	}

	u32 getBufferStart() const
	{
		return getU32(eBuffStartOffset);
	}

	void setBufferSize(const u32 bsize)
	{
		setU32(eBuffSizeOffset, bsize);
	}

	void setBufferStart(const u32 bsize)
	{
		setU32(eBuffStartOffset, bsize);
	}

	void setBlocksize(const u32 blksize)
	{
		setU32(eBlockSizeOffset, blksize);
	}

	void setNumRcd(const u32 rcdnum)
	{
		setU32(eNumRcdsOffset, rcdnum);
	}

	// Ketty 2013/01/26
	//void setRcdDocsize(const u32 crtpos, const u32 size)
	//{
	//	setU32(crtpos+eRcdDocsizeOffset, size);
	//}
	void setRcdOrigLen(const u32 crtpos, const u32 size)
	{
		setU32(crtpos+eRcdOrigLenOffset, size);
	}
	void setRcdCompressLen(const u32 crtpos, const u32 size)
	{
		setU32(crtpos+eRcdCompressLenOffset, size);
	}

	void increaseBuffer(const u32 delta)
	{
		// Some docs shrunk its size, resulting some more 
		// buffer space. This function increases the buffer
		// size, which means:
		// 1. Move back the buffer record 'delta' numebr of bytes
		// 2. Modify the buffer record
		// 3. Modify the block control record: 
		// 		buffer start pos
		// 		buffer size
		u32 buffsize = getBufferSize() + delta;
		setBufferSize(buffsize);
		u32 buffstart = getBufferStart();
		setBufferStart(buffstart - delta);
		memmove(&mData[buffstart-delta], &mData[buffstart], eRcdHeaderSize);
	}
	bool decreaseBuffer(const u32 delta)
	{
		u32 buffersize = getBufferSize();
		aos_assert_r(buffersize >= delta, false);
		buffersize -= delta;
		setBufferSize(buffersize);
		u32 buffstart = getBufferStart();
		setBufferStart(buffstart + delta);
		aos_assert_r(buffstart + delta + eRcdHeaderSize <= getBlockSize(), false);
		memmove(&mData[buffstart+delta], &mData[buffstart], eRcdHeaderSize);
		return true;
	}

	void setReserved(const u32 crtpos){}

	void setFlag(const u32 crtpos)
	{
		mData[crtpos+eRcdFlagOffset]   = eFlagByte0;
		mData[crtpos+eRcdFlagOffset+1] = eFlagByte1;
		mData[crtpos+eRcdFlagOffset+2] = eFlagByte2;
		mData[crtpos+eRcdFlagOffset+3] = eFlagByte3;
	}

	void setEntryId(const u32 crtpos, const u32 entry_id)
	{
		setU32(crtpos+ eRcdEntryidOffset, entry_id);
	}

	u32 getEntryId(const u32 crtpos)
	{
		return getU32(crtpos+ eRcdEntryidOffset);
	}

	u32 getNewEntryid()
	{
		u32 entry_id = getU32(eNumRcdsOffset);
		return entry_id;
	}

	u32 getNumRcd() const
	{
		return getU32(eNumRcdsOffset);
	}

	bool updateIdx(
		const u32 crtpos,
		const int delta);

	u64	convertToRcdStart(const u64 &offset);

	bool sanityCheck() const;
	
	//bool isBlockGood() const
	//{
	//	return (mData[eBlockFlagOffset] == eBlockFlagByte0 &&
	//	        mData[eBlockFlagOffset+1] == eBlockFlagByte1 &&
	//	        mData[eBlockFlagOffset+2] == eBlockFlagByte2 &&
	//	        mData[eBlockFlagOffset+3] == eBlockFlagByte3);
	//}
	bool checkSaving(
				const AosRundataPtr &rdata,
				const u64 &offset, 
				const char *data, 
				const u32 docsize);

	// Ketty 2012/09/10
	void setSign()
	{
		// The signature is save in the block header.
		char * sign_data = (char *)mData + eSignLen;
		u32 data_len = getBlockSize() - eSignLen;
		OmnString signature = AosCalculateSign((AosSignatureAlgo::E)eSignAlgo, sign_data, data_len);
		memcpy(&mData[eSignOffset], signature.data(), eSignLen);
	}

	// Ketty 2012/09/10
	bool checkSign()
	{
		// The signature is save in the block header.
		char * sign_data = (char *)mData + eSignLen;
		u32 data_len = getBlockSize() - eSignLen;
		OmnString cal_sign = AosCalculateSign((AosSignatureAlgo::E)eSignAlgo, sign_data, data_len);
		return memcmp(&mData[eSignOffset], cal_sign.data(), eSignLen) == 0;
	}
	
};
#endif

