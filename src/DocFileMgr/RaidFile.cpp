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
// 05/13/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/RaidFile.h"

#include "DocFileMgr/RaidReadReq.h"
#include "DocFileMgr/RaidWriteReq.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Util/File.h"


static bool 			sgSanityCheck = true;


AosRaidFile::AosRaidFile()
:
OmnLocalFile("AosRaidFile"),
mFile(0),
mDeviceNum(0),
mRwLock(OmnNew OmnRwLock()),
mSeekPos(0),
mIsGood(false)
{
}


AosRaidFile::AosRaidFile(
		const AosRundataPtr &rdata,
		const OmnFilePtr &file,
		const vector<AosStorageFileInfo> &fileinfos)
:
OmnLocalFile("AosRaidFile"),
mFile(file),
mRwLock(OmnNew OmnRwLock()),
mSeekPos(0),
mIsGood(false)
{	
	aos_assert(mFile && mFile->isGood());
	aos_assert(fileinfos.size() > 0);

	mFileInfos = fileinfos;
	mDeviceNum = mFileInfos.size();

	u64 offset = 0;
	mFile->setU64(offset, eCheckSum, false);
	offset += sizeof(u64);

	mFile->setU32(offset, mDeviceNum, false);
	offset += sizeof(u32);

	AosBuffPtr buff = OmnNew AosBuff(eEntrySize * mDeviceNum, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eEntrySize * mDeviceNum);

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);

	for (u32 i=0; i<mDeviceNum; i++)
	{
		aos_assert(mFileInfos[i].deviceId >= 0);
		aos_assert((u32)mFileInfos[i].deviceId == i);

		buff->setU64(mFileInfos[i].fileId);
		buff->setU32(mFileInfos[i].siteId);
		buff->setU32(mFileInfos[i].deviceId);
		buff->setU64(mFileInfos[i].requestSize);
		buff->setOmnStr(mFileInfos[i].fname);

		OmnFilePtr file = vfsMgr->openRawFile(
			mFileInfos[i].fileId, rdata.getPtr());
		aos_assert(file && file->isGood());
		mFiles.push_back(file);
	}

	int len = buff->dataLen();
	mFile->setInt(offset, len, false);
	offset += sizeof(int);

	bool rslt = mFile->put(offset, buff->data(), len, true);
	aos_assert(rslt);
	offset += len;

	mFile->setU64(offset, eCheckSum, false);
	offset += sizeof(u64);
	
	mIsGood = true;
}


AosRaidFile::AosRaidFile(
		const AosRundataPtr &rdata,
		const OmnFilePtr &file)
:
OmnLocalFile("AosRaidFile"),
mFile(file),
mRwLock(OmnNew OmnRwLock()),
mSeekPos(0),
mIsGood(false)
{
	aos_assert(mFile && mFile->isGood());

	u64 len = mFile->getLength();
	aos_assert(len > 0);

	u64 offset = 0;
	AosBuffPtr buff = OmnNew AosBuff(len + 20, 0 AosMemoryCheckerArgs);
	int64_t buffLen = mFile->readToBuff(offset, len, buff->data());
if ((int64_t)len != buffLen)
{
	OmnAlarm << enderr;
	buffLen = mFile->readToBuff(offset, len, buff->data());
}

	if ((int64_t)len != buffLen)
	{
		OmnAlarm << "Test" << enderr;
		buffLen = mFile->readToBuff(offset, len, buff->data());
	}
	aos_assert((int64_t)len == buffLen);


	buff->setDataLen(buffLen);
	buff->reset();

	u64 checkSum = buff->getU64(0);
	aos_assert(checkSum == eCheckSum);

	mDeviceNum = buff->getU32(0);
	aos_assert(mDeviceNum > 0);

	int ll = buff->getInt(0);
	aos_assert(ll > 0);
	aos_assert(buff->getCrtIdx() + ll + (int)sizeof(u64) == buff->dataLen());

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);

	for (u32 i=0; i<mDeviceNum; i++)
	{
		AosStorageFileInfo file_info;
		file_info.fileId = buff->getU64(0);
		file_info.siteId = buff->getU32(0);                  
		file_info.deviceId = buff->getU32(0);
		file_info.requestSize = buff->getU64(0);
		file_info.fname = buff->getOmnStr("");

		aos_assert(file_info.deviceId >= 0);
		aos_assert((u32)file_info.deviceId == i);
		aos_assert(file_info.fname != "");

		OmnFilePtr file = vfsMgr->openRawFile(
			file_info.fileId, rdata.getPtr());
		aos_assert(file && file->isGood());

		mFiles.push_back(file);
		mFileInfos.push_back(file_info);
	}

	aos_assert(mFileInfos.size() == mDeviceNum);

	checkSum = buff->getU64(0);
	aos_assert(checkSum == eCheckSum);

	mIsGood = true;
}


AosRaidFile::AosRaidFile(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff)
:
OmnLocalFile("AosRaidFile"),
mDeviceNum(0),
mRwLock(OmnNew OmnRwLock()),
mSeekPos(0),
mIsGood(false)
{
	aos_assert(buff);

	OmnString fname = buff->getOmnStr("");
	aos_assert(fname != "");

	mFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert(mFile->isGood());

	mDeviceNum = buff->getU32(0);
	aos_assert(mDeviceNum > 0);

	for (u32 i = 0; i < mDeviceNum; i++)
	{
		AosStorageFileInfo file_info;
		file_info.fileId = buff->getU64(0);
		file_info.siteId = buff->getU32(0);                  
		file_info.deviceId = buff->getU32(0);
		file_info.requestSize = buff->getU64(0);
		file_info.fname = buff->getOmnStr("");

		aos_assert(file_info.deviceId >= 0);
		aos_assert((u32)file_info.deviceId == i);
		aos_assert(file_info.fname != "");

		OmnFilePtr file = OmnNew OmnFile(file_info.fname,
			OmnFile::eReadWrite AosMemoryCheckerArgs);
		aos_assert(file && file->isGood());

		mFiles.push_back(file);
		mFileInfos.push_back(file_info);
	}

	aos_assert(mFileInfos.size() == mDeviceNum);
	
	mIsGood = true;
}


AosRaidFile::~AosRaidFile()
{
	mFileInfos.clear();
	mFiles.clear();
}


bool
AosRaidFile::closeFile()
{
	/*if (!mFile || !mFile->isGood()) return true;

	mFile->closeFile();

	for (u32 i = 0; i < mFiles.size(); i++)
	{
		if (mFiles[i] && mFile->isGood())
		{
			mFiles[i]->closeFile();
		}
	}*/
	OmnNotImplementedYet;
	return false;
}


OmnString
AosRaidFile::getFileName() const
{
	aos_assert_r(mFile, "");
	return mFile->getFileName();
}


bool
AosRaidFile::append(
		const char *buff,
		const i64 &len,
		const bool flush)
{
	OmnSemPtr sem = OmnNew OmnSem(0);
	AosRaidFileReqPtr request = OmnNew AosRaidWriteReq(buff, len, sem);
	bool rslt = addReq(request, mSeekPos, len);
	aos_assert_r(rslt, false);

	u64 total_req = request->getTotalReq();
	for (u64 i = 0; i < total_req; i++)
	{
		sem->wait();
	}
	mSeekPos += len;
	request->clear();
	return true;
}


bool
AosRaidFile::write(
		const i64 &start_pos,
		const char *buff,
		const i64 &len,
		const bool flush)
{
	OmnSemPtr sem = OmnNew OmnSem(0);
	AosRaidFileReqPtr request = OmnNew AosRaidWriteReq(buff, len, sem);
	bool rslt = addReq(request, start_pos, len);
	aos_assert_r(rslt, false);

	u64 total_req = request->getTotalReq();
	for (u64 i = 0; i < total_req; i++)
	{
		sem->wait();
	}
	request->clear();
	request = 0;
	return true;
}


i64
AosRaidFile::read(
		const i64 &start_pos,
		const char *buff,
		const i64 &size)

{
	OmnSemPtr sem = OmnNew OmnSem(0);
	i64 bytesread = 0; 
	AosRaidFileReqPtr request = OmnNew AosRaidReadReq((char*)buff, size, sem, bytesread);
	bool rslt = addReq(request, start_pos, size);
	aos_assert_r(rslt, -1);

	u64 total_req = request->getTotalReq();
	for (u64 i = 0; i < total_req; i++)
	{
		sem->wait();
	}

	if (request->getErrorCode()) 
	{
		request->clear();
		request = 0;
		return -1;
	}

	request->clear();
	request = 0;
	return bytesread;
}


bool
AosRaidFile::addReq(
		const AosRaidFileReqPtr &request,
		const i64 &start_pos,
		const i64 &size)
{
	i64 global_off = start_pos;
	u64 crt_block_id = global_off / eBlockSize;

	i64 reqId = 0;
	i64 seek_pos, bytes_to_read;
	while(reqId < size)
	{
//OmnScreen << "g_off: " << global_off << " , reqId: " << reqId  << " , crt_block_id : " << crt_block_id << endl;
		OmnFilePtr ff = getFile(crt_block_id);
		aos_assert_r(ff, false);

		calculateOffSize(crt_block_id, global_off, (size - reqId), seek_pos, bytes_to_read);
//OmnScreen << "seek_pos: " << seek_pos << " , bytes_to_read: " << bytes_to_read << endl;
		request->addRequest(reqId, ff, seek_pos, bytes_to_read);

		reqId += bytes_to_read;
		global_off += bytes_to_read;
		crt_block_id ++;
	}
	return request->sendRequest();
}


void
AosRaidFile::calculateOffSize(
		const u64 &crt_block_id, 
		const i64 &global_off, 
		const i64 &surplus_size,
		i64	&seek_pos,
		i64 &bytes_to_read)
{
	i64 block_off = global_off % eBlockSize;

	u64 file_block_num = crt_block_id / mDeviceNum;
	seek_pos = (file_block_num * eBlockSize) + block_off;

	bytes_to_read = eBlockSize - block_off;
	bytes_to_read = bytes_to_read <= surplus_size? bytes_to_read : surplus_size;

	sanityCheck(global_off, crt_block_id, seek_pos, bytes_to_read);
}


OmnFilePtr
AosRaidFile::getFile(
		const u64 &block_id)
{
	u64 device_id = block_id % mDeviceNum;
	AosStorageFileInfo file_info = mFileInfos[device_id];
	OmnFilePtr ff = mFiles[device_id]; 
	aos_assert_r(ff && ff->isGood(), 0);
	return ff;
}


void
AosRaidFile::deleteFile(AosRundata *rdata)
{
	aos_assert(mFile);
	for (u32 i = 0; i < mFileInfos.size(); i++)
	{
		bool rslt = AosVfsMgrObj::getVfsMgr()->removeFile(mFileInfos[i].fileId, rdata);
		aos_assert(rslt);
	}
}


bool
AosRaidFile::sanityCheck(
		const i64 &global_off,
		const u64 block_id, 
		const i64 &seekPos,
		const i64 &bytes_to_read)
{
	if (!sgSanityCheck) return true;

	u64 device_id = block_id % mDeviceNum;
	AosStorageFileInfo file_info = mFileInfos[device_id];
	aos_assert_r((u64)file_info.deviceId == device_id, false);
	//aos_assert_r(seekPos + bytes_to_read <= (i64)file_info.requestSize, false); 

	return true;
}


bool
AosRaidFile::isGood() const
{
	if (!mIsGood) return false;
	for (u32 i = 0; i < mFiles.size(); i++)
	{
		aos_assert_r(mFiles[i], false);
		if (!mFiles[i]->isGood()) return false;
	}	
	aos_assert_r(mFile, false);
	return mFile->isGood();
}


void
AosRaidFile::resetFile()
{
	aos_assert(mFiles.size() != 0);
	for (u32 i = 0; i < mFiles.size(); i++)
	{
		aos_assert(mFiles[i] && mFiles[i]->isGood());
		mFiles[i]->resetFile();
	}	
}

u64 
AosRaidFile::getLength()
{
	u64 length = 0;
	aos_assert_r(mFiles.size() != 0, 0);
	for (u32 i = 0; i < mFiles.size(); i++)
	{
		aos_assert_r(mFiles[i] && mFiles[i]->isGood(), 0);
		u64 len = mFiles[i]->getLength();
		aos_assert_r(len >= 0, 0);
		length += len;
	}	
	return length;
}


bool
AosRaidFile::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);

	buff->setOmnStr(mFile->getFileName());
	buff->setU32(mDeviceNum);

	for (u32 i = 0; i < mDeviceNum; i++)
	{
		buff->setU64(mFileInfos[i].fileId);
		buff->setU32(mFileInfos[i].siteId);
		buff->setU32(mFileInfos[i].deviceId);
		buff->setU64(mFileInfos[i].requestSize);
		buff->setOmnStr(mFileInfos[i].getFullFname());
	}
	return true;
}

