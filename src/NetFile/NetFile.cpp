////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2015/07/07 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "NetFile/NetFile.h"


AosNetFile::AosNetFile()
:
mServerId(-1),
mStartOffset(0),
mFileLength(-1),
mCrtOffset(0),
mBuff(0),
mCaller(0)
{
}


AosNetFile::AosNetFile(const int server_id)
:
mServerId(server_id),
mStartOffset(0),
mFileLength(-1),
mCrtOffset(0),
mBuff(0),
mCaller(0)
{
}

AosNetFile::~AosNetFile()
{
}


int 
AosNetFile::getServerId() 
{ 
	return mServerId; 
}


AosBuffPtr 
AosNetFile::getIoBuff(
		const u64 &reqId, 
		const int64_t &size AosMemoryCheckDecl)
{
	AosBuffPtr buff  = OmnNew AosBuff(size AosMemoryCheckerFileLine);	
	return buff;
}


void 
AosNetFile::dataRead(const Parameter &parm, const AosBuffPtr &buff) 
{
	mBuff = buff;
	aos_assert(mCaller);
	AosDiskStat disk_stat;
/*
OmnScreen << "====================NetFile dataRead reqId : " << parm.reqId 
		  << " expected_size : " << parm.expected_size 
		  << " finished : " << parm.finished << endl;
OmnScreen << "======================================================" << endl;
*/
    aos_assert(parm.offset != -1);
	int64_t offset = parm.offset+parm.expected_size;

	aos_assert(offset <= mStartOffset + mFileLength);
	bool finished = parm.finished;
	if (offset == mStartOffset + mFileLength)
	{
/*
OmnScreen << "====================NetFile dataRead reqId : " << parm.reqId << endl;
OmnScreen << " mCrtOffset : " << offset << endl;
OmnScreen << " mStartOffset : " << mStartOffset << endl;
OmnScreen << " mFileLength : " << mFileLength << endl;
OmnScreen << "======================================================" << endl;
*/
		finished = true;	
	}
		
	mCaller->fileReadCallBack(parm.reqId, parm.expected_size, finished, disk_stat);
	//	if (parm.finished) mCaller = 0;
}


void 
AosNetFile::readError(u64 &reqid, OmnString &errmsg)
{
	AosDiskStat disk_stat;
	disk_stat.setError(AosDiskStat::eReadErr);
	mCaller->fileReadCallBack(reqid, 0, true, disk_stat);
}


void 
AosNetFile::dataWrite(const u64 &reqid)
{
}


void 
AosNetFile::writeError(u64 &reqid, OmnString &errmsg)
{
}


void 
AosNetFile::setCaller(const AosFileReadListenerPtr &caller)
{
	mCaller = caller;
}

	
AosFileReadListenerPtr 
AosNetFile::getCaller()
{
	return mCaller;
}


void 
AosNetFile::seek(const int64_t start_pos)
{
	mCrtOffset = start_pos;		
}

void 
AosNetFile::moveToFront(const int64_t pos)
{
	if(pos>mCrtOffset) mCrtOffset = 0;
	mCrtOffset -= pos;		
}

AosBuffPtr 
AosNetFile::getBuff()
{
	AosBuffPtr buff = mBuff;
	mBuff = 0;
	return buff;
}


	
AosIdNetFile::AosIdNetFile(
			const int server_id,
			const u64 &fileid)
:
AosNetFile(server_id),
mFileId(fileid)
{
}
	

AosIdNetFile::AosIdNetFile(const AosFileInfo &fileinfo)
:
AosNetFile(fileinfo.mPhysicalId),
mFileId(fileinfo.mFileId)
{
	mStartOffset = fileinfo.mStartOffset;
	mFileLength = fileinfo.mFileLen;
	mCrtOffset = fileinfo.mStartOffset;
}
	
AosIdNetFile::~AosIdNetFile()
{
}
	
u64 
AosIdNetFile::getFileId() 
{ 
	return mFileId; 
}

int64_t 
AosIdNetFile::length(AosRundata *rdata)
{
	if (mFileLength > 0) return mFileLength;
	AosDiskStat disk_stat;
	bool rslt = AosNetFileClt::getSelf()->getFileLength(
		mFileId, mServerId, mFileLength, disk_stat, rdata);
	//for bug trace
	if (!rslt)
	{
		OmnAlarm << "get id file length error, fileid: " << mFileId <<  " ,serverid: " << mServerId << enderr;
		return -1;
	}
	aos_assert_r(rslt, -1);
/*
OmnScreen << "===============================  IdNetFile length " << endl;
OmnScreen << " mStartOffset : " << mStartOffset << endl;
OmnScreen << " mFileLength : " << mFileLength << endl;
OmnScreen << "======================================================" << endl;
*/
	return mFileLength;
}


bool 
AosIdNetFile::readData(
			const u64 &reqId, 
			const int64_t &blocksize, 
			AosRundata *rdata)
{
	if(mFileLength < 0) length(rdata);

	int64_t offset = mCrtOffset;
	int64_t bsize = blocksize;

	if (offset + bsize > mStartOffset + mFileLength) 
	{
		bsize = mStartOffset + mFileLength - offset;
	}
	aos_assert_r(bsize >= 0, false);
	mCrtOffset += bsize;
/*
OmnScreen << "=============================== IdNetFile readdata " 
			<< " reqId : " << reqId 
			<< " mFileId : " << mFileId
			<< " mCrtOffset : " << offset 
			<< " mStartOffset : " << mStartOffset 
			<< " bsize : " << bsize 
			<< " blocksize : " << blocksize
			<< " mServerId : " << mServerId 
			<< "========================================================" 
			<< endl;
*/
	AosAioCallerPtr thisptr(this, false);
	return AosNetFileClt::getSelf()->asyncReadFile(
		mFileId, mServerId, offset, bsize, reqId, thisptr, rdata); 
}


	
AosNameNetFile::AosNameNetFile(
			const int server_id,
			const OmnString &filename)
:
AosNetFile(server_id),
mFileName(filename)
{
}

AosNameNetFile::AosNameNetFile(const AosFileInfo &fileinfo)
:
AosNetFile(fileinfo.mPhysicalId),
mFileName(fileinfo.mFileName)
{
	mStartOffset = fileinfo.mStartOffset;
	mFileLength = fileinfo.mFileLen;
	mCrtOffset = fileinfo.mStartOffset;
}


AosNameNetFile::~AosNameNetFile()
{
}


int64_t 
AosNameNetFile::length(AosRundata *rdata)
{
	if (mFileLength > 0) return mFileLength;
	AosDiskStat disk_stat;
	bool rslt = AosNetFileClt::getSelf()->getFileLength(
		mFileName, mServerId, mFileLength, disk_stat, rdata);
	aos_assert_r(rslt, -1);
	return mFileLength;
}


bool 
AosNameNetFile::readData(
			const u64 &reqId,
			const int64_t &blocksize,
			AosRundata *rdata)
{
	if(mFileLength < 0) length(rdata);

	int64_t offset = mCrtOffset;
	int64_t bsize = blocksize;

	if (offset + bsize > mStartOffset + mFileLength) 
	{
		bsize = mStartOffset + mFileLength - offset;
	}
	aos_assert_r(bsize >= 0, false);
	mCrtOffset += bsize;
/*
OmnScreen << "=============================== NameNetFile readdata " 
			<< " reqId : " << reqId 
			<< " mFileName : " << mFileName
			<< " mCrtOffset : " << offset 
			<< " mStartOffset : " << mStartOffset 
			<< " bsize : " << bsize 
			<< " blocksize : " << blocksize
			<< " mServerId : " << mServerId 
			<< "========================================================" 
			<< endl;
*/
	AosAioCallerPtr thisptr(this, false);
	return AosNetFileClt::getSelf()->asyncReadFile(
		mFileName, mServerId, offset, bsize, reqId, thisptr, rdata); 
}


AosIdNetFileVar::AosIdNetFileVar(
			const int server_id,
			const u64 &fileid)
:
AosNetFile(server_id),
mFileId(fileid),
mUsingArray(true),
mIsReading(false)
{
}


AosIdNetFileVar::AosIdNetFileVar(
			const int server_id,
			const u64 &fileid,
			const AosCompareFunPtr &comp)
:
AosNetFile(server_id),
mFileId(fileid),
mUsingArray(true),
mIsReading(false),
mComp(comp)
{
}
	

AosIdNetFileVar::AosIdNetFileVar(const AosFileInfo &fileinfo)
:
AosNetFile(fileinfo.mPhysicalId),
mFileId(fileinfo.mFileId),
mUsingArray(true),
mIsReading(false)
{
	mStartOffset = fileinfo.mStartOffset;
	mFileLength = fileinfo.mFileLen;
	mCrtOffset = fileinfo.mStartOffset;
}
	
AosIdNetFileVar::~AosIdNetFileVar()
{
}
	

int64_t 
AosIdNetFileVar::length(AosRundata *rdata)
{
	if (mFileLength > 0) return mFileLength;

	AosDiskStat disk_stat;
	bool rslt = AosNetFileClt::getSelf()->getFileLength(
		mFileId, mServerId, mFileLength, disk_stat, rdata);
	aos_assert_r(rslt, -1);

	return mFileLength;
}


bool 
AosIdNetFileVar::readData(
			const u64 &reqId, 
			const int64_t &blocksize, 
			AosRundata *rdata)
{
	aos_assert_r(!mIsReading, false); 

	if (mFileLength < 0) length(rdata);

	int64_t offset = mCrtOffset;
	int64_t bsize = blocksize;

	if (offset + bsize > mStartOffset + mFileLength) 
	{
		bsize = mStartOffset + mFileLength - offset;
	}
	aos_assert_r(bsize >= 0, false);

	mCrtOffset += bsize;
	mIsReading = true;

	AosAioCallerPtr thisptr(this, false);
	return AosNetFileClt::getSelf()->asyncReadFile(
		mFileId, mServerId, offset, bsize, reqId, thisptr, rdata); 
}


void 
AosIdNetFileVar::dataRead(const Parameter &parm, const AosBuffPtr &buff) 
{
	aos_assert(mCaller);
    aos_assert(parm.offset != -1);

	i64 offset = parm.offset + parm.expected_size;
	aos_assert(offset <= mStartOffset + mFileLength);
	aos_assert(offset == mCrtOffset);

	buff->reset();
	int reamin_size = 0;
	if (mUsingArray)
	{
		AosBuffArrayVarPtr array = OmnNew AosBuffArrayVar(true);	
		//Jozhi set compare func
		array->setCompareFunc(mComp);
		bool rslt = array->setBodyBuff(buff, reamin_size);
		aos_assert(rslt);

		mBuff = array->getHeadBuff();
		rslt = sanitycheck(mBuff->data(), mBuff->dataLen());
		aos_assert(rslt);

		mArray.push_back(array);
	}
	else
	{
		bool rslt = AosBuffArrayVar::remainingBodyBuff(buff, reamin_size);
		aos_assert(rslt);
		mBuff = buff;
		rslt = sanitycheck(mBuff->data(), mBuff->dataLen());
		aos_assert(rslt);
	}

	mCrtOffset -= reamin_size;
	mIsReading = false;

	bool finished = parm.finished;
	if (mCrtOffset == mStartOffset + mFileLength)
	{
		finished = true;	
	}
	
	AosDiskStat disk_stat;
	mCaller->fileReadCallBack(parm.reqId, parm.expected_size, finished, disk_stat);
}


bool 
AosIdNetFileVar::sanitycheck(char *begin, int length)
{
//return true;
    aos_assert_r(length%mComp->size == 0, false); 
    int size = length/mComp->size;
    for(int i=0; i<size; i++)
    {
        if (i>0)
        {
            aos_assert_r(mComp->cmp(begin, begin-mComp->size) >= 0, false); 
        } 
        begin += mComp->size;
    }
    return true;                                                                         
}                                                                                        


bool 
AosIdNetFileVar::checkBodyAddrs(set<i64> &bodyAddrSet)
{
	deque<AosBuffArrayVarPtr>::iterator itr = mArray.begin();
	i64 bodyAddr;
	int idx = 0;
	int delete_num = 0;
	while (itr != mArray.end())
	{
		bodyAddr = (*itr)->getBodyAddr();
		if (bodyAddrSet.count(bodyAddr) > 0)
		{
			delete_num = idx;
		}
		itr++;
		idx++;
	}

	while (delete_num > 0)
	{
		mArray.pop_front();
		delete_num--;
	}

	return true;
}

	
AosCompNetFile::AosCompNetFile(const AosFileInfo &fileinfo)
:
AosNetFile(),
mIdx(0),
mFileInfos(fileinfo.mFileBlocks)
{
	mCrtOffset = mFileInfos[mIdx].mStartOffset;
}

AosCompNetFile::~AosCompNetFile()
{
}
	

int64_t 
AosCompNetFile::length(AosRundata *rdata)
{
	if (mFileLength > 0) return mFileLength;
	for (u32 i=0; i<mFileInfos.size(); i++)
	{
		mFileLength += mFileInfos[i].mFileLen;
	}
	return mFileLength;
}


bool 
AosCompNetFile::readData(
			const u64 &reqId, 
			const int64_t &blocksize, 
			AosRundata *rdata)
{
	if(mFileLength < 0) length(rdata);
	int64_t offset = mCrtOffset;
	int64_t bsize = blocksize;

	if (offset + bsize > mFileInfos[mIdx].mStartOffset + mFileInfos[mIdx].mFileLen) 
	{
		bsize = mFileInfos[mIdx].mStartOffset + mFileInfos[mIdx].mFileLen - offset;
	}
	aos_assert_r(bsize >= 0, false);
	mCrtOffset += bsize;
	AosAioCallerPtr thisptr(this, false);
	return AosNetFileClt::getSelf()->asyncReadFile(
		mFileInfos[mIdx].mFileId, mFileInfos[mIdx].mPhysicalId, offset, bsize, reqId, thisptr, rdata); 
}


void 
AosCompNetFile::dataRead(const Parameter &parm, const AosBuffPtr &buff) 
{
	mBuff = buff;
	aos_assert(mCaller);
	AosDiskStat disk_stat;

    aos_assert(parm.offset != -1);
	int64_t offset = parm.offset+parm.expected_size;

	aos_assert(offset <= mFileInfos[mIdx].mStartOffset + mFileInfos[mIdx].mFileLen);
	bool finished = parm.finished;
	if (offset == mFileInfos[mIdx].mStartOffset + mFileInfos[mIdx].mFileLen)
	{
		if (mIdx == mFileInfos.size() - 1)
		{
			finished = true;
		}
		else
		{
			mIdx++;
			mCrtOffset = mFileInfos[mIdx].mStartOffset;
		}
	}
	
	mCaller->fileReadCallBack(parm.reqId, parm.expected_size, finished, disk_stat);
}
