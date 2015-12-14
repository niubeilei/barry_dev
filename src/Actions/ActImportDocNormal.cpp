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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2013/04/26	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActImportDocNormal.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DataAssembler/DataAssemblerType.h"
#include "Rundata/Rundata.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/NetFileObj.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "NetFile/NetFile.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("ActImprotDocNormal", __FILE__, __LINE__);


AosActImportDocNormal::AosActImportDocNormal(const bool flag)
:
AosSdocAction(AOSACTTYPE_IMPORTDOC_NORMAL, AosActionType::eImportDocNormal, flag),
mScanner(0),
mRundata(0),
mThread(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mSem(OmnNew OmnSem(0)),
mTotalReqs(0),
mExitThrd(false)
{
}


AosActImportDocNormal::~AosActImportDocNormal()
{
	//OmnScreen << "act import doc deleted" << endl;
}


bool
AosActImportDocNormal::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!mThread)
	{
		OmnThreadedObjPtr thisptr(this, false); 
		mThread = OmnNew OmnThread(thisptr, "importdocnormal", 0, false, true, __FILE__, __LINE__);
		mThread->start(); 
	}
	
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if(!task)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mTask = task;
	mScanner = AosDataScannerObj::createMultiFileScannerStatic(rdata);
	aos_assert_r(mScanner, false);

	AosXmlTagPtr filestag = sdoc->getFirstChild("files");
	aos_assert_r(filestag, false);

	vector<AosNetFileObjPtr> files;
	AosXmlTagPtr entry = filestag->getFirstChild(true);
	while (entry)
	{
		int data_serverid = entry->getAttrInt(AOSTAG_PHYSICALID, -1);
		OmnString key = entry->getAttrStr(AOSTAG_DATACOLLECTOR_ID, "");

		u64 file_id = entry->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
		AosNetFileObjPtr file = OmnNew AosIdNetFile(data_serverid, file_id);
		u64 reqId = files.size();
		files.push_back(file);
		aos_assert_r(key == AOSDATAASSEMBLER_DOC_VAR, false);

		Info info(file_id, data_serverid);
		mInfo[reqId] = info;
		entry = filestag->getNextChild();
	}

	if (mInfo.size() == 0 || files.size() == 0)
	{
		OmnAlarm << "mInfo size is 0" << enderr;
	}

	mScanner->setFiles(files);

	AosActionCallerPtr thisptr(this, true);
	mScanner->setActionsCaller(thisptr);

	for (u64 i = 0; i < files.size(); i++)
	{
		mLock->lock();
		Req req = {i, eMaxBlockSize};
		mQueue.push(req);
		mCondVar->signal();
		mLock->unlock();
	}
	return true;
}


void
AosActImportDocNormal::callBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished)
{
	mLock->lock();
	if (mScanner->getDiskError())
	{
		mTask->setErrorType(AosTaskErrorType::eInPutError);
		actionFailed(mRundata);
		mLock->unlock();
		return;
	}

	if(finished) mInfo[reqId].finished = finished;
	Info o = mInfo[reqId];

	const AosSengineImportDocObjPtr thisptr(this, true);
	AosStorageEngineMgr::getSelf()->saveDoc(
			AosDataAssemblerType::eDocVar, o.fileid, o.fileid, -1, 
			thisptr, reqId, expected_size, mSnapMaps, mTaskDocid, mRundata);	
	mTotalReqs++;
	
	if (!finished) 
	{
		mLock->unlock();
		return;
	}
	
	bool all_finished = readFileFinishedLocked();
	mLock->unlock();

	if (!all_finished) return;

	OmnThrdShellProcPtr req = OmnNew FinishAllReq(thisptr);
	sgThreadPool->proc(req);
}

	
void
AosActImportDocNormal::allFinished()
{
	for (int i = 0; i < mTotalReqs; i++)
	{
		mSem->wait();
	}

	mExitThrd = true;

	mLock->lock();
	mCondVar->signal();
	mLock->unlock();

	mThread = 0;
	actionFinished(mRundata);
	mTask = 0;
	deleteFile();

	aos_assert(mScanner);
	mScanner->destroyedMember();
	mScanner = 0;
}


bool
AosActImportDocNormal::readFileFinishedLocked()
{
	if (mQueue.size() != 0) return false;

	map<u64, Info>::iterator iter;
	for (iter = mInfo.begin(); iter != mInfo.end(); iter ++)
	{
		Info o = (*iter).second;
		if (!o.finished) 
		{
			return false;
		}
	}
	return true;	
}


void
AosActImportDocNormal::deleteFile()
{
	return;
	map<u64, Info>::iterator iter;

	bool svr_death;
	for (iter = mInfo.begin(); iter != mInfo.end(); iter++)
	{
		Info o = (*iter).second;
		AosNetFileCltObj::deleteFileStatic(
			o.fileid, o.serverid, svr_death, mRundata.getPtr());
	}
}


AosActionObjPtr
AosActImportDocNormal::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActImportDocNormal(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActImportDocNormal::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mExitThrd)
		{
			state = OmnThrdStatus::eExit;
			mLock->unlock();
			return true;
		}

		if (mQueue.empty())
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		
		Req req = mQueue.front();
		mQueue.pop();
		mLock->unlock();

		mScanner->addRequest(req.reqId, eMaxBlockSize);
	}
	return true;
}


bool
AosActImportDocNormal::signal(const int threadLogicId)
{
	return true;
}


bool
AosActImportDocNormal::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


AosBuffPtr
AosActImportDocNormal::getNextBlock(
		const u64 &reqId,
		const int64_t &expected_size)
{
	mLock->lock();
	Info o = mInfo[reqId];
	mLock->unlock();

	AosBuffPtr buff = mScanner->getNextBlock(reqId, expected_size);
	aos_assert_r(buff, 0);

	if (!o.finished) 
	{
		mLock->lock();
		Req req = {reqId, eMaxBlockSize};
		mQueue.push(req);
		mCondVar->signal();
		mLock->unlock();
	}

	return buff;
}


void
AosActImportDocNormal::semPost()
{
	aos_assert(mSem);
	mSem->post();
}

//bool 	
//AosActImportDocNormal::actionFailed(const AosRundataPtr &rdata)
//{
//	return actionFailed(mRundata);
//}

bool
AosActImportDocNormal::storageEngineError(const AosRundataPtr &rdata)
{
	if (mTask)
	{
		mTask->setErrorType(AosTaskErrorType::eStorageError);
	}
	bool rslt = actionFailed(rdata);
	return rslt;
}


bool
AosActImportDocNormal::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
{
	vector<AosTaskDataObjPtr> snapshots;
	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_r(doc_client, false);
	aos_assert_r(task, false);
	mTaskDocid = task->getTaskDocid();
	AosTaskDataObjPtr snapshot;
	vector<u32> virtualids = AosGetTotalCubeIds();
	for (u32 i = 0; i < virtualids.size(); i++)
	{
		//may be not rollback here
		OmnString key;
		key << virtualids[i] << "__"
			<< AosDocType::eNormalDoc << "__"
			<< mTaskDocid;
		snapshot = task->getOutPut(AosTaskDataType::eDocSnapShot, key, rdata);
		u64 snapshot_id = 0;
		aos_assert_r(!snapshot, false);
		snapshot_id = doc_client->createSnapshot(rdata, virtualids[i], snapshot_id, 
				AosDocType::eNormalDoc, mTaskDocid);
		snapshot = AosTaskDataObj::createTaskDataDocSnapShot(virtualids[i], snapshot_id, AosDocType::eNormalDoc, mTaskDocid, false);
		aos_assert_r(snapshot, false);
OmnScreen << "importdoc,vid " << virtualids[i] << " . snapshot_id: " << snapshot_id << endl;
		mSnapMaps[virtualids[i]] = snapshot_id;
		snapshots.push_back(snapshot);
	}
	return task->updateTaskSnapShots(snapshots, rdata);
}
