////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/08/06	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/NetFileMgrObj.h"

#include "Util/DiskStat.h"
#include "Util/File.h"
#include "Util/LocalFile.h"


AosNetFileMgrObjPtr AosNetFileMgrObj::smNetFileMgrObj;

	
bool
AosNetFileMgrObj::dirIsGoodStatic(
		const OmnString &path,
		const int physicalid,
		const bool checkfile,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->dirIsGood(path, physicalid, checkfile, rdata);
}


bool
AosNetFileMgrObj::dirIsGoodLocalStatic(
		const OmnString &path,
		const bool checkfile,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->dirIsGoodLocal(path, checkfile, rdata);
}


bool
AosNetFileMgrObj::fileIsGoodStatic(
		const OmnString &filename,
		const int physicalid,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->fileIsGood(filename, physicalid, rdata);
}


bool
AosNetFileMgrObj::fileIsGoodLocalStatic(
		const OmnString &filename,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->fileIsGoodLocal(filename, rdata);
}


bool
AosNetFileMgrObj::getFileLengthStatic(
		const u64 &fileId,
		const int physicalid,
		int64_t &fileLen,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileLength(
		fileId, physicalid, fileLen, disk_stat, rdata);
}


bool
AosNetFileMgrObj::getFileLengthLocalStatic(
		const u64 &fileId,
		int64_t &fileLen,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileLengthLocal(fileId, fileLen, rdata);
}


bool
AosNetFileMgrObj::getFileLengthStatic(
		const OmnString &filename,
		const int physicalid,
		int64_t &fileLen,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileLength(
		filename, physicalid, fileLen, disk_stat, rdata);
}


bool
AosNetFileMgrObj::getFileLengthLocalStatic(
		const OmnString &filename,
		int64_t &fileLen,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileLengthLocal(filename, fileLen, rdata);
}


bool
AosNetFileMgrObj::readFileToBuffStatic(
		const u64 &fileId,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->readFileToBuff(
		fileId, physicalid, seekPos, bytes_to_read, buff, disk_stat, rdata);
}


bool
AosNetFileMgrObj::readFileToBuffLocalStatic(
		const u64 &fileId,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->readFileToBuffLocal(
		fileId, seekPos, bytes_to_read, buff, rdata);
}


bool
AosNetFileMgrObj::readFileToBuffStatic(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->readFileToBuff(
		filename, physicalid, seekPos, bytes_to_read, buff, disk_stat, rdata);
}


bool
AosNetFileMgrObj::readFileToBuffLocalStatic(
		const OmnString &filename,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->readFileToBuffLocal(
		filename, seekPos, bytes_to_read, buff, rdata);
}


bool
AosNetFileMgrObj::writeBuffToFileStatic(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->writeBuffToFile(
		filename, physicalid, seekPos, buff, rdata);
}


bool
AosNetFileMgrObj::writeBuffToFileLocalStatic(
		const OmnString &filename,
		const int64_t &seekPos,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->writeBuffToFileLocal(
		filename, seekPos, buff, rdata);
}


bool
AosNetFileMgrObj::appendBuffToFileStatic(
		const OmnString &filename,
		const int physicalid,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->appendBuffToFile(
		filename, physicalid, buff, rdata);
}


bool
AosNetFileMgrObj::appendBuffToFileLocalStatic(
		const OmnString &filename,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->appendBuffToFileLocal(filename, buff, rdata);
}


bool
AosNetFileMgrObj::getDirListStatic(
		const OmnString &path,
		const int physicalid,
		vector<AosDirInfo> &dir_list,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getDirList(path, physicalid, dir_list, rdata);
}


bool
AosNetFileMgrObj::getDirListLocalStatic(
		const OmnString &path,
		vector<AosDirInfo> &dir_list,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getDirListLocal(path, dir_list, rdata);
}


bool
AosNetFileMgrObj::getFileListStatic(
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileList(
		path, physicalid, rec_level, file_list, rdata);
}


bool
AosNetFileMgrObj::getFileListLocalStatic(
		const OmnString &path,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileListLocal(
		path, rec_level, file_list, rdata);
}


bool
AosNetFileMgrObj::getFileInfoStatic(
		const OmnString &filename,
		const int physicalid,
		AosFileInfo &file_info,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileInfo(
		filename, physicalid, file_info, rdata);
}


bool
AosNetFileMgrObj::getFileInfoLocalStatic(
		const OmnString &filename,
		const int physicalid,
		AosFileInfo &file_info,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileInfoLocal(
		filename, physicalid, file_info, rdata);
}


bool
AosNetFileMgrObj::asyncReadFileStatic(
		const u64 &fileId,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->asyncReadFile(
		fileId, physicalid, seekPos, bytes_to_read, reqid, caller, rdata);
}


bool
AosNetFileMgrObj::asyncReadFileLocalStatic(
		const u64 &fileId,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->asyncReadFileLocal(
		fileId, seekPos, bytes_to_read, reqid, caller, rdata);
}


bool
AosNetFileMgrObj::asyncReadFileStatic(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->asyncReadFile(
		filename, physicalid, seekPos, bytes_to_read, reqid, caller, rdata);
}


bool
AosNetFileMgrObj::asyncReadFileLocalStatic(
		const OmnString &filename,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->asyncReadFileLocal(
		filename, seekPos, bytes_to_read, reqid, caller, rdata);
}


bool
AosNetFileMgrObj::deleteFileStatic(
		const u64 &fileId,
		const int physicalid,
		bool &svr_death,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->deleteFile(
		fileId, physicalid, svr_death, rdata);
}


bool
AosNetFileMgrObj::deleteFileLocalStatic(
		const u64 &fileId,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->deleteFileLocal(fileId, rdata);
}


bool
AosNetFileMgrObj::deleteFileStatic(
		const OmnString &filename,
		const int physicalid,
		bool &svr_death,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->deleteFile(
		filename, physicalid, svr_death, rdata);
}


bool
AosNetFileMgrObj::deleteFileLocalStatic(
		const OmnString &filename,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->deleteFileLocal(filename, rdata);
}


bool
AosNetFileMgrObj::getFileListByAssignExtStatic(
		const OmnString &ext,
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileListByAssignExt(
		ext, path, physicalid, rec_level, file_list, rdata);
}


bool
AosNetFileMgrObj::getFileListByAssignExtLocalStatic(
		const OmnString &ext,
		const OmnString &path,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, false);
	return smNetFileMgrObj->getFileListByAssignExtLocal(
		ext, path, rec_level, file_list, rdata);
}


OmnLocalFilePtr
AosNetFileMgrObj::createRaidFileStatic(
		AosRundata *rdata,
		u64 &fileId,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
{
	aos_assert_r(smNetFileMgrObj, 0);
	return smNetFileMgrObj->createRaidFile(
		rdata, fileId, fname_prefix, requested_space, reserve_flag);
}


OmnFilePtr
AosNetFileMgrObj::createTempFileStatic(
		AosRundata *rdata,
		u64 &fileId,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
{
	aos_assert_r(smNetFileMgrObj, 0);
	return smNetFileMgrObj->createTempFile(
		rdata, fileId, fname_prefix, requested_space, reserve_flag);
}


OmnLocalFilePtr
AosNetFileMgrObj::openLocalFileStatic(
		const u64 &fileId,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileMgrObj, 0);
	return smNetFileMgrObj->openLocalFile(fileId, rdata);
}

