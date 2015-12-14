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
#include "SEInterfaces/NetFileCltObj.h"

#include "Util/DiskStat.h"
#include "Util/File.h"
#include "Util/LocalFile.h"


AosNetFileCltObjPtr AosNetFileCltObj::smNetFileCltObj;

	
bool
AosNetFileCltObj::dirIsGoodStatic(
		const OmnString &path,
		const int physicalid,
		const bool checkfile,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->dirIsGood(path, physicalid, checkfile, rdata);
}


bool
AosNetFileCltObj::fileIsGoodStatic(
		const OmnString &filename,
		const int physicalid,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->fileIsGood(filename, physicalid, rdata);
}


bool
AosNetFileCltObj::getFileLengthStatic(
		const u64 &fileId,
		const int physicalid,
		int64_t &fileLen,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->getFileLength(
		fileId, physicalid, fileLen, disk_stat, rdata);
}


bool
AosNetFileCltObj::getFileLengthStatic(
		const OmnString &filename,
		const int physicalid,
		int64_t &fileLen,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->getFileLength(
		filename, physicalid, fileLen, disk_stat, rdata);
}


bool
AosNetFileCltObj::readFileToBuffStatic(
		const u64 &fileId,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->readFileToBuff(
		fileId, physicalid, seekPos, bytes_to_read, buff, disk_stat, rdata);
}


bool
AosNetFileCltObj::readFileToBuffStatic(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->readFileToBuff(
		filename, physicalid, seekPos, bytes_to_read, buff, disk_stat, rdata);
}


bool
AosNetFileCltObj::writeBuffToFileStatic(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->writeBuffToFile(
		filename, physicalid, seekPos, buff, rdata);
}


bool
AosNetFileCltObj::appendBuffToFileStatic(
		const OmnString &filename,
		const int physicalid,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->appendBuffToFile(
		filename, physicalid, buff, rdata);
}


bool
AosNetFileCltObj::getDirListStatic(
		const OmnString &path,
		const int physicalid,
		vector<AosDirInfo> &dir_list,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->getDirList(path, physicalid, dir_list, rdata);
}


bool
AosNetFileCltObj::getFileListStatic(
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->getFileList(
		path, physicalid, rec_level, file_list, rdata);
}


bool
AosNetFileCltObj::getFileInfoStatic(
		const OmnString &filename,
		const int physicalid,
		AosFileInfo &file_info,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->getFileInfo(
		filename, physicalid, file_info, rdata);
}


bool
AosNetFileCltObj::asyncReadFileStatic(
		const u64 &fileId,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->asyncReadFile(
		fileId, physicalid, seekPos, bytes_to_read, reqid, caller, rdata);
}


bool
AosNetFileCltObj::asyncReadFileStatic(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->asyncReadFile(
		filename, physicalid, seekPos, bytes_to_read, reqid, caller, rdata);
}


bool
AosNetFileCltObj::deleteFileStatic(
		const u64 &fileId,
		const int physicalid,
		bool &svr_death,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->deleteFile(
		fileId, physicalid, svr_death, rdata);
}


bool
AosNetFileCltObj::deleteFileStatic(
		const OmnString &filename,
		const int physicalid,
		bool &svr_death,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->deleteFile(
		filename, physicalid, svr_death, rdata);
}


bool
AosNetFileCltObj::getFileListByAssignExtStatic(
		const OmnString &ext,
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, false);
	return smNetFileCltObj->getFileListByAssignExt(
		ext, path, physicalid, rec_level, file_list, rdata);
}

	
OmnLocalFilePtr
AosNetFileCltObj::createRaidFileStatic(
		AosRundata *rdata,
		u64 &fileId,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
{
	aos_assert_r(smNetFileCltObj, 0);
	return smNetFileCltObj->createRaidFile(
		rdata, fileId, fname_prefix, requested_space, reserve_flag);
}


OmnFilePtr
AosNetFileCltObj::createTempFileStatic(
		AosRundata *rdata,
		u64 &fileId,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
{
	aos_assert_r(smNetFileCltObj, 0);
	return smNetFileCltObj->createTempFile(
		rdata, fileId, fname_prefix, requested_space, reserve_flag);
}


OmnLocalFilePtr
AosNetFileCltObj::openLocalFileStatic(
		const u64 &fileId,
		AosRundata *rdata)
{
	aos_assert_r(smNetFileCltObj, 0);
	return smNetFileCltObj->openLocalFile(fileId, rdata);
}

