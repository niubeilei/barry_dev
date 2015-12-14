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
//	To prevent from concurrent operating on the same file, wherenever
//	to do something on a file, lock the file. After that, release the file.
//
// Each doc file has a header, which is in the following format:
// 	eOffsetBlocksize(4bytes):	the blocksize
// 	eOffsetFlags(4bytes):		the flags
// 	eOffsetBlockStart(4bytes):	the flags
//
// Modification History:
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DocFileMgr1.cpp
#include "SEUtil/DocFileMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "IdGen/IdGen.h"
#include "Porting/ThreadDef.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/DiskBlock.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/DiskBlock.h"
#include "SEUtil/DiskBlockMgr.h"
#include "SEUtil/IdleBlockMgr.h"
#include "SEUtil/SeUtil.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"

const bool sgWriteSpace = true;
const int sgMaxDocFiles = 5000;

AosDocFileMgr::AosDocFileMgr()
:
mLock(OmnNew OmnMutex()),
mMaxDocFiles(0),
mNumHeaders(0),
mCrtOffset(0),
mDiskBlocksize(eDftDiskBlocksize),
mBlockMgr(OmnNew AosDiskBlockMgr())
{
	// Chen Ding, 07/11/2011
	memset(mBuffs, 0, sizeof(char *) * eMaxFiles);
}


AosDocFileMgr::AosDocFileMgr(
		const AosXmlTagPtr &config, 
		const OmnString &defname)
:
mLock(OmnNew OmnMutex()),
mNumHeaders(0),
mCrtOffset(0),
mDiskBlocksize(eDftDiskBlocksize),
mBlockMgr(OmnNew AosDiskBlockMgr())
{
	// Chen Ding, 07/11/2011
	memset(mBuffs, 0, sizeof(char *) * eMaxFiles);

	AosXmlTagPtr def = config->getFirstChild(defname);
	aos_assert(def);

	mMaxDocFiles = def->getAttrU64(AOSCONFIG_XMLDOC_MAX_DOCFILES, 0);
	aos_assert(mMaxDocFiles > 0);

	OmnString fname = def->getAttrStr(AOSCONFIG_DOC_FILENAME);
	aos_assert(fname != "");

	OmnString dirname = config->getAttrStr(AOSCONFIG_DIRNAME);
	aos_assert(dirname != "");

	bool rslt = init(dirname, fname, mMaxDocFiles);
	aos_assert(rslt);
}


AosDocFileMgr::AosDocFileMgr(
		const u32 maxdocfiles, 
		const OmnString &dirname, 
		const OmnString &fname)
:
mLock(OmnNew OmnMutex()),
mNumHeaders(0),
mCrtOffset(0),
mDiskBlocksize(eDftDiskBlocksize),
mBlockMgr(OmnNew AosDiskBlockMgr())
{
	// Chen Ding, 07/11/2011
	memset(mBuffs, 0, sizeof(char *) * eMaxFiles);

	bool rslt = init(dirname, fname, maxdocfiles);
	aos_assert(rslt);
}


AosDocFileMgr::~AosDocFileMgr()
{
}


bool 		
AosDocFileMgr::init(
	const OmnString &dirname,
	const OmnString &fname,
	const u32 maxdocfiles) 
{
	mIdleBlockMgr = OmnNew AosIdleBlockMgr(dirname, fname);
	mMaxDocFiles = maxdocfiles;
	if (mMaxDocFiles <= 0) mMaxDocFiles = eAosMinDocFiles;
	mDirname = dirname;
	mFilename = fname;

	mBlocksizeIdxs[0] = 10000; 		mBlocksizes[0] = 500000;		// 10k, 500k
	mBlocksizeIdxs[1] = 100000; 	mBlocksizes[1] = 2000000;		// 100k, 2M
	mBlocksizeIdxs[2] = 500000; 	mBlocksizes[2] = 5000000;		// 500k, 5M
	mBlocksizeIdxs[3] = 1000000; 	mBlocksizes[3] = 10000000;		// 1M, 10M
	mSmallDocMaxSize = mBlocksizeIdxs[3];

	return true;
}


bool
AosDocFileMgr::findLogFilePriv(const u32 newsize, u32 &seqno)
{
	// This function finds a file that has the room
	// for 'newsize'. It searches from the first file and returns
	// [seqno, offset] that can grow 'newsize'. 
	// It will also update the index record in the file
	// If no such files, it tries to create new one. If it 
	// exceeds the maximum, it returns false. 
	// 
	// IMPORTANT: the caller should have locked the class.
	/*
	aos_assert_r(seqno < eMaxFiles, false);
	if (!mCrtLogFile) aos_assert_r(openLogFilePriv(seqno), false);

	if (mCrtLogFileSize + newsize + eDocOffset < eAosMaxFileSize)
	{
		// The current file is good enough
		seqno = mCrtSeqno;
		return true;
	}

	// Need to create a new log file
	seqno++;
	bool rslt = openLogFilePriv(seqno);
	aos_assert_r(rslt, false);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosDocFileMgr::createHeader(
		const OmnFilePtr &ff,
		const u32 seqno, 
		const u32 blocksize) 
{
	// Header:
	// 	eOffsetBlocksize(4bytes):	the blocksize
	// 	eOffsetFlags(4bytes):		the flags
	// 	eOffsetBlockStart(4bytes):	the flags
	char *buff = OmnNew char[eFileHeaderSize+10];
	aos_assert_r(buff, false);
	mBuffs[seqno] = buff;
	*(int *)&buff[eOffsetBlocksize] = blocksize;
	*(u32 *)&buff[eOffsetSanityFlag] = eSanityFlags;
	*(u32 *)&buff[eOffsetActiveBlockStart] = blocksize;
	ff->put(0, buff, eFileHeaderSize, true);

	mDocFiles[seqno] = ff;
	return true;
}


bool
AosDocFileMgr::updateHeader(
		const u32 seqno, 
		const u32 blocksize, 
		const u32 blockstart)
{
	// The first given number of bytes are reserved. Currently,
	// 	eOffsetBlocksize(4bytes):	the blocksize
	// 	eOffsetFlags(4bytes):		the flags
	// 	eOffsetBlockStart(4bytes):	the flags
	
	OmnFilePtr ff = mDocFiles[seqno];
	aos_assert_r(ff, false);
	aos_assert_r(ff->isGood(), false);

	char *buff = mBuffs[seqno];
	if (!buff)
	{
		buff = OmnNew char[eFileHeaderSize+10];
		aos_assert_r(buff, false);
		mBuffs[seqno] = buff;
		ff->readToBuff(eFileHeaderStart, eFileHeaderSize, buff);
	}

	u32 bsize = *(u32 *)&buff[eOffsetBlocksize];
	u32 flags = *(u32 *)&buff[eOffsetSanityFlag];
	aos_assert_r(bsize == blocksize, false);
	aos_assert_r(flags == eSanityFlags, false);

	*(u32 *)&buff[eOffsetActiveBlockStart] = blockstart;
	ff->put(0, buff, eFileHeaderSize, true);
	return true;
}


bool
AosDocFileMgr::readDoc(
		const u32 seqno, 
		const u64 offset, 
		char *data, 
		const u32 docsize) 
{
	// This function reads the doc (seqno, offset) into the buffer 'data'.
	// The expected size is 'docsize'. 
	aos_assert_r(offset > 0, false);
	aos_assert_r(data, false);

	//if(seqno & 0x0001 == 1) 
	if(seqno % 2 == 1) 
	{
		bool rs = mIdleBlockMgr->readDoc(seqno, offset, docsize, data);
		aos_assert_r(rs, false);
		return true;
	}

	mLock->lock();
	OmnFilePtr ff = openDocFilePriv(seqno AosMemoryCheckerArgs);
	aos_assert_rl(ff && ff->isGood(), mLock, false);

	u32 blocksize = getBlocksize(seqno);

	AosDiskBlockPtr block = mBlockMgr->getBlock2(ff, seqno, offset, blocksize, false);//enl
	//enl AosDiskBlockPtr block = mBlockMgr->getBlock(ff, seqno, 
	//	offset, blocksize, false, __FILE__, __LINE__);
	aos_assert_rl(block, mLock, false);

	bool rslt = block->readRcd(offset, data, docsize);
	mBlockMgr->returnBlock(block);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();

	return true;
}


OmnFilePtr
AosDocFileMgr::openDocFilePriv(const u32 seqno, bool &isnewfile AosMemoryCheckDecl) 
{
	// This is a private member function. The caller should
	// have locked the class. It opens the file 'seqno'. If the 
	// file does not exist, it will create it.
	if (mDocFiles[seqno].getPtr()) return mDocFiles[seqno];

	OmnString fname = mDirname;
	fname << "/" << mFilename << "_" << seqno;
	mServerReadonly = (OmnApp::getAppConfig()->getAttrStr("serverReadonly") == "true");
	OmnFilePtr ff;
	isnewfile = false;
	if (mServerReadonly)
	{
		ff = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerFileLine);
	}
	else
	{
		ff = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerFileLine);
		if (!ff->isGood())
		{
			// The file has not been created yet. Create it.
			ff = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerFileLine);
			isnewfile = true;
			aos_assert_r(ff && ff->isGood(), 0);
		}
	}

	mDocFiles[seqno] = ff;
	return ff;
}


bool
AosDocFileMgr::modifyDoc(
		u32 &seqno,
		u64 &offset,
		const u32 &docsize, 
		const char *data)
{
	// It updates the doc at [seqno, offset]. 
	// The new doc size is 'docisize'.
	// new docsize is not bigger than the allocated space, simply
	// write it to the file. Otherwise, it will mark the record as
	// being deleted, and allocate a new space for the doc.

	// 1. Ask the DiskBlockMgr to modify the doc
	if((seqno & 0x01) == 1)
	{
		if(docsize > mSmallDocMaxSize)
		{
	    	aos_assert_r(offset>0, false);
			bool rs = mIdleBlockMgr->saveDoc(seqno, offset, docsize, data);
	    	aos_assert_r(rs, false);
			return true;
		}

	   	 aos_assert_r(offset>0, false);
		 bool rs = mIdleBlockMgr->deleteDoc(seqno, offset);
		 aos_assert_r(rs, false);

		 rs = saveNewDoc(docsize, seqno, offset, data);
		 aos_assert_r(rs, false);
		 return true;
	}

    if (docsize > mSmallDocMaxSize)
	{
		 
		bool rs = deleteDoc(seqno, offset);
		aos_assert_r(rs, false);

		// Chen Ding, 10/13/2010
		offset = 0;
	   	aos_assert_r(offset == 0, false);
		rs = mIdleBlockMgr->saveDoc(seqno, offset, docsize, data);
		aos_assert_r(rs, false);
		return true;
	}


	aos_assert_r(offset > 0, false);
	aos_assert_r(data, false);

	mLock->lock();
	bool kickout = false; 
	OmnFilePtr ff = openDocFilePriv(seqno AosMemoryCheckerArgs);
	aos_assert_rl(ff, mLock, false);

	u32 blocksize = getBlocksize(seqno);
	AosDiskBlockPtr block = mBlockMgr->getBlock(
		ff, seqno, offset, blocksize, false, __FILE__, __LINE__);
	aos_assert_rl(block, mLock, false);

	if (!block->modifyRcd(offset, docsize, data, kickout))
	{
		OmnAlarm << "Failed to modify!" << enderr;
		mLock->unlock();
		mBlockMgr->returnBlock(block);
		return false;
	}

	if (!kickout) 
	{
		mBlockMgr->returnBlock(block); //0728
		aos_assert_rl(offset > 0, mLock, false);
		mLock->unlock();
		return true;
	}

	// The doc cannot be wr into the existing block. 
	// Need to find a new place. In the current implementation, 
	// this class keeps an active block that is used to add 
	// new docs.
	block->saveToDisk(false);
	mBlockMgr->returnBlock(block); //0728

	int docsizeIdx = getDocsizeIdx(docsize);

	aos_assert_rl(docsizeIdx>=0, mLock, false);
	block = findActiveBlock(seqno, docsizeIdx);
	aos_assert_rl(block, mLock, false);
	offset = block->addRcd(docsize, data);
	if (offset > 0) 
	{
		mLock->unlock();
		return true;
	}

	block = getEmptyBlock(block, seqno, docsize);
	aos_assert_rl(block, mLock, false);
	offset = block->addRcd(docsize, data);

	aos_assert_rl(offset, mLock, false);
	mLock->unlock();
	return true;
}


bool
AosDocFileMgr::saveNewDoc(
		const u32 &docsize,
		u32 &seqno, 
		u64 &offset, 
		const char *data)
{
	if (docsize > mSmallDocMaxSize)
	{ 
	   	 aos_assert_r(offset == 0, false);
		 bool rs = mIdleBlockMgr->saveDoc(seqno, offset, docsize, data);
		 aos_assert_r(rs, false);
		 return true;
	}
	
	mLock->lock();
	int docsizeIdx = getDocsizeIdx(docsize);

	aos_assert_rl(docsizeIdx >= 0, mLock, false);

	seqno = 0;
	AosDiskBlockPtr block = findActiveBlock(seqno, docsizeIdx);
	aos_assert_rl(block, mLock, false);
	offset = block->addRcd(docsize, data);
	if (offset >0)
	{
		mLock->unlock();
		return true;
	}

	block = getEmptyBlock(block, seqno, docsize);
	aos_assert_rl(block, mLock, false);
	offset = block->addRcd(docsize, data);

	aos_assert_rl(offset, mLock, false);
	mLock->unlock();

	return true;
}


bool
AosDocFileMgr::readHeader(
		const u32 seqno,
		const u32 offset, 
		char *data, 
		const u32 max, 
		int &len)
{
	mLock->lock();
	OmnFilePtr ff = openHeaderFilePriv(seqno AosMemoryCheckerArgs);
	aos_assert_rl(ff, mLock, false);
	ff->lock();

	len = ff->readBinaryInt(offset, -1);
	if (len <= 0 || (u32)len > max)
	{
		// OmnAlarm << "Invalid length: " << len << enderr;
		ff->unlock();
		mLock->unlock();
		return false;
	}

	len = ff->readToBuff(offset+4, len, data);
	ff->unlock();
	mLock->unlock();
	aos_assert_r(len >= 0, false);
	return true;
}


OmnFilePtr
AosDocFileMgr::openHeaderFilePriv(const u32 seqno AosMemoryCheckDecl)
{
	aos_assert_r(seqno < eMaxHeaderFiles, 0);
	OmnFilePtr ff = mHeaderFiles[seqno];
	if (ff) return ff;
	
	OmnString fname = mDirname;
	fname << "/" << mFilename << "_idx_" << seqno;

	mServerReadonly = (OmnApp::getAppConfig()->getAttrStr("serverReadonly") == "true");
	if (mServerReadonly)
	{
		ff = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerFileLine);
	}
	else
	{
		ff = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerFileLine);
	}
	aos_assert_r(ff, NULL);
	if (!ff->isGood())
	{
		ff = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerFileLine);
		aos_assert_r(ff, NULL);
	}

	aos_assert_r(ff->isGood(), NULL);
	mHeaderFiles[seqno] = ff;
	return ff;
}


bool
AosDocFileMgr::saveHeader(
		const u32 seqno,
		const u64 offset, 
		const char *data, 
		const int len, 
		const bool flushflag)
{
	// It saves the header length in the first four bytes, and 
	// then the contents
	mLock->lock();
	OmnFilePtr ff = openHeaderFilePriv(seqno AosMemoryCheckerArgs);
	aos_assert_rl(ff, mLock, false);
	ff->lock();

	ff->setInt(offset, len, false);
	ff->put(offset+sizeof(int), data, len, flushflag);
	ff->unlock();
	mLock->unlock();
	return true;
}

bool
AosDocFileMgr::saveHeaderSegment(
		const u32 seqno,
		const u64 offset, 
		const int totalLen, 
		const u64 offset2, 
		const char *data, 
		const int len, 
		const bool flushflag)
{
	// It saves the header length in the first four bytes, and 
	// then the contents
	mLock->lock();
	OmnFilePtr ff = openHeaderFilePriv(seqno AosMemoryCheckerArgs);
	aos_assert_rl(ff, mLock, false);
	ff->lock();

	ff->setInt(offset, totalLen, false);
	ff->put(offset2, data, len, flushflag);
	ff->unlock();
	mLock->unlock();
	return true;
}


AosXmlTagPtr
AosDocFileMgr::getXmlDoc2(const u32 seqno, const u32 offset)
{
	mLock->lock();
	OmnFilePtr file = openDocFilePriv(seqno AosMemoryCheckerArgs);
	if (!file) 
	{
		mLock->unlock();
		return 0;
	}

	file->lock();
	mLock->unlock();

	int size = file->readBinaryInt(offset + eDocSizeOffset, -2);
	if (size == -1)
	{
		file->unlock();
		OmnAlarm << "Record being deleted: "
			<< seqno << ":" << offset << enderr;
		return 0;
	}

	if (size < 0)
	{
		file->unlock();
		OmnAlarm << "Failed to read the file: " << seqno << ":" << offset << enderr;
		return 0;
	}

	if (size == 0) 
	{
		file->unlock();
		return 0;
	}

	OmnConnBuffPtr buff = OmnNew OmnConnBuff(size);
	char *data = buff->getData();
	int bytesRead = file->readToBuff(offset + eDocOffset, size, data);
	file->unlock();

	aos_assert_r(bytesRead >= 0 && bytesRead == size, 0); 
	buff->setDataLength(size);

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(buff, "" AosMemoryCheckerArgs);
	return xml;
}

bool
AosDocFileMgr::stop()
{
	mLock->lock();
	mBlockMgr->saveAllBlocks();
	for (int i=0; i<eMaxFiles; i++)
	{
		if (mDocFiles[i])
		{
			mDocFiles[i]->closeFile();
			mDocFiles[i] = 0;
		}

		if (mHeaderFiles[i])
		{
			mHeaderFiles[i]->closeFile();
			mHeaderFiles[i] = 0;
		}
	}
	mLock->unlock();

	return true;
}


// Temp
AosXmlTagPtr
AosDocFileMgr::getXmlDoc3(const u32 seqno, const u32 offset)
{
	mLock->lock();
	OmnFilePtr file = openDocFilePriv(seqno AosMemoryCheckerArgs);
	if (!file) 
	{
		mLock->unlock();
		return 0;
	}

	file->lock();
	mLock->unlock();

	int size = file->readBinaryInt(offset + eDocSizeOffset, -2);
	if (size == -1)
	{
		file->unlock();
		OmnAlarm << "Record being deleted: "
			<< seqno << ":" << offset << enderr;
		return 0;
	}

	if (size < 0)
	{
		file->unlock();
		OmnAlarm << "Failed to read the file: " << seqno << ":" << offset << enderr;
		return 0;
	}

	if (size == 0) 
	{
		file->unlock();
		return 0;
	}

	OmnConnBuffPtr buff = OmnNew OmnConnBuff(size);
	char *data = buff->getData();
	int bytesRead = file->readToBuff(offset + 4, size, data);
	file->unlock();

	aos_assert_r(bytesRead >= 0 && bytesRead == size, 0); 
	buff->setDataLength(size);

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(buff, "" AosMemoryCheckerArgs);
	return xml;
}


AosDiskBlockPtr
AosDocFileMgr::findActiveBlock(u32 &seqno, const int size_idx)
{
	// This function checks whether the current file 'seqno'
	// has an active block. If yes, it creates the block and
	// returns the block. A file has an active block if the
	// current block start (eOffsetBlockStart) + block size
	// is smaller than the max file size.
	if (mActiveBlocks[size_idx].getPtr()) 
	{
		seqno = mActiveBlocks[size_idx]->getSeqno();
		return mActiveBlocks[size_idx];
	}

	u32 blocksize = mBlocksizes[size_idx];
	for (; seqno < mMaxDocFiles; seqno=seqno+2)
	{
		bool isnewfile = false;
		OmnFilePtr ff = openDocFilePriv(seqno, isnewfile AosMemoryCheckerArgs);
		aos_assert_r(ff, 0);
		if (isnewfile)
		{
			createHeader(ff, seqno, blocksize);
			u32 blockstart = getBlockStart(seqno);
			aos_assert_r(blockstart > 0, 0);
			AosDiskBlockPtr block = mBlockMgr->getBlock(
				ff, seqno, blockstart, blocksize, true, __FILE__, __LINE__);
			aos_assert_r(block, 0);
			mActiveBlocks[size_idx] = block;
			return block;
		}

		// The file is not a new one. Check its block size
		u32 bsize = getBlocksize(seqno);
		if (bsize == blocksize)
		{
			u32 blockstart = getBlockStart(seqno);
			if (blockstart + blocksize <= eAosMaxFileSize)
			{
				AosDiskBlockPtr block = mBlockMgr->getBlock(
					ff, seqno, blockstart, blocksize, false, __FILE__, __LINE__);
				aos_assert_r(block, 0);
				mActiveBlocks[size_idx] = block;
				return block;
			}
		}
	}

	OmnAlarm << "System run out of space: " << mMaxDocFiles << enderr;
	return 0;
}


char *
AosDocFileMgr::readFileHeaderfi(const OmnFilePtr file)
{
	// The first given number of bytes are reserved. Currently,
	// 	eOffsetBlocksize(4bytes):	the blocksize
	// 	eOffsetFlags(4bytes):		the flags
	// 	eOffsetBlockStart(4bytes):	the flags

	aos_assert_r(file, 0);
	aos_assert_r(file->isGood(), 0);

	char *buff = OmnNew char[eFileHeaderSize+10];
	aos_assert_r(buff, 0);
	file->readToBuff(eFileHeaderStart, eFileHeaderSize, buff);
	return buff;
}


bool
AosDocFileMgr::doesFileSizeMatch(const u32 seqno, const int size_idx)
{
	// This function checks whether the file block size matches
	// the one indicated by 'sie_idx'. 
	u32 blocksize = getBlocksize(seqno);
	aos_assert_r(blocksize > 0, false);

	for (int i=0; i<eMaxBlockSizes; i++)
	{
		if (blocksize == mBlocksizes[size_idx]) return true;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDocFileMgr::openLogFilePriv(const u32 seqno) 
{
	// This is a private member function. The caller should
	// have locked the class. It opens the file 'seqno'. If the 
	// file does not exist, it will create it.
	/*
	OmnString fname = mDirname;
	fname << "/" << mLogFilename << "_" << seqno;
	mServerReadonly = (OmnApp::getAppConfig()->getAttrStr("serverReadonly") == "true");
	OmnFilePtr ff;
	if (mServerReadonly)
	{
		ff = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	}
	else
	{
		ff = OmnNew OmnFile(fname, OmnFile::eReadWrite);
		if (!ff->isGood())
		{
			// The file has not been created yet. Create it.
			ff = OmnNew OmnFile(fname, OmnFile::eCreate);
			aos_assert_r(ff->isGood(), false);
		}
	}

	mLogFiles[seqno] = ff;
	mCrtLogSeqno = seqno;
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


char *
AosDocFileMgr::readFileHeader(const u32 seqno)
{
	// The first given number of bytes are reserved. Currently,
	// 	eOffsetBlocksize(4bytes):	the blocksize
	// 	eOffsetFlags(4bytes):		the flags
	// 	eOffsetBlockStart(4bytes):	the flags
	if (mBuffs[seqno]) return mBuffs[seqno];

	OmnFilePtr ff = mDocFiles[seqno];
	if (!ff) ff = openDocFilePriv(seqno);
	aos_assert_r(ff, 0);
	aos_assert_r(ff->isGood(), 0);

	char *buff = OmnNew char[eFileHeaderSize+10];
	aos_assert_r(buff, 0);
	mBuffs[seqno] = buff;
	ff->readToBuff(eFileHeaderStart, eFileHeaderSize, buff);
	return mBuffs[seqno];
}


u32
AosDocFileMgr::getBlockStart(const u32 seqno)
{
	char *buff = readFileHeader(seqno);
	aos_assert_r(buff, 0);

	return *(u32 *)&buff[eOffsetActiveBlockStart];
}

u32
AosDocFileMgr::getBlocksize(const OmnFilePtr file)
{
	// This function retrieves the block size for the file 'file'
	char *buff = readFileHeaderfi(file);
	aos_assert_r(buff, 0);
	u32 flags = *(u32 *)&buff[eOffsetSanityFlag];
	if (flags != eSanityFlags) return 0;
	return *(u32 *)&buff[eOffsetBlocksize];
}

u32
AosDocFileMgr::getBlocksize(const u32 seqno)
{
	char *buff = readFileHeader(seqno);
	aos_assert_r(buff, 0);
	u32 flags = *(u32 *)&buff[eOffsetSanityFlag];
	if (flags != eSanityFlags) return 0;
	return *(u32 *)&buff[eOffsetBlocksize];
}


AosDiskBlockPtr
AosDocFileMgr::getEmptyBlock(
		const AosDiskBlockPtr &block, 
		u32 &seqno,
		u32 needsize)
{
	// It closes the current block and creates the next block, if possible.
	// Otherwise, it looks for a new one.
 	bool rslt = block->saveToDisk(true);
	aos_assert_r(rslt, 0);
	u32 blocksize = getBlocksize(seqno);
	u32 blockstart = getBlockStart(seqno) + blocksize;
	updateHeader(seqno, blocksize, blockstart);

	// Chen Ding, 10/15/2010
	AosDiskBlockPtr newblock;
	if (blockstart + blocksize <= eAosMaxFileSize)
	{
		// We can create a new active block for the file.
		OmnFilePtr ff = openDocFilePriv(seqno AosMemoryCheckerArgs);
		aos_assert_r(ff, 0);
		newblock = mBlockMgr->getBlock(
			ff, seqno, blockstart, blocksize, true, __FILE__, __LINE__);
	}

	bool found = false;
	for (int i=0; i<eMaxBlockSizes; i++)
	{
		if (block == mActiveBlocks[i]) 
		{
			mActiveBlocks[i] = newblock;
			mBlockMgr->returnBlock(block);
			if (newblock.getPtr()) return newblock;
			found = true;
			break;
		}
	}
	aos_assert_r(found, 0);

	int docsizeIdx = getDocsizeIdx(needsize);
	aos_assert_r(docsizeIdx >= 0, NULL);
	return findActiveBlock(seqno, docsizeIdx);
}


int 
AosDocFileMgr::getDocsizeIdx(const u32 docsize)
{
	for (int i=0; i<eMaxBlockSizes; i++)
	{
		if (docsize <= mBlocksizeIdxs[i]) return i;
	}

	OmnAlarm << "Docsize too big!" << enderr;
	return -1;
}

bool
AosDocFileMgr::deleteDoc(
		const u32 seqno, 
		const u64 offset) 
{
	aos_assert_r(offset > 0, false);

	if(seqno % 2 == 1)
	{
		bool rs = mIdleBlockMgr->deleteDoc(seqno, offset);
		aos_assert_r(rs, false);
		return true;
	}

	mLock->lock();
	OmnFilePtr ff = openDocFilePriv(seqno AosMemoryCheckerArgs);
	aos_assert_rl(ff && ff->isGood(), mLock, false);

	u32 blocksize = getBlocksize(seqno);

	AosDiskBlockPtr block = mBlockMgr->getBlock2(ff, seqno, offset, blocksize, false);//enl
	aos_assert_rl(block, mLock, false);

	bool rslt = block->deleteRcd(offset);
	mBlockMgr->returnBlock(block);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();

	return true;
}
#endif
