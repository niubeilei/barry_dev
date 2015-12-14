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
#if 0
#include "Actions/ActImportDocCSV.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DataAssembler/DataAssemblerType.h"
#include "Rundata/Rundata.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/VirtualFileObj.h"
#include "SEInterfaces/SysInfo.h"
#include "SEInterfaces/NetFileObj.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "NetFile/NetFile.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "SEModules/SnapshotIdMgr.h"

static bool	sgSanityCheck = true;
static int	sgWaitSec = 5;

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("ActImprotDocCSV", __FILE__, __LINE__);


AosActImportDocCSV::AosActImportDocCSV(const bool flag)
:
AosSdocAction(AOSACTTYPE_IMPORTDOC_CSV, AosActionType::eImportDocCSV, flag),
mScanner(0),
mRundata(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mSem(OmnNew OmnSem(0)),
mTotalReqs(0),
mSaveDocReqs(0),
mBlockSize(eMaxBlockSize)
{
}


AosActImportDocCSV::~AosActImportDocCSV()
{
	OmnScreen << "act import doc csv deleted" << endl;
}


bool
AosActImportDocCSV::runInMem(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	//
	//In streaming mode, the data is in buff already
	//therefore there is no need to read from files
	//in info_list
	//
	//In this mode, data should be already set in
	//mBuff
	//AosSengineImportDocObjPtr thisptr2(this, true);

	//mLock->lock();
	//mSaveDocReqs++;
	//mLock->unlock();
	
	//AosBuffPtr buff = mTask->getBuff();
	AosBuffPtr buff = mBuff;
	u64 docid = buff->getU64(0); 
	u32 sizeid = AosGetSizeIdByDocid(docid);
	map<OmnString, OmnString> infoData;

	//sender append the record size at the end of the buffer
	int record_size = buff->removeInt();

	OmnTagFuncInfo << "docid is: " << docid << endl;
	OmnTagFuncInfo << "runInMem: record_size is: " << record_size
		<< " sizeid is: " << sizeid << " taskdoc id is: " << mTaskDocid << endl;

	//Need to use batchSaveCSVDoc trans for this purpose
	//Jozhi 2015/01/23 change to call batchInsertDoc()
	//AosStorageEngineMgr::getSelf()->saveCSVDoc(
	//			sizeid, record_size, buff, mSnapMaps, mTaskDocid, rdata);
	AosStorageEngineMgr::getSelf()->batchInsertDoc(buff, mSnapMaps, mTaskDocid, rdata);
	//mLock->lock();
	//mTotalReqs++;
	//mLock->unlock();
	
	//update the mointoring data in db
	//AosStmtInsertItem::insertJobData(rdata, &map);

	actionFinished(rdata);
	OmnScreen << "act import doc csv finished" << endl;
	return true;
}

bool
AosActImportDocCSV::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (task->isService())
		return runInMem(task, sdoc, rdata);

	OmnScreen << "act import doc csv started" << endl;
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

		Info info(sizeid, record_size, key, file_id, data_serverid);
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
		mScanner->addRequest(i, mBlockSize);
	}

	AosSengineImportDocObjPtr thisptr2(this, true);

	mLock->lock();
	bool isTimeout;
	while (1)
	{
		mCondVar->timedWait(mLock, isTimeout, sgWaitSec);

		while (!mQueue.empty())
		{
			Req req = mQueue.front();
			mQueue.pop();

			Info o = mInfo[req.reqId];
			AosBuffPtr ob = o.buff;
			if (ob)
			{
				if (req.buff) ob->addBuff(req.buff);	
			}
			else
			{
				ob = req.buff;
			}
			mInfo[req.reqId].buff = ob;

			aos_assert_rl(ob, mLock, false);

			ob->reset();

			int len = ob->getInt(-1);
			if (ob->dataLen() > (int)sizeof(int) && ob->dataLen() >= len + (int)sizeof(int))
			{
				mSaveDocReqs++;
				mLock->unlock();
				OmnTagFuncInfo << "record_size is: " << o.record_size
					<< "sizeid is: " << o.sizeid << "savedocreq is: " << mSaveDocReqs << endl;
				AosStorageEngineMgr::getSelf()->saveDoc(
					AosDataAssemblerType::eDocCSV, o.sizeid, o.fileid, o.record_size,
					thisptr2, req.reqId, mBlockSize, mSnapMaps, mTaskDocid, rdata);
				mTotalReqs++;
			}
			else
			{
				mLock->unlock();
				if (ob->dataLen() == 0)
				{
					if (!o.finished)
					{
						mScanner->addRequest(req.reqId, mBlockSize);
					}
				}
				else
				{
					aos_assert_r(!o.finished, false);
					mScanner->addRequest(req.reqId, mBlockSize);
				}
			}
			mLock->lock();
		}

		bool all_finished = readFileFinishedLocked();
		if (all_finished) break;
	}

	mLock->unlock();

	OmnThrdShellProcPtr req = OmnNew FinishAllReq(thisptr2);
	OmnTagFuncInfo << "use thread to proc finishall action. Total files are: " 
		<< files.size() << endl;
	sgThreadPool->proc(req);

	return true;
}


void
AosActImportDocCSV::callBack(
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

	AosBuffPtr buff = mScanner->getNextBlock(reqId, expected_size);
	aos_assert_l(buff, mLock);

	if(finished) mInfo[reqId].finished = finished;
	Info o = mInfo[reqId];

	Req req = {reqId, buff};
	mQueue.push(req);
	mCondVar->signal();
	mLock->unlock();
}

	
void
AosActImportDocCSV::allFinished()
{
	for (int i = 0; i < mTotalReqs; i++)
	{
		mSem->wait();
	}

	sanityCheck();

	actionFinished(mRundata);
	deleteFile();
	mTask = 0;

	aos_assert(mScanner);
	mScanner->destroyedMember();
	mScanner = 0;

	OmnScreen << "act import doc csv finished" << endl;
	OmnTagFuncInfo << endl;
}


bool
AosActImportDocCSV::readFileFinishedLocked()
{
	if (mQueue.size() != 0) return false;
	if (mSaveDocReqs != 0) return false;

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
AosActImportDocCSV::deleteFile()
{
	if (!mTask || !mTask->getIsDeleteFile())
	{
		OmnTagFuncInfo << "sorry, file not deleted." << endl; 
		return;
	}

	bool svr_death;
	map<u64, Info>::iterator iter;
	for (iter = mInfo.begin(); iter != mInfo.end(); iter++)
	{
		Info o = (*iter).second;
		AosNetFileCltObj::deleteFileStatic(
			o.fileid, o.serverid, svr_death, mRundata.getPtr());

		OmnTagFuncInfo << "delete file: " << 
			o.fileid << " from server " << o.serverid << endl; 
	}

}


AosActionObjPtr
AosActImportDocCSV::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActImportDocCSV(false);
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
AosActImportDocCSV::sanityCheck()
{
	if (!sgSanityCheck) return true;
	map<u64, Info>::iterator iter;
	for (iter = mInfo.begin(); iter != mInfo.end(); iter ++)
	{
		Info o = (*iter).second;
		bool rslt = AosStorageEngineMgr::getSelf()->sanityCheck(o.fileid);
		aos_assert_r(rslt, false);
	}
	return true;
}


AosBuffPtr
AosActImportDocCSV::getNextBlock(
		const u64 &reqId,
		const int64_t &expected_size)
{
	mLock->lock();
	mSaveDocReqs--;
	Info o = mInfo[reqId];
	AosBuffPtr ob = o.buff;
	aos_assert_rl(ob, mLock, 0);

	ob->reset();
	int len = ob->getInt(-1);
	aos_assert_rl(len > 0, mLock, 0);

	AosBuffPtr buff = ob->getBuff(len, true AosMemoryCheckerArgs);
	aos_assert_rl(buff, mLock, 0);

	int offset = ob->getCrtIdx();
	int last_size = ob->dataLen() - offset;
	if (ob->dataLen() > offset)
	{
		AosBuffPtr buff = OmnNew AosBuff(last_size + 10 AosMemoryCheckerArgs);
		buff->setBuff(&ob->data()[offset], last_size);
		mInfo[reqId].buff = buff;
	}
	else
	{
		mInfo[reqId].buff = 0;
		if (!o.finished)
		{
			OmnScreen << "=======================Ken Lee, 2014/08/21" << endl;
			mInfo[reqId].buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		}
	}

	if (!o.finished || mInfo[reqId].buff) 
	{
		Req req = {reqId, 0};
		mQueue.push(req);
		mCondVar->signal();
	}
	mLock->unlock();

	OmnTagFuncInfo << mSaveDocReqs << endl;
	return buff;
}


void
AosActImportDocCSV::semPost()
{
	aos_assert(mSem);
	mSem->post();
}

//bool 	
//AosActImportDocCSV::actionFailed(const AosRundataPtr &rdata)
//{
//	return actionFailed(mRundata);
//}

bool
AosActImportDocCSV::storageEngineError(const AosRundataPtr &rdata)
{
	if (mTask)
	{
		mTask->setErrorType(AosTaskErrorType::eStorageError);
	}
	bool rslt = actionFailed(rdata);
	return rslt;
}


bool
AosActImportDocCSV::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
{
	vector<AosTaskDataObjPtr> snapshots;
	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_r(doc_client, false);
	aos_assert_r(task, false);
	mTaskDocid = task->getTaskDocid();
	AosTaskDataObjPtr snapshot;
	vector<u32> virtualids = AosGetTotalCubeIds();

	u64 snapshot_id = AosSnapshotIdMgr::getSelf()->createSnapshotId(rdata);
	OmnTagFuncInfo << "create snapshot at the beginning of importdocCSV" << endl;
	for (u32 i = 0; i < virtualids.size(); i++)
	{
		//may be not rollback here
		OmnString key;
		key << virtualids[i] << "__"
			<< AosDocType::eGroupedDoc << "__"
			<< mTaskDocid;
		snapshot = task->getOutPut(AosTaskDataType::eDocSnapShot, key, rdata);

		aos_assert_r(!snapshot, false);
		//if (snapshot)
		//{
		//	snapshot_id = snapshot->getSnapShotId();
		//	doc_client->rollbackSnapshot(rdata, virtualids[i], AosDocType::eGroupedDoc, snapshot_id, mTaskDocid);
		//}
		snapshot_id = doc_client->createSnapshot(rdata, virtualids[i], snapshot_id,  AosDocType::eGroupedDoc, mTaskDocid);
		snapshot = AosTaskDataObj::createTaskDataDocSnapShot(virtualids[i], snapshot_id, AosDocType::eGroupedDoc, mTaskDocid, false);
		aos_assert_r(snapshot, false);
OmnScreen << "importdoc,vid " << virtualids[i] << " . snapshot_id: " << snapshot_id << endl;
		mSnapMaps[virtualids[i]] = snapshot_id;
		snapshots.push_back(snapshot);
	}
	return task->updateTaskSnapShots(snapshots, rdata);
}

#endif
