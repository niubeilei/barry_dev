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
// 2014/09/24 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/ActJimoTableBatchOpr.h"

#include "Alarm/Alarm.h"
//#include "API/AosApiG.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/NetFileObj.h" 
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SmartDoc/SmartDoc.h"
#include "UtilData/ModuleId.h"
#include "Util/AppendFile.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "NetFile/NetFile.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "SEModules/SnapshotIdMgr.h"
#include "SEUtil/IILName.h"


AosActJimoTableBatchOpr::AosActJimoTableBatchOpr(const bool flag)
:
AosSdocAction(AOSACTTYPE_JIMOTABLE_BATCHOPR, AosActionType::eJimoTableBatchOpr, flag),
mMaxLength(0),
mProcLength(0),
mCrtPos(0),
mProcgress(0),
mIsTest(false),
mFinished(false),
mMaxBuffSize(eMaxBuffSize)
{
}


AosActJimoTableBatchOpr::AosActJimoTableBatchOpr(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_JIMOTABLE_BATCHOPR, AosActionType::eJimoTableBatchOpr, false),
mMaxLength(0),
mProcLength(0),
mCrtPos(0),
mProcgress(0),
mIsTest(false),
mFinished(false),
mMaxBuffSize(eMaxBuffSize)
{
	bool rslt = config(def, rdata);
	if(!rslt) OmnThrowException(rdata->getErrmsg());
}


AosActJimoTableBatchOpr::~AosActJimoTableBatchOpr()
{
	//OmnScreen << "act jimotable batch opr deleted" << endl;
}


bool
AosActJimoTableBatchOpr::config(
        const AosXmlTagPtr &def,
        const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	AosXmlTagPtr dataCol_tag = def->getFirstChild("datacollector");
	aos_assert_r(dataCol_tag, false);

	AosXmlTagPtr iiltag = dataCol_tag->getFirstChild("asm");
	aos_assert_r(iiltag, false);

	mIILName = def->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(mIILName != "", false);
	
	const char *pos = strstr((const char*)mIILName.data(), "__$group"); 
	if (pos)
	{
		OmnString iil_name = OmnString(mIILName.data(), pos-mIILName.data()); 
		int cube_id = dataCol_tag->getAttrInt(AOSTAG_CUBE_ID, -1);
		aos_assert_r(cube_id != -1, false);
		mIILName = AosIILName::composeCubeIILName(cube_id, iil_name);
	}
	
	mType = iiltag->getAttrStr(AOSTAG_TYPE);
	aos_assert_r(mType!= "", false);

	if (mType == "strinc")
	{
		OmnString inctype = iiltag->getAttrStr(AOSTAG_INCTYPE);
		mIncType = AosIILUtil::IILIncType_toEnum(inctype);
		mInitdocid = iiltag->getAttrU64(AOSTAG_INITDOCID, 0);
	}

	mMaxBuffSize = def->getAttrU64("max_buff_size", eMaxBuffSize);
	mBuildBitmap = def->getAttrBool("build_bitmap", false);

	mCmpTag = dataCol_tag->getFirstChild("CompareFun");
	aos_assert_r(mCmpTag, false);

	OmnTagFuncInfo << "iilname is: " << mIILName << endl;
	return true;
}

bool
AosActJimoTableBatchOpr::runInMem(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	mTask = task;

	//
	//coming here, the input data for reduce task is 
	//not in files, but in memory: mBuff
	//
	AosBuffPtr buff = mBuff;
	aos_assert_r(buff, false);

	OmnTagFuncInfo << "Start to run iil Batch Opr reduce task: " << 
		task->getTaskDocid() << endl;

	i64 bufflen = buff->dataLen();
	if (bufflen > 0)
	{
		AosIILExecutorObjPtr executor;
		//u64 executor_id = 0;
		if(mBuildBitmap)
		{
		//	executor_id = 1;
		}

		if (mType == "strinc")
		{
			//bool rslt = AosIILClientObj::getIILClient()->StrBatchInc(
			//	mIILName, mLen, buff, mInitdocid, mIncType, mSnapId, mTaskDocid, mRundata);
			//aos_assert_r(rslt, false);
		}
		else if(mType == "stradd")
		{
			OmnTagFuncInfo << "call strBatchAdd API to really save iil to jimo" << 
				". IIL name is: " << mIILName << ". bufflen is: " << buff->dataLen() << endl;

			//bool rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
			//	mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid, mRundata);
			//aos_assert_r(rslt, false);
		}
		else if(mType == "u64add")
		{
			//bool rslt = AosIILClientObj::getIILClient()->U64BatchAdd(
			//	mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid, mRundata);
			//aos_assert_r(rslt, false);
		}
		else
		{
			OmnAlarm << "Invalid type : " << mType << enderr;
		}

		mCrtPos += bufflen;
		mProcLength += bufflen;
		OmnTagFuncInfo << "proceeded data len: " <<
			bufflen << " proceeded total len so far: " << mProcLength << endl;
		//aos_assert_r(mProcLength <= mMaxLength, false);

		//int progress = mProcLength * 100 / mMaxLength;
		//if (progress - mProcgress >= 3 || mProcgress == 100)
		//{
		//   mProcgress = progress;
		//   if (mTask) mTask->actionProgressed(mProcgress, mRundata);
		//}	
	}

	AosActionObjPtr thisptr(this, true);
	mTask->actionFinished(thisptr, mRundata);

	mFinished = true;
	return true;
}

bool
AosActJimoTableBatchOpr::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (task->isService())
		return runInMem(task, sdoc, rdata);

	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!task && !mIsTest)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	mTask = task;
	
	mFinished = false;

	//Jozhi 2014-03-11
	//bool rslt = snapShot(rdata);
	//aos_assert_r(rslt, false);
	
	//------------test
	//AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	//AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	//query_context->setOpr(eAosOpr_an);
	//query_context->setStrValue("");
	//query_context->setBlockSize(100);
	//rslt = AosQueryColumn(mIILName, query_rslt, 0, query_context,rdata);
	//aos_assert_r(query_rslt->getNumDocs() == 0, false);
	//-------------end

	AosXmlTagPtr files_tag = sdoc->getFirstChild("files");
	aos_assert_r(files_tag, false);

	AosXmlTagPtr file_tag = files_tag->getFirstChild(true);
	aos_assert_r(file_tag, false);

	mCrtPos = file_tag->getAttrInt64(AOSTAG_START_POS, -1);
	aos_assert_r(mCrtPos != -1, false);

	mMaxLength = file_tag->getAttrInt64(AOSTAG_LENGTH, -1);
	// Chen Ding, 2013/10/16
	// aos_assert_r(mMaxLength != -1, false);
	aos_assert_r(mMaxLength > 0, false);

	mProcLength = 0;
	
	mFileId = file_tag->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(mFileId != 0, false);
		
	mPhysicalId = file_tag->getAttrInt(AOSTAG_PHYSICALID, -1);
	aos_assert_r(mPhysicalId != -1, false);

	AosIdNetFileVar * filevar = OmnNew AosIdNetFileVar(mPhysicalId, mFileId);
	filevar->mUsingArray = false;

	AosNetFileObjPtr file = filevar;
	aos_assert_r(file, false);

	file->seek(mCrtPos);

	vector<AosNetFileObjPtr> virFiles;
	virFiles.push_back(file);

	mScanner = AosDataScannerObj::createMultiFileScannerStatic(rdata);
	aos_assert_r(mScanner, false);

	mScanner->setFiles(virFiles);
	AosActionCallerPtr thisptr(this, true);
	mScanner->setActionsCaller(thisptr);

	i64 buffsize = mMaxBuffSize;
	if (buffsize > mMaxLength - mProcLength)
	{
		buffsize = mMaxLength - mProcLength;
	}
	
	OmnTagFuncInfo << "add a batchIIL request to scanner" << endl;
	mScanner->addRequest(0, buffsize);
	return true;
}

void
AosActJimoTableBatchOpr::callBack(
		const u64 &reqId, 
		const i64 &expected_size, 
		const bool &finished)
{
	if (mScanner->getDiskError())
	{
		mTask->setErrorType(AosTaskErrorType::eInPutError);
		actionFailed(mRundata);
		return;
	}
	OmnThrdShellProcPtr runner = OmnNew AosCreateIIL(this, reqId, expected_size, finished);
	bool rslt = addThreadShellProc(runner, mRundata);
	if (!rslt)
	{
		actionFailed(mRundata);
	}
}	

	
void
AosActJimoTableBatchOpr::createIIL(
		const u64 &reqId, 
		const i64 &expected_size, 
		const bool &finished)
{
	if (mScanner->getDiskError())
	{
		mTask->setErrorType(AosTaskErrorType::eInPutError);
		actionFailed(mRundata);
		return;
	}
	AosBuffPtr buff = mScanner->getNextBlock(reqId, expected_size);
	aos_assert(buff);

	i64 bufflen = buff->dataLen();
	if (bufflen > 0)
	{
		AosIILExecutorObjPtr executor;
		u64 executor_id = 0;
		if(mBuildBitmap)
		{
			executor_id = 1;
		}
		if (mType == "strinc")
		{
	//		bool rslt = AosIILClientObj::getIILClient()->StrBatchInc(
	//			mIILName, mLen, buff, mInitdocid, mIncType, mSnapId, mTaskDocid, mRundata);
	//		aos_assert(rslt);
		}
		else if(mType == "jimotable_add")
		{
//			AosBitmapEngineObjPtr engine = AosBitmapEngineObj::getBitmapEngine();
//			if (engine)
//			{
//				executor = engine->getIILExecutor(mRundata);
//				if(executor)
//				{
//					executor_id = executor->getExecutorID();
//				}
//				else
//				{
//					executor_id = 0;
//				}
//			}

			OmnTagFuncInfo << "call strBatchAdd API to really save iil to jimo" << 
				". IIL name is: " << mIILName << endl;

			bool rslt = AosIILClientObj::getIILClient()->JimoTableBatchAdd(
				mIILName, mCmpTag, buff, executor_id, mSnapId, mTaskDocid, mRundata);

//			if(executor)
//			{
//				executor->finish(mRundata, 0);
//				AosBitmapEngineObj::getBitmapEngine()->IILExecutorFinished(executor_id);
//			}
			aos_assert(rslt);
		}
		else if(mType == "u64add")
		{
//			AosBitmapEngineObjPtr engine = AosBitmapEngineObj::getBitmapEngine();
//			if (engine)
//			{
//				executor = engine->getIILExecutor(mRundata);
//				if(executor)
//				{
//					executor_id = executor->getExecutorID();
//				}
//				else
//				{
//					executor_id = 0;
//				}
//			}

//			bool rslt = AosIILClientObj::getIILClient()->U64BatchAdd(
//				mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid, mRundata);

//			if(executor)
//			{
//				executor->finish(mRundata, 0);
//				AosBitmapEngineObj::getBitmapEngine()->IILExecutorFinished(executor_id);
//			}
//			aos_assert(rslt);
		}
		else
		{
			OmnAlarm << "Invalid type : " << mType << enderr;
		}

		mCrtPos += bufflen;
		mProcLength += bufflen;
		aos_assert(mProcLength <= mMaxLength);

		int progress = mProcLength * 100 / mMaxLength;
		if (progress - mProcgress >= 3 || mProcgress == 100)
		{
		    mProcgress = progress;                                                                                           
			if (mTask) mTask->actionProgressed(mProcgress, mRundata);
		}	
	}

	if (finished || mProcLength == mMaxLength)
	{
		AosActionObjPtr thisptr(this, true);
		//Jozhi 2013-11-19 Testing
		if (mTask) mTask->actionFinished(thisptr, mRundata);
		//if (mTask) mTask->actionFailed(thisptr, mRundata);
		mFinished = true;
		aos_assert(mScanner);
		mScanner->destroyedMember();
		mScanner = 0;
		//Jozhi 2014-04-28 is delete file
		if (!mTask || !mTask->getIsDeleteFile())
		{
			return;
		}
		mTask = 0;
		aos_assert(mFileId != 0);
		aos_assert(mPhysicalId != -1);
		bool svr_death;
		bool rslt =  AosNetFileCltObj::deleteFileStatic(
			mFileId, mPhysicalId, svr_death, mRundata.getPtr());
		aos_assert(rslt);
		return;
	}

	i64 buffsize = mMaxBuffSize;
	if (buffsize > mMaxLength - mProcLength)
	{
		buffsize = mMaxLength - mProcLength;
	}

	mScanner->addRequest(reqId, buffsize); 

	OmnTagFuncInfo << endl;
}


AosActionObjPtr
AosActJimoTableBatchOpr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActJimoTableBatchOpr(def, rdata);
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
AosActJimoTableBatchOpr::AosCreateIIL::run()
{
	mCaller->createIIL(mReqId, mExpectedSize, mFinished);
	return true;
}


bool
AosActJimoTableBatchOpr::AosCreateIIL::procFinished()
{
	return true;
}

bool
AosActJimoTableBatchOpr::createSnapShot(
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	//1. if task is not finish check snapshot is exist, 
	//2. if snapshot exist, rollback it
	//3. otherwise to create snapshot
	vector<AosTaskDataObjPtr> snapshots;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	u32 virtual_id = AosGetCubeId(mIILName);
	aos_assert_r(task, false);
	mTaskDocid = task->getTaskDocid();
	//may be not rollback here
	OmnString key;
	key << AOSTASKDATA_IIL_SNAPSHOT << "__"
		<< virtual_id << "__"
		<< mTaskDocid;

	AosTaskDataObjPtr snapshot;
	if (!task->isService())
	{
		snapshot = task->getSnapShot(key, rdata);
		aos_assert_r(!snapshot, false);
	}
	//if (snapshot)
	//{
	//	u64 snapid = snapshot->getSnapShotId();
	//	iil_client->rollBackSnapshot(virtual_id, snapid, mTaskDocid, rdata);
	//}
	u64 snapshot_id = AosSnapshotIdMgr::getSelf()->createSnapshotId(rdata);
	mSnapId = iil_client->createSnapshot(virtual_id, snapshot_id, mTaskDocid, rdata);
	set<OmnString> iilnames;
	iilnames.insert(mIILName);
	snapshot = AosTaskDataObj::createTaskDataIILSnapShot(virtual_id, mSnapId, iilnames, mTaskDocid, false);
	aos_assert_r(snapshot, false);
	snapshots.push_back(snapshot);
	aos_assert_r(snapshots.size() == 1, false);
	return task->updateTaskSnapShots(snapshots, rdata);
}

#endif
