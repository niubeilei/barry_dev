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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
//	All documents are stored in 'mDocFilename' + seqno. There is 
//	a Document Index, that is stored in files 'mDocFilename' + 
//		'Idx_' + seqno
//
// Modification History:
// 01/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DfmUtil_h
#define AOS_DocFileMgr_DfmUtil_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"


#define AOS_PERCENT		"%"
#define AOS_BYTE		"B"
#define AOS_KB			"KB"

class AosDfmType
{
public:
	enum E
	{
		eInvalid,

		eNormDfm,
		eSnapShotDfm,

		eMax,
	};
	
	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
};

class AosDfmFileType
{
public:
	enum E
	{
		eInvalid,
		
		eHeaderFile,
		eDiskFile,    
		eSlabFile,
		eReadOnlyFile,

		eMax,
	};
	
	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
};


class AosDfmHeaderStatus
{
public:
	enum E 
	{
		eInvalidSts = 0,

		eNew,
		eModify,
		eDelete,
		eClean,     // create and then delete.

		eInvalidSts2
	};
	static bool isValid(const E id) {return id > eInvalidSts && id < eInvalidSts2;}
};


class AosDfmUtil
{
public:
	enum 
	{
		eMaxBlockSizes = 7,
		eMaxSlabLevel = 100,

		e200 = 200,
		e500 = 500,
		e1K = 1000,
		e2K = 2000,
		e3K = 3000,
		e4K = 4000,
		e5K = 5000,
		e6K = 6000,

		e10K = 10000,
		e15K = 15000,
		e25K = 25000,
		e30K = 30000,
		e50K = 50000,
		e100K = 100000,
		e200K = 200000,
		e300K = 300000,
		e400K = 400000,
		e500K = 500000,
		e1M = 1000000,
		e10M = 10000000,
		e50M = 50000000,
		e128M = 128000000,
		e512M = 512000000,
	};

private:
	static u32 	smBlockSizeIdxs[eMaxBlockSizes];
	static u32 	smDocSizeIdxs[eMaxBlockSizes];
	static int 	smMaxBlocks[eMaxBlockSizes];
	static u64  smDocFileMaxSize;
	static bool smInited;
	static bool init();

	static u32  smSlabDataMaxSize;
	static u32	smSlabLevel[eMaxSlabLevel];
	static u32	smSlabLvlSkip[eMaxSlabLevel];
	static OmnString smSlabLvlSkipType[eMaxSlabLevel];
public:
	inline static u32 getBlockSizeBySizeIdx(const u32 size_idx)
	{
		if (!smInited) init();
		aos_assert_r(size_idx < eMaxBlockSizes, 0);
		return smBlockSizeIdxs[size_idx];
	}

	// Ketty 2012/02/08
	//inline static u32 getSmallDocMaxSize()
	//{
	//	if (!smInited) init();
	//	return smBlockSizeIdxs[3];
	//}

	inline static int docsize2SizeIndex(const u32 docsize)
	{
		if (!smInited) init();
		for (u32 i=0; i<eMaxBlockSizes; i++)
		{
			if (docsize <= smDocSizeIdxs[i]) return i;
		}

		OmnAlarm << "Docsize too big!" << enderr;
		return -1;
	}
	
	inline static u32 docsize2Blocksize(const u32 docsize)
	{
		int size_idx = docsize2SizeIndex(docsize);
		aos_assert_r(size_idx >= 0 && (u32)size_idx < AosDfmUtil::eMaxBlockSizes, 0);
	
		u32 blocksize = getBlockSizeBySizeIdx(size_idx);
		return blocksize;	
	}

	inline static int blocksize2SizeIndex(const u32 block_size)
	{
		// IMPORTANT!!!!!!!!!!!!!!!!!
		// There are two ways to convert size to size index. One is
		// docsize to size index and the other blocksize to size index.
		// This function converts blocksize to size index
		if (!smInited) init();
		for (u32 i=0; i<eMaxBlockSizes; i++)
		{
			if (block_size <= smBlockSizeIdxs[i]) return i;
		}

		OmnAlarm << "Blocksize too big!" << enderr;
		return -1;
	}

	static int getMaxBlocks(const u32 size_idx)
	{
		if (!smInited) init();
		aos_assert_r(size_idx < eMaxBlockSizes, -1);
		return smMaxBlocks[size_idx];
	}

	inline static bool isSizeIdxValid(const u32 size_idx)
	{
		return size_idx < eMaxBlockSizes;
	}

	inline static u32 getDocFileMaxSize()
	{
		return smDocFileMaxSize;
	}

	// Ketty 2012/02/07
	inline static bool isBigData(const u32 docsize)
	{
		return true;
		if(!smInited) init();
		u32 smallDataMaxSize = smDocSizeIdxs[eMaxBlockSizes-1];
		if(docsize > smallDataMaxSize)	return true;

		return false;	
	}
	
	static u32 docsize2SlabBlockSize(const u32 docsize);
};
#endif
