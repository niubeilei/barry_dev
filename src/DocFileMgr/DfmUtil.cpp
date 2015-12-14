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
#include "DocFileMgr/DfmUtil.h"

#include "aosUtil/Types.h"
#include "DocFileMgr/SlabBlockFile.h"
#include "Thread/Mutex.h"

static OmnMutex sgLock;
u32 	AosDfmUtil::smBlockSizeIdxs[eMaxBlockSizes];
u32 	AosDfmUtil::smDocSizeIdxs[eMaxBlockSizes];
int 	AosDfmUtil::smMaxBlocks[eMaxBlockSizes];
bool 	AosDfmUtil::smInited = false;
u64		AosDfmUtil::smDocFileMaxSize = 1000000;		// 1M

u32		AosDfmUtil::smSlabDataMaxSize = 0;
u32		AosDfmUtil::smSlabLevel[eMaxSlabLevel];		// Ketty 2012/02/20
u32		AosDfmUtil::smSlabLvlSkip[eMaxSlabLevel];		// Ketty 2012/02/20
OmnString AosDfmUtil::smSlabLvlSkipType[eMaxSlabLevel];		// Ketty 2012/02/20

bool
AosDfmUtil::init()
{
	if (smInited) return true;
	sgLock.lock();
	
	// Ketty 2012/02/08	
	// smDocSizeIdxs[0] = e500; 	smBlockSizeIdxs[0] = e25K;		
	// smDocSizeIdxs[1] = e1K; 	smBlockSizeIdxs[1] = e50K;		
	// smDocSizeIdxs[2] = e2K; 	smBlockSizeIdxs[2] = e200K;	
	// smDocSizeIdxs[3] = e3K; 	smBlockSizeIdxs[3] = e300K;	
	// smDocSizeIdxs[4] = e4K; 	smBlockSizeIdxs[4] = e400K;	
	// smDocSizeIdxs[5] = e5K; 	smBlockSizeIdxs[5] = e500K;	

	smDocSizeIdxs[0] = e200; 	smBlockSizeIdxs[0] = e15K;		
	smDocSizeIdxs[1] = e500; 	smBlockSizeIdxs[1] = e25K;		
	smDocSizeIdxs[2] = e1K; 	smBlockSizeIdxs[2] = e50K;		
	smDocSizeIdxs[3] = e2K; 	smBlockSizeIdxs[3] = e200K;	
	smDocSizeIdxs[4] = e3K; 	smBlockSizeIdxs[4] = e300K;	
	smDocSizeIdxs[5] = e4K; 	smBlockSizeIdxs[5] = e400K;	
	smDocSizeIdxs[6] = e5K; 	smBlockSizeIdxs[6] = e500K;	

	aos_assert_r(6 + 1 == eMaxBlockSizes, false);
	
	smSlabDataMaxSize = e512M;
	smSlabLevel[0] = 0;		smSlabLvlSkip[0] = 32;	smSlabLvlSkipType[0] = AOS_BYTE; 
	smSlabLevel[1] = 256;	smSlabLvlSkip[1] = 64;	smSlabLvlSkipType[1] = AOS_BYTE; 
	smSlabLevel[2] = e1K;	smSlabLvlSkip[2] = 128;	smSlabLvlSkipType[2] = AOS_BYTE; 
	smSlabLevel[3] = e5K;	smSlabLvlSkip[3] = 5;	smSlabLvlSkipType[3] = AOS_KB; 
	smSlabLevel[4] = e30K;	smSlabLvlSkip[4] = 10;	smSlabLvlSkipType[4] = AOS_PERCENT; 
	smSlabLevel[5] = e1M;	smSlabLvlSkip[5] = 50;	smSlabLvlSkipType[5] = AOS_PERCENT; 
	smSlabLevel[6] = e10M;	smSlabLvlSkip[6] = 50;	smSlabLvlSkipType[6] = AOS_PERCENT; 
	smSlabLevel[7] = e50M;	smSlabLvlSkip[7] = 50;	smSlabLvlSkipType[7] = AOS_PERCENT; 
	smSlabLevel[8] = smSlabDataMaxSize;

	smMaxBlocks[0] = 10;
	smMaxBlocks[1] = 10;
	smMaxBlocks[2] = 5;
	smMaxBlocks[3] = 5;
	smMaxBlocks[4] = 5;
	smMaxBlocks[5] = 5;
	smMaxBlocks[6] = 5;

	smInited = true;
	sgLock.unlock();
	return true;
}


u32
AosDfmUtil::docsize2SlabBlockSize(const u32 docsize)
{
	if(!smInited) init();
	aos_assert_r(eMaxSlabLevel >0 && docsize >= smSlabLevel[0], 0);

	u32 level = 0;
	bool isBigData = false;
	for(; level<eMaxSlabLevel-1; level++)
	{
		if(smSlabLevel[level] == smSlabDataMaxSize)
		{
			isBigData = true;
			break;	
		}
		if(docsize >= smSlabLevel[level] && docsize < smSlabLevel[level+1])	break;	
	}

	if(isBigData)
	{
		aos_assert_r(docsize >= smSlabDataMaxSize, 0);

		OmnScreen << "Data is too big! size:" << docsize << endl;
		OmnAlarm << "Not complete yet!" << enderr;
		return 0;
	}

	u32 blockSize = smSlabLevel[level];
	while(blockSize < smSlabLevel[level+1])
	{
		u32 maxDocSize = AosSlabBlockFile::getMaxDocSize(blockSize); 
		if(docsize <= maxDocSize) break;
			
		if(smSlabLvlSkipType[level] == AOS_PERCENT)
		{
			blockSize += (blockSize * smSlabLvlSkip[level] / 100); 
		}
		else if(smSlabLvlSkipType[level] == AOS_KB)
		{
			blockSize += (smSlabLvlSkip[level] * e1K); 
		}
		else if(smSlabLvlSkipType[level] == AOS_BYTE)
		{
			blockSize += (smSlabLvlSkip[level]); 
		}
		else
		{
			OmnAlarm << "Unrecogined type:" << smSlabLvlSkipType[level] << enderr;
			return 0;
		}
	}
	
	// Important! each slabBlock contain sizeofDocsize(u32) and the doc.
	return AosSlabBlockFile::calculateBlockSize(blockSize);
}

			

