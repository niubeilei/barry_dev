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
// 06/13/2012 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "DataAssembler/DataAssemblerSort.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "Debug/Debug.h"
#include "Util/BuffArrayVar.h"
#include "Util/File.h"
#include "Job/Job.h"
#include "TaskMgr/Task.h"
#include "Thread/ThreadPool.h"
#include "NetFile/NetFileClt.h"
#include "NetFile/NetFile.h"
#include "API/AosApi.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("SaveFileThrd", __FILE__, __LINE__);

AosDataAssemblerSort::AosDataAssemblerSort(
		const OmnString &asm_key,
		const u64 task_docid)
:
AosDataAssembler(AosDataAssemblerType::eSort, AOSDATAASSEMBLER_SORT, asm_key, task_docid),
mNumEntry(0),
mBuffMaxSizeToSend(AosDataAssembler::sgDftBuffLen),
mComp(0),
mCompRaw(0),
mArray(0),
mArrayRaw(0),
mArrayVar(0),
mArrayVarRaw(0),
mTaskDocid(task_docid),
mRecordType(AosDataRecordType::eInvalid),
mJobDocid(0),
mTaskId(""),
mTmpFileSeqno(0),
mOutPutSeqno(0),
mTotalFiles(0),
mTmpFileSem(OmnNew OmnSem(0)),
mOutPutSem(OmnNew OmnSem(0)),
mStreamSem(OmnNew OmnSem(10)),
mMergeFileSem(OmnNew OmnSem(5))
{
}


AosDataAssemblerSort::~AosDataAssemblerSort()
{
}


bool
AosDataAssemblerSort::config(const AosXmlTagPtr &def, AosRundata *rdata)
{
	aos_assert_r(def, false);
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);

	mJobDocid = task->getJobDocid();
	aos_assert_r(mJobDocid, false);

	mTaskId = task->getTaskId();
	aos_assert_r(mTaskId != "", false);

	mTmpFileSem = OmnNew OmnSem(0);
	mOutPutSem = OmnNew OmnSem(0);

	mTmpFileSeqno = 0;
	mOutPutSeqno = 0;
	mTotalFiles = 0;

	bool rslt = AosDataAssembler::config(def, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr datarecord = def->getFirstChild("datarecord");
	aos_assert_r(datarecord, false);
	datarecord = datarecord->getFirstChild("datarecord");
	aos_assert_r(datarecord, false);
	mRecordType = AosDataRecordType::toEnum(datarecord->getAttrStr("type", "buff"));
	aos_assert_r(mRecordType != AosDataRecordType::eInvalid, false);

	AosXmlTagPtr cmp_tag = def->getFirstChild("CompareFun");
	aos_assert_r(cmp_tag, false);
	mComp = AosCompareFun::getCompareFunc(cmp_tag);
	mCompRaw = mComp.getPtr();
	aos_assert_r(mCompRaw, false);

	if (mRecordType == AosDataRecordType::eBuff)
	{
		mArrayVar = AosBuffArrayVar::create(mCompRaw, false, rdata);
		mArrayVarRaw = mArrayVar.getPtr();
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		mArray = OmnNew AosBuffArray(mCompRaw, true, false, mBuffMaxSizeToSend);
		mArrayRaw = mArray.getPtr();
	}
	else 
	{
		OmnNotImplementedYet;
	}
	
	return true;
}


bool
AosDataAssemblerSort::sendStart(AosRundata *rdata)
{
	AosDataAssembler::showDataAssemblerInfo(__FILE__, __LINE__, "start");
	return true;
}

	
bool
AosDataAssemblerSort::sendFinish(AosRundata *rdata)
{
	bool rslt = false;
	if (mRecordType == AosDataRecordType::eBuff)
	{
		rslt = sortAndFlush(mArrayVar, rdata);
		aos_assert_rl(rslt, mLockRaw, false);
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		rslt = sortAndFlush(mArray, rdata);
		aos_assert_rl(rslt, mLockRaw, false);
	}
	aos_assert_rl(rslt, mLockRaw, false);

	for (u32 i=0; i < mTotalFiles; i++)
	{
		mTmpFileSem->wait();
	}

	rslt = mergeFiles(mFiles, rdata);
	aos_assert_r(rslt, false);

	mFiles.clear();
	return true;
}

bool 
AosDataAssemblerSort::waitFinish(AosRundata *rdata)
{
	//wait all multi sorter finish
	u32 total_outputs = mTotalFiles%eNumFilesToMerge != 0 ?
		(mTotalFiles/eNumFilesToMerge) + 1: (mTotalFiles/eNumFilesToMerge);
	for (u32 i = 0; i < total_outputs; i++)
	{
		 mOutPutSem->wait();
	}

	OmnScreen << "str attr, mAsmKey:" << mAsmKey 
			  << " generate outputs: " << total_outputs 
			  << " , send Finish mNumEntry: " << mNumEntry << endl;
	showDataAssemblerInfo(__FILE__, __LINE__, total_outputs);
	AosDataAssembler::showDataAssemblerInfo(__FILE__, __LINE__, "finish");
	return true;	
}


bool
AosDataAssemblerSort::appendEntry(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	bool rslt = false;
	AosDataRecordType::E type = record->getType();
	aos_assert_r(type == mRecordType, false);
	if (mRecordType == AosDataRecordType::eBuff)
	{
		AosBuffArrayVarPtr arrayVar;
		aos_assert_r(mArrayVarRaw, false);
		mLockRaw->lock();
		mNumEntry++;
		rslt = mArrayVarRaw->appendEntry(record, rdata);
		aos_assert_rl(rslt, mLockRaw, false);
		if (mArrayVarRaw->dataLength() >= mBuffMaxSizeToSend)
		{
			arrayVar = mArrayVarRaw;
			mArrayVar = AosBuffArrayVar::create(mCompRaw, false, rdata);
			mArrayVarRaw = mArrayVar.getPtr();
		}
		mLockRaw->unlock();
		if (arrayVar)
		{
			sortAndFlush(arrayVar, rdata);
		}
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		AosBuffArrayPtr array;
		aos_assert_r(mArrayRaw, false);
		mLockRaw->lock();
		mNumEntry++;
		rslt = mArrayRaw->appendEntry(record, rdata);
		aos_assert_rl(rslt, mLockRaw, false);
		if (mArrayRaw->dataLength() >= mBuffMaxSizeToSend)
		{
			array = mArrayRaw;
			mArray = OmnNew AosBuffArray(mCompRaw, true, false, mBuffMaxSizeToSend);
			mArrayRaw = mArray.getPtr();
		}
		mLockRaw->unlock();
		if (array)
		{
			sortAndFlush(array, rdata);
		}
	}
	else
	{
		OmnNotImplementedYet;
	}
	return true;
}

bool
AosDataAssemblerSort::sanitycheck(
		char* begin,
		int length)
{
	//return true;
	aos_assert_r(length%mCompRaw->size == 0, false);
	int size = length/mCompRaw->size;
	for(int i=0; i<size; i++)
	{
		if (i>0)
		{
			aos_assert_r(mCompRaw->cmp(begin, begin-mCompRaw->size) >= 0, false);
		}
		begin += mCompRaw->size;
	}
	return true;
}


bool
AosDataAssemblerSort::sortAndFlush(
		const AosBuffArrayVarPtr &arrayVar,
		const AosRundataPtr &rdata)
{
	AosBuffPtr headBuff = arrayVar->getHeadBuff();
	if (!headBuff || headBuff->dataLen() == 0)
	{
		return true;
	}
	bool rslt = arrayVar->sort();
	aos_assert_r(rslt, false);

	set<i64> bodyAddrSet;
	headBuff = arrayVar->getHeadBuff();

	rslt = sanitycheck(headBuff->data(), headBuff->dataLen());
	aos_assert_r(rslt, false);

	AosBuffPtr bodyBuff;
	AosBuffArrayVar::procHeaderBuff(mCompRaw, headBuff, bodyBuff, bodyAddrSet);
	aos_assert_r(bodyBuff, false);

	rslt = AosMergeFileSorter::sanitycheck_var(mCompRaw, bodyBuff->data(), bodyBuff->dataLen());
	aos_assert_r(rslt, false);

	rslt = flush(bodyBuff, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDataAssemblerSort::sortAndFlush(
		const AosBuffArrayPtr &array,
		const AosRundataPtr &rdata)
{
	AosBuffPtr buff = array->getBuff();
	if (!buff || buff->dataLen() == 0)
	{
		return true;
	}
	
	bool rslt = array->sort();
	aos_assert_r(rslt, false);

	buff = array->getBuff();
	aos_assert_r(buff, false);

	rslt = AosMergeFileSorter::sanitycheck(mCompRaw, buff->data(), buff->dataLen());
	aos_assert_r(rslt, false);

	rslt = flush(buff, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDataAssemblerSort::flush(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	mLockRaw->lock();
	mTotalFiles++;
	mLockRaw->unlock();
	OmnString fname = createTmpFileName();
	aos_assert_r(fname != "", false);

	AosDataAssemblerObjPtr thisptr(this, false);
	sgThreadPool->proc(OmnNew AosSaveFileThrd(thisptr, fname, buff, rdata));
	mStreamSem->wait();
	return true;
}

void
AosDataAssemblerSort::streamPost()
{
	mStreamSem->post();
}


AosDataAssemblerObjPtr
AosDataAssemblerSort::createAssemblerSort(
		const OmnString &asm_key, 
		const u64 task_docid, 
		const AosXmlTagPtr &def, 
		AosRundata* rdata)
{
	AosDataAssemblerSort* asm_sort = OmnNew AosDataAssemblerSort(asm_key, task_docid);
	bool rslt = asm_sort->config(def, rdata);
	aos_assert_r(rslt, 0);
	return asm_sort;
}

bool
AosDataAssemblerSort::AosSaveFileThrd::run()
{
	aos_assert_r(mBuff, false);
	aos_assert_r(mAssembler, false);

	u64 file_id = 0;
	OmnFilePtr file = AosNetFileCltObj::createTempFileStatic(
		mRundata.getPtr(), file_id, mFileName, 0, false);
	aos_assert_r(file && file->isGood(), false);

	bool rslt = file->append(mBuff->data(), mBuff->dataLen(), true);
	aos_assert_r(rslt, false);

	mBuff = 0;

	mAssembler->appendSortedFile(file_id, mRundata);
	mAssembler->tmpFilePost();
	mAssembler->streamPost();
//OmnScreen << "for debug generate tmp file" << endl;
	return true;
}

bool
AosDataAssemblerSort::AosSaveFileThrd::procFinished()
{
	return true;
}

bool
AosDataAssemblerSort::mergeFiles(
		const vector<u64> &files, 
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
	if (files.empty())
	{
		return true;
	}
	AosTaskDataObjPtr task_data =  task->getOutPut(
			AosTaskDataType::eOutPutIIL, mAsmKey, rdata);
	u64 file_id;
	OmnLocalFilePtr output_file;
	if (!task_data)
	{
		OmnString fname = createFileName();
		aos_assert_r(fname != "", false);

		output_file = AosNetFileCltObj::createRaidFileStatic(
				rdata.getPtr(), file_id, fname, 0, false);
		aos_assert_r(output_file && output_file->isGood(), false);

		task_data = AosTaskDataObj::createTaskDataOutPutIIL(
				AosGetSelfServerId(), mAsmKey, file_id, 0);
		aos_assert_r(task_data, false);

		bool rslt = task->updateTaskOutPut(task_data, rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
		AosXmlTagPtr output;
		bool rslt = task_data->serializeTo(output, rdata);
		aos_assert_r(rslt && output, false);
		file_id = output->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
		output_file = AosNetFileCltObj::openLocalFileStatic(file_id, rdata.getPtr());
		aos_assert_r(output_file && output_file->isGood(), false);
		output_file->resetFile();
	}

	vector<AosNetFileObjPtr> net_files;
	int serverid = AosGetSelfServerId();
	for(u32 i=0; i<files.size(); i++)
	{
		AosNetFileObjPtr file = OmnNew AosIdNetFile(serverid, files[i]);
		aos_assert_r(file, false);
		net_files.push_back(file);
	}
	AosDataAssemblerObjPtr thisptr(this, false);
	sgThreadPool->proc(OmnNew AosMultiFileSortThrd(thisptr, mCompRaw, output_file, net_files, rdata));
	mMergeFileSem->wait();
	return true;
}


void
AosDataAssemblerSort::mergeFilePost()
{
	mMergeFileSem->post();
}

bool
AosDataAssemblerSort::appendSortedFile(const u64 &file_id, const AosRundataPtr &rdata)
{
	vector<u64> files;
	mLockRaw->lock();
	mFiles.push_back(file_id);
	if (mFiles.size() >= eNumFilesToMerge)
	{
		files = mFiles;
		mFiles.clear();
	}
	mLockRaw->unlock();
	if (files.size())
	{
		return mergeFiles(files, rdata);
	}
	return true;
}


bool
AosDataAssemblerSort::AosMultiFileSortThrd::run()
{
	AosMergeFileSorterPtr sorter = OmnNew AosMergeFileSorter(4000000, mCompRaw, mNetFiles, mRundata);
	sgThreadPool->proc(OmnNew AosGenerateOutPutThrd(mAssembler, sorter, mNetFiles, mOutPutFile, mRundata));
	sorter->sort();
	mAssembler->mergeFilePost();
//OmnScreen << "for debug sort finish" << endl;
	return true;
}


bool
AosDataAssemblerSort::AosMultiFileSortThrd::procFinished()
{
	return true;
}

bool
AosDataAssemblerSort::AosGenerateOutPutThrd::run()
{
	aos_assert_r(mSorter, false);
	bool rslt = false;
	while(1)
	{
		AosBuffPtr buff = mSorter->nextBuff();
		if (!buff)
		{
			break;
		}
		aos_assert_r(buff, false);
		rslt = mOutPutFile->append(buff->data(), buff->dataLen(), true);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(rslt, false);
	int server_id = -1;
	u64 file_id = 0;
	bool svr_death = false;
	for (u32 i=0; i<mNetFiles.size(); i++)
	{
		server_id = mNetFiles[i]->getServerId();
		file_id = mNetFiles[i]->getFileId();
		rslt = AosNetFileCltObj::deleteFileStatic(file_id, server_id, svr_death, mRundata.getPtr());
		aos_assert_r(rslt, false);
	}
	mAssembler->outPutPost();
//OmnScreen << "for debug generate output" << endl;
	return true;
}

bool
AosDataAssemblerSort::AosGenerateOutPutThrd::procFinished()
{
	return true;
}

void
AosDataAssemblerSort::outPutPost()
{
	mOutPutSem->post();
}

void
AosDataAssemblerSort::tmpFilePost()
{
	mTmpFileSem->post();
}

bool
AosDataAssemblerSort::checkConfigStatic(
		const AosXmlTagPtr &def,
		AosRundata* rdata)
{
	return true;
}


void
AosDataAssemblerSort::showDataAssemblerInfo(
		const char *file,
		const int line,
		const u32 total_outputs)
{
	OmnString msg;
	msg << " DataAssembler:" << mAsmKey 
		<< ", generate outputs:" << total_outputs
		<< ", send finish entryNum:" << mNumEntry;
	report(file, line, "show", msg);
}

