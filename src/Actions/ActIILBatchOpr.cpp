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
// 2012/05/11	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/ActIILBatchOpr.h"

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
#include "SmartDoc/SmartDoc.h"
#include "UtilData/ModuleId.h"
#include "Util/AppendFile.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "NetFile/NetFile.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "SEUtil/IILName.h"


AosActIILBatchOpr::AosActIILBatchOpr(const bool flag)
:
AosSdocAction(AOSACTTYPE_IILBATCHOPR, AosActionType::eIILBatchOpr, flag),
mMaxLength(0),
mProcLength(0),
mCrtPos(0),
mProcgress(0),
mIsTest(false),
mFinished(false),
mMaxBuffSize(eMaxBuffSize)
{
}


AosActIILBatchOpr::AosActIILBatchOpr(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_IILBATCHOPR, AosActionType::eIILBatchOpr, false),
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


AosActIILBatchOpr::~AosActIILBatchOpr()
{
	//OmnScreen << "act iil batch opr deleted" << endl;
}


bool
AosActIILBatchOpr::config(
        const AosXmlTagPtr &def,
        const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	//AosXmlTagPtr dataCol_tag = def->getFirstChild("data_collector");
	AosXmlTagPtr dataCol_tag = def->getFirstChild("datacollector");
	aos_assert_r(dataCol_tag, false);

	//AosXmlTagPtr iiltag = dataCol_tag->getFirstChild("iilasm");
	AosXmlTagPtr iiltag = dataCol_tag->getFirstChild("asm");
	aos_assert_r(iiltag, false);

	//mIILName = iiltag->getAttrStr(AOSTAG_IILNAME);
	mIILName = def->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(mIILName != "", false);
	
	// Ketty 2014/08/25
	const char *pos = strstr((const char*)mIILName.data(), "__$group"); 
	if (pos)
	{
		OmnString iil_name = OmnString(mIILName.data(), pos-mIILName.data()); 
		int cube_id = dataCol_tag->getAttrInt(AOSTAG_CUBE_ID, -1);
		aos_assert_r(cube_id != -1, false);
		mIILName = AosIILName::composeCubeIILName(cube_id, iil_name);
	}
	
	mLen = iiltag->getAttrInt(AOSTAG_LENGTH, -1);
	//aos_assert_r(mLen > 0, false);
	if (mLen < 0)
	{
		//AosXmlTagPtr rcd_xml = iiltag->getFirstChild(AOSTAG_RECORD);
		AosXmlTagPtr rcd_xml = iiltag->getFirstChild("datarecord");
		aos_assert_r(rcd_xml, false);

		OmnString rcd_objid = rcd_xml->getAttrStr(AOSTAG_OBJID, "");
	
		AosXmlTagPtr record;
		if (rcd_objid != "")
		{
			record = AosGetDocByObjid(rcd_objid, rdata);
		}
		else 
		{
			record = rcd_xml;
		}

		aos_assert_r(record, false);
		if (record->getAttrStr("type") == "ctnr")
		{
			record = record->getFirstChild("datarecord");
			aos_assert_r(record, false);
		}

		//mLen = record->getAttrInt("record_len", -1);
		mLen = record->getAttrInt("zky_length", -1);
		aos_assert_r(mLen > 0, false);
	}

	mType = def->getAttrStr(AOSTAG_OPERATOR);
	aos_assert_r(mType!= "", false);

	if (mType == "strinc")
	{
		OmnString inctype = iiltag->getAttrStr(AOSTAG_INCTYPE);
		mIncType = AosIILUtil::IILIncType_toEnum(inctype);
		mInitdocid = iiltag->getAttrU64(AOSTAG_INITDOCID, 0);
	}

	mMaxBuffSize = def->getAttrU64("max_buff_size", eMaxBuffSize);
	mBuildBitmap = def->getAttrBool("build_bitmap", false);

	OmnTagFuncInfo << "iilname is: " << mIILName << endl;
	return true;
}

bool
AosActIILBatchOpr::runInMem(
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
	AosBuffPtr buff =  mBuff;
	aos_assert_r(buff, false);
	int64_t bufflen = buff->dataLen();

	OmnTagFuncInfo << "Start to run iil Batch Opr reduce task: " << 
		task->getTaskDocid() << endl;
	if (bufflen > 0)
	{
		//aos_assert(bufflen % mLen == 0);
		AosIILExecutorObjPtr executor;
		u64 executor_id = 0;
		if(mBuildBitmap)
		{
			executor_id = 1;
		}

		if (mType == "strinc")
		{
			bool rslt = AosIILClientObj::getIILClient()->StrBatchInc(
					mIILName, mLen, buff, mInitdocid, mIncType, mSnapId, 
					mTaskDocid, mRundata);

			aos_assert_r(rslt, false);
		}
		else if(mType == "stradd")
		{
			OmnTagFuncInfo << "call strBatchAdd API to really save iil to jimo" << 
					". IIL name is: " << mIILName << ". mLen is: " 
					<< mLen << ". bufflen is: " << buff->dataLen() << endl;
			bool rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
					mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid,
					mRundata);

			aos_assert_r(rslt, false);
		}
		else if(mType == "u64add")
		{
			bool rslt = AosIILClientObj::getIILClient()->U64BatchAdd(
					mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid,
					mRundata);

			aos_assert_r(rslt, false);
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
AosActIILBatchOpr::run(
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
	// Jozhi 2014-10-27
	if (mMaxLength < 0 || (mMaxLength % mLen != 0))
	{
		OmnAlarm << "file contents error" << enderr;
		mTask->setErrorType(AosTaskErrorType::eInPutError);
		actionFailed(mRundata);
		return false;
	}
	aos_assert_r(mMaxLength > 0, false);
	aos_assert_r(mMaxLength % mLen == 0, false);


	mProcLength = 0;
	
	mFileId = file_tag->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(mFileId != 0, false);
		
	mPhysicalId = file_tag->getAttrInt(AOSTAG_PHYSICALID, -1);
	aos_assert_r(mPhysicalId != -1, false);
	
	AosNetFileObjPtr file = OmnNew AosIdNetFile(mPhysicalId, mFileId);
	aos_assert_r(file, false);

	file->seek(mCrtPos);

	vector<AosNetFileObjPtr> virFiles;
	virFiles.push_back(file);

	mScanner = AosDataScannerObj::createMultiFileScannerStatic(rdata);
	aos_assert_r(mScanner, false);

	mScanner->setFiles(virFiles);
	AosActionCallerPtr thisptr(this, true);
	mScanner->setActionsCaller(thisptr);

	int64_t buffsize = mMaxBuffSize / mLen;
	buffsize *= mLen;

	// ????
	if (buffsize > mMaxLength - mProcLength)
	{
		buffsize = mMaxLength - mProcLength;
	}
	
	aos_assert_r(buffsize % mLen == 0, false);
	OmnTagFuncInfo << "add a batchIIL request to scanner" << endl;
	mScanner->addRequest(0, buffsize);
	return true;
}

void
AosActIILBatchOpr::callBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
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
AosActIILBatchOpr::createIIL(
		const u64 &reqId, 
		const int64_t &expected_size, 
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
	int64_t bufflen = buff->dataLen();

	if (bufflen > 0)
	{
		aos_assert(bufflen % mLen == 0);
		AosIILExecutorObjPtr executor;
		u64 executor_id = 0;
		if(mBuildBitmap)
		{
			executor_id = 1;
		}
		if (mType == "strinc")
		{
			bool rslt = AosIILClientObj::getIILClient()->StrBatchInc(
					mIILName, mLen, buff, mInitdocid, mIncType, mSnapId, 
					mTaskDocid, mRundata);
			aos_assert(rslt);
		}
		else if(mType == "stradd")
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
					". IIL name is: " << mIILName << ". mLen is: " << mLen << endl;
			bool rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
					mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid,
					mRundata);

//			if(executor)
//			{
//				executor->finish(mRundata, 0);
//				AosBitmapEngineObj::getBitmapEngine()->IILExecutorFinished(executor_id);
//			}
			aos_assert(rslt);
		}
		else if(mType == "strdel")
		{
			OmnTagFuncInfo << "call strBatchDel API to really del iil to jimo" << 
					". IIL name is: " << mIILName << ". mLen is: " << mLen << endl;
			bool rslt = AosIILClientObj::getIILClient()->StrBatchDel(
					mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid,
					mRundata);

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

			bool rslt = AosIILClientObj::getIILClient()->U64BatchAdd(
					mIILName, mLen, buff, executor_id, mSnapId, mTaskDocid,
					mRundata);

//			if(executor)
//			{
//				executor->finish(mRundata, 0);
//				AosBitmapEngineObj::getBitmapEngine()->IILExecutorFinished(executor_id);
//			}
			aos_assert(rslt);
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
		// Ice Temp 2014/10/17
		AosIILClientObj::getIILClient()->rebuildBitmap(mIILName, mRundata);

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

	int64_t buffsize = mMaxBuffSize / mLen;
	buffsize *= mLen;
	
	if (buffsize > mMaxLength - mProcLength)
	{
		buffsize = mMaxLength - mProcLength;
	}

	aos_assert(buffsize % mLen == 0);
	mScanner->addRequest(reqId, buffsize); 

	OmnTagFuncInfo << endl;
}


AosActionObjPtr
AosActIILBatchOpr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActIILBatchOpr(def, rdata);
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
AosActIILBatchOpr::AosCreateIIL::run()
{
	mCaller->createIIL(mReqId, mExpectedSize, mFinished);
	return true;
}


bool
AosActIILBatchOpr::AosCreateIIL::procFinished()
{
	return true;
}

bool
AosActIILBatchOpr::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
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
	// Ice Temp 2014/10/17
	//mSnapId = iil_client->createSnapshot(virtual_id, mTaskDocid, rdata);
	mSnapId = 0;
	set<OmnString> iilnames;
	iilnames.insert(mIILName);
	snapshot = AosTaskDataObj::createTaskDataIILSnapShot(virtual_id, mSnapId, iilnames, mTaskDocid, false);
	aos_assert_r(snapshot, false);
	snapshots.push_back(snapshot);
	aos_assert_r(snapshots.size() == 1, false);
	return task->updateTaskSnapShots(snapshots, rdata);
}

#endif
