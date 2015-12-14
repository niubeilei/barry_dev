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
// IIL joins are over-simplified 'table joins', where IILs are special 
// form of 'tables'. There are only two columns in IILs: Key and Value.
// Two IIls can be joined in a number of ways:
// 		eKey
// 		eValue
// 		eFamily
// 		eMember
// 		eSpecificMember
//
// This action supports only equi-joins. Further, this action assumes the
// matching column (either 'key' or 'value' column) is sorted. For instance, 
// if the matching column is the key-column, the normal IIL is used; if
// the matching column is the value-column, the companion IIL is used. 
//
// This action supports both inner or outer equi-joins. For two IILs: IIL1
// and IIL2, 
// 1. it is an inner join if both is configured to return values only upon matching
// 2. it is a left join if IIL2 is configured to always return a value 
// 3. it is a right join if IIL1 is configured to always return a value
// 4. it is a full join if both are configured to always return a value
//
// Modification History:
// 2012/07/30	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/DataJoinCtlr.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataAssembler/DataAssembler.h"
#include "DataJoin/DataJoin.h"
#include "DataRecord/DataRecord.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Rundata/Rundata.h"
#include "Thread/ThreadPool.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


static int sgWaitSec = 5;
static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("datajoin", __FILE__, __LINE__);


AosDataJoinCtlr::AosDataJoinCtlr(
		const AosXmlTagPtr &def,
		const AosDataJoinListenerPtr &listener,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
:
mStartTime(0),
mEndTime(0),
mProgress(0),
mPrevProgress(0),
mIsFinished(false),
mNeedSplit(true),
mLock(OmnNew OmnMutex()),
mCondLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mTask(task),
mListener(listener)
{
}


AosDataJoinCtlr::~AosDataJoinCtlr()
{
	OmnScreen << "join ctlr is deleted" << endl;
}


bool
AosDataJoinCtlr::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// The config is in the form:
	// 	<sdoc 
	// 		AOSTAG_PREFIX_LENGTH="xxx">
	// 		<AOSTAG_IIL_SCANNERS>
	// 			<AOSTAG_IILSCANNER .../>
	// 			<AOSTAG_IILSCANNER .../>
	// 			...
	// 		</AOSTAG_IIL_SCANNERS>
	// 		<AOSTAG_GROUPBYOPR>
	// 			...
	// 		</AOSTAG_GROUPBYOPR>
	// 		<iilassembler .../>
	// 		<docassembler .../>
	// 	</sdoc>
	aos_assert_r(def, false);
	
	mJoins.clear();
	mFilters.clear();
	mDataProcs.clear();
	mIILAssemblers.clear();

	mProcKey = def->getAttrStr(AOSTAG_TYPE, "norm");
	if (mProcKey == "")
	{
		AosSetErrorU(rdata, "missing_proc_key:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr record = def->getFirstChild(AOSTAG_RECORD);
	if (record)
	{
		mIILRecord = AosDataRecordObj::createDataRecordStatic(record, mTask, rdata.getPtrNoLock());
		aos_assert_r(mIILRecord, false);
	}
	
	// Create IILAssembler
	AosXmlTagPtr iil_asb = def->getFirstChild(AOSTAG_IIL_ASSEMBLER);
	if (iil_asb)
	{
		OmnString dataColId = iil_asb->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
		if (dataColId != "")
		{
			AosXmlTagPtr dataColTag = mTask->getDataColTag(dataColId, rdata);
			aos_assert_r(dataColTag, false);
			
			mIILAssembler = AosCreateIILAssembler(rdata.getPtrNoLock(), dataColId, mTask, dataColTag);
			aos_assert_r(mIILAssembler, false);
		}
	}
	
	// Create DocAssembler
	AosXmlTagPtr doc_asb = def->getFirstChild(AOSTAG_DOC_ASSEMBLER);
	if (doc_asb)
	{
		// Check whether it is a valid tag. 
		OmnString type = doc_asb->getAttrStr(AOSTAG_TYPE);
		if (AosDataAssemblerType::isValidDocAsmType(type))
		{
			mDocAssembler = AosCreateDocAssembler(rdata.getPtrNoLock(), mProcKey, mTask, doc_asb);
			aos_assert_r(mDocAssembler, false);
		
			mDocRecord = mDocAssembler->cloneDataRecord(rdata.getPtrNoLock());
			aos_assert_r(mDocRecord, false);
		}
	}
		
	bool rslt = true;
	AosXmlTagPtr entry;
	AosXmlTagPtr proc_def = def->getFirstChild(AOSTAG_DATA_PROCS);
	if (proc_def && (entry = proc_def->getFirstChild()))
	{
		OmnString dataColId;
		AosDataProcObjPtr proc;
		AosDataAssemblerObjPtr iil_asm;
		AosXmlTagPtr dataColTag;

		while (entry)
		{
			dataColId = entry->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
			if (dataColId != "" && (mIILAssemblers.find(dataColId) == mIILAssemblers.end()))
			{
				dataColTag = mTask->getDataColTag(dataColId, rdata);
				aos_assert_r(dataColTag, false);
					
				iil_asm = AosCreateIILAssembler(rdata.getPtrNoLock(), dataColId, mTask, dataColTag);
				aos_assert_r(iil_asm, false);

				mIILAssemblers[dataColId] = iil_asm;
			}
			
			proc = AosDataProcObj::createDataProcStatic(entry, rdata);
			aos_assert_r(proc, false);
			mDataProcs.push_back(proc);
			
			entry = proc_def->getNextChild();
		}
	
		bool needDocid = false;
		bool needConvert = false;
		for(u32 i=0; i<mDataProcs.size(); i++)
		{
			rslt = mDataProcs[i]->resolveDataProc(
				mIILAssemblers, mDocRecord, rdata);
			aos_assert_r(rslt, false);
		
			if (!needDocid) needDocid = mDataProcs[i]->needDocid();
			if (!needConvert) needConvert = mDataProcs[i]->needConvert();
		}
		
		if (needConvert)
		{
			AosSetErrorU(rdata, "do not need convert record:") << def->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (needDocid && !mDocAssembler)
		{
			AosSetErrorU(rdata, "missing doc asb:") << def->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
		
	AosXmlTagPtr fliters = def->getFirstChild(AOSTAG_FILTERS);
	if (fliters)
	{
		AosCondType::E type;
		AosConditionObjPtr filter;
		AosXmlTagPtr filter_tag = fliters->getFirstChild(); 
		while(filter_tag)
		{
			type = AosCondType::toEnum(filter_tag->getAttrStr(AOSTAG_ZKY_TYPE));
			if (AosCondType::isValid(type))
			{
				filter = AosConditionObj::getConditionStatic(filter_tag, rdata);
				aos_assert_r(filter, false);
				mFilters.push_back(filter);
			}
			filter_tag = fliters->getNextChild();
		}
	}

	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);
	
	AosXmlTagPtr scanner_tag = scanners->getFirstChild();
	AosIILScannerObjPtr scanner = AosIILScannerObj::createIILScannerStatic(0, 0, scanner_tag, rdata);
	aos_assert_r(scanner, false);

	OmnString iilname = scanner->getIILName();
	AosQueryContextObjPtr context = scanner->getQueryContext();
	aos_assert_r(context, false);

	int core_num = AosGetNumCpuCores();
	vector<AosQueryContextObjPtr> contexts;

	mNeedSplit = def->getAttrBool(AOSTAG_NEEDSPLIT, true);
	if (mNeedSplit)
	{
		OmnString split_type = def->getAttrStr(AOSTAG_SPLIT_TYPE);

		AosQueryContextObj::SplitValueType type = AosQueryContextObj::SplitValueType_toEnum(split_type);
		context->setSplitValueType(type);
		OmnString split_str = def->getAttrStr(AOSTAG_SPLIT_STR);
		//AosConvertAsciiBinary(split_str);
		context->setSplitValueStr(split_str);
		
		int size = def->getAttrInt(AOSTAG_SPLIT_NUM, -1);
		if (size <= 0 || size > core_num) size = core_num;
		rslt = AosGetSplitValue(iilname, context, size, contexts, rdata);
		if (!rslt)
		{
			contexts.clear();
			contexts.push_back(context);
		}
	}
	else
	{
		contexts.push_back(context);
	}

	int size = contexts.size();
	aos_assert_r(size > 0, false);
	
	mJoins.clear();
	AosDataJoinCtlrPtr thisptr(this, true);
	AosDataJoinPtr join;
	AosXmlTagPtr query_context;

	for (int i=0; i<size; i++)
	{
		rslt = contexts[i]->serializeToXml(query_context, rdata);
		aos_assert_r(rslt, false);
			
		OmnScreen << "query_context[:" << i << "]:" << query_context->toString() << endl;

		join = AosDataJoin::createDataJoin(def, thisptr, rdata);
		aos_assert_r(join, false);

		rslt = join->setQueryContext(contexts[i], rdata);
		aos_assert_r(rslt, false);

		mJoins.push_back(join);
	}
	
	return true;
}


bool
AosDataJoinCtlr::start(
		const AosTaskObjPtr &task, 
		const AosTaskDataObjPtr &task_data, 
		const AosRundataPtr &rdata)
{
	mTask = task;
	if (mIILAssembler) mIILAssembler->setTask(task);
	if (mDocAssembler) mDocAssembler->setTask(task);
	
	map<OmnString, AosDataAssemblerObjPtr>::iterator itr = mIILAssemblers.begin();
	while(itr != mIILAssemblers.end())
	{
		itr->second->setTask(task);
		itr++;
	}
	
	return start(rdata);
}


bool 
AosDataJoinCtlr::start(const AosRundataPtr &rdata)
{
	mLock->lock();
	mStartTime = OmnGetSecond();
	OmnScreen << "join iil start, start time:" << mStartTime << endl;

	bool rslt = sendStart(rdata);
	aos_assert_rl(rslt, mLock, false);
	
	u32 size = mJoins.size();
	aos_assert_rl(size > 0, mLock, false);

	OmnThrdShellProcPtr runner;
	for (u32 i=0; i<size; i++)
	{
		runner = (OmnThrdShellProc *)mJoins[i].getPtr(); 
		rslt = addThreadShellProc(runner, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	
	bool isTimeout;
	while (1)
	{
		mCondLock->lock();
		mCondVar->timedWait(mCondLock, isTimeout, sgWaitSec);
		mCondLock->unlock();
		
		mLock->lock();
		if (mIsFinished)
		{
			mLock->unlock();
			break;
		}
		updateTaskProgress(rdata);
		mLock->unlock();
	}
	return true;
}
	

bool
AosDataJoinCtlr::sendStart(const AosRundataPtr &rdata)
{
	bool rslt = true;
	if (mIILAssembler)
	{
		rslt = mIILAssembler->sendStart(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}

	if (mDocAssembler)
	{
		rslt = mDocAssembler->sendStart(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}

	AosDataAssemblerObjPtr iil_asm;
	map<OmnString, AosDataAssemblerObjPtr>::iterator itr = mIILAssemblers.begin();
	while(itr != mIILAssemblers.end())
	{
		iil_asm = itr->second;
		rslt = iil_asm->sendStart(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
		itr++;
	}

	return true;
}

	
bool
AosDataJoinCtlr::sendFinish(const AosRundataPtr &rdata)
{
	bool rslt = true;
	if (mIILAssembler)
	{
		rslt = mIILAssembler->sendFinish(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
		mIILAssembler->setTask(0);
	}

	if (mDocAssembler)
	{
		rslt = mDocAssembler->sendFinish(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
		mDocAssembler->setTask(0);
	}
	
	AosDataAssemblerObjPtr iil_asm;
	map<OmnString, AosDataAssemblerObjPtr>::iterator itr = mIILAssemblers.begin();
	while(itr != mIILAssemblers.end())
	{
		iil_asm = itr->second;
		rslt = iil_asm->sendFinish(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
		iil_asm->setTask(0);
		itr++;
	}

	return true;
}


bool
AosDataJoinCtlr::joinFinished(
		const AosDataJoinPtr &join,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	u32 size = mJoins.size();
	if (size == 0 && mIsFinished)
	{
		mLock->unlock();
		return true;
	}

	bool found = false;
	u32 i=0;
	for (; i<size; i++)
	{
		if (mJoins[i] == join)
		{
			found = true;
			break;
		}
	}
	
	aos_assert_rl(found, mLock, false);
	OmnScreen << "Join finished: " << i << "," << join->toString() << endl;
	
	bool all_finished = true;
	bool all_success = true;
	OmnString status_records;
	for (u32 i=0; i < mJoins.size(); i++)
	{
		if (!mJoins[i]->isFinished())
		{
			all_finished = false;	
		}
		if (!mJoins[i]->isSuccess())
		{
			all_success = false;
		}
	}
	
	OmnScreen << "Results: " << all_finished << ":" << all_success << ":" << endl;
	
	if (all_finished && !mIsFinished)
	{
		finish(all_success, status_records, rdata);

		mCondLock->lock();
		mCondVar->signal();
		mCondLock->unlock();
	}
	
	mLock->unlock();
	return true;
}


bool
AosDataJoinCtlr::finish(
		const bool all_success,
		const OmnString &status_records,
		const AosRundataPtr &rdata)
{
	if (mIsFinished) 
	{
		return true;
	}

	bool rslt = sendFinish(rdata);
	aos_assert_r(rslt, false);

	mIsFinished = true;
	mEndTime = OmnGetSecond();
	
	OmnString docstr;
	docstr << "<join_report "
		   << AOSTAG_START_TIME << "=\"" << mStartTime << "\" "
		   << AOSTAG_END_TIME << "=\"" << mEndTime << "\" "
		   << AOSTAG_TIME_TAKEN << "=\"" << mEndTime - mStartTime << "\" "
		   << AOSTAG_STATUS << "=\"" << all_success << "\">"
		   << "<" << AOSTAG_STATUS_RECORDS << ">" << status_records << "</" << AOSTAG_STATUS_RECORDS << ">"
		   << "</join_report>";

	OmnScreen << "Data Join Finished: " << endl << docstr << endl;

	if (mListener)
	{
		mListener->joinFinished(all_success, 0, rdata);
	}

	mTask = 0;
	mListener = 0;
	if (mIILRecord) mIILRecord->clear();
	if (mDocRecord) mDocRecord->clear();
	mIILRecord = 0;
	mDocRecord = 0;
	mIILAssembler = 0;
	mDocAssembler = 0;
	mIILAssemblers.clear();
	mDataProcs.clear();
	mJoins.clear();

	return true;
}


AosDataRecordObjPtr
AosDataJoinCtlr::cloneIILRecord(const AosRundataPtr &rdata) const
{
	if (!mIILRecord) return 0;
	return mIILRecord->clone(rdata.getPtrNoLock());
}


AosDataRecordObjPtr
AosDataJoinCtlr::cloneDocRecord(const AosRundataPtr &rdata) const
{
	if (!mDocRecord) return 0;
	return mDocRecord->clone(rdata.getPtrNoLock());
}


bool
AosDataJoinCtlr::updateTaskProgress(const AosRundataPtr &rdata)
{
	aos_assert_r(mTask, false);
	
	u32 size = mJoins.size();
	aos_assert_r(size > 0, false);
	
	int progress = 0;
	int progresses = 0;
	for(u32 i=0; i<size; i++)
	{
		progress = mJoins[i]->getProgress();
		aos_assert_r(progress >= 0 && progress <= 100, false);
		progresses += progress;
	}

	progresses = progresses / size;
	if (mProgress >= progresses)
	{
		mLock->unlock();
		return true;
	}
	mProgress = progresses;

	OmnScreen << "join data ctlr:[" << this << "], progress:" << mProgress << endl;
	
	if (mProgress - mPrevProgress >= 3 || mProgress == 100)
	{
		mPrevProgress = mProgress;
		mTask->actionProgressed(mProgress, rdata);		
	}
	return false;
}

	
AosDataJoinCtlrPtr
AosDataJoinCtlr::createDataJoinCtlr(
		const AosXmlTagPtr &def,
		const AosDataJoinListenerPtr &listener,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	AosDataJoinCtlrPtr ctlr = OmnNew AosDataJoinCtlr(
		def, listener, task, rdata);
	bool rslt = ctlr->config(def, rdata);
	aos_assert_r(rslt, 0);
	return ctlr;
}


bool
AosDataJoinCtlr::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	
	OmnString key = def->getAttrStr(AOSTAG_TYPE, "norm");
	if (key == "")
	{
		AosSetErrorU(rdata, "missing_proc_key:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosDataRecordObjPtr iil_rec, doc_rec;
	AosXmlTagPtr record = def->getFirstChild(AOSTAG_RECORD);
	if (record)
	{
		iil_rec = AosDataRecordObj::createDataRecordStatic(record, task, rdata.getPtrNoLock());
		aos_assert_r(iil_rec, false);
	}
	
	AosDataAssemblerObjPtr iil_asm, doc_asm;
	AosXmlTagPtr iil_asb = def->getFirstChild(AOSTAG_IIL_ASSEMBLERS);
	if (iil_asb)
	{
		OmnString dataColId = iil_asb->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
		if (dataColId != "")
		{
			AosXmlTagPtr dataColTag = task->getDataColTag(dataColId, rdata);
			aos_assert_r(dataColTag, false);
			
			iil_asm = AosCreateIILAssembler(rdata.getPtrNoLock(), dataColId, task, dataColTag);
			aos_assert_r(iil_asm, false);
		}
	}

	// Create DocAssembler
	AosXmlTagPtr doc_asb = def->getFirstChild(AOSTAG_DOC_ASSEMBLER);
	if (doc_asb)
	{
		// Check whether it is a valid tag. 
		OmnString type = doc_asb->getAttrStr(AOSTAG_TYPE);
		if (AosDataAssemblerType::isValidDocAsmType(type))
		{
			doc_asm = AosCreateDocAssembler(rdata.getPtrNoLock(), key, task, doc_asb);
			aos_assert_r(doc_asm, false);
			
			doc_rec = doc_asm->cloneDataRecord(rdata.getPtrNoLock());
			aos_assert_r(doc_rec, false);
		}
	}
	
	bool rslt = true;
	AosXmlTagPtr entry;
	AosXmlTagPtr proc_def = def->getFirstChild(AOSTAG_DATA_PROCS);
	if (proc_def && (entry = proc_def->getFirstChild()))
	{
		OmnString dataColId;
		AosDataProcObjPtr proc;
		AosDataAssemblerObjPtr iil_asm;
		AosXmlTagPtr dataColTag;
		vector<AosDataProcObjPtr> data_procs;
		map<OmnString, AosDataAssemblerObjPtr> iil_asms;

		while (entry)
		{
			dataColId = entry->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
			if (dataColId != "" && (iil_asms.find(dataColId) == iil_asms.end()))
			{
				dataColTag = task->getDataColTag(dataColId, rdata);
				aos_assert_r(dataColTag, false);
					
				iil_asm = AosCreateIILAssembler(rdata.getPtrNoLock(), dataColId, task, dataColTag);
				aos_assert_r(iil_asm, false);

				iil_asms[dataColId] = iil_asm;
			}
			
			proc = AosDataProcObj::createDataProcStatic(entry, rdata);
			aos_assert_r(proc, false);
			data_procs.push_back(proc);
			
			entry = proc_def->getNextChild();
		}
	
		bool needDocid = false;
		bool needConvert = false;
		for(u32 i=0; i<data_procs.size(); i++)
		{
			rslt = data_procs[i]->resolveDataProc(
				iil_asms, doc_rec, rdata);
			aos_assert_r(rslt, false);
		
			if (!needDocid) needDocid = data_procs[i]->needDocid();
			if (!needConvert) needConvert = data_procs[i]->needConvert();
		}
		
		if (needConvert)
		{
			AosSetErrorU(rdata, "do not need convert record:") << def->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (needDocid && !doc_asm)
		{
			AosSetErrorU(rdata, "missing doc asb:") << def->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	
	AosXmlTagPtr fliters = def->getFirstChild(AOSTAG_FILTERS);
	if (fliters)
	{
		AosCondType::E type;
		AosConditionObjPtr filter;
		AosXmlTagPtr filter_tag = fliters->getFirstChild(); 
		while(filter_tag)
		{
			type = AosCondType::toEnum(filter_tag->getAttrStr(AOSTAG_ZKY_TYPE));
			if (AosCondType::isValid(type))
			{
				filter = AosConditionObj::getConditionStatic(filter_tag, rdata);
				aos_assert_r(filter, false);
			}
			filter_tag = fliters->getNextChild();
		}
	}

	rslt = AosDataJoin::checkConfigStatic(def, task, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}
	

bool
AosDataJoinCtlr::addThreadShellProc(
		const OmnThrdShellProcPtr &runner,
		const AosRundataPtr &rdata)
{
	aos_assert_r(runner, false);
	aos_assert_r(sgThreadPool, false);

	return sgThreadPool->proc(runner);
}

