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
#include "NetFile/NetFileMgr.h"

#include "API/AosApi.h"
#include "DocFileMgr/RaidFile.h"
#include "JobTrans/AllJobTrans.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "StorageMgr/AioRequest.h"
#include "StorageMgr/StorageMgrAyscIo.h"
#include "Util/File.h"
#include "Util/ReadFile.h"
#include "Util/DiskStat.h"


OmnSingletonImpl(AosNetFileMgrSingleton,
                 AosNetFileMgr,
                 AosNetFileMgrSelf,
                 "AosNetFileMgr");


AosNetFileMgr::AosNetFileMgr()
:
mDeleteFileLock(OmnNew OmnMutex()),
mDeleteFileCondVar(OmnNew OmnCondVar())
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
AosNetFileMgr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
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
		mDeleteFileQueue.pop_front();
		mDeleteFileLock->unlock();
		req->procReq();
	}
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
	aos_assert_r(path != "", false);
	if (isLocal(physicalid))
	{
		return dirIsGoodLocal(path, checkfile, rdata);
	}

	AosTransPtr trans = OmnNew AosDirIsGoodTrans(
		path, physicalid, checkfile);
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
	aos_assert_r(path != "", false);

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
		const OmnString &filename,
		const int physicalid,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	if (isLocal(physicalid))
	{
		return fileIsGoodLocal(filename, rdata);
	}

	AosTransPtr trans = OmnNew AosFileIsGoodTrans(
		filename, physicalid);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);
	return resp->getU8(0);
}


bool
AosNetFileMgr::fileIsGoodLocal(
		const OmnString &filename,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	AosReadFilePtr file = OmnNew AosReadFile(filename);
	if (file && file->isGood())
	{
		return true;
	}
	return false;
}


bool
AosNetFileMgr::getFileLength(
		const u64 &fileId,
		const int physicalid,
		int64_t &fileLen,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(fileId, false);
	if (isLocal(physicalid))
	{
		return getFileLengthLocal(fileId, fileLen, rdata);
	}

	AosTransPtr trans = OmnNew AosGetFileLengthTrans(
		fileId, physicalid);
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
	fileLen = resp->getI64(-1);
	return true;
}

bool
AosNetFileMgr::getFileLengthLocal(
			const u64 &fileId,
			int64_t &fileLen,
			AosRundata *rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	OmnLocalFilePtr ff = vfsMgr->openLocalFile(fileId, rdata);
	aos_assert_r(ff && ff->isGood(), false);

	fileLen = ff->getLength();
	return true;
}


bool
AosNetFileMgr::getFileLength(
		const OmnString &filename,
		const int physicalid,
		int64_t &fileLen,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	if (isLocal(physicalid))
	{
		return getFileLengthLocal(filename, fileLen, rdata);
	}

	AosTransPtr trans = OmnNew AosGetFileLengthTrans(
		filename, physicalid);
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

	fileLen = resp->getI64(-1);
	return true;
}


bool
AosNetFileMgr::getFileLengthLocal(
		const OmnString &filename,
		int64_t &fileLen,
		AosRundata *rdata)
{
	fileLen = OmnFile::getFileLengthStatic(filename);
	return true;
}


bool
AosNetFileMgr::readFileToBuff(
		const u64 &fileId,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(fileId, false);
	if (isLocal(physicalid))
	{
		return readFileToBuffLocal(
			fileId, seekPos, bytes_to_read, buff, rdata);
	}

	AosTransPtr trans = OmnNew AosReadFileTrans(
		fileId, physicalid, seekPos, bytes_to_read);
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
		const u64 &fileId,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosRundata *rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	OmnLocalFilePtr file = vfsMgr->openLocalFile(fileId, rdata);
	aos_assert_r(file && file->isGood(), false);

	buff = OmnNew AosBuff(bytes_to_read, 0 AosMemoryCheckerArgs);

	file->readlock();
	u32 bytes_read = file->read(seekPos, buff->data(), bytes_to_read);
	file->unlock();

	buff->setDataLen(bytes_read);
	return true;
}


bool
AosNetFileMgr::readFileToBuff(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	if (isLocal(physicalid))
	{
		return readFileToBuffLocal(
			filename, seekPos, bytes_to_read, buff, rdata);
	}

	AosTransPtr trans = OmnNew AosReadFileTrans(
		filename, physicalid, seekPos, bytes_to_read);
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
		const OmnString &filename,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	AosReadFilePtr file = OmnNew AosReadFile(filename);
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
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	if (isLocal(physicalid))
	{
		return writeBuffToFileLocal(filename, seekPos, buff, rdata);
	}

	AosTransPtr trans = OmnNew AosWriteFileTrans(
		filename, physicalid, seekPos, buff);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileMgr::writeBuffToFileLocal(
		const OmnString &filename,
		const int64_t &seekPos,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	OmnFilePtr file = OmnNew OmnFile(filename, OmnFile::eWriteCreate AosMemoryCheckerArgs);
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
		const OmnString &filename,
		const int physicalid,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	AosTransPtr trans = OmnNew AosAppendFileTrans(
		filename, physicalid, buff);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileMgr::appendBuffToFileLocal(
		const OmnString &filename,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	OmnFilePtr file = OmnNew OmnFile(filename, OmnFile::eWriteCreate AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);

	file->lock();
	file->goToFileEnd();
	file->append(buff->data(), buff->dataLen(), true);
	file->unlock();
	return true;
}


bool
AosNetFileMgr::getDirList(
		const OmnString &path,
		const int physicalid,
		vector<AosDirInfo> &dir_list,
		AosRundata *rdata)
{
	aos_assert_r(path != "", false);
	if (isLocal(physicalid))
	{
		return getDirListLocal(path, dir_list, rdata);
	}

	AosTransPtr trans = OmnNew AosGetDirListTrans(
		path, physicalid);
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
AosNetFileMgr::getDirListLocal(
		const OmnString &path,
		vector<AosDirInfo> &dir_list,
		AosRundata *rdata)
{
	aos_assert_r(path != "", false);
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
AosNetFileMgr::getFileList(
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	// if rec_level == -1 means recurive to the end.
	aos_assert_r(path != "", false);
	if (isLocal(physicalid))
	{
		return getFileListLocal(path, rec_level, file_list, rdata);
	}

	AosTransPtr trans = OmnNew AosGetFileListTrans(
		path, physicalid, rec_level);
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
AosNetFileMgr::getFileListLocal(
		const OmnString &path,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(path != "", false);
	return getFileListRec(path, rec_level, 0, file_list, rdata);
}


bool
AosNetFileMgr::getFileListRec(
		const OmnString &path,
		const int rec_level,
		const int crt_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	DIR * pDir = opendir(path.data());
	if (!pDir)
	{
		OmnAlarm << "open dir failed: " << path << enderr;
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
				rslt = getFileListRec(childPath, rec_level, new_level, file_list, rdata);
				aos_assert_r(rslt, false);
			}
			continue;
		}
	}

	closedir(pDir);
	return true;
}


bool
AosNetFileMgr::getFileInfo(
		const OmnString &filename,
		const int physicalid,
		AosFileInfo &file_info,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	if (isLocal(physicalid))
	{
		return getFileInfoLocal(filename, physicalid, file_info, rdata);
	}

	AosTransPtr trans = OmnNew AosGetFileInfoTrans(
		filename, physicalid);
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
AosNetFileMgr::getFileInfoLocal(
		const OmnString &filename,
		const int physicalid,
		AosFileInfo &file_info,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	struct stat st;
	int ret = stat(filename.data(), &st);
	if (ret < 0) return false;

	if (st.st_size > 0)
	{
		AosFileInfo info(st);
		info.mFileName = filename;
		info.mPhysicalId = physicalid;
		file_info = info;
	}
		
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
	aos_assert_r(fileId, false);
	if (isLocal(physicalid))
	{
		return asyncReadFileLocal(
			fileId, seekPos, bytes_to_read, reqid, caller, rdata);
	}
	AosTransPtr trans = OmnNew AosAsyncReadFileTrans(
		reqid, caller, fileId, physicalid, seekPos, bytes_to_read);
	return AosSendTransAsyncResp(rdata, trans);
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
	aos_assert_r(fileId, false);
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	OmnLocalFilePtr file = vfsMgr->openLocalFile(fileId, rdata);
	aos_assert_r(file && file->isGood(), false);

	AosAioRequestPtr req = OmnNew AosAioRequest(reqid, caller, file, seekPos, bytes_to_read);
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
	aos_assert_r(filename != "", false);
	if (isLocal(physicalid))
	{
		return asyncReadFileLocal(
			filename, seekPos, bytes_to_read, reqid, caller, rdata);
	}
	AosTransPtr trans = OmnNew AosAsyncReadFileTrans(
		reqid, caller, filename, physicalid, seekPos, bytes_to_read);
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
	aos_assert_r(filename != "", false);
	OmnFilePtr file = OmnNew OmnFile(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);
	AosAioRequestPtr req = OmnNew AosAioRequest(reqid, caller, file, seekPos, bytes_to_read);
	bool rslt = AosStorageMgrAyscIo::getSelf()->addRequest(req);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileMgr::deleteFile(
		const u64 &fileId,
		const int physicalid,
		bool &svr_death,
		AosRundata *rdata)
{
	aos_assert_r(fileId, false);
	svr_death = false;
	if (isLocal(physicalid))
	{
		return deleteFileLocal(fileId, rdata);
	}
	AosTransPtr trans = OmnNew AosDeleteFileTrans(
		fileId, physicalid);
	bool rslt = addReq(rdata, trans, svr_death);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileMgr::deleteFileLocal(
		const u64 &fileId,
		AosRundata *rdata)
{
	aos_assert_r(fileId, false);
	u32 sec = OmnGetSecond();
	DeleteFileReqPtr req = OmnNew DeleteFileReq(fileId, sec, rdata);
	mDeleteFileLock->lock();
	mDeleteFileQueue.push_back(req);
	mDeleteFileCondVar->signal();
	mDeleteFileLock->unlock();
	return true;
}


bool
AosNetFileMgr::deleteFile(
		const OmnString &filename,
		const int physicalid,
		bool &svr_death,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	svr_death = false;
	if (isLocal(physicalid))
	{
		return deleteFileLocal(filename, rdata);
	}
	AosTransPtr trans = OmnNew AosDeleteFileTrans(
		filename, physicalid);
	bool rslt = addReq(rdata, trans, svr_death);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileMgr::deleteFileLocal(
		const OmnString &filename,
		AosRundata *rdata)
{
	OmnFile file(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
	file.deleteFile();
	return true;
}


bool
AosNetFileMgr::getFileListByAssignExt(
		const OmnString &ext,
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(ext != "" && path != "", false);
	if (isLocal(physicalid))
	{
		return getFileListByAssignExtLocal(
			ext, path, rec_level, file_list, rdata);
	}

	AosTransPtr trans = OmnNew AosGetFileListByAssignExtTrans(
		ext, path, physicalid, rec_level);
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
		const OmnString &ext,
		const OmnString &path,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(ext != "" && path != "", false);
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
				rslt = getFileListByAssignExtLocal(
					ext, childPath, rec_level, file_list, rdata);
				aos_assert_r(rslt, false);
			}
			continue;
		}
	}

	closedir(pDir);
	return true;
}


OmnLocalFilePtr
AosNetFileMgr::createRaidFile(
		AosRundata *rdata,
		u64 &fileId,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	u32 vid = 0;
	OmnLocalFilePtr file = vfsMgr->createRaidFile(
		rdata, vid, fileId, fname_prefix, requested_space, reserve_flag);
	aos_assert_r(file && file->isGood(), 0);

	return file;
}


OmnFilePtr
AosNetFileMgr::createTempFile(
		AosRundata *rdata,
		u64 &fileId,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	OmnFilePtr file = vfsMgr->createNewTempFile(
		rdata, fileId, fname_prefix, requested_space, reserve_flag);
	aos_assert_r(file && file->isGood(), 0);

	return file;
}


OmnLocalFilePtr
AosNetFileMgr::openLocalFile(
		const u64 &fileId,
		AosRundata *rdata)
{
	aos_assert_r(fileId, 0);
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	OmnLocalFilePtr file = vfsMgr->openLocalFile(fileId, rdata);
	aos_assert_r(file && file->isGood(), 0);

	return file;
}

