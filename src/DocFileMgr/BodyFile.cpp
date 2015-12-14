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
//
// Modification History:
// 01/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/BodyFile.h"

#include "API/AosApiC.h"
#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"



AosBodyFile::AosBodyFile(const AosReliableFilePtr &file, const AosRundataPtr &rdata)
:
mFile(file),
mBlockSize(0),
mLastBlock(0)
{
	readHeader(rdata);
}


AosBodyFile::AosBodyFile(
		const AosReliableFilePtr &file, 
		const u32 blocksize, 
		const u32 lastblock,
		const AosRundataPtr &rdata)
:
mFile(file),
mBlockSize(blocksize),
mLastBlock(lastblock)
{
	saveHeader(rdata);
}
	

AosBodyFile::~AosBodyFile()
{
}

bool
AosBodyFile::readHeader(const AosRundataPtr &rdata)
{
	// Header:
	// 	eOffsetBlocksize(4bytes):	the blocksize
	// 	eOffsetFlags(4bytes):		the flags
	// 	eOffsetBlockStart(4bytes):	the flags
	aos_assert_rr(mFile, rdata, false);

	//char buff[eFileHeaderSize+10];
	AosBuffPtr buff = OmnNew AosBuff(eFileHeaderSize+10, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eFileHeaderSize+10);

	mFile->readlock();
	bool rslt = mFile->readToBuff(0, eFileHeaderSize, buff->data(), rdata.getPtr());
	mFile->unlock();
	aos_assert_r(rslt, false);

	//aos_assert_rr(nn == eFileHeaderSize, rdata, false);
	buff->setDataLen(eFileHeaderSize);

	// Ketty 2012/09/10
	bool data_good = AosCheckCheckSum(buff->data(), eFileHeaderSize);
	if(!data_good)
	{
		bool rslt = mFile->recover(rdata.getPtr());
		aos_assert_r(rslt, false);
		
		rslt = mFile->readToBuff(0, eFileHeaderSize, buff->data(), rdata.getPtr());
		aos_assert_r(rslt, false);
		data_good = AosCheckCheckSum(buff->data(), eFileHeaderSize);
		if(!data_good)
		{
			AosSetErrorU(rdata, "file_not_good") << ": " << mFile->getFileId() << enderr;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	//aos_assert_r(*(u32 *)&buff[eOffsetSanityFlag] == eSanityFlags, false);
	//mBlockSize = *(int *)&buff[eOffsetBlocksize];
	//mLastBlock = *(u32 *)&buff[eOffsetActiveBlockStart];
	aos_assert_r(buff->getU32(0) == eSanityFlags, false);
	mBlockSize = buff->getInt(-1);
	mLastBlock = buff->getU32(0);
	return true;
}


bool
AosBodyFile::saveHeader(const AosRundataPtr &rdata)
{
	// Header:
	// 	eOffsetBlocksize(4bytes):	the blocksize
	// 	eOffsetFlags(4bytes):		the flags
	// 	eOffsetBlockStart(4bytes):	the flags
	aos_assert_rr(mFile, rdata, false);
	//char buff[eFileHeaderSize+10];
	AosBuffPtr buff = OmnNew AosBuff(eFileHeaderSize+10, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eFileHeaderSize+10);

	// Ketty 2012/09/10
	//*(int *)&buff[eOffsetBlocksize] = mBlockSize;
	//*(u32 *)&buff[eOffsetSanityFlag] = eSanityFlags;
	//*(u32 *)&buff[eOffsetActiveBlockStart] = mLastBlock;
	buff->setU32(eSanityFlags);
	buff->setInt(mBlockSize);
	buff->setU32(mLastBlock);
	u8 check_sum = AosCalculateCheckSum(buff->data(), buff->dataLen());
	buff->setU8(check_sum);

	mFile->writelock();
	bool rslt = mFile->put(0, buff->data(), eFileHeaderSize, true, rdata.getPtr());
	mFile->unlock();
	aos_assert_rr(rslt, rdata, false);
	return true;
}

// Ketty 2014/02/21
bool
AosBodyFile::closeFile()
{
	aos_assert_r(mFile, false);
	return mFile->closeFile();
}

u32 
AosBodyFile::appendBlock(const AosRundataPtr &rdata)
{
	mLastBlock += mBlockSize;
	bool rslt = saveHeader(rdata);
	aos_assert_rr(rslt, rdata, 0);
	return mLastBlock;
}

	
AosReliableFilePtr
AosBodyFile::getFile() const 
{
	return mFile;
}

