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
// 2014/08/06	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "NetFile/NetFileClt.h"

#include "API/AosApi.h"
#include "DocFileMgr/RaidFile.h"
#include "JobTrans/AllJobTrans.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "StorageMgr/AioRequest.h"
#include "StorageMgr/StorageMgrAyscIo.h"
#include "Util/File.h"
#include "Util/DiskStat.h"


OmnSingletonImpl(AosNetFileCltSingleton,
                 AosNetFileClt,
                 AosNetFileCltSelf,
                 "AosNetFileClt");


AosNetFileClt::AosNetFileClt()
{
}


AosNetFileClt::~AosNetFileClt()
{
}


bool
AosNetFileClt::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosNetFileClt::start()
{
	return true;
}


bool
AosNetFileClt::stop()
{
	return true;
}


bool
AosNetFileClt::addReq(
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
AosNetFileClt::addReq(
		AosRundata *rdata,
		const AosTransPtr &trans)
{
	bool rslt = AosSendTrans(rdata, trans);
	aos_assert_r(rslt, false);
	rdata->setOk();
	return true;
}

bool
AosNetFileClt::addReq(
		AosRundata *rdata,
		const AosTransPtr &trans,
		AosBuffPtr &resp,
		bool &svr_death)
{
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
AosNetFileClt::addReq(
		AosRundata *rdata,
		const AosTransPtr &trans,
		AosBuffPtr &resp)
{
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
AosNetFileClt::dirIsGood(
		const OmnString &path,
		const int physicalid,
		const bool checkfile,
		AosRundata *rdata)
{
	aos_assert_r(path != "", false);
	AosTransPtr trans = OmnNew AosDirIsGoodTrans(
		path, physicalid, checkfile);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);
	return resp->getU8(0);
}


bool
AosNetFileClt::fileIsGood(
		const OmnString &filename,
		const int physicalid,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	AosTransPtr trans = OmnNew AosFileIsGoodTrans(
		filename, physicalid);
	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, false);
	return resp->getU8(0);
}


bool
AosNetFileClt::getFileLength(
		const u64 &fileId,
		const int physicalid,
		int64_t &fileLen,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(fileId, false);
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
AosNetFileClt::getFileLength(
		const OmnString &filename,
		const int physicalid,
		int64_t &fileLen,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
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
AosNetFileClt::readFileToBuff(
		const u64 &fileId,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(fileId, false);
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
AosNetFileClt::readFileToBuff(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		AosBuffPtr &buff,
		AosDiskStat &disk_stat,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
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
AosNetFileClt::writeBuffToFile(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	AosTransPtr trans = OmnNew AosWriteFileTrans(
		filename, physicalid, seekPos, buff);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileClt::appendBuffToFile(
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
AosNetFileClt::getDirList(
		const OmnString &path,
		const int physicalid,
		vector<AosDirInfo> &dir_list,
		AosRundata *rdata)
{
	aos_assert_r(path != "", false);
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
AosNetFileClt::getFileList(
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	// if rec_level == -1 means recurive to the end.
	aos_assert_r(path != "", false);
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
AosNetFileClt::getFileInfo(
		const OmnString &filename,
		const int physicalid,
		AosFileInfo &file_info,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
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
AosNetFileClt::asyncReadFile(
		const u64 &fileId,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		AosRundata *rdata)
{
	aos_assert_r(fileId, false);
	AosTransPtr trans = OmnNew AosAsyncReadFileTrans(
		reqid, caller, fileId, physicalid, seekPos, bytes_to_read);
	return AosSendTransAsyncResp(rdata, trans);
}


bool
AosNetFileClt::asyncReadFile(
		const OmnString &filename,
		const int physicalid,
		const int64_t &seekPos,
		const u32 bytes_to_read,
		const u64 &reqid,
		const AosAioCallerPtr &caller,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	AosTransPtr trans = OmnNew AosAsyncReadFileTrans(
		reqid, caller, filename, physicalid, seekPos, bytes_to_read);
	return AosSendTransAsyncResp(rdata, trans);
}


bool
AosNetFileClt::deleteFile(
		const u64 &fileId,
		const int physicalid,
		bool &svr_death,
		AosRundata *rdata)
{
	aos_assert_r(fileId, false);
	svr_death = false;
	AosTransPtr trans = OmnNew AosDeleteFileTrans(
		fileId, physicalid);
	bool rslt = addReq(rdata, trans, svr_death);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileClt::deleteFile(
		const OmnString &filename,
		const int physicalid,
		bool &svr_death,
		AosRundata *rdata)
{
	aos_assert_r(filename != "", false);
	svr_death = false;
	AosTransPtr trans = OmnNew AosDeleteFileTrans(
		filename, physicalid);
	bool rslt = addReq(rdata, trans, svr_death);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetFileClt::getFileListByAssignExt(
		const OmnString &ext,
		const OmnString &path,
		const int physicalid,
		const int rec_level,
		vector<AosFileInfo> &file_list,
		AosRundata *rdata)
{
	aos_assert_r(ext != "" && path != "", false);
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


OmnLocalFilePtr
AosNetFileClt::createRaidFile(
		AosRundata *rdata,
		u64 &fileId,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
{
	AosTransPtr trans = OmnNew AosCreateRaidFileTrans(
		AosGetSelfServerId(), fname_prefix,
		requested_space, reserve_flag);

	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, 0);

	fileId = resp->getU64(0);
	aos_assert_r(fileId, 0);

	OmnLocalFilePtr file = OmnNew AosRaidFile(rdata, resp);
	aos_assert_r(file->isGood(), 0);

	return file;
}


OmnFilePtr
AosNetFileClt::createTempFile(
		AosRundata *rdata,
		u64 &fileId,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
{
	AosTransPtr trans = OmnNew AosCreateTempFileTrans(
		AosGetSelfServerId(), fname_prefix,
		requested_space, reserve_flag);

	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, 0);

	fileId = resp->getU64(0);
	aos_assert_r(fileId, 0);

	OmnString filename = resp->getOmnStr("");
	aos_assert_r(filename != "", 0);

	OmnFilePtr file = OmnNew OmnFile(filename, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), 0);

	return file;
}


OmnLocalFilePtr
AosNetFileClt::openLocalFile(
		const u64 &fileId,
		AosRundata *rdata)
{
	aos_assert_r(fileId, 0);
	AosTransPtr trans = OmnNew AosGetLocalFileInfoTrans(
		AosGetSelfServerId(), fileId);

	AosBuffPtr resp;
	bool rslt = addReq(rdata, trans, resp);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, 0);

	OmnLocalFilePtr file;	
	if (AosIsRaidFile(fileId))
	{
		file = OmnNew AosRaidFile(rdata, resp);
		aos_assert_r(file->isGood(), 0);
	}
	else
	{
		OmnString filename = resp->getOmnStr("");
		aos_assert_r(filename != "", 0);
		
		file = OmnNew OmnFile(filename, OmnFile::eReadWrite AosMemoryCheckerArgs);
		aos_assert_r(file->isGood(), 0);
	}

	return file;
}

