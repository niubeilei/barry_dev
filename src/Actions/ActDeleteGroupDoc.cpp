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
#include "Actions/ActDeleteGroupDoc.h"

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

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("deletegroupdoc", __FILE__, __LINE__);

AosActDeleteGroupDoc::AosActDeleteGroupDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_DELETE_GROUPDOC, AosActionType::eDeleteGroupDoc, flag),
mScanner(0),
mRundata(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosActDeleteGroupDoc::~AosActDeleteGroupDoc()
{
	OmnScreen << "act import doc csv deleted" << endl;
}

bool
AosActDeleteGroupDoc::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
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
	mTaskDocid = task->getTaskDocid();
	mScanner = AosDataScannerObj::createMultiFileScannerStatic(rdata);
	aos_assert_r(mScanner, false);

	AosXmlTagPtr filestag = sdoc->getFirstChild("files");
	aos_assert_r(filestag, false);

	AosXmlTagPtr entry = filestag->getFirstChild(true);
	vector<AosNetFileObjPtr> files;
	while (entry)
	{
		int physical_id = entry->getAttrInt(AOSTAG_PHYSICALID, -1);
		u64 file_id = entry->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
		AosNetFileObjPtr file = OmnNew AosIdNetFile(physical_id, file_id);
		FileInfo info = {file_id, physical_id};
		mFileInfos.push_back(info);
		files.push_back(file);
		entry = filestag->getNextChild();
	}
	mScanner->setFiles(files);
	AosActionCallerPtr thisptr(this, true);
	mScanner->setActionsCaller(thisptr);
	int64_t buffsize = eMaxBlockSize/sizeof(u64);
	buffsize *= sizeof(u64);
	
	aos_assert_r(buffsize % sizeof(u64) == 0, false);
	for (u64 i = 0; i < files.size(); i++)
	{
		mScanner->addRequest(i, buffsize);
	}
	return true;
}


void
AosActDeleteGroupDoc::callBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished)
{
OmnScreen << "jozhi : delete group doc callback" << endl;
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

	vector<OmnThrdShellProcPtr> runners;
	OmnThrdShellProcPtr runner = OmnNew deleteDocThrd(this, buff, finished);
	runners.push_back(runner);
	sgThreadPool->procAsync(runners);

	mLock->unlock();
	return;
}


void
AosActDeleteGroupDoc::deleteData(AosBuffPtr &buff, const bool &finished)
{
	mLock->lock();
	AosStorageEngineMgr::getSelf()->batchDeleteDoc(
			buff, mTaskDocid, mRundata);
	if (finished)
	{
//OmnScreen << "jozhi : delete group doc finished" << endl;
		AosActionObjPtr thisptr(this, true);
		if (mTask) mTask->actionFinished(thisptr, mRundata);
		aos_assert(mScanner);
		mScanner->destroyedMember();
		mScanner = 0;
		bool rslt = deleteFiles();
		aos_assert(rslt);
	}
	mLock->unlock();
}


bool
AosActDeleteGroupDoc::deleteDocThrd::run()
{
	mDelGroupDoc->deleteData(mBuff, mFinished);
	return true;
}


bool
AosActDeleteGroupDoc::deleteFiles()
{
	if (!mTask || !mTask->getIsDeleteFile())
	{
		return true;
	}
	mTask = 0;
	for(u32 i=0; i<mFileInfos.size(); i++)
	{
		bool svr_death = false;
		bool rslt = AosNetFileCltObj::deleteFileStatic(
			mFileInfos[i].mFileId, mFileInfos[i].mPhysicalId, svr_death, mRundata.getPtr());
		aos_assert_r(rslt, false);
	}
	return true;
}

AosActionObjPtr
AosActDeleteGroupDoc::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActDeleteGroupDoc(false);
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
AosActDeleteGroupDoc::storageEngineError(const AosRundataPtr &rdata)
{
	if (mTask)
	{
		mTask->setErrorType(AosTaskErrorType::eStorageError);
	}
	bool rslt = actionFailed(rdata);
	return rslt;
}


bool
AosActDeleteGroupDoc::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
{
	return true;
}

