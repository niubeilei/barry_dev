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
//   
//
// Modification History:
// 03/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ImportData/ImportDataCtlrNorm.h"

#include "API/AosApi.h"
#include "DataAssembler/DataAssembler.h"
#include "DataAssembler/Ptrs.h"
#include "ImportData/ImportListener.h"
#include "ImportData/ImportDataThrdNorm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataCacherCreatorObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEUtil/ValueDefs.h"
#include "Util/Ptrs.h"
#include "Util/ReadFile.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


static int sgWaitSec = 5;


AosImportDataCtlrNorm::AosImportDataCtlrNorm(
		const AosXmlTagPtr &def, 
		const AosImportListenerPtr &listener,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
:
AosImportDataCtlr(def, listener, task, rdata),
mTotalLen(0),
mNoUpdateTaskProcNum(false)
{
}


AosImportDataCtlrNorm::~AosImportDataCtlrNorm()
{
}


bool
AosImportDataCtlrNorm::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// This is an import task to import data. An import task is defined as:
	// 	<import
	// 		ctnr_objid="xxx">
	// 		<AOSTAG_ORIG_FORMATTER .../>
	// 		<AOSTAG_TARGET_FORMATTER .../>
	// 		<files>
	// 			<file full_fname="xxx" start_pos="xxx" length="xxx"/>
	// 			<file full_fname="xxx" start_pos="xxx" length="xxx"/>
	// 			...
	// 		</files>
	// 		<AOSTAG_IIL_ASSEMBLERS>
	// 			...
	// 		</AOSTAG_IIL_ASSEMBERS>
	// 		<policy type="xxx" .../>
	// 	</import>
	aos_assert_r(def, false);
	aos_assert_r(mTask, false);
	
	mImportDataThrds.clear();
	mIILAssemblers.clear();
	mDataProcs.clear();
	
	mNoUpdateTaskProcNum = def->getAttrBool(AOSTAG_NOUPDATETASKPROCNUM, false);

	mProcKey = def->getAttrStr(AOSTAG_KEY);
	if (mProcKey == "")
	{
		AosSetErrorU(rdata, "missing_proc_key:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the record
	AosXmlTagPtr record = def->getFirstChild(AOSTAG_RECORD);
	if (!record)
	{
		AosSetErrorU(rdata, "missing record cfg:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mDataRecord = AosDataRecordObj::createDataRecordStatic(record, mTask, rdata.getPtr());
	aos_assert_r(mDataRecord, false);

	AosXmlTagPtr entry;
	AosXmlTagPtr procs_def = def->getFirstChild(AOSTAG_DATA_PROCS);
	if (procs_def && (entry = procs_def->getFirstChild(true)))
	{
		AosDataProcObjPtr proc;
		AosDataAssemblerObjPtr iil_asm;
		OmnString dataColId, config_id;
		AosXmlTagPtr dataColTag, proc_def;
		
		AosXmlTagPtr extension_config, extension_tag;
		OmnString extension_objid = procs_def->getAttrStr(AOSTAG_EXTENSION_CONFIG_OBJID);
		if (extension_objid != "")
		{
			extension_config = AosGetDocByObjid(extension_objid, rdata);
			if (!extension_config)
			{
				AosSetErrorU(rdata, "can't_get_extension_config") << extension_objid;
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			extension_config = extension_config->clone(AosMemoryCheckerArgsBegin);
		}
		
		while (entry)
		{
			proc_def = resolveDataProcConfig(entry, extension_config, rdata);
			aos_assert_r(proc_def, false);
			
			dataColId = proc_def->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
			if (dataColId != "" && (mIILAssemblers.find(dataColId) == mIILAssemblers.end()))
			{
				dataColTag = mTask->getDataColTag(dataColId, rdata);
				aos_assert_r(dataColTag, false);
					
				iil_asm = AosCreateIILAssembler(rdata.getPtrNoLock(), dataColId, mTask, dataColTag);
				aos_assert_r(iil_asm, false);

				mIILAssemblers[dataColId] = iil_asm;
			}
			
			proc = AosDataProcObj::createDataProcStatic(proc_def, rdata);
			aos_assert_r(proc, false);
			mDataProcs.push_back(proc);
			
			entry = procs_def->getNextChild();
		}
	}

	bool rslt = true;
	bool needDocid = false;
	bool needConvert = false;
	for(u32 i=0; i<mDataProcs.size(); i++)
	{
		rslt = mDataProcs[i]->resolveDataProc(mIILAssemblers, mDataRecord, rdata);
		aos_assert_r(rslt, false);

		if (!needDocid) needDocid = mDataProcs[i]->needDocid();
		if (!needConvert) needConvert = mDataProcs[i]->needConvert();
	}

	if (needDocid && !needConvert)
	{
		AosSetErrorU(rdata, "need convert record:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr doc_asb = def->getFirstChild(AOSTAG_DOC_ASSEMBLER);
	if (doc_asb)
	{
		OmnString type = doc_asb->getAttrStr(AOSTAG_TYPE);
		if (AosDataAssemblerType::isValidDocAsmType(type))
		{
			mDocAssembler = AosCreateDocAssembler(rdata.getPtrNoLock(), mProcKey, mTask, doc_asb);
			aos_assert_r(mDocAssembler, false);
	
			mOutputRecord = mDocAssembler->cloneDataRecord(rdata.getPtrNoLock());
			aos_assert_r(mOutputRecord, false);
		}
	}

	if (needDocid || needConvert)
	{
		aos_assert_r(mDocAssembler && mOutputRecord, false);
	}

	rslt = splitDataCacher(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosImportDataCtlrNorm::setFileInfo(
		const AosTaskDataObjPtr &task_data,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mDataCacher = task_data->createDataCacher(mDataRecord, rdata);
	aos_assert_rl(mDataCacher, mLock, false);
	
	bool rslt = splitDataCacher(rdata);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return true;
}


bool
AosImportDataCtlrNorm::splitDataCacher(const AosRundataPtr &rdata)
{
	if (!mDataCacher) return true;

	mTotalLen = mDataCacher->getTotalFileLength();

	vector<AosDataCacherObjPtr> cacher;
	bool rslt = mDataCacher->split(cacher, rdata);
	aos_assert_r(rslt, false);
	
	AosImportDataThrdPtr thrd;
	AosImportDataCtlrPtr thisptr(this, true);
	u32 size = cacher.size();
	for(u32 i=0; i<size; i++)
	{
//		thrd = AosImportDataThrdNorm::createImportDataThrd(
//			cacher[i], thisptr, rdata);
//		aos_assert_r(thrd, false);
//		mImportDataThrds.push_back(thrd);
	}
	return true;
}


bool
AosImportDataCtlrNorm::start(const AosRundataPtr &rdata)
{
	mLock->lock();
	mStartTime = OmnGetSecond();
	OmnScreen << "import data ctlr start, start time:" << mStartTime << endl;

	bool rslt = sendStart(rdata);
	aos_assert_rl(rslt, mLock, false);
	
	u32 size = mImportDataThrds.size();
	aos_assert_rl(size > 0, mLock, false);

	vector<OmnThrdShellProcPtr> runners;
	OmnThrdShellProcPtr runner;
	for(u32 i=0; i<size; i++)
	{
		runner = (OmnThrdShellProc *)mImportDataThrds[i].getPtr(); 
		runners.push_back(runner);
	}

	rslt = addThreadShellProcAsync(runners, rdata);
	aos_assert_rl(rslt, mLock, false);

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
AosImportDataCtlrNorm::sendStart(const AosRundataPtr &rdata)
{
	bool rslt = true;
	AosDataAssemblerObjPtr iil_asm;
	map<OmnString, AosDataAssemblerObjPtr>::iterator itr = mIILAssemblers.begin();
	while (itr != mIILAssemblers.end())
	{
		iil_asm = itr->second;
		rslt = iil_asm->sendStart(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
		itr++;	
	}

	if (mDocAssembler)
	{
		rslt = mDocAssembler->sendStart(rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}
	
	return true;
}


bool
AosImportDataCtlrNorm::sendFinish(const AosRundataPtr &rdata)
{
	bool rslt = true;
	vector<OmnThrdShellProcPtr> runners;
	OmnThrdShellProcPtr runner;
	AosDataAssemblerObjPtr iil_asm;
	map<OmnString, AosDataAssemblerObjPtr>::iterator itr = mIILAssemblers.begin();
	while (itr != mIILAssemblers.end())
	{
		iil_asm = itr->second;
		runner = createDataAsmFinishThrd(iil_asm, rdata);
		runners.push_back(runner);
		itr++;	
	}

	if (mDocAssembler)
	{
		runner = createDataAsmFinishThrd(mDocAssembler, rdata);
		runners.push_back(runner);
	}

	rslt = addThreadShellProcSync(runners, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(!mAsmFinishError, false);

	return true;
}


bool
AosImportDataCtlrNorm::thrdFinished(
		const AosImportDataThrdPtr &thrd,
		const AosRundataPtr &rdata)
{
	// A thrd finished. The thrd must be in mImportDataThrds.
	// Otherwise, it is an error.
	mLock->lock();
	u32 size = mImportDataThrds.size();
	if (size == 0 && mIsFinished) 
	{
		mLock->unlock();
		return true;
	}
	
	bool found = false;
	u32 i = 0;
	for (; i<size; i++)
	{
		if (mImportDataThrds[i] == thrd)
		{
			found = true;
			break;
		}
	}

	aos_assert_rl(found, mLock, false);
	OmnScreen << "Import Data thrd finished:" << i << endl;

	// Check whether all finished
	bool all_finished = true;
	bool all_success = true;
	OmnString status_records;
	for (u32 i=0; i<size; i++)
	{
		status_records << mImportDataThrds[i]->getStatusRecord();
		
		if (!mImportDataThrds[i]->isFinished())
		{
			all_finished = false;
		}

		if (!mImportDataThrds[i]->isSuccess())
		{
			all_success = false;
		}
	}
	//Jozhi test taskFailed
	//all_success = false;

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
AosImportDataCtlrNorm::finish(
		const bool all_success,
		const OmnString &status_records,
		const AosRundataPtr &rdata)
{
	// The import is finished. It will create an XML doc for this import.
	// The doc format is:
	// 	<doc AOSTAG_NAME="xxx" 
	// 		AOSTAG_START_TIME="xxx"
	// 		AOSTAG_END_TIME="xxx"
	// 		AOSTAG_TIME_TAKEN="xxx"
	// 		AOSTAG_STATUS="true|false">
	// 		<AOSTAG_STATUS_RECORDS>
	// 			<record .../>
	// 			<record .../>
	// 			...
	// 		</AOSTAG_STATUS_RECORDS>
	// 		<AOSTAG_TASK_INFO>
	// 			the configuration XML
	// 		</AOSTAG_TASK_INFO>
	//	</doc>
	
	bool success = all_success;
	if (mIsFinished) 
	{
		return true;
	}

	bool rslt = sendFinish(rdata);
	if (rslt)
	{
		mIsFinished = true;
		mEndTime = OmnGetSecond();

		OmnString docstr;
		docstr << "<import_report "
			<< AOSTAG_START_TIME << "=\"" << mStartTime << "\" "
			<< AOSTAG_END_TIME << "=\"" << mEndTime << "\" "
			<< AOSTAG_TIME_TAKEN << "=\"" << mEndTime - mStartTime << "\" "
			<< AOSTAG_STATUS << "=\"" << success << "\">"
			<< "<" << AOSTAG_STATUS_RECORDS << ">" << status_records << "</" << AOSTAG_STATUS_RECORDS << ">"
			<< "</import_report>";

		OmnScreen << "Import Data Finished: " << endl << docstr << endl;
		updateTaskProcNum(rdata);
	}
	else
	{
		success = false;
		if (mListener)
		{
			mListener->importFinished(success, 0, rdata);
			return true;
		}
	}
	
	if (mListener)
	{
		mListener->importFinished(success, 0, rdata);
	}

	mTask = 0;
	mListener = 0;
	mDataCacher = 0;
	if (mDataRecord) mDataRecord->clear();
	if (mOutputRecord) mOutputRecord->clear();
	mDataRecord = 0;
	mOutputRecord = 0;
	mDocAssembler = 0;
	mIILAssemblers.clear();
	mDataProcs.clear();
	mImportDataThrds.clear();
	return true;
}


AosDataRecordObjPtr
AosImportDataCtlrNorm::cloneDataRecord(const AosRundataPtr &rdata) const
{
	if (!mDataRecord) return 0;
	return mDataRecord->clone(rdata.getPtr());
}


AosDataRecordObjPtr
AosImportDataCtlrNorm::cloneOutputRecord(const AosRundataPtr &rdata) const
{
	if (!mOutputRecord) return 0;
	return mOutputRecord->clone(rdata.getPtr());
}


bool
AosImportDataCtlrNorm::updateTaskProgress(const AosRundataPtr &rdata)
{
	aos_assert_r(mTask, false);
	
	u64 procLen = 0;
	u64 procTotal = 0;

	bool rslt = getProcTotal(procLen, procTotal, rdata);
	aos_assert_r(rslt, false);

	int progress = procLen * 100 / mTotalLen;
	if (progress > 100) progress = 100;

	if (mProgress >= progress)
	{
		return true;
	}
	mProgress = progress;
	
	OmnScreen << "import data ctlr progress:" << mProgress << endl;

	if (mProgress - mPrevProgress >= 3 || mProgress == 100)
	{
		mPrevProgress = mProgress;
		mTask->actionProgressed(mProgress, rdata);		
	}
	return true;
}


bool
AosImportDataCtlrNorm::getProcTotal(
		u64 &procLen,
		u64 &procTotal,
		const AosRundataPtr &rdata)
{
	procLen = 0;
	procTotal = 0;
	u32 size = mImportDataThrds.size();
	for (u32 i=0; i<size; i++)
	{
		AosImportDataThrdNorm * thrd = (AosImportDataThrdNorm *)(mImportDataThrds[i].getPtr());

		procLen += thrd->getProcLen();
		procTotal += thrd->getProcTotal();
	}
	return true;
}


bool
AosImportDataCtlrNorm::updateTaskProcNum(const AosRundataPtr &rdata)
{
	aos_assert_r(mProcKey != "", false);

	u64 procLen = 0;
	u64 procTotal = 0;
	bool rslt = getProcTotal(procLen, procTotal, rdata);
	aos_assert_r(rslt, false);
	
	OmnString str;
	str << "<entries total_len=\"" << mTotalLen << "\" "
		<< "noupdate_flag=\"" << mNoUpdateTaskProcNum << "\">"
		<< "<entry zky_key=\"" << mProcKey << "\">" << procTotal << "</entry>"
		<< "</entries>";
	OmnScreen << str << endl;
	
	AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	
	aos_assert_r(mTask, false);
	mTask->updateTaskProcNum(xml, mNoUpdateTaskProcNum, rdata);
	return true;
}


AosImportDataCtlrPtr
AosImportDataCtlrNorm::createImportDataCtlr(
		const AosXmlTagPtr &def, 
		const AosImportListenerPtr &listener,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	AosImportDataCtlrNorm * ctlr = OmnNew AosImportDataCtlrNorm(
		def, listener, task, rdata);
	bool rslt = ctlr->config(def, rdata);
	aos_assert_r(rslt, 0);
	return ctlr;
}
	

bool
AosImportDataCtlrNorm::checkConfig(
		const AosXmlTagPtr &def, 
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	aos_assert_r(task, false);
	
	vector<AosDataProcObjPtr> data_procs;
	map<OmnString, AosDataAssemblerObjPtr> iilAssemblers;

	OmnString key = def->getAttrStr(AOSTAG_KEY);
	if (key == "")
	{
		AosSetErrorU(rdata, "missing_proc_key:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr record = def->getFirstChild(AOSTAG_RECORD);
	if (!record)
	{
		AosSetErrorU(rdata, "missing record cfg:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosDataRecordObjPtr rec, out_rec;
	rec = AosDataRecordObj::createDataRecordStatic(record, task, rdata.getPtr());
	aos_assert_r(rec, false);
	
	AosXmlTagPtr entry;
	AosXmlTagPtr procs_def = def->getFirstChild(AOSTAG_DATA_PROCS);
	if (procs_def && (entry = procs_def->getFirstChild()))
	{
		AosDataProcObjPtr proc;
		AosDataAssemblerObjPtr iil_asm;
		OmnString dataColId, config_id;
		AosXmlTagPtr dataColTag, proc_def;
		
		AosXmlTagPtr extension_config, extension_tag;
		OmnString extension_objid = procs_def->getAttrStr(AOSTAG_EXTENSION_CONFIG_OBJID);
		if (extension_objid != "")
		{
			extension_config = AosGetDocByObjid(extension_objid, rdata);
			if (!extension_config)
			{
				AosSetErrorU(rdata, "can't_get_extension_config") << extension_objid;
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			extension_config = extension_config->clone(AosMemoryCheckerArgsBegin);
		}
		
		while (entry)
		{
			proc_def = resolveDataProcConfig(entry, extension_config, rdata);
			aos_assert_r(proc_def, false);
			
			dataColId = proc_def->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
			if (dataColId != "" && (iilAssemblers.find(dataColId) == iilAssemblers.end()))
			{
				dataColTag = task->getDataColTag(dataColId, rdata);
				//aos_assert_r(dataColTag, false);
				if (!dataColTag)
				{
					AosSetEntityError(rdata, "importdatactlrnorm_missing_datacol_tag", 
						"ImportData", "ImportDataCtlrNorm") << dataColId << enderr;
					return false;
				}
					
				iil_asm = AosCreateIILAssembler(rdata.getPtrNoLock(), dataColId, task, dataColTag);
				aos_assert_r(iil_asm, false);

				iilAssemblers[dataColId] = iil_asm;
			}
			
			proc = AosDataProcObj::createDataProcStatic(proc_def, rdata);
			aos_assert_r(proc, false);
			data_procs.push_back(proc);
			
			entry = procs_def->getNextChild();
		}
	}
	
	bool rslt = true;
	bool needDocid = false;
	bool needConvert = false;
	for(u32 i=0; i<data_procs.size(); i++)
	{
		rslt = data_procs[i]->resolveDataProc(iilAssemblers, rec, rdata);
		aos_assert_r(rslt, false);
		
		if (!needDocid) needDocid = data_procs[i]->needDocid();
		if (!needConvert) needConvert = data_procs[i]->needConvert();
	}
	
	if (needDocid && !needConvert)
	{
		AosSetEntityError(rdata, "importdatactlrnorm_need convert record", 
				"Task", "Task Controller") << def << enderr;
		return false;
	}
	
	AosDataAssemblerObjPtr doc_asm;
	AosXmlTagPtr doc_asb = def->getFirstChild(AOSTAG_DOC_ASSEMBLER);
	if (doc_asb)
	{
		OmnString type = doc_asb->getAttrStr(AOSTAG_TYPE);
		if (AosDataAssemblerType::isValidDocAsmType(type))
		{
			doc_asm = AosCreateDocAssembler(rdata.getPtrNoLock(), key, task, doc_asb);
			aos_assert_r(doc_asm, false);
		
			out_rec = doc_asm->cloneDataRecord(rdata.getPtrNoLock());
			aos_assert_r(out_rec, false);
		}
	}
	
	if (needDocid || needConvert)
	{
		aos_assert_r(doc_asm && out_rec, false);
	}
	
	doc_asm = 0;
	if (rec) rec->clear();
	if (out_rec) out_rec->clear(); 
	return true;
}

