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
#include "Actions/ActImportDocFixed.h"

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

static bool	sgSanityCheck = true;

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("ActImprotDocFixed", __FILE__, __LINE__);


AosActImportDocFixed::AosActImportDocFixed(const bool flag)
:
AosSdocAction(AOSACTTYPE_IMPORTDOC_FIXED, AosActionType::eImportDocFixed, flag),
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


AosActImportDocFixed::~AosActImportDocFixed()
{
	//OmnScreen << "act import doc deleted" << endl;
}


bool
AosActImportDocFixed::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!mThread)
	{
		OmnThreadedObjPtr thisptr(this, false); 
		mThread = OmnNew OmnThread(thisptr, "importdoc", 0, false, true, __FILE__, __LINE__);
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

		u32 sizeid = entry->getAttrU32(AOSTAG_SIZEID, 0);
		int record_size = entry->getAttrInt(AOSTAG_RECORD_LEN, -1);

		u64 file_id = entry->getAttrU64(AOSTAG_STORAGE_FILEID, 0);

		AosNetFileObjPtr file = OmnNew AosIdNetFile(data_serverid, file_id);
		u64 reqId = files.size();
		files.push_back(file);

		int64_t blocksize = 0;
		aos_assert_r(key != AOSDATAASSEMBLER_DOC_VAR, false);
		//AosDataAssemblerType::E ass_type;
		//if (record_size == -1 && sizeid == (u32)-1  && key == AOSDATAASSEMBLER_DOC_VAR)
		//{
		//	OmnAlarm << "xxxxxxxxxxxxxx" << endl;
		//	// doc var 
		//	ass_type = AosDataAssemblerType::eDocVar;
		//	blocksize = eMaxBlockSize;
		//	sizeid = (u32)file_id; 
		//}
		//else
		//{
		// doc fixed length
		AosDataAssemblerType::E ass_type = AosDataAssemblerType::eDocNorm;
		aos_assert_r(record_size > 0 && sizeid > 0, false);
		blocksize = eMaxBlockSize - (eMaxBlockSize % (record_size + sizeof(u64) + sizeof(int)));
		//}

		Info info(ass_type, sizeid, record_size, key, blocksize, file_id, data_serverid);
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
		Req req = {i, mInfo[i].blocksize};
		mQueue.push(req);
		mCondVar->signal();
		mLock->unlock();
	}
	return true;
}


void
AosActImportDocFixed::callBack(
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
		o.docAssType, o.sizeid, o.fileid, o.record_size, 
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
AosActImportDocFixed::allFinished()
{
OmnScreen << "jozhi allFinished" << this << endl;
	for (int i = 0; i < mTotalReqs; i++)
	{
OmnScreen << "jozhi wait, i: " << i << " , mTotalReqs: " << mTotalReqs << ", " << this << endl;
		mSem->wait();
	}

	sanityCheck();

	mExitThrd = true;

	mLock->lock();
	mCondVar->signal();
	mLock->unlock();

	mThread = 0;
	actionFinished(mRundata);
	deleteFile();
	mTask = 0;
	aos_assert(mScanner);
OmnScreen << "jozhi destroyedMember, start" << this << endl;
	mScanner->destroyedMember();
OmnScreen << "jozhi destroyedMember, end" << this << endl;
	mScanner = 0;
}


bool
AosActImportDocFixed::readFileFinishedLocked()
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
AosActImportDocFixed::deleteFile()
{
	//Jozhi 2014-04-28 is delete file
	if (!mTask || !mTask->getIsDeleteFile())
	{
		return;
	}

	bool svr_death;
	map<u64, Info>::iterator iter;
	for (iter = mInfo.begin(); iter != mInfo.end(); iter++)
	{
		Info o = (*iter).second;
		AosNetFileCltObj::deleteFileStatic(
			o.fileid, o.serverid, svr_death, mRundata.getPtr());
	}
}


AosActionObjPtr
AosActImportDocFixed::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActImportDocFixed(false);
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
AosActImportDocFixed::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
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

		mScanner->addRequest(req.reqId, req.blocksize);
	}
	return true;
}


bool
AosActImportDocFixed::signal(const int threadLogicId)
{
	return true;
}


bool
AosActImportDocFixed::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosActImportDocFixed::sanityCheck()
{
	if (!sgSanityCheck) return true;
	map<u64, Info>::iterator iter;
	for (iter = mInfo.begin(); iter != mInfo.end(); iter ++)
	{
		Info o = (*iter).second;
		if (o.docAssType != AosDataAssemblerType::eDocNorm) return true;
		bool rslt = AosStorageEngineMgr::getSelf()->sanityCheck(o.fileid);
		aos_assert_r(rslt, false);
	}
	return true;
}


AosBuffPtr
AosActImportDocFixed::getNextBlock(
		const u64 &reqId,
		const int64_t &expected_size)
{
	mLock->lock();
	Info o = mInfo[reqId];
	mLock->unlock();

	if (mScanner->getDiskError())
	{
		mTask->setErrorType(AosTaskErrorType::eInPutError);
		actionFailed(mRundata);
		return 0;
	}
	AosBuffPtr buff = mScanner->getNextBlock(reqId, expected_size);
	aos_assert_r(buff, 0);

	if (!o.finished) 
	{
		mLock->lock();
		Req req = {reqId, o.blocksize};
		mQueue.push(req);
		mCondVar->signal();
		mLock->unlock();
	}

	return buff;
}


void
AosActImportDocFixed::semPost()
{
	aos_assert(mSem);
	mSem->post();
OmnScreen << "jozhi post, mTotalReqs: " << mTotalReqs << " , "<< this << endl;
}



//bool 	
//AosActImportDocFixed::actionFailed(const AosRundataPtr &rdata)
//{
//OmnScreen << "jozhi actionFailed: start" << this << endl;
//	bool rslt = actionFailed(rdata);
//OmnScreen << "jozhi actionFailed: end" << this << endl;
//	return rslt;
//}

bool
AosActImportDocFixed::storageEngineError(const AosRundataPtr &rdata)
{
	if (mTask)
	{
		mTask->setErrorType(AosTaskErrorType::eStorageError);
	}
	bool rslt = actionFailed(rdata);
	return rslt;
}

bool
AosActImportDocFixed::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
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
			<< AosDocType::eGroupedDoc << "__"
			<< mTaskDocid;
		snapshot = task->getOutPut(AosTaskDataType::eDocSnapShot, key, rdata);
		u64 snapshot_id = 0;
		aos_assert_r(!snapshot, false);
		//if (snapshot)
		//{
		//	snapshot_id = snapshot->getSnapShotId();
		//	doc_client->rollbackSnapshot(rdata, virtualids[i], AosDocType::eGroupedDoc, snapshot_id, mTaskDocid);
		//}
		snapshot_id = doc_client->createSnapshot(rdata, virtualids[i], snapshot_id, 
				AosDocType::eGroupedDoc, mTaskDocid);
		snapshot = AosTaskDataObj::createTaskDataDocSnapShot(virtualids[i], 
				snapshot_id, AosDocType::eGroupedDoc, mTaskDocid, false);
		aos_assert_r(snapshot, false);
OmnScreen << "importdoc,vid " << virtualids[i] << " . snapshot_id: " << snapshot_id << endl;
		mSnapMaps[virtualids[i]] = snapshot_id;
		snapshots.push_back(snapshot);
	}
	return task->updateTaskSnapShots(snapshots, rdata);
}

