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
//	This class is used to manage raw files whose content are not the concern
//
// Modification History:
// 2014-11-21 Created by White
////////////////////////////////////////////////////////////////////////////
#include "RawSE/RawFile.h"

#include "Debug/Except.h"
#include "Util/File.h"
#include "RawSE/PartitionIDMgr.h"
#include "RawSE/Ptrs.h"
#include <boost/filesystem.hpp>

AosRawFile::AosRawFile(
		AosRundata				*rdata,
		AosPartitionIDMgr		*partition_id_mgr,
		const u32				site_id,
		const u32				cube_id,
		const u64				ase_id,
		const u64				raw_file_id,
		const u16				level_1_size,
		const u16				level_2_size,
		const u16				level_3_size,
		const Type				eType)
:
mPrev(NULL),
mNext(NULL),
mRawFileID(raw_file_id),
mSiteID(site_id),
mCubeID(cube_id),
mAseID(ase_id),
mGuard(123456789),
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw),
mFile(OmnNew OmnFile(AosMemoryCheckerArgsBegin)),
mPartitionIDMgr(partition_id_mgr),
meType(eType)
{
	mLocalID = raw_file_id & 0xFFFFFFFFFFFFFF;	//get the lower 56 bits
	int iRet = 0;
	if(0 != (iRet = composeFileNameWithFullPath(rdata, level_1_size, level_2_size, level_3_size, eType)))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosRawFile::~AosRawFile()
{
	OmnScreen << "~AosRawFile() this:@"  << this << " siteid:" << mSiteID << " cubeid:" << mCubeID
			<< " aseid:" << mAseID << " rawfid:" << mRawFileID << " mFile:'" << mFile->getFileName() << "'"<< endl;
	close();
	OmnDelete mFile;
	mFile = 0;
	mGuard = 0;
}


int
AosRawFile::read(
		AosRundata	*rdata,
		const u64	offset,
		const u64	length,
		AosBuffPtr	&buff,
		const bool	bShowAlarm)
{
	//this function tries to read the file's content and return it by buff
	if (0 == length)
	{
		OmnAlarm << "invalid read length:" << length << " rawfid:" << mRawFileID << enderr;
		return -1;
	}
	if (buff.isNull())
	{
		OmnAlarm << "buff.isNull()" << enderr;
		return -2;
	}
	if (buff->buffLen() < (i64)length)
	{
		bool rslt = buff->expandMemory1(length);
		if (!rslt)
		{
			OmnAlarm << "buff->expandMemory1 failed, len:" << length << enderr;
			return -3;
		}
	}
	if(!isGood())
	{
		int iRet = 0;
		if (0 != (iRet = open(rdata, (OmnFile::Mode)meType, bShowAlarm)))
		{
			if (bShowAlarm)
			{
				OmnAlarm << "can not open file '" << mFileNameWithFullPath <<"' with rawfid '"
						<< mRawFileID << "' for reading iRet=" << iRet << enderr;
			}
			else
			{
				OmnScreen << "can not open file '" << mFileNameWithFullPath <<"' with rawfid '"
						<< mRawFileID << "' for reading iRet=" << iRet << endl;
			}
			return -4;
		}
	}
	if (!mFile)
	{
		OmnAlarm << "mFile is NULL for file '" << mFileNameWithFullPath <<"' with rawfid '" << mRawFileID << "'" << enderr;
		return -5;
	}
	int64_t illReadLen = 0;
	if (mFile->getFileCrtSize() < offset)
	{
		return -6;
	}
	if(0 > ( illReadLen = mFile->readToBuff(offset, length, buff->data())))
	{
		OmnAlarm << "can not read to buff for file '" << mFileNameWithFullPath <<"' with rawfid '" << mRawFileID
				<< "' return code is '" << illReadLen << "'" << enderr;
		return -7;
	}
	buff->setDataLen(illReadLen);
	return 0;
}


int
AosRawFile::create(AosRundata *rdata)
{
	//this function creates a raw file
	aos_assert_rr(mFile, rdata, -1);
	if (OmnFile::fileExist(mFileNameWithFullPath))		//meaning the file being created already exists
	{
		OmnAlarm << "the file being created already exists." << enderr;
		return -2;
	}
	int iRet = 0;
	if (0 != (iRet = open(rdata, OmnFile::eCreate)))
	{
		OmnAlarm << "can not create file with rawfid:'" << mRawFileID << "' iRet=" << iRet << enderr;
		return -3;
	}
	if (0 != (iRet = close()))
	{
		OmnAlarm << "can not close file with rawfid:'" << mRawFileID << "' iRet=" << iRet << enderr;
		return -4;
	}
	if (0 != (iRet = open(rdata, (OmnFile::Mode)meType)))
	{
		OmnAlarm << "can not open file with rawfid:'" << mRawFileID << "' iRet=" << iRet << enderr;
		return -3;
	}
	return 0;
}


int
AosRawFile::append(
		AosRundata 			*rdata,
		const char* const	data,
		const u64 			length)
{
	//this function tries to append some data to the file
	if (!isGood())
	{
		int iRet = 0;
		if (0 != (iRet = open(rdata, (OmnFile::Mode)meType)))
		{
			OmnAlarm << "can not open file '" << mFileNameWithFullPath <<"' with rawfid '"
					<< mRawFileID << "' for appending iRet=" << iRet << enderr;
			return -1;
		}
	}
	if (!mFile->seek(mFile->getLength()))
	{
		OmnAlarm << "can not seek file '" << mFileNameWithFullPath <<"' with rawfid '"
				<< mRawFileID << "' for appending." << enderr;
		return -2;
	}
	if (0 == length)
	{
		OmnAlarm << "0 == length" << enderr;
		return -3;
	}
	try
	{
		if (!mFile->append(data, length, true))
		{
			OmnAlarm << "can not append to file '" << mFileNameWithFullPath <<"' with rawfid '"
					<< mRawFileID << "' length:" << length << enderr;
			return -4;
		}
	}
	catch(const OmnExcept &e)
	{
		OmnAlarm << "Failed to append: " << e.getErrmsg() << enderr;
		return -5;
	}
	return 0;
}


int
AosRawFile::close()
{
	//this function closes the file
	if (!isGood())
	{
		return 0;
	}
	if (mFile)
	{
		bool bSuccess = mFile->closeFile();
		if (bSuccess)
		{
			return 0;
		}
		else
		{
			OmnAlarm << "failed to close file '" << mFileNameWithFullPath <<"' with rawfid '" << mRawFileID << "'" << enderr;
			return -1;
		}
	}
	else
	{
		OmnAlarm << "mFile is NULL for file '" << mFileNameWithFullPath <<"' with rawfid '" << mRawFileID << "'" << enderr;
		return -2;
	}
}


int
AosRawFile::del(AosRundata *rdata)
{
	//this function deletes the file on the media according to the media type
	if (!isGood())
	{
		int iRet = 0;
		if (0 != (iRet = open(rdata, (OmnFile::Mode)meType)))
		{
			OmnAlarm << "can not open file '" << mFileNameWithFullPath <<"' with rawfid '"
					<< mRawFileID << "' for deleting iRet=" << iRet << enderr;
			return -1;
		}
	}

	if (!mFile)
	{
		OmnAlarm << "mFile is NULL for file '" << mFileNameWithFullPath <<"' with rawfid '" << mRawFileID << "'" << enderr;
		return -2;
	}

	mFile->deleteFile();
	return 0;
}


int
AosRawFile::overwrite(
		AosRundata 			*rdata,
		const char* const	data,
		const u64 			length)
{
	//this function overwrites the raw file
	if (!isGood())
	{
		int iRet = 0;
		if (0 != (iRet = open(rdata, OmnFile::eWriteCreate)))
		{
			OmnAlarm << "can not open file '" << mFileNameWithFullPath <<"' with rawfid '"
					<< mRawFileID << "' for overwriting, iRet=" << iRet << enderr;
			return -1;
		}
	}
	bool rslt = false;
	if (!mFile)
	{
		OmnAlarm << "mFile is NULL for file '" << mFileNameWithFullPath <<"' with rawfid '" << mRawFileID << "'" << enderr;
		return -2;
	}
	try
	{
		rslt = mFile->write(0, data, length, true);
		if (!rslt)
		{
			OmnAlarm << "writing failed for rawfile with rawfid:" << mRawFileID << enderr;
			return -3;
		}
	}
	catch(const OmnExcept &e)
	{
		OmnAlarm << "Failed to overwrite rawfile with rawfid:" << mRawFileID << ", errmsg:"
				<< e.getErrmsg() << enderr;
		return -4;
	}
	catch(...)
	{
		OmnAlarm << "Failed to overwrite rawfile with rawfid:" << mRawFileID << enderr;
		return -5;
	}
	return 0;
}


int
AosRawFile::composeFileNameWithFullPath(
		AosRundata	*rdata,
		const u16	level_1_size,
		const u16	level_2_size,
		const u16	level_3_size,
		const Type	eType)
{
	//this function composes file name with full path
	u32 partition_id = (mRawFileID & 0xFF00000000000000) >> 56;
	u16 level_1_dir = mLocalID / level_1_size;
	u16 level_2_dir = mLocalID % level_1_size / level_2_size;
	u16 level_3_dir = mLocalID % level_1_size % level_2_size;
	if (!mPartitionIDMgr->getPathByPartitionID(rdata, partition_id, eType, mFileNameWithFullPath))
	{
		OmnAlarm << "getPathByPartitionID for partition_id '" << partition_id << "' failed" << enderr;
		return -1;
	}
	aos_assert_rr(0 <= mFileNameWithFullPath.length(), rdata, -2);
	mFileNameWithFullPath << "/" << mCubeID << "/" << mSiteID << "/" << mAseID << "/" << "rawfiles"	//raw file root dir
			<< "/" << level_1_dir << "/" << level_2_dir << "/" << level_3_dir << "/";
	try
	{
		boost::filesystem::create_directories(mFileNameWithFullPath.getBuffer());
	}
	catch(const boost::filesystem::filesystem_error& e)
	{
		OmnAlarm << "something is wrong. path to create:'" << mFileNameWithFullPath << "' errmsg:'" << e.what() << "'" << enderr;
		mFileNameWithFullPath << mLocalID;
		return -3;
	}
	catch(...)
	{
		OmnAlarm << "something is wrong. path to create:'" << mFileNameWithFullPath <<"' with rawfid '" << mRawFileID << "'" << enderr;
		mFileNameWithFullPath << mLocalID;
		return -4;
	}
	mFileNameWithFullPath << mLocalID;
	return 0;
}


int
AosRawFile::open(
		AosRundata			*rdata,
		AosRawFile::Type	open_mode,
		const bool			bShowAlarm)
{
	return open(rdata, (OmnFile::Mode)open_mode, bShowAlarm);
}


int
AosRawFile::open(
		AosRundata		*rdata,
		OmnFile::Mode	open_mode,
		const bool		bShowAlarm)
{
	//this function opens a raw file
	if (!mFile)
	{
		OmnAlarm << "mFile is NULL for file '" << mFileNameWithFullPath <<"' with rawfid '" << mRawFileID << "'" << enderr;
		return -1;
	}

	if(!mFile->openFile1(mFileNameWithFullPath, open_mode))
	{
		if (bShowAlarm)
		{
			OmnAlarm << "open file '" << mFileNameWithFullPath << "' rawfid '"
					<< mRawFileID << "' failed with open mode:'" << open_mode << "'" << enderr;
		}
		else
		{
			OmnScreen << "open file '" << mFileNameWithFullPath << "' rawfid '"
					<< mRawFileID << "' failed with open mode:'" << open_mode << "'" << endl;
		}
		return -2;
	}

	return 0;
}


u32
AosRawFile::getLastModifyTime()
{
	return OmnFile::getLastModifyTime(mFileNameWithFullPath);
}


void
AosRawFile::dumpInfo()
{
	OmnScreen << "dumping rawfile info:" << endl
			<< "rawfid:" << mRawFileID << endl
			<< "siteid:" << mSiteID << endl
			<< "aseid:" << mAseID << endl
			<< "cubeid:" << mCubeID << endl;
}


int
AosRawFile::truncate(
		AosRundata 	*rdata,
		const u64 	length)
{
	OmnScreen << "truncating file '" << mFileNameWithFullPath << "' origin len:" << getLength()
			<< " to new len:" << length << endl;
	int iRet = ::truncate(mFileNameWithFullPath.data(), length);
	if (0 != iRet)
	{
		OmnAlarm << "truncate file '" << mFileNameWithFullPath << "' failed, iRet=" << iRet << enderr;
		return -1;
	}
	OmnScreen << "file truncated to length:" << length << " new file len:" << getLength() << endl;
	return 0;
}
