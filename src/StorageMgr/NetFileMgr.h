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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 2013/04/13	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_StorageMgr_NetFileMgr_h
#define AOS_StorageMgr_NetFileMgr_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "SEInterfaces/AioCaller.h"
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
		const u64  file_id,
		const u32  second,
		AosRundata *rdata)
	:
	mFileId(file_id),
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
private:
	OmnMutexPtr								mLock;
	OmnMutexPtr								mDeleteFileLock;
	OmnCondVarPtr							mDeleteFileCondVar;
	OmnThreadPtr							mThread;
	deque<DeleteFileReqPtr>              	mDeleteFileQueue;
	//int										mCallerId;
	//map<int, AosAioCallerPtr>				mCallerMap;

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
	bool	addReq(
				AosRundata *rdata,
				const AosTransPtr &trans,
				bool &svr_death);

	bool	addReq(
				AosRundata *rdata,
				const AosTransPtr &trans);

	bool	addReq(
				AosRundata *rdata,
				const AosTransPtr &trans,
				AosBuffPtr &resp);

	bool	addReq(
				AosRundata *rdata,
				const AosTransPtr &trans,
				AosBuffPtr &resp,
				bool &svr_death);


	bool	getFileListRec(
				vector<AosFileInfo> &file_list,
				const OmnString &path,
				const int rec_level,
				const int crt_level,
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
						const OmnString &fname,
						const int physicalid,
						AosRundata *rdata);

	virtual bool	fileIsGoodLocal(
						const OmnString &fname,
						AosRundata *rdata);

	virtual bool	getFileLength(
						int64_t &filelen, 
						const OmnString &fname, 
						const int physicalid, 
						AosDiskStat &disk_stat,
						AosRundata *rdata);

	virtual bool	getFileLengthLocal(
						int64_t &filelen,
						const OmnString &fname,
						AosRundata *rdata);

	virtual bool	readFileToBuff(
						AosBuffPtr &buff,
						const u64 &file_id,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosDiskStat &disk_stat,
						AosRundata *rdata);

	virtual bool	readFileToBuffLocal(
						AosBuffPtr &buff,
						const u64 &file_id,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosRundata *rdata);
	
	virtual bool	readFileToBuff(
						AosBuffPtr &buff,
						const OmnString &fname,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosDiskStat &disk_stat,
						AosRundata *rdata);

	virtual bool	readFileToBuffLocal(
						AosBuffPtr &buff,
						const OmnString &fname,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosRundata *rdata);
	
	virtual bool	writeBuffToFile(
						const AosBuffPtr &buff,
						const OmnString &fname,
						const int physicalid,
						const int64_t &seekPos,
						AosRundata *rdata);

	virtual bool	writeBuffToFileLocal(
						const AosBuffPtr &buff,
						const OmnString &fname,
						const int64_t &seekPos,
						AosRundata *rdata);

	virtual bool	appendBuffToFile(
						const AosBuffPtr &buff,
						const OmnString &fname,
						const int physicalid,
						AosRundata *rdata);

	virtual bool	appendBuffToFileLocal(
						const AosBuffPtr &buff,
						const OmnString &fname,
						AosRundata *rdata);

	virtual bool 	getFileListByAssignExt(
						vector<AosFileInfo> &file_list,
						const OmnString &ext,
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						AosRundata *rdata);

	virtual bool 	getFileListByAssignExtLocal(
						vector<AosFileInfo> &file_list,
						const OmnString &ext,
						const OmnString &path,
						const int rec_level,
						AosRundata *rdata);

	virtual bool	getDirList(
						vector<AosDirInfo> &dir_list,
						const OmnString &path,
						const int physicalid,
						AosRundata *rdata);

	virtual bool	getFileList(
						vector<AosFileInfo> &file_list,
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						AosRundata *rdata);

	virtual bool	getFileInfo(
						AosFileInfo &file_info,
						const OmnString &file_name,
						const int physicalid,
						AosRundata *rdata);

	virtual bool	getDirListLocal(
						vector<AosDirInfo> &dir_list,
						const OmnString &path,
						AosRundata *rdata);

	virtual bool	getFileListLocal(
						vector<AosFileInfo> &file_list,
						const OmnString &path,
						const int rec_level,
						AosRundata *rdata);

	virtual bool	getFileInfoLocal(
						AosFileInfo &file_info,
						const OmnString &file_name,
						const int physicalid,
						AosRundata *rdata);

	virtual bool 	getFileLengthById(
						int64_t &fileLen,
						const u64 &fileId,
						const int physicalid,
						AosDiskStat &disk_stat,
						AosRundata *rdata);

	virtual bool 	getFileLengthByIdLocal(
						int64_t &fileLen,
						const u64 &fileId,
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

	virtual bool	deleteFileById(
						const u64 &fileId,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata);

	virtual bool	deleteFileByIdLocal(
						const u64 &fileId,
						AosRundata *rdata);
	
	virtual bool	deleteFileByName(
						const OmnString &fname,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata);

	virtual bool	deleteFileByNameLocal(
						const OmnString &fname,
						AosRundata *rdata);

	//bool			getCaller(
	//					AosAioCallerPtr &caller,
	//					const int callerId,
	//					AosRundata *rdata)
	//{
	//	mLock->lock();
	//	map<int, AosAioCallerPtr>::iterator itr = mCallerMap.find(callerId);
	//	if (itr == mCallerMap.end())
	//	{
	//		caller = 0;
	//		mLock->unlock();
	//		return true;
	//	}
	//	caller = itr->second;
	//	mCallerMap.erase(itr);
	//	mLock->unlock();
	//	return true;
	//}
	//int				addCaller(const AosAioCallerPtr &caller)
	//{
	//	int id = 0;
	//	mLock->lock();
	//	id = mCallerId++;
	//	mCallerMap[id] = caller;
	//	mLock->unlock();
	//	return id;
	//}
};
#endif
#endif
