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
// This is a utility to select docs.
//
// Modification History:
// 2013/04/13	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#include "StorageMgr/NetFileMgr.h"

#include "API/AosApi.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/ReadFile.h"
#include "Util/File.h"
#include "Util/DiskStat.h"
#include "JobTrans/DirIsGoodTrans.h"
#include "JobTrans/FileIsGoodTrans.h"
#include "JobTrans/GetFileLengthTrans.h"
#include "JobTrans/GetDirListTrans.h"
#include "JobTrans/GetFileListTrans.h"
#include "JobTrans/GetFileInfoTrans.h"
#include "JobTrans/ReadFileTrans.h"
#include "JobTrans/WriteFileTrans.h"
#include "JobTrans/AppendFileTrans.h"
#include "JobTrans/AsyncReadFileTrans.h"
#include "JobTrans/AsyncReadFileByFileNameTrans.h"
#include "JobTrans/GetFileLengthByIdTrans.h"
#include "JobTrans/DeleteFileByIdTrans.h"
#include "JobTrans/DeleteFileByNameTrans.h"
#include "JobTrans/GetFileListByAssignExtTrans.h"
#include "StorageMgr/AioRequest.h"
#include "StorageMgr/StorageMgrAyscIo.h"
#include "Debug/Debug.h"


OmnSingletonImpl(AosNetFileMgrSingleton,
                 AosNetFileMgr,
                 AosNetFileMgrSelf,
                 "AosNetFileMgr");


AosNetFileMgr::AosNetFileMgr()
:
mLock(OmnNew OmnMutex()),
mDeleteFileLock(OmnNew OmnMutex()),
mDeleteFileCondVar(OmnNew OmnCondVar())
//mCallerId(100)
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "NetFileMgrThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosNetFileMgr::~AosNetFileMgr()
{
}


bool
AosNetFileMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosNetFileMgr::start()
{
	return true;
}


bool
AosNetFileMgr::stop()
{
	return true;
}


bool
AosNetFileMgr::addReq(
		AosRundata *rdata,
		const AosTransPtr &trans,
		bool &svr_death)
{
	bool rslt = AosSendTrans(rdata, trans, svr_death);
	aos_assert_r(rslt, false);
	rdata->setOk();
	return true;
}


bool
AosNetFileMgr::addReq(
		AosRundata *rdata,
		const AosTransPtr &trans)
{
	//Jozhi 2013/03/20
	bool rslt = AosSendTrans(rdata, trans);
	aos_assert_r(rslt, false);
	rdata->setOk();
	return true;
}

bool
AosNetFileMgr::addReq(
		AosRundata *rdata,
		const AosTransPtr &trans,
		AosBuffPtr &resp,
		bool &svr_death)
{
	//Jozhi 2013/03/20
	bool rslt = AosSendTrans(rdata, trans, resp, svr_death);
	aos_assert_r(rslt, false);

	if (svr_death)
	{
		return true;
	}
	
	if (!resp|| resp->dataLen() < 0)
	{
		AosSetErrorU(rdata, "no response");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;

}


bool
AosNetFileMgr::addReq(
		AosRundata *rdata,
		const AosTransPtr &trans,
		AosBuffPtr &resp)
{
	//Jozhi 2013/03/20
	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt, false);

	if (timeout)
	{
		AosSetErrorU(rdata, "Faild to add the trans, timeout");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (!resp|| resp->dataLen() < 0)
	{
		AosSetErrorU(rdata, "no response");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool
AosNetFileMgr::isLocal(const int physicalid)
{
	return physicalid == AosGetSelfServerId();
}


bool 
AosNetFileMgr::dirIsGood(
		const OmnString &path,
		const int physicalid,
		const bool checkfile,
		AosRundata *rdata)
{
	if (isLocal(physicalid))
	{
		return dirIsGoodLocal(path, checkfile, rdata);
	}

	AosTransPtr trans = OmnNew AosDirIsGoodTrans(path, checkfile, physicalid, false, true);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);
	return resp->getU8(0);
}


bool
AosNetFileMgr::dirIsGoodLocal(
		const OmnString &path,
		const bool checkfile,
		AosRundata *rdata)
{
	DIR *dir;
	bool dir_exist = ((dir = opendir(path.data())) != NULL);
	if (!dir_exist)
	{
		closedir(dir);
		return false;
	}

	if (!checkfile)
	{
		closedir(dir);
		return dir_exist;
	}

	bool file_exist = false;
	struct dirent * file;
	while((file = readdir(dir)) != NULL)
	{
		if (strncmp(file->d_name, ".", 1) == 0) continue;
		file_exist = true;
		break;
	}

	closedir(dir);
	return file_exist;
}


bool 
AosNetFileMgr::fileIsGood(
		const OmnString &fname,
		const int physicalid,
		AosRundata *rdata)
{
	if (isLocal(physicalid))
	{
		return fileIsGoodLocal(fname, rdata);
	}

	AosTransPtr trans = OmnNew AosFileIsGoodTrans(fname, physicalid, false, true);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);
	OmnTagFuncInfo << fname << " is " << resp->getU8(0) << endl;
	return resp->getU8(0);
}


bool
AosNetFileMgr::fileIsGoodLocal(
		const OmnString &fname,
		AosRundata *rdata)
{
	AosReadFilePtr file = OmnNew AosReadFile(fname);
	if (file && file->isGood())
	{
		return true;
	}
	return false;
}


bool
AosNetFileMgr::getFileLength(
		int64_t &filelen, 
		const OmnString &fname, 
		const int physicalid, 
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	if (isLocal(physicalid))
	{
		return getFileLengthLocal(filelen, fname, rdata);
	}

	AosTransPtr trans = OmnNew AosGetFileLengthTrans(fname, physicalid, false, true);
	bool svr_death = false;
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp, svr_death);
	if (svr_death)
	{
		disk_stat.setServerIsDown(svr_death);
		return true;
	}

	aos_assert_r(rslt && resp, false);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);

	filelen = resp->getInt64(-1);
	return true;
}


bool
AosNetFileMgr::getFileLengthLocal(
		int64_t &filelen,
		const OmnString &fname,
		AosRundata *rdata)
{
	filelen = OmnFile::getFileLengthStatic(fname);
	OmnTagFuncInfo << "file len is: " << filelen << endl;
	return true;
}


bool
AosNetFileMgr::readFileToBuff(
		AosBuffPtr &buff,
		const u64 &file_id,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	if (isLocal(physicalid))
	{
		return readFileToBuffLocal(buff, file_id, seekPos, bytes_to_read, rdata);
	}

	AosTransPtr trans = OmnNew AosReadFileTrans(file_id, seekPos, bytes_to_read, physicalid, false, true);
	AosBuffPtr resp;
	bool svr_death = false;
	bool rslt = addReq(rdata, trans, resp, svr_death);
	if (svr_death)
	{
		disk_stat.setServerIsDown(svr_death);
		return true;
	}
	aos_assert_r(rslt && resp, false);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);

	u32 buff_len = resp->getU32(0);
	buff = resp->getBuff(buff_len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosNetFileMgr::readFileToBuffLocal(
		AosBuffPtr &buff,
		const u64 &file_id,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosRundata *rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	OmnLocalFilePtr file = vfsMgr->openLocalFile(file_id, rdata);
	aos_assert_r(file && file->isGood(), false);

	buff = OmnNew AosBuff(bytes_to_read, 0 AosMemoryCheckerArgs);

	file->readlock();
	u32 bytes_read = file->read(seekPos, buff->data(), bytes_to_read);
	file->unlock();

	OmnTagFuncInfo << "read bytes: " << bytes_read << endl;
	buff->setDataLen(bytes_read);
	return true;
}


bool
AosNetFileMgr::readFileToBuff(
		AosBuffPtr &buff,
		const OmnString &fname,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	//buff = OmnNew AosBuff(0, 0 AosMemoryCheckerArgs);
	//disk_stat.setServerIsDown(true);
	//return true;
	if (isLocal(physicalid))
	{
		return readFileToBuffLocal(buff, fname, seekPos, bytes_to_read, rdata);
	}

	AosTransPtr trans = OmnNew AosReadFileTrans(fname, seekPos, bytes_to_read, physicalid, false, true);
	AosBuffPtr resp;
	bool svr_death = false;
	bool rslt = addReq(rdata, trans, resp, svr_death);
	if (svr_death)
	{
		disk_stat.setServerIsDown(svr_death);
		return true;
	}
	aos_assert_r(rslt && resp, false);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);

	u32 buff_len = resp->getU32(0);
	buff = resp->getBuff(buff_len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosNetFileMgr::readFileToBuffLocal(
		AosBuffPtr &buff,
		const OmnString &fname,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosRundata *rdata)
{
	AosReadFilePtr file = OmnNew AosReadFile(fname);
	aos_assert_r(file && file->isGood(), false);

	buff = OmnNew AosBuff(bytes_to_read, 0 AosMemoryCheckerArgs);

	file->lock();
	bool rslt = file->seek(seekPos);
	if (!rslt)
	{
		OmnAlarm << "seek error" << enderr;
		file->unlock();
		return false;
	}

	u32 bytes_read = file->readToBuff(bytes_to_read, buff->data());
	file->unlock();

	buff->setDataLen(bytes_read);
	return true;
}


bool 
AosNetFileMgr::writeBuffToFile(
		const AosBuffPtr &buff,
		const OmnString &fname,
		const int physicalid,
		const int64_t &seekPos,
		AosRundata *rdata)
{
	if (isLocal(physicalid))
	{
		return writeBuffToFileLocal(buff, fname, seekPos, rdata);
	}

	AosTransPtr trans = OmnNew AosWriteFileTrans(fname, seekPos, buff, physicalid, false, false);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileMgr::writeBuffToFileLocal(
		const AosBuffPtr &buff,
		const OmnString &fname,
		const int64_t &seekPos,
		AosRundata *rdata)
{
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eWriteCreate AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);

	file->lock();
	bool rslt = file->seek(seekPos);
	if (!rslt)
	{
		OmnAlarm << "seek error" << enderr;
		file->unlock();
		return false;
	}
	file->append(buff->data(), buff->dataLen(), true);
	file->unlock();
	return true;
}


bool
AosNetFileMgr::appendBuffToFile(
		const AosBuffPtr &buff,
		const OmnString &fname,
		const int physicalid,
		AosRundata *rdata)
{
	AosTransPtr trans = OmnNew AosAppendFileTrans(physicalid, fname, buff, false, false);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileMgr::appendBuffToFileLocal(
		const AosBuffPtr &buff,
		const OmnString &fname,
		AosRundata *rdata)
{
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eWriteCreate AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);

	file->lock();
	file->goToFileEnd();
	file->append(buff->data(), buff->dataLen(), true);
	file->unlock();
	return true;
}

bool
AosNetFileMgr::getFileListByAssignExt(
		vector<AosFileInfo> &file_list,
		const OmnString &ext,
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		AosRundata *rdata)
{
	if (isLocal(physicalid))
	{
		return getFileListByAssignExtLocal(file_list, ext, path, rec_level, rdata);
	}

	AosTransPtr trans = OmnNew AosGetFileListByAssignExtTrans(ext, path, physicalid, rec_level, false, true);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);

	int num = resp->getInt(-1);
	for(int i=0; i<num; i++)
	{
		AosFileInfo info;
		rslt = info.serializeFrom(resp);
		aos_assert_r(rslt, false);

		file_list.push_back(info);
	}
	
	return true;
}


bool
AosNetFileMgr::getFileListByAssignExtLocal(
		vector<AosFileInfo> &file_list,
		const OmnString &ext,
		const OmnString &path,
		const int rec_level,
		AosRundata *rdata)
{
	//ext is  file format 
	DIR * pDir = opendir(path.data());
	aos_assert_r(pDir, false);

	bool rslt = true;
	int ret = 0;
	struct stat st;
	struct dirent *ent;
	OmnString childPath;

	while ((ent = readdir(pDir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}

		childPath = path;
		childPath << "/" << ent->d_name;

		ret = stat(childPath.data(), &st);
		if (ret < 0) 
		{
			continue;
		}

		if (S_ISREG(st.st_mode))
		{
			if (st.st_size > 0)
			{
				OmnString fileName = ent->d_name;
				int idx = fileName.indexOf(0, '.', true);              
				int len = fileName.length();
				OmnString fileFormat = fileName.substr(idx+1, len - 1);
				if (fileFormat == ext)
				{
					AosFileInfo info(st);
					info.mFileName = childPath;
					file_list.push_back(info);
				}
			}
			continue;
		}

		if (S_ISDIR(st.st_mode))
		{
			if(rec_level == -1 )
			{
				rslt = getFileListByAssignExtLocal(file_list, ext, childPath, rec_level, rdata);
				aos_assert_r(rslt, false);
			}
			continue;
		}
	}

	closedir(pDir);
	return true;
}

bool
AosNetFileMgr::getDirList(
		vector<AosDirInfo> &dir_list,
		const OmnString &path,
		const int physicalid,
		AosRundata *rdata)
{
	aos_assert_r(path != "", false);

	if (isLocal(physicalid))
	{
		return getDirListLocal(dir_list, path, rdata);
	}

	AosTransPtr trans = OmnNew AosGetDirListTrans(path, physicalid, false, true);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);

	int num = resp->getInt(-1);
	for(int i=0; i<num; i++)
	{
		AosDirInfo info;
		rslt = info.serializeFrom(resp);
		aos_assert_r(rslt, false);

		dir_list.push_back(info);
	}
	
	return true;
}


bool
AosNetFileMgr::getFileList(
		vector<AosFileInfo> &file_list,
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		AosRundata *rdata)
{
	// if rec_level == -1 means recurive to the end.
	aos_assert_r(path != "", false);

	if (isLocal(physicalid))
	{
		return getFileListLocal(file_list, path, rec_level, rdata);
	}

	AosTransPtr trans = OmnNew AosGetFileListTrans(path, physicalid, rec_level, false, true);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);

	int num = resp->getInt(-1);
	for(int i=0; i<num; i++)
	{
		AosFileInfo info;
		rslt = info.serializeFrom(resp);
		aos_assert_r(rslt, false);

		file_list.push_back(info);
	}
	
	return true;
}


bool	
AosNetFileMgr::getFileInfo(
		AosFileInfo &file_info,
		const OmnString &file_name,
		const int physicalid,
		AosRundata *rdata)
{
	aos_assert_r(file_name != "", false);

	if (isLocal(physicalid))
	{
		return getFileInfoLocal(file_info, file_name, physicalid, rdata);
	}

	AosTransPtr trans = OmnNew AosGetFileInfoTrans(file_name, physicalid, false, true);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);

	rslt = file_info.serializeFrom(resp);
	aos_assert_r(rslt, false);

	return true;
}
	

bool
AosNetFileMgr::getDirListLocal(
		vector<AosDirInfo> &dir_list,
		const OmnString &path,
		AosRundata *rdata)
{
	DIR * pDir = opendir(path.data());
	if (!pDir)
	{
		AosSetEntityError(rdata, "NetFileMgr_open_dir_failed", 
			"NetFileMgr", "NetFileMgr") << path << enderr;
		return false;
	}

	struct dirent *ent;
	while ((ent = readdir(pDir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}

		if (ent->d_type == DT_DIR)
		{
			OmnString crtDirName = path;
			crtDirName << "/" << ent->d_name;
			AosDirInfo info(crtDirName);
			dir_list.push_back(info);
		}
	}

	closedir(pDir);
	return true;
}


bool
AosNetFileMgr::getFileListLocal(
		vector<AosFileInfo> &file_list,
		const OmnString &path,
		const int rec_level,
		AosRundata *rdata)
{
	return getFileListRec(file_list, path, rec_level, 0, rdata);
}


bool
AosNetFileMgr::getFileInfoLocal(
		AosFileInfo &file_info,
		const OmnString &file_name,
		const int physicalid,
		AosRundata *rdata)
{
	struct stat st;
	int ret = stat(file_name.data(), &st);
	if (ret < 0) return false;

	if (st.st_size > 0)
	{
		AosFileInfo info(st);
		info.mFileName = file_name;
		info.mPhysicalId = physicalid;
		file_info = info;
	}
		
	return true;
}


bool
AosNetFileMgr::getFileListRec(
		vector<AosFileInfo> &file_list,
		const OmnString &path,
		const int rec_level,
		const int crt_level,
		AosRundata *rdata)
{
	DIR * pDir = opendir(path.data());
	//aos_assert_r(pDir, false);
	if (!pDir)
	{
		AosSetEntityError(rdata, "NetFileMgr_open_dir_failed", 
			"NetFileMgr", "NetFileMgr") << path << enderr;
		return false;
	}

	bool rslt = true;
	int ret = 0;
	int new_level = crt_level + 1;
	struct stat st;
	struct dirent *ent;
	OmnString childPath;

	while ((ent = readdir(pDir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}

		childPath = path;
		childPath << "/" << ent->d_name;

		ret = stat(childPath.data(), &st);
		if (ret < 0) 
		{
			continue;
		}

		if (S_ISREG(st.st_mode))
		{
			if (st.st_size > 0)
			{
				AosFileInfo info(st);
				info.mFileName = childPath;
				file_list.push_back(info);
			}
			continue;
		}

		if (S_ISDIR(st.st_mode))
		{
			if(rec_level == -1 || rec_level >= new_level)
			{
				rslt = getFileListRec(file_list, childPath, rec_level, new_level, rdata);
				aos_assert_r(rslt, false);
			}
			continue;
		}
	}

	closedir(pDir);
	return true;
}

bool
AosNetFileMgr::asyncReadFile(
			const u64 &fileId,
			const int physicalid,
			const int64_t &seekPos,
			const u32 bytes_to_read,
			const u64 &reqid,
			const AosAioCallerPtr &caller,
			AosRundata *rdata)
{
	if (isLocal(physicalid))
	{
		return asyncReadFileLocal(fileId, seekPos, bytes_to_read, reqid, caller, rdata);
	}
	//int callerId = addCaller(caller);
	AosTransPtr trans = OmnNew AosAsyncReadFileTrans(reqid, caller, fileId, seekPos, bytes_to_read, physicalid, false, false);
	//Linda, 2013/12/14
	return AosSendTransAsyncResp(rdata, trans);
	//bool rslt = addReq(rdata, trans);
	//aos_assert_r(rslt, false);
	return true;
}

bool 
AosNetFileMgr::asyncReadFileLocal(
			const u64 &fileId,
			const int64_t &seekPos,
			const u32 bytes_to_read,
			const u64 &reqid,
			const AosAioCallerPtr &caller,
			AosRundata *rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	OmnLocalFilePtr ff = vfsMgr->openLocalFile(fileId, rdata);
	aos_assert_r(ff && ff->isGood(), false);
	AosAioRequestPtr req = OmnNew AosAioRequest(reqid, caller, ff, seekPos, bytes_to_read);
	bool rslt = AosStorageMgrAyscIo::getSelf()->addRequest(req);
	aos_assert_r(rslt, false);
	return true;
}
	
bool    
AosNetFileMgr::asyncReadFile(
			const OmnString &filename,
			const int physicalid,
			const int64_t &seekPos,
			const u32 bytes_to_read,
			const u64 &reqid,
			const AosAioCallerPtr &caller,
			AosRundata *rdata)
{
	if (isLocal(physicalid))
	{
		return asyncReadFileLocal(filename, seekPos, bytes_to_read, reqid, caller, rdata);
	}
	AosTransPtr trans = OmnNew AosAsyncReadFileByFileNameTrans(reqid, caller, filename, seekPos, bytes_to_read, physicalid, false, false);
	return AosSendTransAsyncResp(rdata, trans);
}

bool  
AosNetFileMgr::asyncReadFileLocal(
			const OmnString &filename,
			const int64_t &seekPos,
			const u32 bytes_to_read,
			const u64 &reqid,
			const AosAioCallerPtr &caller,
			AosRundata *rdata)
{
//	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
//	aos_assert_r(vfsMgr, false);
//	OmnLocalFilePtr ff = vfsMgr->openFile(fileId, rdata AosMemoryCheckerArgs);
//	aos_assert_r(ff && ff->isGood(), false);
	OmnFilePtr ff = OmnNew OmnFile(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(ff && ff->isGood(), false);
	AosAioRequestPtr req = OmnNew AosAioRequest(reqid, caller, ff, seekPos, bytes_to_read);
	bool rslt = AosStorageMgrAyscIo::getSelf()->addRequest(req);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosNetFileMgr::getFileLengthById(
			int64_t &fileLen,
			const u64 &fileId,
			const int physicalid,
			AosDiskStat &disk_stat,
			AosRundata *rdata)
{
	if (isLocal(physicalid))
	{
		return getFileLengthByIdLocal(fileLen, fileId, rdata);
	}
	AosTransPtr trans = OmnNew AosGetFileLengthByIdTrans(fileId, physicalid, false, true);
	AosBuffPtr resp;
	bool svr_death = false;
	bool rslt = addReq(rdata, trans, resp, svr_death);
	if (svr_death)
	{
		disk_stat.setServerIsDown(svr_death);
		return true;
	}
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	fileLen = resp->getInt64(-1);
	return true;
}

bool
AosNetFileMgr::getFileLengthByIdLocal(
			int64_t &fileLen,
			const u64 &fileId,
			AosRundata *rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	OmnLocalFilePtr ff = vfsMgr->openLocalFile(fileId, rdata);
	aos_assert_r(ff && ff->isGood(), false);
	fileLen = ff->getLength();
	//OmnString fileName = ff->getFileName();
	//aos_assert_r(fileName != "", false);
	//fileLen = OmnGetFileSize(fileName.data());
	return true;
}

bool
AosNetFileMgr::deleteFileById(
			const u64 &fileId,
			const int physicalid,
			bool &svr_death,
			AosRundata *rdata)
{
	svr_death = false;
	if (isLocal(physicalid))
	{
		return deleteFileByIdLocal(fileId, rdata);
	}
	AosTransPtr trans = OmnNew AosDeleteFileByIdTrans(fileId, physicalid, false, false);
	bool rslt = addReq(rdata, trans, svr_death);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosNetFileMgr::deleteFileByIdLocal(
			const u64 &fileId,
			AosRundata *rdata)
{
	u32 sec = OmnGetSecond();	
	DeleteFileReqPtr req = OmnNew DeleteFileReq(fileId, sec, rdata);
	mDeleteFileLock->lock();
	mDeleteFileQueue.push_back(req);
	mDeleteFileCondVar->signal();
	mDeleteFileLock->unlock();
	return true;
}

bool
AosNetFileMgr::deleteFileByName(
			const OmnString &fname,
			const int physicalid,
			bool &svr_death,
			AosRundata *rdata)
{
	svr_death = false;
	if (isLocal(physicalid))
	{
		OmnTagFuncInfo << "delete local file: " << fname << endl;
		return deleteFileByNameLocal(fname, rdata);
	}
	AosTransPtr trans = OmnNew AosDeleteFileByNameTrans(fname, physicalid, false, false);
	bool rslt = addReq(rdata, trans, svr_death);
	aos_assert_r(rslt, false);
	OmnTagFuncInfo << "delete remote file: " << fname << endl;
	return true;
}

bool
AosNetFileMgr::deleteFileByNameLocal(
			const OmnString &fname,
			AosRundata *rdata)
{
	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	file.deleteFile();
	return true;
}

bool
AosNetFileMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mDeleteFileLock->lock();
		if (mDeleteFileQueue.size() <= 0)
		{
			bool timeout = false;
			mDeleteFileCondVar->timedWait(mDeleteFileLock, timeout, 10);
			mDeleteFileLock->unlock();
			continue;
		}

		DeleteFileReqPtr req = mDeleteFileQueue.front();
		//Jozhi 2014-04-29 instant delete files
		//u32 oldsec = req->getStartTime();
		//u32 newsec = OmnGetSecond();
		//if (newsec - oldsec < 600)
		//{
		//	mDeleteFileLock->unlock();
		//	OmnSleep(10);
		//	continue;
		//}
		mDeleteFileQueue.pop_front();
		mDeleteFileLock->unlock();
		req->procReq();
	}
	return true;
}
#endif

