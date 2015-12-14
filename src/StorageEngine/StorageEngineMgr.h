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
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageEngine_StorageEngineMgr_h
#define AOS_StorageEngine_StorageEngineMgr_h

#include "alarm_c/alarm.h"
#include "Actions/Ptrs.h"
#include "DataAssembler/DataAssemblerType.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEInterfaces/Ptrs.h"
#include "StorageEngine/Ptrs.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Sem.h"

#include <queue>

OmnDefineSingletonClass(AosStorageEngineMgrSingleton,
						AosStorageEngineMgr,
						AosStorageEngineMgrSelf,
						OmnSingletonObjId::eStorageEngineMgr,
						"StorageEngineMgr");

class AosStorageEngineMgr : public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eMaxThrds = 10,
		eDocidLength = sizeof(u64) + sizeof(int), //docid + length	
		//eMaxBuffSize = 12000000 // 12M
		eMaxBuffSize = 40000000 // 40M
	};

private:
	OmnThreadPtr								mThread[eMaxThrds];
	OmnMutexPtr     							mLock[eMaxThrds];
	OmnCondVarPtr   							mCondVar[eMaxThrds];
	queue<AosSengineImportDocRequestPtr>		mProcData[eMaxThrds];
	u32 										mNumThreads;
	AosRundataPtr								mRundata[eMaxThrds];
	bool										mFlush[eMaxThrds];
	OmnMutexPtr     							mTLock;
	// fileid, AosBuffPtr
	map<u64, AosBuffPtr>						mCacheData[eMaxThrds];

public:

	AosStorageEngineMgr();
	~AosStorageEngineMgr();

public:
    // Singleton class interface
    static AosStorageEngineMgr* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);

	bool    checkThread(OmnString &err, const int thrdLogicId) const;
	
	bool	saveDoc(
				const AosDataAssemblerType::E doc_ass_type,
				const u32 sizeid, 
				const u64 &fileid,
				const int record_len,
				const AosSengineImportDocObjPtr &caller,
				const u64 &reqId,
				const int64_t &size,
				map<int, u64> &snap_ids,
				const u64 &task_docid,
				const AosRundataPtr &rdata);

	void 	flushContents();

	bool	sanityCheck(const u64 &fileid);

	//
	//we have reqirements to save csv data from buff directly
	//Jozhi 2015/01/23
	//bool	saveCSVDoc(
	//		const u32 sizeid,
	//		const int record_len,
	//		const AosBuffPtr &buff, 
	//		map<int, u64> &snap_ids,
	//		const u64 &task_docid,
	//		const AosRundataPtr &rdata);

	bool	batchInsertDoc(
			const AosBuffPtr &buff, 
			map<int, u64> &snap_ids,
			const u64 &task_docid,
			const AosRundataPtr &rdata);

	bool	batchInsertDocCommon(
			const AosBuffPtr &buff,
			map<int, u64> &snap_ids,
			const u64 &task_docid,
			const AosRundataPtr &rdata);

	void	printChars(const AosBuffPtr &buff);

	bool	insertDoc(
			const AosBuffPtr &buff,
			const u64 snap_id,
			const AosRundataPtr &rdata);

	bool	batchDeleteDoc(
			const AosBuffPtr &buff, 
			const u64 &task_docid,
			const AosRundataPtr &rdata);

	//Jozhi 2015/01/23
	/*
	bool	deleteCSVDoc(
			const AosBuffPtr &buff, 
			const u64 &task_docid,
			const AosRundataPtr &rdata);
	*/

	bool	updateDoc(
			const AosBuffPtr &buff, 
			map<int, u64> &snap_ids,
			const u64 &task_docid,
			const AosRundataPtr &rdata);

	bool	updateCSVDoc(
			const u32 sizeid,
			const int record_len,
			const AosBuffPtr &buff, 
			map<int, u64> &snap_ids,
			const u64 &task_docid,
			const AosRundataPtr &rdata);
private:
	bool readyData(
			const u32 logicid,
			const AosBuffPtr &buff,
			const AosSengineImportDocRequestPtr &ptr,
			const AosRundataPtr &rdata);

	void startThread(const AosRundataPtr &rdata);

	//Jozhi 2015/01/23
	//bool	saveFixedLengthDoc(
	//		const u32 sizeid,
	//		const int record_len,
	//		const AosBuffPtr &buff, 
	//		map<int, u64> &snap_ids,
	//		const u64 &task_docid,
	//		const AosRundataPtr &rdata);

	bool	saveNormalDoc(
			const u32 logicid,
			const AosBuffPtr &buff,
			const u64 &fileid,
			map<int, u64> &snap_ids,
			const u64 &task_docid,
			const AosRundataPtr &rdata);

	bool	procCacheBuff(
			const u32 logicid,
			const u64 &fileid,
			const AosBuffPtr &buff,
			map<int, u64> &snap_ids,
			const u64 &task_docid,
			const AosRundataPtr &rdata);

	bool	sanityCheck2(
			const u64 &docid,
			const int64_t record_size,
			const char *doc);



};

#endif

