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
//
// Modification History:
// 01/12/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "StorageEngine/SengineFixedCacheFile.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"

static bool                     sgSanityCheck = true;

AosSengineFixedCacheFile::AosSengineFixedCacheFile(
			const u64 &id,
			const u64 &job_id,
			const u32 &logic_taskid,
			const u32 &sizeid, 
			const int &record_size,
			const bool remove_file)
:
mId(id),
mWriteFile(0),
mSizeid(sizeid),
mRecordSize(record_size),
mLock(OmnNew OmnMutex()),
mJobId(job_id),
mLogicTaskid(logic_taskid),
mIsRemoveFile(remove_file)
{
}


AosSengineFixedCacheFile::~AosSengineFixedCacheFile()
{
}


bool
AosSengineFixedCacheFile::saveToFile(
		const u64 &num_docs,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &buff,
		bool &full_file,
		const AosRundataPtr &rdata)
{
	// u32 		rcd_total
	// u64 		num_docs
	// int		docidsize
	// char * 	docids
	// char 	checkFlag
	// char 	checkFlag
	// char 	checkFlag
	// char 	checkFlag
	// int 		buffsize
	// char * 	buff
	u32 rcd_total = sizeof(u32) + sizeof(u64) + sizeof(int) 
		+ docid_buff->dataLen()+ 4 * sizeof(char) 
		+ sizeof(int) + buff->dataLen();

	mLock->lock();
	AosFixedFileInfoPtr fileinfo = getWriteFileLocked(rcd_total, full_file, rdata);
	aos_assert_rl(fileinfo, mLock, false);
	aos_assert_rl(!fileinfo->getFull(), mLock, false);

	OmnFilePtr file = fileinfo->getFile();
	u32 offset = fileinfo->getLastOffset();

	u32 first_offset = offset;
	file->lock();
	// set rcd_total
	file->setU32(offset, rcd_total, false); 

	// set num_docs
	offset = offset +  sizeof(u32);
	file->setU64(offset, num_docs, false);
	
	// set docid size
	offset = offset + sizeof(u64);
	file->setInt(offset, docid_buff->dataLen(), false);

	// set docids
	offset = offset + sizeof(int);
	file->put(offset, docid_buff->data(), docid_buff->dataLen(), false);

	// set check flag
	offset = offset + docid_buff->dataLen();
	file->setChar(offset, eFlagByte0, false);
	file->setChar(offset + 1, eFlagByte1, false);
	file->setChar(offset + 2, eFlagByte2, false);
	file->setChar(offset + 3, eFlagByte3, false);
	sanityCheck(file, offset);
	// set buffsize
	offset = offset + 4 * sizeof(char);
	file->setInt(offset, buff->dataLen(), false);

	// set buff
	offset = offset + sizeof(int);
	file->put(offset, buff->data(), buff->dataLen(), true);
	file->unlock();

	u32 last_offset = offset +  buff->dataLen();
	aos_assert_rl(last_offset - first_offset == rcd_total, mLock, false);
	
	u64 *docids = (u64*)docid_buff->data();
	fileinfo->setLastOffset(last_offset);
	fileinfo->setSortTransOffset(docids[0], first_offset);
	fileinfo->numReqPlus();
	mLock->unlock();
	return true;
}


AosFixedFileInfoPtr
AosSengineFixedCacheFile::getWriteFileLocked(
		const u32 &requested_space,
		bool &full_file,
		const AosRundataPtr &rdata)
{
	if (mWriteFile)
	{
		u32 last_offset = mWriteFile->getLastOffset();
		if (last_offset + requested_space <= eMaxFileSize)
		{
			return mWriteFile;	
		}
		mWriteFile->setFull(true);
		mReadQueue.push(mWriteFile);
		full_file = true;
		mWriteFile = 0;
	}

	OmnString fname_prefix = "cacher_";
	fname_prefix << mJobId << "_" << mLogicTaskid << "_" << mSizeid;
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	//vector<u32> total_vids;           
	//AosGetLocalVirtualIds(total_vids);
	//aos_assert_r(total_vids.size()>0 && mCrtVidIdx < (int)total_vids.size(), 0);
	//u32 virtual_id = total_vids[mCrtVidIdx];
	//mCrtVidIdx++;
	//if(mCrtVidIdx == (int)total_vids.size())	mCrtVidIdx = 0;

	u64 file_id;
	OmnFilePtr file = vfsMgr->createNewTempFile(rdata, file_id, fname_prefix, eMaxFileSize, false);
	aos_assert_r(file && file->isGood(), 0);

	mWriteFile = OmnNew AosFixedFileInfo(file_id, file);
	aos_assert_r(mWriteFile, 0);
	return mWriteFile;
}


bool
AosSengineFixedCacheFile::deleteFileinfoLock(
		const AosFixedFileInfoPtr &fileinfo,
		const AosRundataPtr &rdata)
{
	u32 nn = fileinfo->getNumReq();
	aos_assert_r(nn == 0, false);

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	
	mLock->lock();
	mReadQueue.pop();
	mLock->unlock();
	if (mIsRemoveFile)
	{
		u64 fileid = fileinfo->getFileId();	
		vfsMgr->removeFile(fileid, rdata);
	}
	return true;
}


AosFixedFileInfoPtr
AosSengineFixedCacheFile::getReadFile()
{
	mLock->lock();
	if (!mReadQueue.empty())
	{
		AosFixedFileInfoPtr fileinfo =  mReadQueue.front();
		mLock->unlock();
		return fileinfo;
	}
	mLock->unlock();
	return 0;
}


bool
AosSengineFixedCacheFile::readFromFile(
		u64 &num_docs,
		AosBuffPtr &docidbuff,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	// u32 		rcd_total
	// u64 		num_docs
	// int		docidsize
	// char * 	docids
	// char 	checkFlag
	// char 	checkFlag
	// char 	checkFlag
	// char 	checkFlag
	// int 		buffsize
	// char * 	buff
	AosFixedFileInfoPtr fileinfo = getReadFile();
	aos_assert_r(fileinfo, false);

	OmnFilePtr file = fileinfo->getFile();
	u32 offset = fileinfo->getReadOffset();
	if (offset == (u32) -1)
	{
		// remove file
		aos_assert_r(fileinfo->isReadFinished(), false);
		bool rslt = deleteFileinfoLock(fileinfo, rdata);
		aos_assert_r(rslt, false); 
		return false;
	}
	aos_assert_r(!fileinfo->isReadFinished(), false);

	u32 first_offset = offset;
	file->lock();
	// get rcd_total
	u32 rcd_total = file->readBinaryU32(offset, 0);	
	if (rcd_total <= 0)
	{
		file->unlock();
		OmnAlarm << "rcd_total" << enderr;
		return false;
	}

	// get num_docs
	offset = offset + sizeof(u32);
	num_docs = file->readBinaryU64(offset, 0);
	if (num_docs <= 0)
	{
		file->unlock();
		OmnAlarm << "num_docs " << enderr;
		return false;
	}

	// get data_len
	offset = offset + sizeof(u64);
	int data_len = file->readBinaryInt(offset, -1);
	if (data_len <= 0)
	{
		file->unlock();
		OmnAlarm << "data_len" << enderr;
		return false;
	}

	// get docids
	docidbuff = OmnNew AosBuff(data_len AosMemoryCheckerArgs);
	offset = offset + sizeof(int);
	int len = file->readToBuff(offset, data_len, docidbuff->data());
	docidbuff->setDataLen(data_len);
	if (len != docidbuff->dataLen())	
	{
		file->unlock();
		OmnAlarm << " len != datalen" << enderr;
		return false;
	}

	// get checkFlag and check 
	offset = offset + data_len;
	char f0 = file->readChar(offset, '0');

	char f1 = file->readChar(offset + 1, '0');

	char f2 = file->readChar(offset + 2, '0');

	char f3 = file->readChar(offset + 3, '0');

	if (f0 != eFlagByte0 || f1 != eFlagByte1 || f2 != eFlagByte2 || f3 != eFlagByte3)
	{
		file->unlock();
		OmnAlarm << " flag " << enderr;
		return false;
	}

	// get buffsize
	offset = offset + 4 * sizeof(char);
	int buffsize = file->readBinaryInt(offset, -1); 
	if (buffsize <= 0)
	{
		file->unlock();
		OmnAlarm << " buffsize " << enderr;
		return false;
	}

	buff = OmnNew AosBuff(buffsize AosMemoryCheckerArgs);
	offset = offset + sizeof(int);
	len = file->readToBuff(offset, buffsize, buff->data());
	buff->setDataLen(buffsize);
	if (len != buffsize)
	{
		file->unlock();
		OmnAlarm << " len != buffsize " << enderr;
		return false;
	}
	file->unlock();

	u32 last_offset = offset + buffsize; 
	aos_assert_r(last_offset - first_offset == rcd_total, false);

	fileinfo->numReqMinus();		
	return true;
}


bool
AosSengineFixedCacheFile::cleanData(const AosRundataPtr &rdata)
{
	//aos_assert_r(mWriteFile, false);
	mLock->lock();
	if (!mWriteFile) 
	{
		mLock->unlock();
		return false;
	}
	mReadQueue.push(mWriteFile);
	mWriteFile = 0;
	mLock->unlock();
	return true;
}


bool
AosSengineFixedCacheFile::sanityCheck(const OmnFilePtr &file, const u32 &offset)
{
	if (!sgSanityCheck) return true;
	char f0 = file->readChar(offset, '0');
	char f1 = file->readChar(offset + 1, '0');
	char f2 = file->readChar(offset + 2, '0');
	char f3 = file->readChar(offset + 3, '0');

	if (f0 != eFlagByte0 || f1 != eFlagByte1 || f2 != eFlagByte2 || f3 != eFlagByte3)
	{
		OmnAlarm << " flag " << enderr;
		return false;
	}
	return true;
}

bool
AosSengineFixedCacheFile::readFromFileStatic(
		const OmnFilePtr &file,
		const u32 &first_offset,
		const u64 &file_size,
		u64 &num_docs,
		AosBuffPtr &docidbuff,
		AosBuffPtr &buff,
		u32 &last_offset,
		const AosRundataPtr &rdata)
{
	// u32 		rcd_total
	// u64 		num_docs
	// int		docidsize
	// char * 	docids
	// char 	checkFlag
	// char 	checkFlag
	// char 	checkFlag
	// char 	checkFlag
	// int 		buffsize
	// char * 	buff
	u32 offset = first_offset;
	if (offset == (u32) -1)
	{
		return false;
	}
	file->lock();
	// get rcd_total
	u32 rcd_total = file->readBinaryU32(offset, 0);	
	if (rcd_total <= 0)
	{
		file->unlock();
		OmnAlarm << "rcd_total" << enderr;
		return false;
	}

	// get num_docs
	offset = offset + sizeof(u32);
	num_docs = file->readBinaryU64(offset, -1);
	if (num_docs <= 0)
	{
		file->unlock();
		OmnAlarm << "num_docs " << enderr;
		return false;
	}

	// get data_len
	offset = offset + sizeof(u64);
	int data_len = file->readBinaryInt(offset, -1);
	if (data_len <= 0)
	{
		file->unlock();
		OmnAlarm << "data_len" << enderr;
		return false;
	}

	// get docids
	docidbuff = OmnNew AosBuff(data_len AosMemoryCheckerArgs);
	offset = offset + sizeof(int);
	int len = file->readToBuff(offset, data_len, docidbuff->data());
	docidbuff->setDataLen(data_len);
	if (len != docidbuff->dataLen())	
	{
		file->unlock();
		OmnAlarm << " len != datalen" << enderr;
		return false;
	}

	// get checkFlag and check 
	offset = offset + data_len;
	char f0 = file->readChar(offset, '0');

	char f1 = file->readChar(offset + 1, '0');

	char f2 = file->readChar(offset + 2, '0');

	char f3 = file->readChar(offset + 3, '0');

	if (f0 != eFlagByte0 || f1 != eFlagByte1 || f2 != eFlagByte2 || f3 != eFlagByte3)
	{
		file->unlock();
		OmnAlarm << " flag " << enderr;
		return false;
	}

	// get buffsize
	offset = offset + 4 * sizeof(char);
	int buffsize = file->readBinaryInt(offset, -1); 
	if (buffsize <= 0)
	{
		file->unlock();
		OmnAlarm << " buffsize " << enderr;
		return false;
	}

	buff = OmnNew AosBuff(buffsize AosMemoryCheckerArgs);
	offset = offset + sizeof(int);
	len = file->readToBuff(offset, buffsize, buff->data());
	buff->setDataLen(buffsize);
	if (len != buffsize)
	{
		OmnScreen << "len(" << len <<")!= buffsize(" << buffsize<<") offset:" << offset <<", " <<file_size << ":" << offset+len << endl;
		if (offset+len < file_size)
		{
			file->unlock();
			OmnAlarm << " len != buffsize " << enderr;
			return false;
		}
	}
	file->unlock();

	last_offset = offset + buffsize; 
	aos_assert_r(last_offset - first_offset == rcd_total, false);
	return true;
}

#endif
