////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This class manages idle blocks. Idle blocks are defined in form
// of Idle Block Entry. Each idle block entry is a sequence of
// blocks (each block is fixed size, currently 1M). Initially,
// there are no idle blocks. When opening a new file, it adds an
// entry for the entire file.
//
// 09/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_IdleBlockMgr_h
#define Aos_SEUtil_IdleBlockMgr_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "alarm_c/alarm.h"
#include "Util/File.h"
#include "Thread/Ptrs.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"

struct Area
{
	u32 blocks;
	u32 seqno;
	u64 offset;
	u32 flag;
}__attribute__ ((packed));

struct AreaSeqno
{
	u32 idx;
	u32 num;
	u32 seqno;
	u64 offset;
	u32 size;
	u32 flag;
}__attribute__ ((packed));

class AosIdleBlockMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eNumBlockOffset = 0,
		eSeqnoOffset = 4,
		eSeqnoNum = 8,
		eOffsetOffset = 12,
		eFlagOffset = 16,
		eSizeOffset = 20,
		eSeqnoFlag = 24,
		eEntryFlag = 0xa4bfd314,

		eIdxSize = 100,  //100M
		eStartFlag = 0xa4bfd315,
		eEndFlag   = 0xa4bfd316,
		eBlockSize = 1000000,	// 1M
		// area
		eSeqnoStartAddr = 50,
		eAreaStartAddr = 100,
		eMaxAreas = 110,  // 11G/100M
		eMaxAreaEntries = 1000,
		eAreaSize = 100000, // eMaxAreaEntries * eEntrySize
		// entry
		eMaxNumEntries = 100, // read to memory
		eEntrySize = 100,
		eSeqnoNums = 20,
		// doc
		eMaxFiles = 2000,
		eFileHead = 10


	};

	OmnFilePtr		mIdxFile; // sequence by idx
	OmnFilePtr		mSeqnoFile; // sequence by seqno
	OmnFilePtr		mFiles[eMaxFiles]; // DocFile by seqno
	OmnMutexPtr     mLock;
	int             mTotal[eMaxAreas];
	int             mSTotal[eMaxAreas];
	Area            mAreas[eMaxAreas][eMaxAreaEntries];
	AreaSeqno       mSAreas[eMaxFiles][eMaxAreaEntries];
	OmnString       mDirname;
	OmnString       mFilename;
	OmnString       mSFilename;
	OmnString		mDocFilename;
	u64				mMaxFileSize;
	char *          mBuffs;
public:
	AosIdleBlockMgr(const OmnString &dirname, const OmnString &fname);
	~AosIdleBlockMgr();

	bool init(const OmnString &dirname, const OmnString &fname);
	
	bool saveDoc(
			u32 &seqno, 
			u64 &offset, 
			const u32 &docsize, 
			const char *data)
	{
		if (offset == 0) return appendDoc(data, docsize, seqno, offset);
		return modifyDoc(data, docsize, seqno, offset);
	};
	
	bool readDoc(const u32 seqno, const u64 &offset, const int length, char *data);
	bool deleteDoc(u32 seqno, u64 offset);

private:
	bool appendDoc(const char *data, const u32 size, u32 &seqno, u64 &offset);
	bool modifyDoc(const char *data, const u32 bytes, u32 &seqno, u64 &offset);
	bool getSpace(const u32 num_bytes, u32 &seqno, u64 &offset);
	bool getSpace(u32 idx, const u32 num_bytes, u32 &seqno, u64 &offset);

	u32 convertBytesToSize(u32 num_bytes);
	u32 convertToSizeIdx(const u32 size);

	bool createNewFile(const u32 size, u32 &seqno, u64 &offset);


	bool appendEntry(const u32 size,  const u32 seqno, const u64 offset);
	bool updateEntry(const u32 area_idx, const u32 entry_idx);

	bool readArea(const u32 idx);
	bool readSArea(const u32 seqno);

	bool returnSpaceSafe(const u32 size, const u32 seqno, const u64 offset);
	bool returnEntry(const u32 area_idx, const u32 size, const u32 seqno, const u64 offset);



	u32 getLastEntryStart(u32 area_idx);


	bool saveTotal(u32 area_idx);
	bool saveSTotal(u32 seqno);

	bool appendSEntry(const u32 idx, const u32 num, const u32 seqno, const u64 offset, const u32 size);
	bool updateSEntry(const u32 seqno, const u32 idx, const u32 num);

	bool changeNum(const u32 seqno,	const u64 offset, const u32 oldnum,	const u32 newnum);

	bool removeEntry(const u32 area_idx, const u32 entry_idx);
	bool removeSEntry(const u32 seqno,const u64 offset);

	u32  getSeqno();
	bool setSeqno(const u32 seqno);
	int BinSearch(u32 seqno,u64 K, u32 size, int &num);
	bool sortt(const u32 seqno);
	OmnFilePtr getFile(const u32 seqno AosMemoryCheckDecl);
    u32  getSize(const u32 seqno, const u64 offset);

	bool openIdxFile();
	bool openSeqnoFile();
	bool sanityCheck();

	bool
	readEntry(
		const u32 start,
		u32 &numBlocks,
		u32 &seqno,
		u64 &offset)
	{
		if (!mIdxFile) openIdxFile();
	aos_assert_r(mIdxFile, false);
		
		char *buff = mBuffs;
		if(!buff)
		{
			buff = OmnNew char[eEntrySize+10];
			mBuffs = buff;
		}

		mIdxFile->readToBuff(start, eEntrySize, buff);

		numBlocks = *(u32 *)&buff[eNumBlockOffset];

		seqno = *(u32 *)&buff[eSeqnoOffset];

		offset =  *(u64 *)&buff[eSeqnoNum];

		u32 flag = *(u32 *)&buff[eFlagOffset];
		aos_assert_r(flag == eEntryFlag, false);
		return true;
	}

	bool
	readSEntry(
		const u32 start,
		u32 &idx,
		u32 &num,
		u32 &seqno,
		u64 &offset,
		u32 &size)
	{
		if (!mSeqnoFile) openSeqnoFile();
	aos_assert_r(mSeqnoFile && mSeqnoFile->isGood(), false);


		char *buff = mBuffs;
		if(!buff)
		{
			buff = OmnNew char[eEntrySize+10];
			mBuffs = buff;
		}

		mSeqnoFile->readToBuff(start, eEntrySize, buff);


		idx = *(u32 *)&buff[eNumBlockOffset];

		num = *(u32 *)&buff[eSeqnoOffset];

		seqno = *(u32 *)&buff[eSeqnoNum];

		offset = *(u64 *)&buff[eOffsetOffset];

		size = *(u32 *)&buff[eSizeOffset];

		u32 flag = *(u32 *)&buff[eSeqnoFlag];
		aos_assert_r(flag == eEntryFlag, false);
		return true;
	}

};

#endif
