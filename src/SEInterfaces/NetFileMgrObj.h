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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_NetFileMgrObj_h
#define AOS_SEInterfaces_NetFileMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/AioCaller.h"
#include "SEInterfaces/NetFileObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

#include <dirent.h>
#include <sys/stat.h>
#include <vector>

using namespace std;
class AosDiskStat;


class AosNetFileMgrObj : virtual public OmnRCObject
{
	static AosNetFileMgrObjPtr	smNetFileMgrObj;

public:
	static AosNetFileMgrObjPtr getObj() {return smNetFileMgrObj;}
	static void setObj(const AosNetFileMgrObjPtr &d) {smNetFileMgrObj = d;}

	virtual bool	isLocal(const int physicalid) = 0;

	virtual bool	dirIsGood(
						const OmnString &path,
						const int physicalid,
						const bool checkfile,
						AosRundata *rdata) = 0;
	virtual bool	dirIsGoodLocal(
						const OmnString &path,
						const bool checkfile,
						AosRundata *rdata) = 0;

	virtual bool	fileIsGood(
						const OmnString &filename,
						const int physicalid,
						AosRundata *rdata) = 0;
	virtual bool	fileIsGoodLocal(
						const OmnString &filename,
						AosRundata *rdata) = 0;

	virtual bool	getFileLength(
						const u64 &fileId,
						const int physicalid,
						int64_t &fileLen,
						AosDiskStat &disk_stat,
						AosRundata *rdata) = 0;
	virtual bool 	getFileLengthLocal(
						const u64 &fileId,
						int64_t &fileLen,
						AosRundata *rdata) = 0;

	virtual bool	getFileLength(
						const OmnString &filename,
						const int physicalid,
						int64_t &fileLen,
						AosDiskStat &disk_stat,
						AosRundata *rdata) = 0;
	virtual bool	getFileLengthLocal(
						const OmnString &filename,
						int64_t &fileLen,
						AosRundata *rdata) = 0;

	virtual bool	readFileToBuff(
						const u64 &fileId,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosDiskStat &disk_stat,
						AosRundata *rdata) = 0;
	virtual bool	readFileToBuffLocal(
						const u64 &fileId,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosRundata *rdata) = 0;

	virtual bool	readFileToBuff(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosDiskStat &disk_stat,
						AosRundata *rdata) = 0;
	virtual bool	readFileToBuffLocal(
						const OmnString &filename,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosRundata *rdata) = 0;

	virtual bool	writeBuffToFile(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const AosBuffPtr &buff,
						AosRundata *rdata) = 0;
	virtual bool	writeBuffToFileLocal(
						const OmnString &filename,
						const int64_t &seekPos,
						const AosBuffPtr &buff,
						AosRundata *rdata) = 0;

	virtual bool	appendBuffToFile(
						const OmnString &filename,
						const int physicalid,
						const AosBuffPtr &buff,
						AosRundata *rdata) = 0;
	virtual bool	appendBuffToFileLocal(
						const OmnString &filename,
						const AosBuffPtr &buff,
						AosRundata *rdata) = 0;

	virtual bool	getDirList(
						const OmnString &path,
						const int physicalid,
						vector<AosDirInfo> &dir_list,
						AosRundata *rdata) = 0;
	virtual bool	getDirListLocal(
						const OmnString &path,
						vector<AosDirInfo> &dir_list,
						AosRundata *rdata) = 0;

	virtual bool	getFileList(
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata) = 0;
	virtual bool	getFileListLocal(
						const OmnString &path,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata) = 0;

	virtual bool	getFileInfo(
						const OmnString &filename,
						const int physicalid,
						AosFileInfo &file_info,
						AosRundata *rdata) = 0;
	virtual bool	getFileInfoLocal(
						const OmnString &filename,
						const int physicalid,
						AosFileInfo &file_info,
						AosRundata *rdata) = 0;

	virtual bool	asyncReadFile(
						const u64 &fileId,
						const int physicalid,
						const int64_t &seekPos, 
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata) = 0;
	virtual bool 	asyncReadFileLocal(
						const u64 &fileId,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata) = 0;

	virtual bool	asyncReadFile(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata) = 0;
	virtual bool 	asyncReadFileLocal(
						const OmnString &filename,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata) = 0;

	virtual bool	deleteFile(
						const u64 &fileId,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata) = 0;
	virtual bool 	deleteFileLocal(
						const u64 &fileId,
						AosRundata *rdata) = 0;

	virtual bool	deleteFile(
						const OmnString &filename,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata) = 0;
	virtual bool 	deleteFileLocal(
						const OmnString &filename,
						AosRundata *rdata) = 0;

	virtual bool 	getFileListByAssignExt(
						const OmnString &ext,
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata) = 0;
	virtual bool 	getFileListByAssignExtLocal(
						const OmnString &ext,
						const OmnString &path,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata) = 0;

	virtual OmnLocalFilePtr createRaidFile(
						AosRundata *rdata,
						u64 &fileId,
						const OmnString &fname_prefix,
						const u64 &requested_space,
						const bool reserve_flag) = 0;

	virtual OmnFilePtr createTempFile(
						AosRundata *rdata,
						u64 &fileId,
						const OmnString &fname_prefix,
						const u64 &requested_space,
						const bool reserve_flag) = 0;

	virtual OmnLocalFilePtr openLocalFile(
						const u64 &fileId,
						AosRundata *rdata) = 0;

public:
	static bool		dirIsGoodStatic(
						const OmnString &path,
						const int physicalid,
						const bool checkfile,
						AosRundata *rdata);
	static bool		dirIsGoodLocalStatic(
						const OmnString &path,
						const bool checkfile,
						AosRundata *rdata);

	static bool		fileIsGoodStatic(
						const OmnString &filename,
						const int physicalid,
						AosRundata *rdata);
	static bool		fileIsGoodLocalStatic(
						const OmnString &filename,
						AosRundata *rdata);

	static bool		getFileLengthStatic(
						const u64 &fileId,
						const int physicalid,
						int64_t &fileLen,
						AosDiskStat &disk_stat,
						AosRundata *rdata);
	static bool		getFileLengthLocalStatic(
						const u64 &fileId,
						int64_t &fileLen,
						AosRundata *rdata);

	static bool		getFileLengthStatic(
						const OmnString &filename,
						const int physicalid,
						int64_t &fileLen,
						AosDiskStat &disk_stat,
						AosRundata *rdata);
	static bool		getFileLengthLocalStatic(
						const OmnString &filename,
						int64_t &fileLen,
						AosRundata *rdata);

	static bool		readFileToBuffStatic(
						const u64 &fileId,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosDiskStat &disk_stat,
						AosRundata *rdata);
	static bool		readFileToBuffLocalStatic(
						const u64 &fileId,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosRundata *rdata);

	static bool		readFileToBuffStatic(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosDiskStat &disk_stat,
						AosRundata *rdata);
	static bool		readFileToBuffLocalStatic(
						const OmnString &filename,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosRundata *rdata);

	static bool		writeBuffToFileStatic(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const AosBuffPtr &buff,
						AosRundata *rdata);
	static bool		writeBuffToFileLocalStatic(
						const OmnString &filename,
						const int64_t &seekPos,
						const AosBuffPtr &buff,
						AosRundata *rdata);

	static bool		appendBuffToFileStatic(
						const OmnString &filename,
						const int physicalid,
						const AosBuffPtr &buff,
						AosRundata *rdata);
	static bool		appendBuffToFileLocalStatic(
						const OmnString &filename,
						const AosBuffPtr &buff,
						AosRundata *rdata);

	static bool		getDirListStatic(
						const OmnString &path,
						const int physicalid,
						vector<AosDirInfo> &dir_list,
						AosRundata *rdata);
	static bool		getDirListLocalStatic(
						const OmnString &path,
						vector<AosDirInfo> &dir_list,
						AosRundata *rdata);

	static bool		getFileListStatic(
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);
	static bool		getFileListLocalStatic(
						const OmnString &path,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);

	static bool		getFileInfoStatic(
						const OmnString &filename,
						const int physicalid,
						AosFileInfo &file_info,
						AosRundata *rdata);
	static bool		getFileInfoLocalStatic(
						const OmnString &filename,
						const int physicalid,
						AosFileInfo &file_info,
						AosRundata *rdata);

	static bool 	asyncReadFileStatic(
						const u64 &fileId,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);
	static bool		asyncReadFileLocalStatic(
						const u64 &fileId,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);

	static bool    asyncReadFileStatic(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);
	static bool    asyncReadFileLocalStatic(
						const OmnString &filename,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);

	static bool 	deleteFileStatic(
						const u64 &fileId,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata);
	static bool 	deleteFileLocalStatic(
						const u64 &fileId,
						AosRundata *rdata);

	static bool 	deleteFileStatic(
						const OmnString &filename,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata);
	static bool 	deleteFileLocalStatic(
						const OmnString &fname,
						AosRundata *rdata);

	static bool 	getFileListByAssignExtStatic(
						const OmnString &ext,
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);
	static bool 	getFileListByAssignExtLocalStatic(
						const OmnString &ext,
						const OmnString &path,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);

	static OmnLocalFilePtr createRaidFileStatic(
						AosRundata *rdata,
						u64 &fileId,
						const OmnString &fname_prefix,
						const u64 &requested_space,
						const bool reserve_flag);

	static OmnFilePtr createTempFileStatic(
						AosRundata *rdata,
						u64 &fileId,
						const OmnString &fname_prefix,
						const u64 &requested_space,
						const bool reserve_flag);

	static OmnLocalFilePtr openLocalFileStatic(
						const u64 &fileId,
						AosRundata *rdata);

};

#endif

