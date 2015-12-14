////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 08/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "ReliableFile/ReliableFile.h"

#include "API/AosApiI.h"
#include "FmtMgr/Fmt.h"
#include "ReliableFile/RlbFileNormal.h"
#include "ReliableFile/RlbFileLog.h"
#include "ReliableFile/RlbFileNotSend.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/FmtMgrObj.h"
	

AosReliableFilePtr AosReliableFile::smReliableFiles[AosRlbFileType::eMax];
static OmnMutex sgLock;
static bool 	sgInited = false;

static bool	sgShowLog = false;

AosReliableFile::AosReliableFile(const AosRlbFileType::E type)
:
OmnLocalFile("ReliableFile"),
mLock(OmnNew OmnMutex()),
mFileId(0),
mType(type),
mIsNewFile(true),
mUseGlobalFmt(false)
{
}


AosReliableFile::AosReliableFile(
		const AosRlbFileType::E type, 
		const AosStorageFileInfo &file_info,
		const bool create_flag,
		AosRundata *rdata)
:
OmnLocalFile("ReliableFile"),
mLock(OmnNew OmnMutex()),
mFileId(file_info.fileId),
mFileInfo(file_info),
mType(type),
mIsNewFile(true),
mUseGlobalFmt(false)
{
	if (!openFile(create_flag, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosReliableFile::~AosReliableFile()
{
}


bool
AosReliableFile::init()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	smReliableFiles[AosRlbFileType::eNormal] = OmnNew AosRlbFileNormal();
	smReliableFiles[AosRlbFileType::eLog] = OmnNew AosRlbFileLog();
	smReliableFiles[AosRlbFileType::eNotSend] = OmnNew AosRlbFileNotSend();

	sgInited = true;
	sgLock.unlock();
	return true;

}


// Chen Ding, 2014/08/02
OmnString
AosReliableFile::getFullFilename() 
{
	return mFileInfo.getFullFname();
}


bool
AosReliableFile::openFile(
		const bool create_flag,
		AosRundata *rdata)
{
	// open loc file.
	OmnString full_fname = mFileInfo.getFullFname();
	mIsNewFile = false;
	if(!OmnFile::fileExist(full_fname))
	{
		if(!create_flag)
		{
			// This file not create yet. and not need create.	
			mLocFile = 0;
			//OmnAlarm << "file is not Create yet." << full_fname << enderr;
			return false;
		}
		mLocFile = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
		if (!mLocFile->isGood())
		{
			mLocFile = 0;
			OmnAlarm << "Failed creating file: " << full_fname << enderr;
			return false;
		}
		mIsNewFile = true;
		//Linda, 2013/08/22 LINDAFMT
		addCreateFileFmt(rdata);
	
		if (sgShowLog)
		{
			OmnScreen << "ReplicMgr log; create LocFile" 
				<< "; virtual_id:" << (mFileId >> 32) 
				<< "; fileId:" << mFileId
				<< "; fileSeqno:" << (u32)mFileId
				<< "; fname" << full_fname << endl
				<< endl; 
		}
		return true;
	}

	// This file is exist.
	mIsNewFile = false;
	mLocFile = OmnNew OmnFile(full_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!mLocFile->isGood())
	{
		// The local file is damaged. 
		mLocFile = 0;
		OmnAlarm << "file is not good: " << full_fname << enderr;
		return false;
	}

	if(sgShowLog)
	{
		OmnScreen << "ReplicMgr log; open LocFile" 
			<< "; virtual_id:" << (mFileId >> 32) 
			<< "; fileId:" << mFileId
			<< "; fileSeqno:" << (u32)mFileId
			<< "; fname" << full_fname << endl
			<< endl; 
	}
	return true;
}


AosReliableFilePtr 
AosReliableFile::getReliableFile(
		const u64 &file_id, 
		const AosRlbFileType::E type, 
		const bool create_flag, 
		AosRundata *rdata)
{
	if (!sgInited) init();
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	bool find = false;
	AosStorageFileInfo file_info;
	bool rslt = vfsMgr->getFileInfo(file_id, file_info, find, rdata);
	aos_assert_r(rslt, 0);
	if(!find)	
	{
		OmnAlarm << "can't find this file."	<< enderr;
		return 0;
	}

	return getReliableFile(file_info, type, create_flag, rdata);
}


AosReliableFilePtr 
AosReliableFile::getReliableFile(
		const AosStorageFileInfo &file_info,
		const AosRlbFileType::E type, 
		const bool create_flag, 
		AosRundata *rdata)
{
	if (!sgInited) init();

	if (!AosRlbFileType::isValid(type))
	{
		AosSetErrorU(rdata, "invalid_reliablefile type") << ": " << type << enderr;
		return 0;
	}

	AosReliableFilePtr file = smReliableFiles[type];
	aos_assert_rr(file, rdata, 0);
	return file->clone(file_info, create_flag, rdata);
}


u64
AosReliableFile::getFileCrtSize() const
{
	mLock->lock();
	u64 fsize = mLocFile->getFileCrtSize();
	mLock->unlock();
	return fsize;
}

	
bool
AosReliableFile::isGood() const
{
	mLock->lock();
	bool good = mLocFile->isGood();
	mLock->unlock();
	return good;
}


bool
AosReliableFile::flushFileContents()
{
	mLock->lock();
	bool rslt = mLocFile->flushFileContents();
	mLock->unlock();
	return rslt;
}

// Ketty 2014/02/21
bool
AosReliableFile::closeFile()
{
	mLock->lock();
	bool rslt = mLocFile->closeFile();
	mLock->unlock();
	return rslt;
}

bool
AosReliableFile::modifyLocalFile(
		const u64 &offset, 
		const char *data, 
		const int len, 
		const bool flush, 
		AosRundata *rdata)
{
	mLock->lock();
	bool rslt = mLocFile->put(offset, data, len, flush);
	aos_assert_rl(rslt, mLock, false);

	if (sgShowLog)
	{
		OmnScreen << "ReplicMgr log; Modify LocFile; "
				<< "; virtual_id:" << (mFileId >> 32) 
				<< "; fileId:" << mFileId 
				<< "; fileSeqno:" << (u32)mFileId
				<< "; offset:" << offset
				<< "; len:" << len
				<< endl;
	}
	if (!rslt)
	{
		// This will never happend.
		OmnAlarm << "modify file error: fname:" << mLocFile->getFileName() << enderr;
		mLock->unlock();
		return false;
	}
	
	rslt = sendFmt(rdata, offset, data, len, flush);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();	
	return true;
}


bool
AosReliableFile::readFile(
		const u64 offset, 
		const u32 size,
		char *data,
		AosRundata *rdata)
{
	mLock->lock();
	int bytes_read = mLocFile->readToBuff(offset, size, data);
	mLock->unlock();
	aos_assert_rr(bytes_read >=0, rdata, false);
	return true;
}


bool
AosReliableFile::recover(AosRundata *rdata)
{
	//OmnScreen << "!!!! mFileId:" << mFileId << endl;
	OmnAlarm << "false" << enderr;
	//return true;
	return false;

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	bool rslt = vfsMgr->rebindFile(mFileInfo, rdata);
	if(!rslt)
	{
		// This is a serious problem. It should never happen.
		OmnAlarm << "Failed rebinding" << enderr;
		return false;
	}
	
	OmnString full_fname = mFileInfo.getFullFname();
	mLock->lock();
	mLocFile = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!mLocFile->isGood())
	{
		OmnAlarm << "Failed rebinding; new fname:" << full_fname << enderr;
		return false;
	}
		
	//rslt = AosReplicMgrClt::getSelf()->recoverFile(rdata, mFileId, mLocFile);
	mLock->unlock();
	aos_assert_r(rslt, false);

	if (sgShowLog)
	{
		OmnScreen << "recover LocFile; "
			<< "; fileId:" << mFileInfo.fileId 
			<< "; new_fname" << mLocFile->getFileName() << endl;
	}
	return true;
}


bool
AosReliableFile::removeFile(AosRundata *rdata)
{
	//Linda 2013/04/19
	aos_assert_r(mLocFile, false);
	OmnString full_fname = mFileInfo.getFullFname();
	if (!mLocFile->isGood())
	{
		OmnAlarm << "Failed creating file: " << full_fname << enderr;
		return false;
	}
	mLocFile->deleteFile();

	//Linda, 2013/08/22 LINDAFMT	
	// add fmt
	if(!AosIsSelfCubeSvr())	return true;

	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, false);
	
	mLock->lock();
	AosFmtPtr fmt = fmt_mgr->getFmt();
	if (mUseGlobalFmt || !fmt)
	{
		fmt_mgr->addDeleteFileToGlobalFmt(rdata, mFileId);
		mLock->unlock();
		return true;
	}
	fmt->addDeleteFile(mFileId);
	mLock->unlock();
	return true;
}


void
AosReliableFile::addCreateFileFmt(AosRundata *rdata)
{
	if(!AosIsSelfCubeSvr())	return;
	
	// add fmt
	mLock->lock();
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert(fmt_mgr);
		
	AosFmtPtr fmt = fmt_mgr->getFmt();
	fmt_mgr->addToGlobalFmt(rdata, mFileInfo);
	
	//if (mUseGlobalFmt || !fmt)
	//{
	//	fmt_mgr->addToGlobalFmt(rdata, mFileInfo);
	//}
	//else
	//{
	//	fmt->add(mFileInfo);
	//}
	mLock->unlock();
}
	
bool
AosReliableFile::append(
		AosRundata *rdata,
		const u64 offset,
		const char *buff,
		const int64_t &len,
		const bool flush)
{
	// offset is temp. Ketty.
	mLock->lock();
	bool rslt = mLocFile->append(buff, len, flush);
	aos_assert_rl(rslt, mLock, false);
	
	if (sgShowLog)
	{
		OmnScreen << "ReplicMgr log; append LocFile; "
				<< "; virtual_id:" << (mFileId >> 32) 
				<< "; fileId:" << mFileId 
				<< "; fileSeqno:" << (u32)mFileId
				<< "; offset:" << offset
				<< "; len:" << len
				<< endl;
	}

	rslt = sendFmt(rdata, offset, buff, len, flush);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();	
	return true;
}

bool
AosReliableFile::sendFmt(
		AosRundata *rdata,
		const u64 offset, 
		const char *buff,
		const int64_t &len,
		const bool flush)
{
	return true;
	//Linda, 2013/08/22 LINDAFMT
	// add fmt
	if(!AosIsSelfCubeSvr())	return true;
	
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	if(!fmt_mgr)
	{
		OmnScreen << "missing missing missing !!" << endl;
		return true;
	}
	
	AosFmtPtr fmt = fmt_mgr->getFmt();
	if (mUseGlobalFmt || !fmt)
	{
		fmt_mgr->addToGlobalFmt(rdata, mFileId, offset, buff, len, flush);
		return true;
	}
	fmt->add(mFileId, offset, buff, len, flush);
	return true;
}

