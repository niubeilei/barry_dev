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
// 03/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataAssembler/FileAssembler.h"

#include "API/AosApiD.h"
#include "API/AosApiG.h"

#include "Job/Job.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "TaskMgr/Task.h"
#include "Util/File.h"


AosFileAssembler::AosFileAssembler(
		const AosDataAssemblerType::E type,
		const OmnString &name,
		const OmnString &asm_key,
		const u64 task_docid) 
:
AosDataAssembler(type, name, asm_key, task_docid),
mBuffMaxSizeToSend(AosDataAssembler::sgDftBuffLen),
mFinished(false),
mNeedDeleteIIL(false),
mTrueDelete(true),
mIILName(""),
mAppendDataLen(0),
mSendDataLen(0),
mEntryNum(0),
mRecordType(AosDataRecordType::eInvalid)
{
}


AosFileAssembler::~AosFileAssembler()
{
}


bool
AosFileAssembler::config(const AosXmlTagPtr &def, AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosFileAssembler::configV1(const AosXmlTagPtr &def, AosRundata *rdata)
{
	mConfig = def;
	aos_assert_r(def, false);

	bool rslt = AosDataAssembler::config(def, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr datarecord = def->getFirstChild("datarecord");
	aos_assert_r(datarecord, false);
	datarecord = datarecord->getFirstChild("datarecord");
	aos_assert_r(datarecord, false);
	mRecordType = AosDataRecordType::toEnum(datarecord->getAttrStr("type", "buff"));
	aos_assert_r(mRecordType != AosDataRecordType::eInvalid, false);

	mBuff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	mBuffMaxSizeToSend = def->getAttrInt64(AOSTAG_MAX_BUFFSIZE, -1);
	if (mBuffMaxSizeToSend <= 0) mBuffMaxSizeToSend = AosDataAssembler::sgDftBuffLen;

	mNeedDeleteIIL = def->getAttrBool(AOSTAG_NEEDDELETE, false);
	mTrueDelete = def->getAttrBool(AOSTAG_TREUDELETE, true);
	mIILName = def->getAttrStr(AOSTAG_IILNAME);
	aos_assert_r(mIILName != "", false);

	mFileName = def->getAttrStr("zky_file_name", "");

	return true;
}


AosDataAssemblerObjPtr
AosFileAssembler::createFileAssemblerV1(
		const OmnString &asm_key,
		const u64 task_docid,
		const AosXmlTagPtr &def, 
		AosRundata *rdata)
{
	aos_assert_r(def, 0);

	OmnString name = def->getAttrStr(AOSTAG_NAME);
	aos_assert_r(name != "", 0);

	AosXmlTagPtr iilAsmConfig = def->getFirstChild("asm");
	if (!iilAsmConfig)
		iilAsmConfig = def;

	iilAsmConfig->setAttr(AOSTAG_IILNAME, name);

	AosFileAssembler * file_asm = OmnNew AosFileAssembler(AosDataAssemblerType::eFile, 
			AOSDATAASSEMBLER_FILE, asm_key, task_docid);

	bool rslt = file_asm->configV1(iilAsmConfig, rdata);
	aos_assert_r(rslt, 0);

	return file_asm;
}
	

bool
AosFileAssembler::sendStart(AosRundata *rdata)
{
	AosDataAssembler::showDataAssemblerInfo(__FILE__, __LINE__, "start");
	if (mNeedDeleteIIL)
	{
		AosDeleteIIL(mIILName, mTrueDelete, rdata);
	}
	return true;
}

	
bool
AosFileAssembler::sendFinish(AosRundata *rdata)
{
	mLockRaw->lock();
	mFinished = true;
	bool rslt = sendPriv(rdata);
	aos_assert_rl(rslt, mLockRaw, false);
	
	OmnScreen << "str attr, mAsmKey:" << mAsmKey << ", "
			  << "mAppendDataLen: " << mAppendDataLen << ", "
			  << "mSendDataLen: " << mSendDataLen << ", "
			  << "mEntryNum: " << mEntryNum << endl;

	OmnTagFuncInfo << "str attr, mAsmKey:" << mAsmKey << ", "
			  << "mEntryNum: " << mEntryNum << endl;

	showDataAssemblerInfo();
	AosDataAssembler::showDataAssemblerInfo(__FILE__, __LINE__, "finish");
	mLockRaw->unlock();
	return true;	
}

bool 
AosFileAssembler::waitFinish(AosRundata *rdata)
{
	return true;	
}


bool
AosFileAssembler::appendEntry(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	mLockRaw->lock();
	bool rslt = false;
	AosDataRecordType::E type = record->getType();
	aos_assert_r(type == mRecordType, false);

	const char *data = record->getData(rdata);
	i64 rcd_len = record->getRecordLen();
	aos_assert_r(rcd_len > 0, false);

	//1.record type:buff
	//format: rcd_len + data
	if (mRecordType == AosDataRecordType::eBuff)               
	{
		rslt = mBuff->setEncodeCharStr(data, rcd_len);
		aos_assert_r(rslt, false);
	}
	//2.record type:fixbin
	//format:data
	else if (mRecordType == AosDataRecordType::eFixedBinary || mRecordType == AosDataRecordType::eCSV)
	{
		rslt = mBuff->setBuff(data, rcd_len);
		aos_assert_r(rslt, false);
	}
	else                            
	{                               
		OmnNotImplementedYet;       
	}                               

	i64 len = mBuff->dataLen();
	mAppendDataLen += len;
	mEntryNum++;

	if (len >= mBuffMaxSizeToSend)
	{
		bool rslt = sendPriv(rdata);
		aos_assert_rl(rslt, mLockRaw, false);
	}

	mLockRaw->unlock();

	return true;
}


bool
AosFileAssembler::sendPriv(AosRundata* rdata)
{
	bool rslt;
	aos_assert_r(mBuff, false);
	i64 len = mBuff->dataLen();
	if (len > 0)
	{
		mSendDataLen += mBuff->dataLen();

		if (!mFile)
		{
			rslt = createFile(rdata);
			aos_assert_r(rslt, false);
		}

		char* data = mBuff->data();

		u64 filesize = mFile->getLength();
		if (filesize < eDftMaxFileSize)             
		{
			rslt = mFile->append(data, len, true);
			aos_assert_r(rslt, false);
		}   
		else
		{
			rslt = createFile(rdata);
			aos_assert_r(rslt, false);

			rslt = mFile->append(data, len, true);
			aos_assert_r(rslt, false);
		}

		mBuff->reset();
		mBuff->setDataLen(0);
	}

	return true;
}


bool
AosFileAssembler::createFile(
		AosRundata *rdata)
{
	if (mFileName != "")
	{
		if (mFile)
		{
			return true;
		}
		mFile = OmnNew OmnFile(mFileName, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(mFile && mFile->isGood(), false);
		return true;
	}
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
	u64 jobId = task->getJobDocid();
	OmnString taskId = task->getTaskId();
	OmnString tableId = mAsmKey;

	AosTaskDataObjPtr task_data = task->getOutPut(
			    AosTaskDataType::eOutPutIIL, tableId, rdata);

	u64 file_id;	
	OmnString fname = "mg_iil_";
	fname <<  jobId << "_" << taskId << "_" << tableId << "_";
	aos_assert_r(fname != "", false);

	mFile = AosNetFileCltObj::createRaidFileStatic(
			rdata, file_id, fname, 0, false);
	aos_assert_r(mFile && mFile->isGood(), false);

	if (!task_data)
	{
		task_data = AosTaskDataObj::createTaskDataOutPutIIL(
				    AosGetSelfServerId(), tableId, file_id, 0);
		aos_assert_r(task_data, false);

		bool rslt = task->updateTaskOutPut(task_data, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool 
AosFileAssembler::appendEntry(
		const AosValueRslt &value,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}
	

void
AosFileAssembler::showDataAssemblerInfo()
{
	AosTaskLog(mTaskDocid) << " FileAssembler: " << mAsmKey 
		<< ", append dataLen: " << mAppendDataLen 
		<< ", send dataLen: " << mSendDataLen
		<< ", send finish entryNum: " << mEntryNum
		<< endlog;
}

