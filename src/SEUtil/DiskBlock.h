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
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_DiskBlock_h
#define AOS_SEUtil_DiskBlock_h


#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "Alarm/Alarm.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Debug/Debug.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/File.h"

class AosDiskBlock : virtual public OmnRCObject
{
	OmnDefineRCObject;
	const static bool mSanityCheck = false;
public:
	enum
	{
		eMaxDiskBlockSize = 10000000,
		eMaxRecords = 2500,
		eGroupSize = 50,
		eBlockSize = 500000,
		eFlagByte0 = 0x45,
		eFlagByte1 = 0x8d,
		eFlagByte2 = 0xac,
		eFlagByte3 = 0x7d,

		eBlockFlagByte0 = 0x29,
		eBlockFlagByte1 = 0xa3,
		eBlockFlagByte2 = 0xd1,
		eBlockFlagByte3 = 0x2c,

		eReservedRcdSize = 20,

		eRcdDocsizeOffset 	= 0,
		eRcdFlagOffset 		= eRcdDocsizeOffset + 4,
		eRcdEntryidOffset	= eRcdFlagOffset + 4,
		eRcdReservedOffset 	= eRcdEntryidOffset + 4,
		eRcdBodyOffset 		= eRcdReservedOffset + eReservedRcdSize,
		eRcdHeaderSize		= eRcdBodyOffset,

		eBlkReserved		= 100,
		eBlockSizeOffset 	= 0,
		eBlockFlagOffset 	= eBlockSizeOffset + 4,
		eNumRcdsOffset      = eBlockFlagOffset + 4,
		eBuffSizeOffset     = eNumRcdsOffset + 4,
		eBuffStartOffset    = eBuffSizeOffset + 4,
		eReservedOffset     = eBuffStartOffset + 4,
		eBlkReservedOffset  = eReservedOffset + 4,
		eIdxStart			= eBlkReservedOffset + eBlkReserved,

		eBodyStart			= eIdxStart + sizeof(u32) * (eMaxRecords/eGroupSize+1),
	};

private:
	u32				mBlocksize;
	u64				mBlockStart;
	unsigned char *	mData;
	OmnFilePtr		mFile;
	u32			  * mGroupIdx;
	bool 			mIsGood;
	u32 			mSeqno;
	OmnMutexPtr     mLock;
	int				mRefCount;

public:
	AosDiskBlock(
			const OmnFilePtr &file,
			const u32 seqno,
			const u64 &offset,
			const u32 blocksize,
			const bool flag);

	~AosDiskBlock();

	u32 getBlockStart() const
	{
		return mBlockStart; 
	}

	u32 getBlocksize() const
	{
		return mBlocksize; 
	}

	u32 getSeqno() const
	{
		return mSeqno; 
	}


	u64 addRcd(const u32 docsize, const char *data);

	bool canFitNewRcd(const u32 newsize)
	{
		mLock->lock();
		u32 buffsize = getBufferSize();
		mLock ->unlock();
		return (buffsize >= newsize);
	}


	bool saveToDisk(bool needLock)
	{
		if (needLock) mLock->lock();
		if (!(sanityCheck()))
		{
			OmnAlarm << "Failed to check!" << enderr;
			if (needLock) mLock->unlock();

		}
		//aos_assert_rl(sanityCheck(),mLock, false);

		if (!mFile)
		{
			OmnAlarm << "Failed to read the file!" << enderr;
			if (needLock) mLock->unlock();
		}
		//aos_assert_rl(mFile, mLock, false);
		mFile->lock();
		mFile->put(mBlockStart, (char *)mData, mBlocksize, true);
		mFile->unlock();
		if (needLock) mLock->unlock();
		return true;
	}

	bool readRcd(const u64 &offset, char *data, const u32 expected_size);

	bool modifyRcd(
		u64 &offset, 
		const u32 &newsize, 
		const char *data,
		bool &kickout);


	bool deleteRcd(const u64 &offset);

	/*
	static bool
	readDoc(
		const OmnFilePtr file, 
		const u32 offset, 
		char *data, 
		const u32 expected_size);
		*/

	bool blockMatch(const u32 seqno, u32 blockstart, const u32 blocksize)
	{

		return (seqno == mSeqno && blockstart == mBlockStart && blocksize == mBlocksize);
	}
	
	bool isIdle() const {return mRefCount == 0;}

	void removeReference() {mRefCount--;}
	void addReference()
		{
			mRefCount++;
		}
	int	 getReference() const {return mRefCount;}

	bool reload(
		const OmnFilePtr &file,
		const u32 seqno,
		const u64 &offset, 
		const u32 blocksize,
		const bool createnew);

private:
	bool
	createNewBlock();
	
	bool
	readBlock();

	bool 
	moveBackward(
			const u32 crtpos, 
			const u32 delta);

	bool 
	moveForward(
			const u64 &offset,
			const u32 crtpos, 
			const u32 delta,
			bool &kickedout);

	u32 getU32(const u32 crtpos)
	{
if (crtpos >= mBlocksize)
	OmnMark;
		aos_assert_r(crtpos< mBlocksize, false);
		u32 value = (mData[crtpos] << 24) +
			(mData[crtpos+1] << 16) +
			(mData[crtpos+2] << 8) +
			 mData[crtpos+3];
		return value;
	}

	bool setU32(const u32 crtpos, const u32 value)
	{
		aos_assert_r(crtpos< mBlocksize, false);
		mData[crtpos]  = (unsigned char)(value >> 24);
		mData[crtpos+1] = (unsigned char)(value >> 16);
		mData[crtpos+2] = (unsigned char)(value >> 8);
		mData[crtpos+3] = (unsigned char)value;
		return true;
	}

	bool isValidRcd(const u32 crtpos)
	{
		// A doc is valid if its flag is correct
		aos_assert_r(crtpos < mBlocksize, false);
		u32 pp = crtpos + eRcdFlagOffset;
if (!(mData[pp] == eFlagByte0 && mData[pp+1] == eFlagByte1 &&mData[pp+2] == eFlagByte2 && mData[pp+3] == eFlagByte3))
	OmnMark;
		aos_assert_r(mData[pp] == eFlagByte0 &&
				mData[pp+1] == eFlagByte1 &&
				mData[pp+2] == eFlagByte2 &&
				mData[pp+3] == eFlagByte3, false);
		return true;
	}

	u32 getDocsize(const u32 crtpos)
	{
		return getU32(crtpos + eRcdDocsizeOffset);
	}

	u32 getRcdDisksize(const u32 crtpos)
	{
		return getDocsize(crtpos) + eRcdHeaderSize;
	}

	u32 getBufferSize()
	{
		return getU32(eBuffSizeOffset);
	}

	u32 getNextRcdStart(const u32 crtpos)
	{
		if (getU32(crtpos + eRcdEntryidOffset)>>31 & 1) return crtpos;
		//if (getEntryId(crtpos) == 0xffffffff) return crtpos;
		return getRcdDisksize(crtpos) + crtpos;
	}

	u32 getBufferStart()
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

	void setRcdDocsize(const u32 crtpos, const u32 size)
	{
		setU32(crtpos+eRcdDocsizeOffset, size);
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
//OmnScreen << "buffstart:"<<buffstart << "  buffstart + delta:"<<buffstart + delta << "  buffersize :"<<endl;
		aos_assert_r(buffstart + delta + eRcdHeaderSize <= mBlocksize, false);
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

	u32 getNumRcd()
	{
		return getU32(eNumRcdsOffset);
	}

	bool updateIdx(
		const u32 crtpos,
		const int delta);

	u64	convertToRcdStart(const u64 &offset);

	bool sanityCheck()
	{
		if(!mSanityCheck)
		{
			return true;
		}
		//Check Block
		u32 ee = getU32(eBlockSizeOffset);
		if ( ee != mBlocksize)
			OmnMark;
		aos_assert_r(getU32(eBlockSizeOffset) == mBlocksize, false);
		aos_assert_r(isBlockGood(), false);
		aos_assert_r(mBlockStart% mBlocksize == 0, false);
		aos_assert_r(getBufferSize()+getBufferStart() + eRcdHeaderSize == mBlocksize,false);
		aos_assert_r(mData[eBlockFlagOffset]==eBlockFlagByte0 &&
				mData[eBlockFlagOffset+1]==eBlockFlagByte1 &&
				mData[eBlockFlagOffset+2]==eBlockFlagByte2 &&
				mData[eBlockFlagOffset+3]==eBlockFlagByte3, false);

		//Check Records
		u32 numrcds = getNumRcd();
		u32 pos = eBodyStart;
		for (u32 i=0; i<numrcds; i++)
		{

			if (i % eGroupSize == 0)
			{
if(!(pos == mGroupIdx[i/eGroupSize]))
	OmnMark;
				aos_assert_r(pos == mGroupIdx[i/eGroupSize], false);
			}
			aos_assert_r(isValidRcd(pos), false);
			pos += getRcdDisksize(pos);
		}
		return true;
	}

	bool isBlockGood()
	{
		return (mData[eBlockFlagOffset] == eBlockFlagByte0 &&
		        mData[eBlockFlagOffset+1] == eBlockFlagByte1 &&
		        mData[eBlockFlagOffset+2] == eBlockFlagByte2 &&
		        mData[eBlockFlagOffset+3] == eBlockFlagByte3);
	}

};
#endif
