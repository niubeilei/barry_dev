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
// This dataset uses one data scanner as its inputs. The data scanner
// can be any type. Most datasets can be implemented by this class.
//
// Modification History:
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Jimos/DatasetByDataScanner.h"

#include "API/AosApi.h"
#include "DocClient/DocidMgr.h"
#include "Job/Job.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataSplitterObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/SchemaObj.h"
#include "TaskMgr/Task.h"
#include "XmlUtil/XmlTag.h"
#include "DataRecord/Recordset2.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByDataScanner_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByDataScanner(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosDatasetByDataScanner::AosDatasetByDataScanner(
		const OmnString &type, 
		const int version)
:
AosDataset(type, version)
{
}


AosDatasetByDataScanner::AosDatasetByDataScanner(const int version)
:
AosDataset(AOSDATASET_DATASCANNER, version)
{
}


AosDatasetByDataScanner::~AosDatasetByDataScanner()
{
}


bool
AosDatasetByDataScanner::config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc) 
{
	// 	<conf ..>
	// 		<recordset .../>
	// 		<schema .../>	
	// 		<data_scanner .../>	
	// 	</conf>
//	aos_assert_r(worker_doc, false);

	aos_assert_r(worker_doc, false);
	mConf = worker_doc->clone(AosMemoryCheckerArgsBegin);

	aos_assert_r(mConf, false);
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);

	mLock = OmnNew OmnMutex();
	mCondVar = OmnNew OmnCondVar();
	mOffset = 0;
	mDataBuff = 0;
	mRemainingBuff = 0;
	mNumEntries = 0;
	mStatus = eFatchingData;
	mFinished = false;
	mUnrecNumEntries = 0;
	mProcSize = 0;

	OmnTagFuncInfo << "worker doc is: " << worker_doc->toString() << endl;

	mObjid = mConf->getAttrStr(AOSTAG_OBJID, "");
	
	mNumEntriesInDoc = mConf->getAttrInt64("num_entries", -1);
	mName = mConf->getAttrStr(AOSTAG_NAME, "");

	// mMaxRecordsets
	mMaxRecordsets = mConf->getAttrInt("zky_max_record_sets", eDftMaxRecordsets);

	// create Recordset
	AosXmlTagPtr recordset_conf = mConf->getFirstChild("recordset");
	if (!recordset_conf)
	{
		OmnString str = "<recordset />";
		AosXmlParser xmlparser;
		recordset_conf = xmlparser.parse(str, "" AosMemoryCheckerArgs);
	}
	aos_assert_r(recordset_conf, false);

	mRecordset = AosRecordsetObj::createRecordsetStatic(mRundata.getPtrNoLock(), recordset_conf);
	aos_assert_r(mRecordset, false);

	// create mSchema
	AosXmlTagPtr schema_conf = mConf->getFirstChild("dataschema");
	aos_assert_r(schema_conf, false);

	mSchema = AosSchemaObj::createSchemaStatic(mRundata.getPtr(), schema_conf);
	aos_assert_r(mSchema, false);
	mSchema->setTaskDocid(mTaskDocid);

	// Create mDataScanner. 
	AosXmlTagPtr scanner_conf = mConf->getFirstChild("datascanner");
	aos_assert_rr(scanner_conf, rdata, false);
	
	mDataScanner = AosCreateDataScanner(mRundata.getPtr(), scanner_conf);
	aos_assert_r(mDataScanner, false);
	mDataScanner->setTargetReporter(this);
	//Linda, 2014/05/09
	mDataScanner->setTaskDocid(mTaskDocid);
	bool rslt = mDataScanner->config(mRundata, scanner_conf);
	//Jozhi 2014-08-12 
	//data scanner may be connfig failed because remote file is invalid
	if (!rslt)
	{
		OmnAlarm << "may be config error" << enderr;
	}
/*
	AosXmlTagPtr doc_asm = mConf->getFirstChild("zky_docassembler");
	if (doc_asm)
	{
		mErrorRcdCollector = AosDataAssembler::createDocVarAssembler(mTask, doc_asm, rdata);
	}
*/	
//	OmnThreadedObjPtr thisPtr(this, false);
//	mThread = OmnNew OmnThread(thisPtr, "DatasetByDataScanner", 0, true, true, __FILE__, __LINE__);
//	mThread->start();
	return true;
}


void 
AosDatasetByDataScanner::setTaskDocid(const u64 task_docid)
{
	mTaskDocid = task_docid;
	//Linda, 2014/05/09
	//mSchema->setTask(mTask);
	//mDataScanner->setTask(mTask);
}


bool 
AosDatasetByDataScanner::reset(const AosRundataPtr &rdata)
{
	mFinished = false;
	aos_assert_rr(mDataScanner, rdata, false);
	return mDataScanner->reset(rdata);
}


bool
AosDatasetByDataScanner::nextRecordset(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &recordset)
{
	AosBuffDataPtr buffdata;
	AosSchemaObjPtr schema;
	aos_assert_r(mDataScanner,false);
	int64_t size;
	if (!recordset)
	{
		AosRecordset2 * recordset2 = OmnNew AosRecordset2(); 
		recordset= recordset2;
		aos_assert_r(recordset, false);

		schema = mSchema->clone(rdata.getPtr() AosMemoryCheckerArgs);
		recordset->setSchema(rdata.getPtr(), schema);

	}

	else
	{
		bool rslt = recordset->getBuffData(buffdata);
		aos_assert_r(buffdata, false);

		size = buffdata->getBuff()->getCrtIdx();
		rslt = mDataScanner->setRemainingBuff(buffdata, recordset->getIncomplete(), size, rdata);

		recordset->reset();
	}


	if (mDataScanner->getDiskError())
	{
		AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
		aos_assert_r(task, false);
		task->setErrorType(AosTaskErrorType::eInPutError);
		task->taskFailed(rdata);
		return true;
	}
	
	bool rslt = mDataScanner->getNextBlock(buffdata, rdata);

	if (!rslt)
	{
		if (mTaskDocid)
		{
			AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
			aos_assert_r(task, false);
			task->setErrorType(AosTaskErrorType::eInPutError);
			task->taskFailed(rdata);
		}
		return true;
	}

	if (!buffdata)
	{
		// There are no more data. It finishes this thread
		readingFinished();
		if (mFinished)
		{
OmnScreen << "***********reading finish *****************" << endl;
			recordset = 0;
		}
		return true;
	}

	aos_assert_r(buffdata->dataLen() > 0, false);

	//AosMetaDataPtr metadata = buffdata->getMetadata();
	//newbuffdata->setMetadata(metadata);
	recordset->setBuffData(rdata.getPtr(), buffdata);
	return  true;
#if 0
	if (recordset)
	{
		recordset->reset();
	}

	int64_t total_procsize = 0;
	while (AosRecordsetObj::checkEmpty(recordset))
	{
		int64_t read_size = 0;
		int64_t remain_size = 0;
		AosBuffDataPtr buffdata;
//OmnScreen << " ###### " << this << " dataset start get block " << mDataScanner.getPtr() << endl;
		if (mDataScanner->getDiskError())
		{
			AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
			aos_assert_r(task, false);
			task->setErrorType(AosTaskErrorType::eInPutError);
			task->taskFailed(rdata);
			return true;
		}
		bool rslt = mDataScanner->getNextBlock(buffdata, rdata);
		//Jozhi 2014-10-17
		//aos_assert_r(rslt, false);
		if (!rslt)
		{
			if (mTaskDocid)
			{
				AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
				aos_assert_r(task, false);
				task->setErrorType(AosTaskErrorType::eInPutError);
				task->taskFailed(rdata);
			}
			return true;
		}

		if (!buffdata)
		{
			// There are no more data. It finishes this thread
//OmnScreen << " ###### " << this << " dataset get null data  " << endl;
			readingFinished();
			return true;
		}

		aos_assert_r(buffdata->dataLen() > 0, false);
		
		read_size = buffdata->dataLen() - buffdata->getCrtIdx();

		// Ketty 2014/05/07
		//rslt = mSchema->nextRecordset(rdata, recordset, buffdata, this, incomplete);
		//aos_assert_r(rslt, false);
		bool incomplete = false;
		//rslt = mSchema->appendData(rdata, buffdata);
		//aos_assert_r(rslt, false);
		
		rslt = mSchema->nextRecordset(rdata.getPtr(), recordset, buffdata, this, incomplete);
		aos_assert_r(rslt, false);

		remain_size = buffdata->dataLen() - buffdata->getCrtIdx();
		aos_assert_r(remain_size >= 0, false);
		
		int64_t size = read_size - remain_size;
		aos_assert_r(size>=0, false);
		total_procsize += size;
//OmnScreen << " ###### " << this << "read size " << read_size << " proc size : " << size << " remain_size : " << remain_size << endl;
		rslt = mDataScanner->setRemainingBuff(buffdata, incomplete, size, rdata);
		aos_assert_r(rslt, false);
	}

	mLock->lock();
	mProcSize += total_procsize;
	mLock->unlock();
	if (recordset)
	{
		mLock->lock();
		mNumEntries += recordset->size();
		mLock->unlock();
//OmnScreen << " ###### " << this << " mNumEntries : " << mNumEntries << endl;
		if (mFinished)
		{
			readingFinished();
		}
	}
	return true;
#endif
}

bool
AosDatasetByDataScanner::sendStart(const AosRundataPtr &rdata)
{
	if (mTaskDocid)
		showDatasetInfo(__FILE__, __LINE__, "start");
	aos_assert_r(mDataScanner, false);
	if (mDataScanner->getDiskError())
	{
		aos_assert_r(mTaskDocid, false);
		AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
		aos_assert_r(task, false);
		task->setErrorType(AosTaskErrorType::eInPutError);
		task->taskFailed(rdata);
		return true;
	}
	mDataScanner->startReadData(rdata);
	return true;
}

bool
AosDatasetByDataScanner::sendFinish(const AosRundataPtr &rdata)
{
	showDatasetInfo(__FILE__, __LINE__, "finish");
	return true;
}

int64_t 
AosDatasetByDataScanner::getNumEntries()
{
	// It assumes the number of entries is stored in the table doc.
	return mNumEntriesInDoc;
}


int64_t 
AosDatasetByDataScanner::calculateNumEntries(const AosRundataPtr &rdata)
{
	AosRecordsetObjPtr recordset = mRecordset->clone(rdata.getPtr());
	aos_assert_r(recordset, -1);
	while (recordset)
	{
		bool rslt = nextRecordset(rdata, recordset);
		aos_assert_r(rslt, -1);
	}

	return mNumEntriesInDoc;
}


bool
AosDatasetByDataScanner::readingFinished()
{
	mLock->lock();
// u32 fnum = mFilterRecords.size();
// u32 inum = mInvalidRecords.size();
//u32 unum = mErrorRecords.size();
// u32 total = fnum + inum + mUnrecNumEntries + mNumEntries;
u32 total = mNumEntries;
OmnScreen << " ########################################## " << endl;
OmnScreen << " dataset : " << this << endl;
OmnScreen << " name : " << mName << endl;
OmnScreen << " unrecog records num : " << mUnrecNumEntries << endl;
OmnScreen << " proc    records num : " << mNumEntries << endl;
OmnScreen << " total   records num : " << total << endl;
OmnScreen << " total   proc size : " << mProcSize << endl;
OmnScreen << " ########################################## " << endl;
	mFinished = true;
	mDataBuff = NULL;
	mRemainingBuff = NULL;
	mRecordset = NULL;
	aos_assert_r(mRecordsets.empty(), false);
	aos_assert_r(mBuffQueue.empty(), false);
	mLock->unlock();
	return true;
	if (mNumEntries != mNumEntriesInDoc)
	{
//		mConf->setAttr("num_entries", mNumEntries);
//		bool rslt = AosDocClient::getSelf()->modifyObj(mConf, mRundata);
//		aos_assert_r(rslt, false);
		mNumEntriesInDoc = mNumEntries;
	}
//	mErrorRcdCollector->sendFinish(mRundata);
	mLock->unlock();
	return true;
}

bool 
AosDatasetByDataScanner::addUnrecogContents(
	const AosRundataPtr &rdata,
	const AosBuffDataPtr &buff_data,
	const char *data,
	const int64_t &start_pos,
	const int64_t &length)
{
	// This function checks whether there is an invalid record collector.
	// If not, it does nothing. 
	
	// for test
//	OmnString errostr(&data[start_pos], length);
//	OmnScreen << " ******* error record ********" << endl;
//	OmnScreen << errostr << endl;
//	OmnScreen << " *****************************" << endl;
	mUnrecNumEntries++;
//	mErrorRecords.push_back(errostr);
	// ///////////
return true;
/*
	if (!mErrorRcdCollector) return true;

	u64 docid = mErrorRcdCollector->getNextDocid(rdata);
	aos_assert_r(docid, false);

	OmnString data_type = buff_data->getDataType();
	OmnString data_name = buff_data->getDataName();
	int offset = buff_data->getCrtBlockIdx();

	OmnString error_type = "unrecognized_record";
	OmnString entry = "<record ";
	entry << "data_type=\"" << data_type << "\" "
		  << "data_name=\"" << data_name << "\" "
		  << "error_type=\"" << error_type << "\" "
		  << "block_offset=\"" << offset << "\" "
		  << "record_offset=\"" << start_pos << "\" "
		  << "record_lenght=\"" << length << "\">"
		  << "<![BDATA[" << length << ":";
	memcpy(&entry[entry.length()-1], &data[start_pos], length);
	entry << "]]></record>";

	AosValueRslt valueRslt;
	valueRslt.setDocid(docid);
	valueRslt.setCharStr1(entry.data(), entry.length(), false);
	mErrorRcdCollector->appendEntry(valueRslt, rdata);

	return true;
*/
}


bool 
AosDatasetByDataScanner::addInvalidContents(
	const AosRundataPtr &rdata,
	AosDataRecordObj * record)
{
	OmnNotImplementedYet;
	return false;

	/*
	// This function checks whether there is an invalid record collector.
	// If not, it does nothing. 
	
	char * data = record->getData(rdata.getPtrNoLock());
	int length = record->getRecordLen();
	
	// for test
	OmnString errostr(&data[0], length);
	OmnScreen << " ******* invalid record ********" << endl;
	OmnScreen << errostr << endl;
	OmnScreen << " *****************************" << endl;

	mInvalidRecords.push_back(errostr);
	*/
	// ///////////
return true;
/*
	u64 docid = mErrorRcdCollector->getNextDocid(rdata);
	aos_assert_r(docid, false);

	OmnString error_type = "Invalid_record";
	OmnString entry = "<record ";
	entry << "block_offset=\"" << offset << "\" "
		  << "error_type=\"" << error_type << "\" "
		  << "record_offset=\"" << offset << "\" "
		  << "record_lenght=\"" << length << "\">"
		  << "<![BDATA[" << length << ":";
	memcpy(&entry[entry.length()-1], &data[offset], length);
	entry << "]]></record>";

	AosValueRslt valueRslt;
	valueRslt.setDocid(docid);
	valueRslt.setCharStr1(entry.data(), entry.length(), false);
	mErrorRcdCollector->appendEntry(valueRslt, rdata);
	return true;
*/
}


bool 
AosDatasetByDataScanner::addFilterContents(
	const AosRundataPtr &rdata,
	AosDataRecordObj * record)
{
	OmnNotImplementedYet;
	return false;
	/*
	// This function checks whether there is an invalid record collector.
	// If not, it does nothing. 
	
	
	char * data = record->getData(rdata.getPtrNoLock());
	int length = record->getRecordLen();
	
	// for test
	OmnString errostr(&data[0], length);
	OmnScreen << " ******* filter record ********" << endl;
	OmnScreen << errostr << endl;
	OmnScreen << " *****************************" << endl;

	mFilterRecords.push_back(errostr);
	// ///////////
return true;
*/
/*
	u64 docid = mErrorRcdCollector->getNextDocid(rdata);
	aos_assert_r(docid, false);

	OmnString error_type = "filtered_record";
	OmnString entry = "<record ";
	entry << "block_offset=\"" << offset << "\" "
		  << "error_type=\"" << error_type << "\" "
		  << "record_offset=\"" << offset << "\" "
		  << "record_lenght=\"" << length << "\">"
		  << "<![BDATA[" << length << ":";
	memcpy(&entry[entry.length()-1], &data[offset], length);
	entry << "]]></record>";

	AosValueRslt valueRslt;
	valueRslt.setDocid(docid);
	valueRslt.setCharStr1(entry.data(), entry.length(), false);
	mErrorRcdCollector->appendEntry(valueRslt, rdata);
	return true;
*/
}

int64_t
AosDatasetByDataScanner::getTotalSize() const
{
	aos_assert_r(mDataScanner, -1);
	return mDataScanner->getTotalSize();
}
	
bool
AosDatasetByDataScanner::getRecord(const OmnString &name, AosDataRecordObjPtr &record)
{
	aos_assert_r(mSchema, false);
	return mSchema->getRecord(name, record);
}

bool
AosDatasetByDataScanner::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_r(mSchema, false);
	return mSchema->getRecords(records);
}


bool 
AosDatasetByDataScanner::addDataConnector(
		const AosRundataPtr &rdata, 
	 	const AosDataConnectorObjPtr &data_cube)
{
	aos_assert_rr(mDataScanner, rdata, false);
	return mDataScanner->addDataConnector(rdata, data_cube);
}


bool
AosDatasetByDataScanner::setValueBuff(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	return mDataScanner->setValueBuff(buff, rdata);
}


AosJimoPtr
AosDatasetByDataScanner::cloneJimo()  const
{
	try
	{
		return OmnNew AosDatasetByDataScanner(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


void
AosDatasetByDataScanner::showDatasetInfo(
		const char *file,
		const int line,
		const OmnString &action)
{
	report(file, line, action, "");
}


void
AosDatasetByDataScanner::report(
		const char *file,
		const int line,
		const OmnString &action,
		const OmnString &msg)
{
	OmnString newmsg;
	newmsg << " DATAS: " << mName << msg;
	if (mTargetReporter)
		mTargetReporter->report(file, line, action, newmsg);
}


bool
AosDatasetByDataScanner::stop(
		const AosRundataPtr &rdata,
		const AosRecordsetObjPtr &recordset)
{
	aos_assert_r(recordset, false);
	AosBuffDataPtr buffdata;
	bool rslt = recordset->getBuffData(buffdata);
	aos_assert_r(buffdata, false);

	int64_t size = buffdata->getBuff()->getCrtIdx();
	rslt = mDataScanner->setRemainingBuff(buffdata, recordset->getIncomplete(), size, rdata);
	aos_assert_r(rslt, false);

	recordset->reset();
	return true;
}
