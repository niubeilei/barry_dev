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
#include "ImportData/ImportDataCtlrTasks.h"

#include "API/AosApi.h"
#include "DataAssembler/DataAssembler.h"
#include "DataAssembler/Ptrs.h"
#include "ImportData/ImportListener.h"
#include "ImportData/ImportDataThrdTasks.h"
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


AosImportDataCtlrTasks::AosImportDataCtlrTasks(
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


AosImportDataCtlrTasks::~AosImportDataCtlrTasks()
{
}


bool
AosImportDataCtlrTasks::config(
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
	mMap.clear();
	
	mNoUpdateTaskProcNum = def->getAttrBool(AOSTAG_NOUPDATETASKPROCNUM, false);
	
	mKeyIdx = def->getAttrInt("zky_key_idx", -1);
	if (mKeyIdx < 0)
	{
		AosSetErrorU(rdata, "missing_key_idx:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mKeyLen = def->getAttrInt("zky_key_len", -1);
	if (mKeyLen <= 0)
	{
		AosSetErrorU(rdata, "missing_key_len:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	int len;
	bool rslt;
	OmnString key, sdoc_objid, dataColId, type, extension_objid;
	AosXmlTagPtr entry, xml, dataColTag, record, procs_def, proc_def, doc_asb;
	AosXmlTagPtr extension_config, extension_tag;
	AosDataProcObjPtr proc;
	AosDataAssemblerObjPtr iil_asm;
	AosDataAssemblerObjPtr doc_asm;
	AosDataRecordObjPtr rec, out_rec;
	vector<AosDataProcObjPtr> data_procs;

	AosXmlTagPtr map_tag = def->getFirstChild();
	if (!map_tag)
	{
		AosSetErrorU(rdata, "map is empty:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	while (map_tag)
	{
		key = map_tag->getAttrStr(AOSTAG_KEY);
		if (key == "")
		{
			AosSetErrorU(rdata, "missing_proc_key:") << map_tag->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		sdoc_objid = map_tag->getNodeText();
		if (sdoc_objid == "")
		{
			AosSetErrorU(rdata, "missing_sdoc_objid:") << map_tag->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		xml = AosGetDocByObjid(sdoc_objid, rdata);
		if (!xml)
		{
			AosSetErrorU(rdata, "can not find sdoc:") << sdoc_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		
		// Retrieve the record
		record = xml->getFirstChild(AOSTAG_RECORD);
		if (!record)
		{
			AosSetErrorU(rdata, "missing record cfg:") << xml->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	
		rec = AosDataRecordObj::createDataRecordStatic(record, mTask, rdata.getPtr());
		aos_assert_r(rec, false);
		
		len = rec->getRecordLen();
		aos_assert_r(len > 0, false);

		key << "_" << len;
		aos_assert_r(mMap.find(key) == mMap.end(), false);
		
		data_procs.clear();
		procs_def = xml->getFirstChild(AOSTAG_DATA_PROCS);
		if (procs_def && (entry = procs_def->getFirstChild(true)))
		{
			extension_config = 0;
			extension_objid = procs_def->getAttrStr(AOSTAG_EXTENSION_CONFIG_OBJID);
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
				data_procs.push_back(proc);
			
				entry = procs_def->getNextChild();
			}
		}
		
		bool needDocid = false;
		bool needConvert = false;
		for(u32 i=0; i<data_procs.size(); i++)
		{
			rslt = data_procs[i]->resolveDataProc(mIILAssemblers, rec, rdata);
			aos_assert_r(rslt, false);
		
			if (!needDocid) needDocid = data_procs[i]->needDocid();
			if (!needConvert) needConvert = data_procs[i]->needConvert();
		}
	
		if (needDocid && !needConvert)
		{
			AosSetErrorU(rdata, "need convert record:") << xml->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	
		doc_asb = xml->getFirstChild(AOSTAG_DOC_ASSEMBLER);
		if (doc_asb)
		{
			type = doc_asb->getAttrStr(AOSTAG_TYPE);
			if (AosDataAssemblerType::isValidDocAsmType(type))
			{
				doc_asm = AosCreateDocAssembler(rdata.getPtrNoLock(), key, mTask, doc_asb);
				aos_assert_r(doc_asm, false);
			
				out_rec = doc_asm->cloneDataRecord(rdata.getPtrNoLock());
				aos_assert_r(out_rec, false);
			}
		}
	
		if (needDocid || needConvert)
		{
			aos_assert_r(doc_asm && out_rec, false);
		}
		
		procGroup proc_group;
		proc_group.mDataRecord = rec;
		proc_group.mOutputRecord = out_rec;
		proc_group.mDocAssembler = doc_asm;
		proc_group.mDataProcs = data_procs;
	
		mMap[key] = proc_group;
		map_tag = def->getNextChild();
	}

	rslt = splitDataCacher(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosImportDataCtlrTasks::setFileInfo(
		const AosTaskDataObjPtr &task_data,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mDataCacher = task_data->createDataCacher(0, rdata);
	aos_assert_rl(mDataCacher, mLock, false);
	
	bool rslt = splitDataCacher(rdata);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return true;
}


bool
AosImportDataCtlrTasks::splitDataCacher(const AosRundataPtr &rdata)
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
		//thrd = AosImportDataThrdTasks::createImportDataThrd(
		//	cacher[i], thisptr, rdata);
		//aos_assert_r(thrd, false);
		//mImportDataThrds.push_back(thrd);
	}
	return true;
}


bool
AosImportDataCtlrTasks::start(const AosRundataPtr &rdata)
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
AosImportDataCtlrTasks::sendStart(const AosRundataPtr &rdata)
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

	AosDataAssemblerObjPtr doc_asm;
	map<OmnString, procGroup>::iterator itr2 = mMap.begin();
	while (itr2 != mMap.end())
	{
		doc_asm = itr2->second.mDocAssembler;
		if (doc_asm)
		{
			rslt = doc_asm->sendStart(rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
		}
		itr2++;
	}
	
	return true;
}


bool
AosImportDataCtlrTasks::sendFinish(const AosRundataPtr &rdata)
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

	AosDataAssemblerObjPtr doc_asm;
	map<OmnString, procGroup>::iterator itr2 = mMap.begin();
	while (itr2 != mMap.end())
	{
		doc_asm = itr2->second.mDocAssembler;
		if (doc_asm)
		{
			runner = createDataAsmFinishThrd(doc_asm, rdata);
			runners.push_back(runner);
		}
		itr2++;
	}

	rslt = addThreadShellProcSync(runners, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(!mAsmFinishError, false);

	return true;
}


bool
AosImportDataCtlrTasks::thrdFinished(
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
AosImportDataCtlrTasks::finish(
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
	
	if (mIsFinished) 
	{
		return true;
	}

	bool rslt = sendFinish(rdata);
	aos_assert_r(rslt, false);
	
	mIsFinished = true;
	mEndTime = OmnGetSecond();

	OmnString docstr;
	docstr << "<import_report "
		   << AOSTAG_START_TIME << "=\"" << mStartTime << "\" "
		   << AOSTAG_END_TIME << "=\"" << mEndTime << "\" "
		   << AOSTAG_TIME_TAKEN << "=\"" << mEndTime - mStartTime << "\" "
		   << AOSTAG_STATUS << "=\"" << all_success << "\">"
		   << "<" << AOSTAG_STATUS_RECORDS << ">" << status_records << "</" << AOSTAG_STATUS_RECORDS << ">"
		   << "</import_report>";

	OmnScreen << "Import Data Finished: " << endl << docstr << endl;
	
	updateTaskProcNum(rdata);
	
	if (mListener)
	{
		mListener->importFinished(all_success, 0, rdata);
	}

	mTask = 0;
	mListener = 0;
	mDataCacher = 0;
	map<OmnString, procGroup>::iterator itr = mMap.begin();
	while (itr != mMap.end())
	{
		if (itr->second.mDataRecord) itr->second.mDataRecord->clear();
		if (itr->second.mOutputRecord) itr->second.mOutputRecord->clear();
		itr->second.mDataRecord = 0;
		itr->second.mOutputRecord = 0;
		itr->second.mDocAssembler = 0;
		itr->second.mDataProcs.clear();
		itr++;
	}
	mIILAssemblers.clear();
	mMap.clear();
	mImportDataThrds.clear();
	return true;
}


bool
AosImportDataCtlrTasks::updateTaskProgress(const AosRundataPtr &rdata)
{
	aos_assert_r(mTask, false);
	
	u64 procLen = 0;
	map<OmnString, u64> procTotal;
	map<OmnString, u64>::iterator itr;

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
		OmnString sc = "---------------------------\n";
		for(itr = procTotal.begin(); itr != procTotal.end(); itr++)
		{
			sc << "---" << itr->first << " : " << itr->second << "---\n";
		}
		sc << "---------------------------\n";
		OmnScreen << sc << endl;

		mPrevProgress = mProgress;
		mTask->actionProgressed(mProgress, rdata);		
	}
	return true;
}

	
bool
AosImportDataCtlrTasks::getProcTotal(
		u64 &procLen,
		map<OmnString, u64> &procTotal,
		const AosRundataPtr &rdata)
{
	map<OmnString, u64> thrd_procTotal;
	map<OmnString, u64>::iterator itr;
	OmnString key;
	u64 v1, v2;
	procLen = 0;
	procTotal.clear();
	u32 size = mImportDataThrds.size();
	for (u32 i=0; i<size; i++)
	{
		AosImportDataThrdTasks * thrd = (AosImportDataThrdTasks *)(mImportDataThrds[i].getPtr());

		procLen += thrd->getProcLen();
		thrd_procTotal = thrd->getProcTotal();
		
		for(itr = thrd_procTotal.begin(); itr != thrd_procTotal.end(); itr++)
		{
			key = itr->first;
			v1 = itr->second;

			if (procTotal.find(key) == procTotal.end())
			{
				procTotal[key] = v1;
			}
			else
			{
				v2 = procTotal[key];
				procTotal[key] = v1 + v2;
			}
		}
	}
	return true;
}


bool
AosImportDataCtlrTasks::updateTaskProcNum(const AosRundataPtr &rdata)
{
	u64 procLen = 0;
	map<OmnString, u64> procTotal;
	bool rslt = getProcTotal(procLen, procTotal, rdata);
	aos_assert_r(rslt, false);
	
	OmnString str;
	str << "<entries total_len=\"" << mTotalLen << "\" "
		<< "noupdate_flag=\"" << mNoUpdateTaskProcNum << "\">";
	for(map<OmnString, u64>::iterator itr = procTotal.begin(); itr != procTotal.end(); itr++)
	{
		str << "<entry zky_key=\"" << itr->first << "\">" << itr->second << "</entry>";
	}
	str << "</entries>";
	OmnScreen << str << endl;
	
	AosXmlTagPtr xml = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	
	aos_assert_r(mTask, false);
	mTask->updateTaskProcNum(xml, mNoUpdateTaskProcNum, rdata);
	return true;
}


AosImportDataCtlrPtr
AosImportDataCtlrTasks::createImportDataCtlr(
		const AosXmlTagPtr &def, 
		const AosImportListenerPtr &listener,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	AosImportDataCtlrTasks * ctlr = OmnNew AosImportDataCtlrTasks(
		def, listener, task, rdata);
	bool rslt = ctlr->config(def, rdata);
	aos_assert_r(rslt, 0);
	return ctlr;
}
	

bool
AosImportDataCtlrTasks::checkConfig(
		const AosXmlTagPtr &def, 
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	aos_assert_r(task, false);
	
	int keyIdx = def->getAttrInt("zky_key_idx", -1);
	if (keyIdx < 0)
	{
		AosSetErrorU(rdata, "missing_key_idx:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int keyLen = def->getAttrInt("zky_key_len", -1);
	if (keyLen <= 0)
	{
		AosSetErrorU(rdata, "missing_key_len:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int len;
	bool rslt = true;
	OmnString key, sdoc_objid, dataColId, type, extension_objid;
	AosXmlTagPtr entry, xml, dataColTag, record, procs_def, proc_def, doc_asb;
	AosXmlTagPtr extension_config, extension_tag;
	AosDataProcObjPtr proc;
	AosDataRecordObjPtr rec, out_rec;
	AosDataAssemblerObjPtr iil_asm;
	AosDataAssemblerObjPtr doc_asm;
	map<OmnString, AosDataAssemblerObjPtr> iil_asms;
	vector<AosDataProcObjPtr> data_procs;
	set<OmnString> keys;	

	AosXmlTagPtr map_tag = def->getFirstChild();
	if (!map_tag)
	{
		AosSetErrorU(rdata, "map is empty:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	while(map_tag)
	{
		key = map_tag->getAttrStr("zky_key");
		if (key == "")
		{
			AosSetErrorU(rdata, "missing_proc_key:") << map_tag->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		sdoc_objid = map_tag->getNodeText();
		if (sdoc_objid == "")
		{
			AosSetErrorU(rdata, "missing_sdoc_objid:") << map_tag->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		xml = AosGetDocByObjid(sdoc_objid, rdata);
		if (!xml)
		{
			AosSetErrorU(rdata, "can not find sdoc:") << sdoc_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	
		// Retrieve the record
		record = xml->getFirstChild(AOSTAG_RECORD);
		if (!record)
		{
			AosSetErrorU(rdata, "missing record cfg:") << xml->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	
		rec = AosDataRecordObj::createDataRecordStatic(record, task, rdata.getPtr());
		aos_assert_r(rec, false);

		len = rec->getRecordLen();
		aos_assert_r(len > 0, false);
		
		key << "_" << len;
		aos_assert_r(keys.find(key) == keys.end(), false);

		data_procs.clear();
		procs_def = xml->getFirstChild(AOSTAG_DATA_PROCS);
		if (procs_def && (entry = procs_def->getFirstChild(true)))
		{
			extension_config = 0;
			extension_objid = procs_def->getAttrStr(AOSTAG_EXTENSION_CONFIG_OBJID);
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
				if (dataColId != "" && (iil_asms.find(dataColId) == iil_asms.end()))
				{
					dataColTag = task->getDataColTag(dataColId, rdata);
					aos_assert_r(dataColTag, false);
					
					iil_asm = AosCreateIILAssembler(rdata.getPtrNoLock(), dataColId, task, dataColTag);
					aos_assert_r(iil_asm, false);

					iil_asms[dataColId] = iil_asm;
				}
			
				proc = AosDataProcObj::createDataProcStatic(proc_def, rdata);
				aos_assert_r(proc, false);
				data_procs.push_back(proc);
			
				entry = procs_def->getNextChild();
			}
		}
		
		bool needDocid = false;
		bool needConvert = false;
		for(u32 i=0; i<data_procs.size(); i++)
		{
			rslt = data_procs[i]->resolveDataProc(iil_asms, rec, rdata);
			aos_assert_r(rslt, false);
			
			if (!needDocid) needDocid = data_procs[i]->needDocid();
			if (!needConvert) needConvert = data_procs[i]->needConvert();
		}
	
		if (needDocid && !needConvert)
		{
			AosSetErrorU(rdata, "need convert record:") << xml->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	
		doc_asb = xml->getFirstChild(AOSTAG_DOC_ASSEMBLER);
		if (doc_asb)
		{
			type = doc_asb->getAttrStr(AOSTAG_TYPE);
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

		keys.insert(key);
	
		doc_asm = 0;
		if (rec) rec->clear();
		if (out_rec) out_rec->clear();
		
		map_tag = def->getNextChild();
	}

	return true;
}

