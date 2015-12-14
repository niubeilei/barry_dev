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
// 2013/04/13	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_NetFile_NetFileMgr_h
#define AOS_NetFile_NetFileMgr_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "TransBasic/Trans.h"


OmnDefineSingletonClass(AosNetFileMgrSingleton,
                        AosNetFileMgr,
                        AosNetFileMgrSelf,
                        OmnSingletonObjId::eNetFileMgr,
                        "NetFileMgr");

struct DeleteFileReq : public OmnRCObject
{
	OmnDefineRCObject;
	
	u64 			mFileId;
	u32 			mSec;
	AosRundataPtr 	mRundata;

public:
	DeleteFileReq(
		const u64  fileId,
		const u32  second,
		AosRundata *rdata)
	:
	mFileId(fileId),
	mSec(second),
	mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
	{
	}

	~DeleteFileReq(){}

	bool procReq()
	{
		AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
		aos_assert_r(vfsMgr, false);
		return vfsMgr->removeFile(mFileId, mRundata.getPtr());
	}

	u32 getStartTime(){return mSec;}
};


class AosNetFileMgr : virtual public OmnThreadedObj, public AosNetFileMgrObj
{
	OmnDefineRCObject;

	OmnMutexPtr					mDeleteFileLock;
	OmnCondVarPtr				mDeleteFileCondVar;
	OmnThreadPtr				mThread;
	deque<DeleteFileReqPtr>     mDeleteFileQueue;

public:
	AosNetFileMgr();
	~AosNetFileMgr();

    // Singleton class interface
    static AosNetFileMgr*	getSelf();
    virtual bool			start();
    virtual bool			stop();
	virtual bool			config(const AosXmlTagPtr &config);
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId) {return true;}
	virtual void    heartbeat(const int tid) {}
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) {return true;}


private:
	bool			addReq(
						AosRundata *rdata,
						const AosTransPtr &trans,
						bool &svr_death);

	bool			addReq(
						AosRundata *rdata,
						const AosTransPtr &trans);

	bool			addReq(
						AosRundata *rdata,
						const AosTransPtr &trans,
						AosBuffPtr &resp);

	bool			addReq(
						AosRundata *rdata,
						const AosTransPtr &trans,
						AosBuffPtr &resp,
						bool &svr_death);

	bool			getFileListRec(
						const OmnString &path,
						const int rec_level,
						const int crt_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);

public:
	virtual bool	isLocal(const int physicalid);

	virtual bool	dirIsGood(
						const OmnString &path,
						const int physicalid,
						const bool checkfile,
						AosRundata *rdata);
	virtual bool	dirIsGoodLocal(
						const OmnString &path,
						const bool checkfile,
						AosRundata *rdata);

	virtual bool	fileIsGood(
						const OmnString &filename,
						const int physicalid,
						AosRundata *rdata);
	virtual bool	fileIsGoodLocal(
						const OmnString &filename,
						AosRundata *rdata);

	virtual bool	getFileLength(
						const u64 &fileId,
						const int physicalid,
						int64_t &fileLen,
						AosDiskStat &disk_stat,
						AosRundata *rdata);
	virtual bool 	getFileLengthLocal(
						const u64 &fileId,
						int64_t &fileLen,
						AosRundata *rdata);

	virtual bool	getFileLength(
						const OmnString &filename,
						const int physicalid,
						int64_t &fileLen,
						AosDiskStat &disk_stat,
						AosRundata *rdata);
	virtual bool	getFileLengthLocal(
						const OmnString &filename,
						int64_t &fileLen,
						AosRundata *rdata);

	virtual bool	readFileToBuff(
						const u64 &fileId,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosDiskStat &disk_stat,
						AosRundata *rdata);
	virtual bool	readFileToBuffLocal(
						const u64 &fileId,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosRundata *rdata);

	virtual bool	readFileToBuff(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosDiskStat &disk_stat,
						AosRundata *rdata);
	virtual bool	readFileToBuffLocal(
						const OmnString &filename,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosRundata *rdata);

	virtual bool	writeBuffToFile(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const AosBuffPtr &buff,
						AosRundata *rdata);
	virtual bool	writeBuffToFileLocal(
						const OmnString &filename,
						const int64_t &seekPos,
						const AosBuffPtr &buff,
						AosRundata *rdata);

	virtual bool	appendBuffToFile(
						const OmnString &filename,
						const int physicalid,
						const AosBuffPtr &buff,
						AosRundata *rdata);
	virtual bool	appendBuffToFileLocal(
						const OmnString &filename,
						const AosBuffPtr &buff,
						AosRundata *rdata);

	virtual bool	getDirList(
						const OmnString &path,
						const int physicalid,
						vector<AosDirInfo> &dir_list,
						AosRundata *rdata);
	virtual bool	getDirListLocal(
						const OmnString &path,
						vector<AosDirInfo> &dir_list,
						AosRundata *rdata);

	virtual bool	getFileList(
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);
	virtual bool	getFileListLocal(
						const OmnString &path,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);

	virtual bool	getFileInfo(
						const OmnString &filename,
						const int physicalid,
						AosFileInfo &file_info,
						AosRundata *rdata);
	virtual bool	getFileInfoLocal(
						const OmnString &filename,
						const int physicalid,
						AosFileInfo &file_info,
						AosRundata *rdata);

	virtual bool    asyncReadFile(
						const u64 &fileId,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);
	virtual bool    asyncReadFileLocal(
						const u64 &fileId,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);

	virtual bool    asyncReadFile(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);
	virtual bool    asyncReadFileLocal(
						const OmnString &filename,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);

	virtual bool	deleteFile(
						const u64 &fileId,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata);
	virtual bool	deleteFileLocal(
						const u64 &fileId,
						AosRundata *rdata);

	virtual bool	deleteFile(
						const OmnString &fname,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata);
	virtual bool	deleteFileLocal(
						const OmnString &fname,
						AosRundata *rdata);

	virtual bool 	getFileListByAssignExt(
						const OmnString &ext,
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);
	virtual bool 	getFileListByAssignExtLocal(
						const OmnString &ext,
						const OmnString &path,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);

	virtual OmnLocalFilePtr createRaidFile(
						AosRundata *rdata,
						u64 &fileId,
						const OmnString &fname_prefix,
						const u64 &requested_space,
						const bool reserve_flag);

	virtual OmnFilePtr createTempFile(
						AosRundata *rdata,
						u64 &fileId,
						const OmnString &fname_prefix,
						const u64 &requested_space,
						const bool reserve_flag);

	virtual OmnLocalFilePtr openLocalFile(
						const u64 &fileId,
						AosRundata *rdata);

};

#endif

