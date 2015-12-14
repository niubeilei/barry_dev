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
// 02/07/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/SlabBlockFile.h"

#include "API/AosApiC.h"
#include "Rundata/Rundata.h"
#include "ReliableFile/ReliableFile.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/Mutex.h"

#include <math.h>
char AosSlabBlockFile::smCheck[eCheckSize];
char AosSlabBlockFile::smCheckChar[8];
char AosSlabBlockFile::smCheckChar2[8];
bool AosSlabBlockFile::smInit = false;


AosSlabBlockFile::AosSlabBlockFile(const AosReliableFilePtr &file, const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mFile(file),
mBlockSize(0),
mCrtMaxBlockId(1)
{
	if(!smInit)	 sInit();
	
	mBitmap = OmnNew char[eBitmapSize];
	memset(mBitmap, 0, eBitmapSize);
	readHeader(rdata);
	aos_assert(mBlockSize);
	mMaxBlockNum = (eMaxFileSize - eFileHeaderSize)/mBlockSize; 
	if (mMaxBlockNum>eBitmapSize*8)
		mMaxBlockNum = eBitmapSize * 8;
	//mMaxBlockNum = 20;	//Test
}


AosSlabBlockFile::AosSlabBlockFile(
		const AosReliableFilePtr &file, 
		const u32 blocksize,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mFile(file),
mBlockSize(blocksize),
mCrtMaxBlockId(1)
{
	if(!smInit)	 sInit();
	
	mBitmap = OmnNew char[eBitmapSize];
	memset(mBitmap, 0, eBitmapSize);
	mBitmap[0] = 0x80;
	//appendBitmap(0, rdata);	//blockId 0 is for error check.
	saveHeader(rdata);
	
	aos_assert(mBlockSize);
	mMaxBlockNum = (eMaxFileSize - eFileHeaderSize)/mBlockSize; 
	if (mMaxBlockNum>eBitmapSize*8)
		mMaxBlockNum = eBitmapSize *8;
	//mMaxBlockNum = 20;	//Test
}


AosSlabBlockFile::~AosSlabBlockFile()
{
	OmnDelete [] mBitmap;	
}


void
AosSlabBlockFile::sInit()
{
	smInit = true;
	memset(smCheck, -1, eCheckSize); 
	smCheckChar[0] = 0b01111111;	smCheckChar2[0] = 0b10000000;
	smCheckChar[1] = 0b10111111;	smCheckChar2[1] = 0b01000000;
	smCheckChar[2] = 0b11011111;	smCheckChar2[2] = 0b00100000;
	smCheckChar[3] = 0b11101111;	smCheckChar2[3] = 0b00010000;
	smCheckChar[4] = 0b11110111;	smCheckChar2[4] = 0b00001000;
	smCheckChar[5] = 0b11111011;	smCheckChar2[5] = 0b00000100;
	smCheckChar[6] = 0b11111101;	smCheckChar2[6] = 0b00000010;
	smCheckChar[7] = 0b11111110;	smCheckChar2[7] = 0b00000001;
}


int
AosSlabBlockFile::getSubBlockId(char c)
{
	for(u32 i=0; i<8; i++)
	{
		if((c | smCheckChar[i]) == smCheckChar[i])	return i;
	}
	
	OmnAlarm << "Should never come here" << enderr;
	return -1;
}



bool
AosSlabBlockFile::readHeader(const AosRundataPtr &rdata)
{
	// Header:
	// 	eOffsetBlocksize(4bytes):		the blocksize
	// 	eOffsetCrtMaxBlockId(4bytes):	the crtMaxBlockId
	// 	eOffsetBitmap:					the bitmap info
	
	aos_assert_r(mFile, false);

	AosBuffPtr buff = OmnNew AosBuff(eFileHeaderSize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eFileHeaderSize);

	mFile->readlock(); 
	bool rslt = mFile->readToBuff(0, eFileHeaderSize, buff->data(), rdata.getPtr());
	mFile->unlock();
	aos_assert_r(rslt, false);
	buff->setDataLen(eFileHeaderSize);
	
	OmnString cal_sign = AosCalculateSign((AosSignatureAlgo::E)eSignAlgo,
			        buff->data() + eOffsetBitmap, eBitmapSize);
	char * sign_data = buff->data() + eOffsetHeaderSign;
	if(memcmp(sign_data, cal_sign.data(), eSignLen) != 0)
	{
		// data is not good.
		rslt = mFile->recover(rdata.getPtr());
		aos_assert_r(rslt, false);
		
		memset(buff->data(), 0, eFileHeaderSize);
		rslt = mFile->readToBuff(0, eFileHeaderSize, buff->data(), rdata.getPtr());
		aos_assert_r(rslt, false);
		buff->setDataLen(eFileHeaderSize);
	
		cal_sign = AosCalculateSign((AosSignatureAlgo::E)eSignAlgo,
					buff->data() + eOffsetBitmap, eBitmapSize);
		sign_data = buff->data() + eOffsetHeaderSign;
		if(memcmp(sign_data, cal_sign.data(), eSignLen) != 0)
		{   
			OmnAlarm << "The diskblock file is damaged." << mFile->getFileId() << enderr;
			return false;
		}
	}

	buff->setCrtIdx(eOffsetBlocksize);
	mBlockSize = buff->getU32(0);
	aos_assert_r(mBlockSize, false);
	mCrtMaxBlockId = buff->getU32(0);

	char * bitmap_data = buff->data() + eOffsetBitmap;
	memcpy(mBitmap, bitmap_data, eBitmapSize);
	return true;
}


bool
AosSlabBlockFile::saveBitmap(const AosRundataPtr &rdata)
{
	mLock->lock();
	saveHeader(rdata);
	mLock->unlock();

	return true;
}


bool
AosSlabBlockFile::saveHeader(const AosRundataPtr &rdata)
{
	// Header:
	// 	eOffsetBlocksize(4bytes):		the blocksize
	// 	eOffsetBitmap:					the bitmap info
	
	// When call this func. must locked.
	
	aos_assert_r(mFile, false);
	
	OmnString signature = AosCalculateSign(
			(AosSignatureAlgo::E)eSignAlgo, mBitmap, eBitmapSize);

	//Linda, 2013/08/13
	AosBuffPtr buff = OmnNew AosBuff(eFileHeaderSize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eFileHeaderSize);
	buff->setCrtIdx(eOffsetBlocksize);
	buff->setU32(mBlockSize);

	buff->setCrtIdx(eOffsetCrtMaxBlockId);
	buff->setU32(mCrtMaxBlockId);

	buff->setCrtIdx(eOffsetBitmap);
	buff->setBuff(mBitmap, eBitmapSize);

	buff->setCrtIdx(eOffsetHeaderSign);
	buff->setBuff(signature.data(), eSignLen);
	mFile->put(eOffsetBlocksize, buff->data(), eFileHeaderSize, true, rdata.getPtr());

	//mFile->setU32(eOffsetBlocksize, mBlockSize, false, rdata);
	//mFile->setU32(eOffsetCrtMaxBlockId, mCrtMaxBlockId, false, rdata);
	//mFile->put(eOffsetBitmap, mBitmap, eBitmapSize, false, rdata);
	//mFile->put(eOffsetHeaderSign, signature.data(), eSignLen, true, rdata);	

	mSaved = true;
	return true;
}


bool
AosSlabBlockFile::appendBitmap(const u32 blockId, const AosRundataPtr &rdata)
{
	// When call this func. must locked.
	u32 charLoc = blockId/8;
	u32 subBlockId = blockId%8;
	
	// check whether this blockId is 0
	if((mBitmap[charLoc] & smCheckChar2[subBlockId]) == smCheckChar2[subBlockId])
	{
		OmnAlarm << "this blockId has the data: " << blockId << enderr;
		return false;
	}

	mBitmap[charLoc] |= smCheckChar2[subBlockId];	

	// Ketty maybe will delete later. !!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//
	//Linda, 2013/08/01
	//saveHeader(rdata);	//Ketty 2012/09/24
	return true;	
}


bool
AosSlabBlockFile::removeBitmap(const u32 blockId)
{
	// When call this func. must locked.
	u32 charLoc = blockId/8;
	u32 subBlockId = blockId%8;
	
	// check whether this blockId is 1
	if((mBitmap[charLoc] & smCheckChar2[subBlockId]) != smCheckChar2[subBlockId])
	{
		//OmnAlarm << "this blockId has deleted: " << blockId << enderr;
		//return false;
		return true;
	}
	mBitmap[charLoc] &= smCheckChar[subBlockId];	
	
	return true;	
}


bool
AosSlabBlockFile::findEmptyBlock(
		bool &find, 
		u32 &blockId,
		const AosRundataPtr &rdata)
{
	// find the first emptyBlock.
	//if(mCrtMaxBlockId == 0) mCrtMaxBlockId = 21474;		// test.
	mLock->lock();
	if(mCrtMaxBlockId < mMaxBlockNum)
	{
		find = true;
		blockId = mCrtMaxBlockId++;
		aos_assert_rl(blockId <= eBitmapSize*8, mLock, false);
		bool rslt = appendBitmap(blockId, rdata);		// mark this block Has used.
		aos_assert_rl(rslt, mLock, false);
		mLock->unlock();
		return true;
	}
	
	find = false;
	int bitmapsize = floor(mMaxBlockNum/8.0);
	aos_assert_r(bitmapsize > 0, false);
	u32 totalCmpTimes = bitmapsize/eCheckSize; 
	if(bitmapsize % eCheckSize >0)	totalCmpTimes++;
	u32 crtLoc= 0, cmpNum = eCheckSize;
	int rslt = 0;
	for(u32 i=0; i<totalCmpTimes; i++)
	{
		u32 remainNum = bitmapsize - eCheckSize * i; 	
		if(remainNum < eCheckSize) cmpNum = remainNum;

		rslt = memcmp(&mBitmap[crtLoc], smCheck, cmpNum);
		if(rslt != 0)
		{
			find = true;
			break;
		}
		crtLoc += eCheckSize;
	}

	if(!find)
	{
		mLock->unlock();
		return true;	
	}
	
	// this means the empty block is between in crtLoc-eCheckSize of mBitmap
	u32 left = crtLoc, right = left + cmpNum -1;
	while(left < right)
	{
		u32 subCmpNum = cmpNum /2;	
		rslt = memcmp(&mBitmap[left], smCheck, subCmpNum);
		if(rslt !=0)
		{
			right = left + subCmpNum -1;
			cmpNum = subCmpNum;
			continue;
		}
		
		left = left + subCmpNum;
		cmpNum = cmpNum - subCmpNum;
	}
	rslt = memcmp(&mBitmap[left], smCheck, 1);
	if(rslt == 0)
	{
		mLock->unlock();
		OmnAlarm << " error !" << enderr;
		return false;
	}

	// the empty block is left. becaule left == right.
	int subBlockId = getSubBlockId(mBitmap[left]);
	aos_assert_rl(subBlockId >=0, mLock, false);
	blockId = left * 8 + subBlockId;					
	aos_assert_rl(blockId <= (u32)bitmapsize*8, mLock, false);
	aos_assert_rl(blockId > 0, mLock, false);
	//sanityCheck(blockId);
	
	rslt = appendBitmap(blockId, rdata);		// mark this block Has used.
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}



bool
AosSlabBlockFile::removeDoc(const u32 blockId)
{
	// When returnBlock. the bitmap need't save to file immediately.
	mLock->lock();
	bool rslt = removeBitmap(blockId);
	aos_assert_rl(rslt, mLock, false);
	//sanityAdd(blockId);
	mLock->unlock();
	return true;
}

	
bool
AosSlabBlockFile::saveDoc(
		const u32 blockId, 
		const u32 orig_len,
		const u32 compress_len,
		const char *data,
		const AosRundataPtr &rdata)
{
	// the bitmap has marked when find the empty block.
	//	
	//aos_assert_r(dataLen <= mBlockSize, false);
	aos_assert_r(orig_len <= getMaxDocSize(mBlockSize) 
			|| compress_len <= getMaxDocSize(mBlockSize), false);

	mLock->lock();
	u64 offset = eOffsetBlockStart + (u64)blockId*mBlockSize;
	
	// if the compress len is == 0, means this doc not compress yet.
	u32 data_len = compress_len ? compress_len : orig_len;
	OmnString signature = AosCalculateSign(
			(AosSignatureAlgo::E)eSignAlgo, data, data_len);

	//Linda, 2013/08/13
	AosBuffPtr buff = OmnNew AosBuff(mBlockSize, 0 AosMemoryCheckerArgs); 
	memset(buff->data(), 0, mBlockSize);
	buff->setCrtIdx(eOrigLenOff);
	buff->setU32(orig_len);
	buff->setU32(compress_len);
	buff->setBuff(data, data_len);
	u32 sign_off = mBlockSize - eSignLen; 
	buff->setCrtIdx(sign_off);
	buff->setBuff(signature.data(), eSignLen);

	mFile->put(offset, buff->data(), mBlockSize, true, rdata.getPtr());
	//mFile->setU32(offset + eOrigLenOff, orig_len, false, rdata);
	//mFile->setU32(offset + eCompressLenOff, compress_len, false, rdata);
	//mFile->put(offset + eDataOff, data, data_len, false, rdata);
	//u32 sign_off = mBlockSize - eSignLen; 
	//mFile->put(offset + sign_off, signature.data(), eSignLen, true, rdata);

	mLock->unlock();
	return true;
}


bool
AosSlabBlockFile::readDoc(
		const u32 blockId, 
		AosBuffPtr &buff,
		u32 &orig_len,
		u32 &compress_len,
		const AosRundataPtr &rdata)
{
	buff = OmnNew AosBuff(mBlockSize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, mBlockSize);
	
	mLock->lock();
	u64 offset = eOffsetBlockStart + (u64)blockId*mBlockSize;
	bool rslt = mFile->readToBuff(offset, mBlockSize, buff->data(), rdata.getPtr());
	aos_assert_r(rslt, false);
	buff->setDataLen(mBlockSize);

	u32 sign_off = mBlockSize - eSignLen; 

	orig_len = buff->getU32(0);
	compress_len = buff->getU32(0);
	u32 data_len = compress_len ? compress_len : orig_len;
	aos_assert_r(data_len, false);

	OmnString cal_sign = AosCalculateSign((AosSignatureAlgo::E)eSignAlgo, 
			buff->data() + eDataOff, data_len);
	char * sign_data = buff->data() + sign_off;
	if(memcmp(sign_data, cal_sign.data(), eSignLen) != 0)
	{
		// data is not good. 
		rslt = mFile->recover(rdata.getPtr());
		aos_assert_rl(rslt, mLock, false);
		
		memset(buff->data(), 0, mBlockSize);
		rslt = mFile->readToBuff(offset, mBlockSize, buff->data(), rdata.getPtr());
		aos_assert_rl(rslt, mLock, false);
		buff->setDataLen(mBlockSize);
	
		orig_len = buff->getU32(0);
		compress_len = buff->getU32(0);
		data_len = compress_len ? compress_len : orig_len;
		
		cal_sign = AosCalculateSign((AosSignatureAlgo::E)eSignAlgo, 
				buff->data() + eDataOff, data_len);
		sign_data = buff->data() + sign_off;
		if(memcmp(sign_data, cal_sign.data(), eSignLen) != 0)
		{
			OmnAlarm << "The diskblock file is damaged." << mFile->getFileId() << enderr;
			mLock->unlock();
			return false;
		}
	}
	mLock->unlock();
	
	// We need to remove 'orig_len' and 'compress_len'.
	char *data = buff->data();
	memmove(data, &data[eDataOff], data_len);
	buff->setDataLen(data_len);
	return true;
}


bool
AosSlabBlockFile::sanityCheck(const u32 blockId)
{
	set<u32>::iterator it = mSanitySet.begin();
	aos_assert_r(it != mSanitySet.end(), false);

	u32 id = *it;
	aos_assert_r(id == blockId, false);
	mSanitySet.erase(it);	
	return true;
}

bool
AosSlabBlockFile::sanityAdd(const u32 blockId)
{
	mSanitySet.insert(blockId);
	return true;
}


bool
AosSlabBlockFile::addDocs(
		const AosRundataPtr &rdata,
		const u32 blockId,
		const char *data,
		const u32 data_len)
{
	aos_assert_r(blockId < mMaxBlockNum, false);
	mLock->lock();
	u64 offset = eOffsetBlockStart + (u64)blockId *mBlockSize;
	mFile->put(offset, data, data_len, false, rdata.getPtr());
	mLock->unlock();
	return true;
}


bool
AosSlabBlockFile::saveDocToBuff(
		const u32 index,
		const AosBuffPtr &buff,
		const u32 orig_len,
		const u32 compress_len,
		const char *data)
{
	aos_assert_r(orig_len <= getMaxDocSize(mBlockSize) 
			|| compress_len <= getMaxDocSize(mBlockSize), false);

	u32 offset  = index * mBlockSize;

	// if the compress len is == 0, means this doc not compress yet.
	u32 data_len = compress_len ? compress_len : orig_len;
	aos_assert_r(data_len > 0, false);
	OmnString signature = AosCalculateSign(
			(AosSignatureAlgo::E)eSignAlgo, data, data_len);

	buff->setCrtIdx(offset);
	buff->setU32(orig_len);
	buff->setU32(compress_len);
	buff->setBuff(data, data_len);
	u32 sign_off = mBlockSize - eSignLen; 
	buff->setCrtIdx(offset + sign_off);
	buff->setBuff(signature.data(), eSignLen);
	return true;
}


/*
 * Linda, 2013/08/16
AosBuffPtr
AosSlabBlockFile::readDocs(
		const AosRundataPtr &rdata,
		const u32 begin_blockid,
		u32 &expect_num)
{
	aos_assert_r(begin_blockid < mMaxBlockNum, 0);
	if (expect_num == 0) expect_num = 1;
	u32 read_num = mMaxBlockNum - begin_blockid;
	if (read_num < expect_num) expect_num = read_num;

	u32 read_size = expect_num * mBlockSize;
	AosBuffPtr buff = OmnNew AosBuff(read_size, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, read_size);
	
	mLock->lock();
	u64 offset = eOffsetBlockStart + (u64)begin_blockid*mBlockSize;
	bool rslt = mFile->readToBuff(offset, read_size, buff->data(), rdata);
	aos_assert_r(rslt, 0);
	buff->setDataLen(read_size);
	mLock->unlock();
	return buff;
}


bool
AosSlabBlockFile::saveDocToBuff(
		const u32 idx,
		const AosBuffPtr &buff,
		const u32 orig_len,
		const u32 compress_len,
		const char *data)
{
	aos_assert_r(orig_len <= getMaxDocSize(mBlockSize) 
			|| compress_len <= getMaxDocSize(mBlockSize), false);

	u32 offset  = idx * mBlockSize;
	aos_assert_r(offset < (u32)buff->buffLen(), false);

	// if the compress len is == 0, means this doc not compress yet.
	u32 data_len = compress_len ? compress_len : orig_len;
	aos_assert_r(data_len > 0, false);
	OmnString signature = AosCalculateSign(
			(AosSignatureAlgo::E)eSignAlgo, data, data_len);

	buff->setCrtIdx(offset);
	buff->setU32(orig_len);
	buff->setU32(compress_len);
	buff->setBuff(data, data_len);
	u32 sign_off = mBlockSize - eSignLen; 
	buff->setCrtIdx(offset + sign_off);
	buff->setBuff(signature.data(), eSignLen);
	return true;
}
*/



/*
bool
AosSlabBlockFile::checkSign(const AosBuffPtr &buff, const u32 buff_len)
{
	u32 data_len = buff_len - eSignLen;
	OmnString cal_sign = AosCalculateSign((AosSignatureAlgo::E)eSignAlgo, 
			buff->data(), data_len);
	char * sign_data = buff->data() + data_len;
	return (memcmp(sign_data, cal_sign.data(), eSignLen) == 0);
}


void
AosSlabBlockFile::setSign(const AosBuffPtr &buff, const u32 buff_len)
{
	u32 data_len = buff_len - eSignLen;
	OmnString signature = AosCalculateSign((AosSignatureAlgo::E)eSignAlgo, 
			buff->data(), data_len);
	u32 crt_idx = buff->getCrtIdx();
	aos_assert(data_len >= crt_idx);
	buff->setCrtIdx(data_len);

OmnScreen << "ktttt, signature: " << signature
		<< " crtoff:" << data_len
		<< endl;

	buff->setBuff(signature.data(), signature.length());
}
*/


/*
bool
AosSlabBlockFile::addDoc(
		const u32 blockId, 
		const u32 dataLen,
		const char *data,
		const AosRundataPtr &rdata)
{
	// the bitmap has marked when find the empty block.
	//
	aos_assert_r(dataLen <= mBlockSize, false);

	mLock->lock();
	
	AosBuffPtr buff = OmnNew AosBuff(mBlockSize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, mBlockSize);
	buff->setU32(dataLen);
	buff->setBuff(data, dataLen);
	setSign(buff, mBlockSize);		// Ketty 2012/09/11

	u64 offset = eOffsetBlockStart + (u64)blockId*mBlockSize;
	bool rslt = mFile->put(offset, buff->data(), mBlockSize, true, rdata);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return true;
}


bool
AosSlabBlockFile::modifyDoc(
		const u32 blockId, 
		const u32 dataLen,
		const char *data,
		const AosRundataPtr &rdata)
{
	aos_assert_r(dataLen <= mBlockSize, false);
	
	mLock->lock();
	
	AosBuffPtr buff = OmnNew AosBuff(mBlockSize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, mBlockSize);
	buff->setU32(dataLen);
	buff->setBuff(data, dataLen);
	setSign(buff, mBlockSize);		// Ketty 2012/09/11

	u64 offset = eOffsetBlockStart + (u64)blockId*mBlockSize;
	bool rslt = mFile->put(offset, buff->data(), mBlockSize, true, rdata);
	aos_assert_rl(rslt, mLock, false);

OmnScreen << "ktttt, offset: " << offset 
		<< " datalen:" << dataLen
		<< endl;

	mLock->unlock();
	return true;
}


bool
AosSlabBlockFile::readDoc(
		const u32 blockId, 
		AosBuffPtr &data_buff,
		const AosRundataPtr &rdata)
{
	AosBuffPtr buff = OmnNew AosBuff(mBlockSize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, mBlockSize);
	
	mLock->lock();
	u64 offset = eOffsetBlockStart + (u64)blockId*mBlockSize;
	bool rslt = mFile->readToBuff(offset, mBlockSize, buff->data(), rdata);
	aos_assert_r(rslt, false);
	buff->setDataLen(mBlockSize);

	// Ketty 2012/09/11
	bool data_good = checkSign(buff, mBlockSize);
	if(!data_good)
	{
		rslt = mFile->recover(rdata);
		aos_assert_rl(rslt, mLock, false);
		
		memset(buff->data(), 0, mBlockSize);
		rslt = mFile->readToBuff(offset, mBlockSize, buff->data(), rdata);
		aos_assert_rl(rslt, mLock, false);
		buff->setDataLen(mBlockSize);
		
		data_good = checkSign(buff, mBlockSize);
		if(!data_good)
		{   
			OmnAlarm << "The diskblock file is damaged." << mFile->getFileId() << enderr;
			mLock->unlock();
			return false;
		}
	}
	mLock->unlock();
	
	u32 size = buff->getU32(0);
	data_buff = OmnNew AosBuff(size, 0 AosMemoryCheckerArgs);
	data_buff->setBuff(buff->data() + sizeof(u32), size);
	//data_buff->setDataLen(size);
	//aos_assert_rl(size==dataLen, mLock, false);

	//char* data_buff = buff->data() + sizeof(u32);
	//memcpy(data, data_buff, size);
	return true;
}
*/
	
/*
bool
AosSlabBlockFile::saveHeader(const AosRundataPtr &rdata)
{
	// Header:
	// 	eOffsetBlocksize(4bytes):		the blocksize
	// 	eOffsetBitmap:					the bitmap info
	
	// When call this func. must locked.
	
	aos_assert_r(mFile, false);
	AosBuffPtr buff = OmnNew AosBuff(eFileHeaderSize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eFileHeaderSize);
	buff->setU32(mBlockSize);
	//buff.setU32(eSanityFlags);
	buff->setU32(mCrtMaxBlockId);
	char * data = buff->data();
	memcpy(&data[eOffsetBitmap], mBitmap, eBitmapSize);
	setSign(buff, eFileHeaderSize);
	
	bool rslt = mFile->put(0, buff->data(), eFileHeaderSize, true, rdata);
	aos_assert_r(rslt, false);
	mSaved = true;
	return true;
}
*/

/*
bool
AosSlabBlockFile::readHeader(const AosRundataPtr &rdata)
{
	// Header:
	// 	eOffsetBlocksize(4bytes):		the blocksize
	// 	eOffsetCrtMaxBlockId(4bytes):	the crtMaxBlockId
	// 	eOffsetBitmap:					the bitmap info
	
	aos_assert_r(mFile, false);

	AosBuffPtr buff = OmnNew AosBuff(eFileHeaderSize, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eFileHeaderSize);

	mFile->lock(); 
	bool rslt = mFile->readToBuff(0, eFileHeaderSize, buff->data(), rdata);
	mFile->unlock();
	aos_assert_r(rslt, false);
	buff->setDataLen(eFileHeaderSize);

	// Ketty 2012/09/11
	bool data_good = checkSign(buff, eFileHeaderSize);
	if(!data_good)
	{
		rslt = mFile->recover(rdata);
		aos_assert_r(rslt, false);
		
		memset(buff->data(), 0, eFileHeaderSize);
		rslt = mFile->readToBuff(0, eFileHeaderSize, buff->data(), rdata);
		aos_assert_r(rslt, false);
		buff->setDataLen(eFileHeaderSize);
		
		data_good = checkSign(buff, eFileHeaderSize);
		if(!data_good)
		{   
			OmnAlarm << "The diskblock file is damaged." << mFile->getFileId() << enderr;
			return false;
		}
	}

	mBlockSize = buff->getU32(0);
	//aos_assert_r(mBlockSize && buff.getU32(0) == eSanityFlags, false);
	aos_assert_r(mBlockSize, false);
	mCrtMaxBlockId = buff->getU32(0);

	char * bitmap_data = buff->data() + eOffsetBitmap;
	memcpy(mBitmap, bitmap_data, eBitmapSize);
	return true;
}
*/


