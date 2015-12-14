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
// The idle block file is organized as:
// 	[Area for file definition]
// 	Address 1:	[Area for idle entries of sizes [0M, 10M)]
// 	Address 2:	[Area for idle entries of sizes [0M, 20M)]
// 	Address 3:	[Area for idle entries of sizes [0M, 30M)]
// 	...
//
// Each area is organized as:
// 	[number of blocks(4 bytes), seqno(4 bytes), offset(4 bytes)]
// 	[number of blocks(4 bytes), seqno(4 bytes), offset(4 bytes)]
// 	...
//
// 09/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IdleBlockMgr.h"

#include "Ptrs.h"
#include "DiskBlockMgr.h"
#include "Thread/Mutex.h"
#include "Debug/Debug.h"
#include "SEUtil/SeConfig.h"
#include "Util/OmnNew.h"
#include "Thread/Mutex.h"



#define AosIdleBlockMgrCheck sanityCheck()

const bool sgSanityCheck = false;
AosIdleBlockMgr::AosIdleBlockMgr(
		const OmnString &dirname, 
		const OmnString &fname)
:
mLock(OmnNew OmnMutex()),
mBuffs(0)
{
	bool rslt = init(dirname, fname);
	aos_assert(rslt);
}


AosIdleBlockMgr::~AosIdleBlockMgr()
{
	OmnDelete [] mBuffs;
	mBuffs = 0;
}



bool
AosIdleBlockMgr::init(
	const OmnString &dirname,
	const OmnString &docfilename)
{
	OmnString fname = docfilename;
	fname << "_idx_size";
	OmnString sfname = docfilename;
	sfname << "_idx_seqno";

	mMaxFileSize = eAosMaxFileSize/10000000;
	mDirname = dirname;
	mFilename = fname;
	mSFilename = sfname;
	mDocFilename = docfilename;

	for (u32 i=0; i<eMaxAreas; i++)
	{
		readArea(i);
	}
	for (u32 i=0; i<eMaxFiles; i++)
	{
		readSArea(i);
	}
	return true;
}


bool
AosIdleBlockMgr::getSpace(
		const u32 num_bytes,
		u32 &seqno,
		u64 &offset)
{
	u32 size_in_mega = convertBytesToSize(num_bytes);
	mLock->lock();
	u32 idx = convertToSizeIdx(size_in_mega);
	aos_assert_rl(idx >= 0 && idx < eMaxAreas, mLock, false);

	// Find a space starting from the area 'idx'.
	for (; idx < eMaxAreas; idx++)
	{
		if (getSpace(idx, size_in_mega, seqno, offset))
		{
			mLock->unlock();
			return true;
		}
	}

	bool rslt = createNewFile(size_in_mega, seqno, offset);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}



bool
AosIdleBlockMgr::getSpace(
		const u32 idx,
		const u32 size,
		u32 &seqno,
		u64 &offset)
{
	// This function retrieves the required space: size_in_mega, from
	// the area identified by 'idx'. It should always return true
	// if the area is not empty. If the area is empty, it returns false.
	// This class should have been locked before this function is called.
	aos_assert_r(idx >= 0, false);

	if (mTotal[idx] <= 0) return false;

	for(int i=0;i<mTotal[idx];i++)
	{
		if(mAreas[idx][i].blocks >= size)
		{
			seqno = mAreas[idx][i].seqno;
			offset = mAreas[idx][i].offset;

			u32 newsize = mAreas[idx][i].blocks - size ;
			if (newsize == 0)
			{
				// The entry has been used up. Need to remove the entry
				bool rslt = removeEntry(idx, i);
				aos_assert_r(rslt, false);
				return true;
			}

			// The entry is not empty. Need to move it to the corresponding area.
			u32 newidx = convertToSizeIdx(newsize);
			if (newidx == (u32)idx)
			{
				// It did not change the area.
				mAreas[idx][i].seqno = mAreas[idx][i].seqno;
				mAreas[idx][i].blocks -= size;
				mAreas[idx][i].offset += size * eBlockSize;
				bool rslt = updateEntry(idx, i);
				aos_assert_r(rslt, false);
				aos_assert_r(sanityCheck(), false);
				return true;
			}

			bool rslt = removeEntry(idx, i);
			aos_assert_r(rslt, false);

			u64 off = offset + size * eBlockSize;
			rslt = appendEntry(newsize, seqno, off);
			aos_assert_r(rslt, false);

			return true;
		}
	}
	return false;
}


bool
AosIdleBlockMgr::createNewFile(
		const u32 size,
		u32 &seqno,
		u64 &offset)
{
	// This function creates a new file. This will create a
	// new entry. It then allocates the space for (size, seqno, offset),
	// and then add the new entry to this class.
	// It should not return false unless it runs out of the files
	seqno = getSeqno();
	aos_assert_r(seqno < eMaxFiles, false);

	OmnString filename = mDirname;
	filename << "/" << mDocFilename << "_" << seqno;
	mFiles[seqno] = OmnNew OmnFile(filename, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_r(mFiles[seqno]->isGood(), false);

	bool sno = setSeqno(seqno);
	aos_assert_r(sno, false);


	offset = 100;
	bool rest = appendEntry(mMaxFileSize, seqno, offset);
	aos_assert_r(rest, false);


	u32 idx = convertToSizeIdx(mMaxFileSize);
	rest = getSpace(idx, size, seqno, offset);
	aos_assert_r(rest, false);
	return true;
}


bool
AosIdleBlockMgr::appendEntry(const u32 size, const u32 seqno, const u64 offset)
{
	u32 idx = convertToSizeIdx(size);

	u32 start = getLastEntryStart(idx);
	aos_assert_rl(start > 0, mLock, false);

	aos_assert_r(sanityCheck(), false);


	int total = mTotal[idx]++;
	mAreas[idx][total].blocks = size;
	mAreas[idx][total].seqno = seqno;
	mAreas[idx][total].offset = offset;
	mAreas[idx][total].flag = eEntryFlag;
	mIdxFile->put(start, (char *)&mAreas[idx][total], eEntrySize, true);

	bool rslt = appendSEntry(idx,total,seqno,offset,size);
	aos_assert_r(rslt, false);

	rslt = saveTotal(idx);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIdleBlockMgr::updateEntry(
		const u32 area_idx,
		const u32 entry_idx)
{
	int total = mTotal[area_idx];
	aos_assert_r(total>0, false);

	u32 start = eAreaStartAddr + eEntrySize + area_idx * eAreaSize + eEntrySize * entry_idx;
	mIdxFile->put(start, (char *)&mAreas[area_idx][entry_idx], eEntrySize, true);

	u32 seqno = mAreas[area_idx][entry_idx].seqno;
	bool rs = updateSEntry(seqno, area_idx, entry_idx);
	aos_assert_r(rs, false);

	return true;
}



bool
AosIdleBlockMgr::appendSEntry(const u32 idx, const u32 num, const u32 seqno, const u64 offset, const u32 size)
{
	int total = mSTotal[seqno]++;
	mSAreas[seqno][total].idx = idx;
	mSAreas[seqno][total].num = num;
	mSAreas[seqno][total].seqno = seqno;
	mSAreas[seqno][total].offset = offset;
	mSAreas[seqno][total].size = size;
	mSAreas[seqno][total].flag = eEntryFlag;

	if(total>0) sortt(seqno);

	u32 start = eAreaStartAddr + eEntrySize + seqno * eAreaSize ;
	for(int i=0;i<=total;i++)
	{
		mSeqnoFile->put(start+ eEntrySize * i, (char *)&mSAreas[seqno][i], eEntrySize, true);
	}

	//aos_assert_r(sanityCheck(), false);
	saveSTotal(seqno);
	return true;
}

bool
AosIdleBlockMgr::updateSEntry(const u32 seqno, const u32 idx, const u32 num)
{
	int total = mSTotal[seqno];
	aos_assert_r(total > 0, false);

	for(int i=0;i<total;i++)
	{
		if((mSAreas[seqno][i].num ==num) && (mSAreas[seqno][i].idx ==idx))
		{
			mSAreas[seqno][i].offset = mAreas[idx][num].offset;
			mSAreas[seqno][i].size = mAreas[idx][num].blocks;

			if(total>1) sortt(seqno);

			u32 start = eAreaStartAddr + eEntrySize + seqno * eAreaSize ;

			for(int ii=0;ii<total;ii++)
			{
				mSeqnoFile->put(start+ eEntrySize * ii, (char *)&mSAreas[seqno][ii], eEntrySize, true);
			}

			aos_assert_r(sanityCheck(), false);
			return true;
		}
	}

	OmnShouldNeverComeHere;
	return false;
}



bool
AosIdleBlockMgr::saveTotal(u32 area_idx)
{
	u32 total = mTotal[area_idx];
	u32 start = eAreaStartAddr + area_idx * eAreaSize;
	mIdxFile->setU32(start, total, true);
	return true;
}

bool
AosIdleBlockMgr::saveSTotal(u32 seqno)
{
	u32 total = mSTotal[seqno];
	u32 start = eAreaStartAddr + seqno * eAreaSize;
	mSeqnoFile->setU32(start, total , true);
	return true;
}



bool
AosIdleBlockMgr::removeEntry(
		const u32 area_idx,
		const u32 entry_idx)
{
	int total = mTotal[area_idx]--;
	aos_assert_r(total > 0, false);

	if ((total == 1)||(entry_idx == (u32)(total-1)))
	{
		bool rs = removeSEntry(mAreas[area_idx][entry_idx].seqno,mAreas[area_idx][entry_idx].offset);
		aos_assert_r(rs, false);

		rs = saveTotal(area_idx);
		aos_assert_r(rs, false);

		//aos_assert_r(sanityCheck(), false);
		//clean
		mAreas[area_idx][entry_idx].blocks = 0;
		mAreas[area_idx][entry_idx].seqno = 0;
		mAreas[area_idx][entry_idx].offset = 0;

		u32 start = eAreaStartAddr + eEntrySize + area_idx * eAreaSize + eEntrySize * entry_idx;
		mIdxFile->put(start, (char *)&mAreas[area_idx][entry_idx], eEntrySize, true);


		return true;
	}

	u32 sso = mAreas[area_idx][entry_idx].seqno;
	u64 off = mAreas[area_idx][entry_idx].offset;
	
	u32 sqno = mAreas[area_idx][total-1].seqno;
	u64 ofst = mAreas[area_idx][total-1].offset;

	mAreas[area_idx][entry_idx].blocks = mAreas[area_idx][total-1].blocks;
	mAreas[area_idx][entry_idx].seqno= mAreas[area_idx][total-1].seqno;
	mAreas[area_idx][entry_idx].offset = mAreas[area_idx][total-1].offset;

	//clean
	mAreas[area_idx][total-1].blocks = 0;
	mAreas[area_idx][total-1].seqno = 0;
	mAreas[area_idx][total-1].offset = 0;

	bool rs = removeSEntry(sso, off);
	aos_assert_r(rs, false);


	rs = changeNum(sqno, ofst, total-1, entry_idx);
	aos_assert_r(rs, false);

	rs = saveTotal(area_idx);
	aos_assert_r(rs, false);

	u32 start = eAreaStartAddr + eEntrySize + area_idx * eAreaSize + eEntrySize * entry_idx;
	mIdxFile->put(start, (char *)&mAreas[area_idx][entry_idx], eEntrySize, true);

	start = eAreaStartAddr + eEntrySize + area_idx * eAreaSize + eEntrySize * (total-1);
	mIdxFile->put(start, (char *)&mAreas[area_idx][total-1], eEntrySize, true);

	aos_assert_r(sanityCheck(), false);
	return true;
}

bool
AosIdleBlockMgr::removeSEntry(
		const u32 seqno,
		const u64 offset)
{
	int total = mSTotal[seqno]--;
	aos_assert_r(total > 0, false);


	for(int i=0;i<total;i++)
	{
		if(mSAreas[seqno][i].offset == offset)
		{
			if ((total == 1)||(i == total-1 ))
			{
				bool rs = saveSTotal(seqno);
				aos_assert_r(rs, false);

				//clean
				mSAreas[seqno][i].idx = 0;
				mSAreas[seqno][i].num= 0;
				mSAreas[seqno][i].seqno = 0;
				mSAreas[seqno][i].offset= 0;
				mSAreas[seqno][i].size = 0;

				u32 start = eAreaStartAddr + eEntrySize + seqno * eAreaSize ;
				mSeqnoFile->put(start+ eEntrySize * i, (char *)&mSAreas[seqno][i], eEntrySize, true);

				aos_assert_r(sanityCheck(), false);
				return true;
			}


			mSAreas[seqno][i].idx = mSAreas[seqno][total-1].idx;
			mSAreas[seqno][i].num= mSAreas[seqno][total-1].num;
			mSAreas[seqno][i].seqno = mSAreas[seqno][total-1].seqno;
			mSAreas[seqno][i].offset= mSAreas[seqno][total-1].offset;
			mSAreas[seqno][i].size = mSAreas[seqno][total-1].size;

			//clean
			mSAreas[seqno][total-1].idx = 0;
			mSAreas[seqno][total-1].num= 0;
			mSAreas[seqno][total-1].seqno = 0;
			mSAreas[seqno][total-1].offset= 0;
			mSAreas[seqno][total-1].size = 0;

			bool rs = saveSTotal(seqno);
			aos_assert_r(rs, false);

			if(total>2) sortt(seqno);

			u32 start = eAreaStartAddr + eEntrySize + seqno * eAreaSize ;

			for(int ii=0;ii<total;ii++)
			{
				    mSeqnoFile->put(start+ eEntrySize * ii, (char *)&mSAreas[seqno][ii], eEntrySize, true);
			}
			aos_assert_r(sanityCheck(), false);
			return true;

			break;
		}
	}

	OmnShouldNeverComeHere;
	return false;
}





bool
AosIdleBlockMgr::readArea(const u32 idx)
{
	// This function is called when the class starts up. It reads
	// in the blocks for the given area 'idx'.
	// Each area is fixed size. It contains a number of
	// entries. The first entry is reserved for management (used
	// to determine how many entries in the area):
	// 	[total number of entries, 	seqno(not used), offset(not used)]
	// 	[number of blocks(4 bytes), seqno(4 bytes),  offset(4 bytes)]
	// 	[number of blocks(4 bytes), seqno(4 bytes),  offset(4 bytes)]
	// The start address of each area is determined by:
	// 	eAreaStartAddr + idx * eAreaSize
	//	mTotal[idx] = 0;
	u32 start = eAreaStartAddr + idx * eAreaSize;
	u32 total, seqno, num_blocks;
	u64 offset;
	bool rslt = readEntry(start, total, seqno, offset);
	aos_assert_r(rslt, false);
	
	u32 num = (total < eMaxNumEntries)?total:eMaxNumEntries;
	for (u32 i=0; i<num; i++)
	{
		start += eEntrySize;
		rslt = readEntry(start, num_blocks, seqno, offset);
		aos_assert_r(rslt, false);
		mAreas[idx][i].blocks = num_blocks;
		mAreas[idx][i].seqno = seqno;
		mAreas[idx][i].offset = offset;
		mAreas[idx][i].flag = eEntryFlag;
	}
	mTotal[idx] = num;
	aos_assert_r(sanityCheck(), false);
	return true;
}


bool
AosIdleBlockMgr::readSArea(const u32 seqno)
{
	u32 start = eAreaStartAddr + seqno*eAreaSize;

	u32 numt, idx, numm, seqn, size;
	u64 offset;
	bool rslt = readSEntry(start,numt, numm, seqn, offset, size);
if (!rslt)
	OmnMark;
	aos_assert_r(rslt, false);

	u32 num = (numt < eMaxNumEntries)?numt:eMaxNumEntries;
	for (u32 i=0; i<num; i++)
	{
		start += eEntrySize;
		rslt = readSEntry(start,idx, numm, seqn, offset, size);
		aos_assert_r(rslt, false);
		mSAreas[seqno][i].idx = idx;
		mSAreas[seqno][i].num = numm;
		mSAreas[seqno][i].seqno = seqn;
		mSAreas[seqno][i].offset = offset;
		mSAreas[seqno][i].size = size;
		mSAreas[seqno][i].flag = eEntryFlag;
	}
	mSTotal[seqno] = num;
	aos_assert_r(sanityCheck(), false);
	return true;
}


bool
AosIdleBlockMgr::openIdxFile()
{
	// Read IdxFile
	if (mIdxFile) return true;

	OmnString fname = mDirname;
	fname << "/" << mFilename;

	mIdxFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(mIdxFile, false);


	if (!mIdxFile->isGood())
	{
		mIdxFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(mIdxFile, false);
		aos_assert_r(mIdxFile->isGood(), false);

		u32 start = eAreaStartAddr;
		mAreas[0][0].blocks = 0;
		mAreas[0][0].seqno = 0;
		mAreas[0][0].offset = 0;
		mAreas[0][0].flag = eEntryFlag;
		for(u32 i=0; i<eMaxAreas; i++)
		{
			mIdxFile->put(start + i*eAreaSize, (char *)&mAreas[0][0], eEntrySize, true);
		}
	}
	aos_assert_r(mIdxFile->isGood(), false);
	aos_assert_r(sanityCheck(), false);

	return true;
}



bool
AosIdleBlockMgr::openSeqnoFile()
{
	//Read SeqnoFile
	if (mSeqnoFile) return true;

	OmnString fname = mDirname;
	fname << "/" << mSFilename;

	mSeqnoFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(mSeqnoFile, false);


	if (!mSeqnoFile->isGood())
	{
		mSeqnoFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(mSeqnoFile, false);

		u32 start = eAreaStartAddr;
		mSAreas[0][0].idx = 0;
		mSAreas[0][0].num = 0;
		mSAreas[0][0].seqno = 0;
		mSAreas[0][0].offset = 0;
		mSAreas[0][0].size = 0;
		mSAreas[0][0].flag = eEntryFlag;
		for(u32 i=0; i<eMaxFiles; i++)
		{
			mSeqnoFile->put(start + i*eAreaSize ,(char *)&mSAreas[0][0], eEntrySize, true);
		}
	}
	aos_assert_r(mSeqnoFile->isGood(), false);
	aos_assert_r(sanityCheck(), false);


	return true;
}



bool
AosIdleBlockMgr::returnSpaceSafe(
		const u32 size,
		const u32 seqno,
		const u64 offset)
{
	// This is a public member function. When a doc is removed from
	// the system, it should call this function to return the
	// space allocated for it.
	int i;
	int il = BinSearch(seqno, offset, size, i);
OmnScreen << "return space before: " << size << ":" << seqno << ":" << offset <<":" << il <<endl;
if (offset == 23000100)	OmnMark;

		if(il==33)//double merge
		{
			//i the down mark, i-1 the up mark
			//down
			u64 downoff = offset + size*eBlockSize;
			aos_assert_r(downoff==mSAreas[seqno][i].offset, false);
			//up
			u64 upoff = mSAreas[seqno][i-1].offset + mSAreas[seqno][i-1].size*eBlockSize;
			aos_assert_r(upoff==offset, false);

			u64 upoffss = mSAreas[seqno][i-1].offset;
			
			u32 downnum = mSAreas[seqno][i].num;
			u32 downidx = mSAreas[seqno][i].idx;
			u32 upnum = mSAreas[seqno][i-1].num;
			u32 upidx = mSAreas[seqno][i-1].idx;

			u32	newsize = mSAreas[seqno][i-1].size + size + mSAreas[seqno][i].size;
			u32 newidx = convertToSizeIdx(newsize);

			if (newidx == mSAreas[seqno][i-1].idx)
			{
				// It did not change the area.
				mAreas[upidx][upnum].blocks = newsize;
				mAreas[upidx][upnum].seqno = seqno;
				mAreas[upidx][upnum].offset = upoffss;

				bool rslt = updateEntry(upidx, upnum);
				aos_assert_r(rslt, false);
				
				rslt = removeEntry(downidx, downnum);
				aos_assert_r(rslt, false);

OmnScreen << "return space after: " << newsize << ":" << seqno << ":" <<upoffss <<":" << il <<endl;
				return true;
			}
			bool rslt = removeEntry(upidx, upnum);
			aos_assert_r(rslt, false);
			
			rslt = removeEntry(downidx, downnum);
			aos_assert_r(rslt, false);

			rslt = appendEntry(newsize, seqno, upoffss);
			aos_assert_r(rslt, false);

OmnScreen << "return space after: " << newsize << ":" << seqno << ":" << upoffss <<":" << il <<endl;
			return true;
		}



		if(il==11)//down merge
		{
			u64 off = offset + size*eBlockSize;
			aos_assert_r(off==mSAreas[seqno][i].offset, false);

			u32 num = mSAreas[seqno][i].num;
			u32 idx = mSAreas[seqno][i].idx;

			u32	newsize = size + mSAreas[seqno][i].size;
			u32 newidx = convertToSizeIdx(newsize);

			if (newidx == mSAreas[seqno][i].idx)
			{
				// It did not change the area.
				mAreas[idx][num].blocks = newsize;
				mAreas[idx][num].seqno = seqno;
				mAreas[idx][num].offset = offset;

				bool rslt = updateEntry(idx, num);
				aos_assert_r(rslt, false);

OmnScreen << "return space after: " << newsize << ":" << seqno << ":" << offset <<":" << il <<endl;
				return true;
			}

			bool rslt = removeEntry(idx, num);
			aos_assert_r(rslt, false);

			rslt = appendEntry(newsize, seqno, offset);
			aos_assert_r(rslt, false);

OmnScreen << "return space after: " << newsize << ":" << seqno << ":" << offset <<":" << il <<endl;
			return true;
		}

		if(il==22)//up merge
		{
			u64 off = mSAreas[seqno][i].offset + mSAreas[seqno][i].size*eBlockSize;
			aos_assert_r(off==offset, false);

			u32 num = mSAreas[seqno][i].num;
			u32 idx = mSAreas[seqno][i].idx;

			u32	newsize = size+mSAreas[seqno][i].size;
			u32 newidx = convertToSizeIdx(newsize);

			u64 offs = mSAreas[seqno][i].offset;

			if (newidx == mSAreas[seqno][i].idx)
			{
				// It did not change the area.
				mAreas[idx][num].offset = mSAreas[seqno][i].offset;
				mAreas[idx][num].blocks = newsize;
				bool rslt = updateEntry(idx, num);
				aos_assert_r(rslt, false);
OmnScreen << "return space after: " << newsize << ":" << seqno << ":" << offs <<":" << il <<endl;
				return true;
			}

			// It changed its area. Need to remove it from the current
			// area and add it into the corresponding area.
			bool rslt = removeEntry(idx, num);
			aos_assert_r(rslt, false);

			rslt = appendEntry(newsize, seqno, offs);
			aos_assert_r(rslt, false);

OmnScreen << "return space after: " << newsize << ":" << seqno << ":" << offs <<":" << il <<endl;
			return true;
		}

OmnScreen << "return space: " << size << ":" << seqno << ":" << offset <<":" << il <<endl;

	u32 area_idx = convertToSizeIdx(size);
	bool rslt = returnEntry(area_idx, size, seqno, offset);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIdleBlockMgr::returnEntry(
		const u32 area_idx,
		const u32 size,
		const u32 seqno,
		const u64 offset)
{
	// This function assumes the entry cannot be merged.
	// The class should have been locked before this function is called.
	// It simply add the entry into its area.
	bool rslt = false;
	u32 num = mTotal[area_idx];
	if (num < eMaxAreaEntries)
	{
		// All the entries have been read into the array.
		// We can simply add the entry into the array and then save the entry
		rslt = appendEntry(size, seqno, offset);
		aos_assert_r(rslt, false);
		return true;
	}

	return true;
}



u32
AosIdleBlockMgr::getLastEntryStart(u32 area_idx)
{

	u32 num = mTotal[area_idx];
	u32 start = eAreaStartAddr + area_idx * eAreaSize;
	start += eEntrySize;//head
	start += num * eEntrySize;//entry
	return start;
}



OmnFilePtr
AosIdleBlockMgr::getFile(const u32 seqno AosMemoryCheckDecl)
{
	// It retrieves the file for 'seqno'. If it has not been
	// created yet, it will create it. Otherwise, it simply
	// returns the file.
	aos_assert_r(seqno < eMaxFiles, 0);
	if (mFiles[seqno]) return mFiles[seqno];

	OmnString fname = mDirname;
	fname << "/" << mDocFilename << "_" << seqno;
	mFiles[seqno] = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerFileLine);
	if (!mFiles[seqno] || !mFiles[seqno]->isGood()) 
	{
		mFiles[seqno] = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerFileLine);
	}
	aos_assert_r(mFiles[seqno], NULL);
	return mFiles[seqno];
}




u32
AosIdleBlockMgr::getSeqno()
{
	if (!mIdxFile) openIdxFile();
	aos_assert_r(mIdxFile, false);

	u32 start = eSeqnoStartAddr;

	char *buff = mBuffs;
	if(!buff)
	{
		buff = OmnNew char[eSeqnoNums+10];
		mBuffs = buff;
	}
	mIdxFile->readToBuff(start, eSeqnoNums, buff);

	u32 shead = *(u32 *)&buff[eNumBlockOffset];

	u32 seqno = *(u32 *)&buff[eSeqnoOffset];

	u32 send = *(u32 *)&buff[eSeqnoNum];

	if(shead!= eStartFlag&&send!= eEndFlag)
	{
		return 1;
	}

	else return seqno + 2;
}

bool
AosIdleBlockMgr::setSeqno(const u32 seqno)
{
	u32 start = eSeqnoStartAddr;
	char *buff = mBuffs;
	if(!buff)
	{
		buff = OmnNew char[eSeqnoNums+10];
		mBuffs = buff;
	}
	*(u32 *)&buff[eNumBlockOffset] = eStartFlag;
	*(u32 *)&buff[eSeqnoOffset] = seqno;
	*(u32 *)&buff[eSeqnoNum] = eEndFlag;
	mIdxFile->put(start, buff, eSeqnoNums, true);
	return true;
}



u32
AosIdleBlockMgr::convertBytesToSize(const u32 num_bytes)
{
	u32 size = (num_bytes + 10) / eBlockSize;
	u32 remainder = (num_bytes + 10) % eBlockSize;
	if (remainder == 0) return size;
	return size+1;
}


u32
AosIdleBlockMgr::convertToSizeIdx(const u32 size)
{
	// 		Area Idx		Area Size
	// 		0				100M
	// 		1				200M
	// 		2				300M
	// 		3				400M
	// 		...				...
	//return (size-1) / eIdxSize ;
	u32 idx = size / eIdxSize;
	u32 remainder = size % eIdxSize;
	if ((remainder == 0)&&(size!=0)) return idx-1;
	return idx;
}



bool
AosIdleBlockMgr::sortt(const u32 seqno)
{
	int n,j,i;
	u64 t;
	u32 idx, num, seqn, size;

	n=mSTotal[seqno];
	aos_assert_r(n>1, false);

	for(i=1;i<n;i++)
	{
		if(mSAreas[seqno][i].offset<mSAreas[seqno][i-1].offset)
		{
			t=mSAreas[seqno][i].offset;
			idx = mSAreas[seqno][i].idx;
			num = mSAreas[seqno][i].num;
			seqn = mSAreas[seqno][i].seqno;
			size = mSAreas[seqno][i].size;
			for(j=i-1;t<mSAreas[seqno][j].offset;j--)
			{
				mSAreas[seqno][j+1].idx = mSAreas[seqno][j].idx;
				mSAreas[seqno][j+1].num = mSAreas[seqno][j].num;
				mSAreas[seqno][j+1].seqno = mSAreas[seqno][j].seqno;
				mSAreas[seqno][j+1].offset = mSAreas[seqno][j].offset;
				mSAreas[seqno][j+1].size = mSAreas[seqno][j].size;
			}
			mSAreas[seqno][j+1].offset = t;
			mSAreas[seqno][j+1].idx = idx;
			mSAreas[seqno][j+1].num = num;
			mSAreas[seqno][j+1].seqno =  seqn;
			mSAreas[seqno][j+1].size = size;
		}
	}
	return true;
}



int
AosIdleBlockMgr::BinSearch(u32 seqno,u64 K, u32 size, int &num)
{
 int low=0,high=mSTotal[seqno],mid;
 while(low<=high)
 {
	mid=(low+high)/2;
	u32 block = mSAreas[seqno][mid].size;
	
	if((mid>0) && (mSAreas[seqno][mid].offset==(K+size*eBlockSize))&&((mSAreas[seqno][mid-1].offset +
					mSAreas[seqno][mid-1].size*eBlockSize)==K))
	{
			num = mid;
		OmnScreen <<"BinSearch~~~~~~~~33~~~~~~~~~BinSearch"<<endl;
			return 33;
	}
	
	else if(mSAreas[seqno][mid].offset==(K+size*eBlockSize))
	{
		num = mid;
	OmnScreen <<"BinSearch~~~~~~~~11~~~~~~~~BinSearch"<<endl;
		return 11;
	}

	else if((mSAreas[seqno][mid].offset + block*eBlockSize)==K)
	{
		num = mid;
	OmnScreen <<"BinSearch~~~~~~~~22~~~~~~~~BinSearch"<<endl;
		return 22;
	}
	else if(mSAreas[seqno][mid].offset>K)
	high=mid-1;
	else
	low=mid+1;
 }
 return 3;
}


bool
AosIdleBlockMgr::appendDoc(
		const char *data,
		const u32 datalen,
		u32 &seqno,
		u64 &offset)
{
	bool rslt = getSpace(datalen, seqno, offset);
	aos_assert_r(rslt, false);

	aos_assert_r(seqno < eMaxFiles, false);
	OmnFilePtr ff = getFile(seqno AosMemoryCheckerArgs);
//OmnScreen << "To write: " << seqno << ":" << offset << ":" << datalen << endl;
	ff-> setU32(offset, datalen, true);
	ff-> put(offset + eFileHead, data, datalen, true);

	/*
	char *dat = OmnNew char[datalen];
	rslt = readDoc(seqno, offset, datalen, dat);
	if(memcmp(data,dat, datalen) != 0)
	{
		OmnScreen <<"offset"<<offset;
		appendDoc(data ,datalen, seqno, offset);
	}
	*/
	return true;
}


bool
AosIdleBlockMgr::modifyDoc(
		const char *data,
		const u32 bytes,
		u32 &seqno,
		u64 &offset)
{
	u32 oldbyte = getSize(seqno, offset);
if ((seqno & 0x01) == 0)
	OmnMark;
	u32 size = convertBytesToSize(oldbyte);
	u32 size1 = convertBytesToSize(bytes);
	u32 seqn = seqno;
	u64 offs = offset;

	if(size == size1)
	{
		aos_assert_r(seqno < eMaxFiles, false);
		OmnFilePtr ff = getFile(seqno AosMemoryCheckerArgs);
		OmnScreen << "To write again: " << seqno << ":" << offset << ":" << oldbyte << "~~~~~~new bytes~~~~~" << bytes << endl;
		ff-> setU32(offset, bytes, true);
		ff-> put(offset + eFileHead, data, bytes, true);
		return true;
	}

	bool rslt = appendDoc(data,bytes,seqno,offset);
	aos_assert_r(rslt, false);
 	if(size)	rslt = returnSpaceSafe(size,seqn,offs);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosIdleBlockMgr::deleteDoc(u32 seqno, u64 offset)
{
	u32 bytes = getSize(seqno, offset);
	u32 size = convertBytesToSize(bytes);
	if(size)
	{
		bool rslt = returnSpaceSafe(size,seqno,offset);
		aos_assert_r(rslt, false);
	}
	return true;
}



bool
AosIdleBlockMgr::readDoc(
		const u32 seqno,
		const u64 &offset,
		const int length,
		char *data)
{
	mLock->lock();
	OmnFilePtr ff = getFile(seqno AosMemoryCheckerArgs);
	aos_assert_r(ff, false);

	ff->lock();
	
	u32 size = ff -> readBinaryU32(offset + eNumBlockOffset, 0);
if (size != (u32)length)
	OmnMark;
	aos_assert_r(size == (u32)length, false);

	u32 len = ff->readToBuff(offset + eFileHead, length, data);
	if (!len)
	{
		ff->unlock();
		mLock->unlock();
		OmnAlarm << "Failed to read: " << seqno << ":" << offset << enderr;
		return false;
	}

	ff->unlock();
	mLock->unlock();
	return true;
}

bool
AosIdleBlockMgr::sanityCheck()
{
	if(!sgSanityCheck)
	{
		return true;
	}
	// Sanity Check
/*	u32 seqno = 0;
	for(int p=0;p<mSTotal[seqno];p++)
	{
		u32 idx = mSAreas[seqno][p].idx;
		u32 num =  mSAreas[seqno][p].num;
		if (mTotal[idx] >= 0 && num >= (u32)mTotal[idx])
		{
			OmnAlarm << "Failed the check" << enderr;
			return false;
		}

		if (mSAreas[seqno][p].offset != mAreas[idx][num].offset)
		{
			OmnAlarm << "Failed the check!" << enderr;
			return false;
		}
		if (mSAreas[seqno][p].size != mAreas[idx][num].blocks)
		{
			OmnAlarm << "Failed the check!" << enderr;
			return false;
		}
	}

	u32 total1 = 0;
	u32 total2 = 0;
	for (u32 i=0; i<eMaxAreas; i++)
	{
		total1 += mTotal[i];
		total2 += mSTotal[i];
	}

	if (total1 != total2)
	{
		OmnAlarm << "Failed checking" << enderr;
		return false;
	}*/
	return true;
}


bool
AosIdleBlockMgr::changeNum(
		const u32 seqno,
		const u64 offset,
		const u32 oldnum,
		const u32 newnum)
{

	int total = mSTotal[seqno];
	aos_assert_r(total > 0, false);

	for(int i=0;i<total;i++)
	{
		if((mSAreas[seqno][i].num == oldnum) && (mSAreas[seqno][i].seqno == seqno) && (mSAreas[seqno][i].offset == offset))
		{
			mSAreas[seqno][i].num = newnum;

			u32 start = eAreaStartAddr + eEntrySize + seqno * eAreaSize ;

			mSeqnoFile->put(start+ eEntrySize * i, (char *)&mSAreas[seqno][i], eEntrySize, true);

			return true;
		}
	}
	
	OmnShouldNeverComeHere;
	return false;
}


u32 
AosIdleBlockMgr::getSize(const u32 seqno, const u64 offset)
{
	OmnFilePtr ff = getFile(seqno AosMemoryCheckerArgs);
	u32 size = ff -> readBinaryU32(offset + eNumBlockOffset, 0);
	return size;
}
