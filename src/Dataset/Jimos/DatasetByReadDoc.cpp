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
// 2015/04/27 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Jimos/DatasetByReadDoc.h"

#include "API/AosApi.h"
#include "DocClient/DocidMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataSplitterObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/SchemaObj.h"
#include "TaskMgr/Task.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByReadDoc_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByReadDoc(version);
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


AosDatasetByReadDoc::AosDatasetByReadDoc(
		const OmnString &type, 
		const int version)
:
AosDataset(type, version)
{
}


AosDatasetByReadDoc::AosDatasetByReadDoc(const int version)
:
AosDataset(AOSDATASET_READDOC, version)
{
}


AosDatasetByReadDoc::~AosDatasetByReadDoc()
{
}


bool
AosDatasetByReadDoc::config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc) 
{
	//<dataset jimo_objid="dataset_bydocids_jimodoc_v0">
	//	<datascanner jimo_objid="datascanner_parallel_jimodoc_v0" zky_name="xxxx">
	//		<dataconnector zky_objid=\"dataconnector_idfiles_jimodoc_v0\">
	//			<versions>
	//				<ver_0><![CDATA[libDataCubicJimos.so]]></ver_0>
	//			</versions>
	//			<files>
	//				<file zky_storage_fileid=\"9223372036854775910\" zky_physicalid=\"1\" zky_character=\"UTF8\" \/>
	//			</files>
	//		</dataconnector>
	//	</datascanner>
	//	<dataset jimo_objid="dataset_bydatascanner_jimodoc_v0" zky_name="xxxx">
	//		<datascanner jimo_objid="datascanner_cube_jimodoc_v0" zky_name="xxxx">
	//			<dataconnector doc_type="csv_doc" jimo_objid="dataconnector_readdoc_bysort_jimodoc_v0"/>
	//		</datascanner>
	//		<dataschema jimo_objid="dataschema_record_bylenid_jimodoc_v0" zky_dataschema_type="static" zky_name="xxxx">
	//			<datarecord type="csv" zky_field_delimiter="," zky_name="xxxx" zky_row_delimiter="LF" zky_text_qualifier="DQM">
	//				<datafields>
	//					<datafield type="str" zky_name="k1"/>
	//					<datafield type="str" zky_name="v1"/>
	//				</datafields>
	//			</datarecord>
	//		</dataschema>
	//	</dataset>
	//</dataset>

	aos_assert_r(worker_doc, false);

	mLock = OmnNew OmnMutex();
	mSetBuff = false;
	// create datascanner to scann docid buff
	AosXmlTagPtr scanner_conf = worker_doc->getFirstChild("datascanner");
	aos_assert_rr(scanner_conf, rdata, false);
	mDataScanner = AosCreateDataScanner(rdata.getPtr(), scanner_conf);
	aos_assert_r(mDataScanner, false);
	mDataScanner->setTaskDocid(mTaskDocid);
	bool rslt = mDataScanner->config(rdata, scanner_conf);
	aos_assert_r(rslt, false);

	//create dataset
	AosXmlTagPtr dataset = worker_doc->getFirstChild("dataset");
	aos_assert_r(dataset, false);
	mScanDataset = AosCreateDataset(rdata.getPtr(), dataset);
	aos_assert_r(mScanDataset, false);
	rslt = mScanDataset->config(rdata, dataset);
	aos_assert_r(rslt, false);

	return true;
}


void 
AosDatasetByReadDoc::setTaskDocid(const u64 task_docid)
{
	mTaskDocid = task_docid;
}

bool
AosDatasetByReadDoc::nextRecordset(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &recordset)
{
	aos_assert_r(mScanDataset, false);
	aos_assert_r(mDataScanner, false);
	if (recordset)
	{
		recordset->reset();
	}
	bool rslt = false;
	mLock->lock();
	if (mSetBuff)
	{
		rslt = mScanDataset->nextRecordset(rdata, recordset);
		aos_assert_rl(rslt, mLock, false);

		if (!AosRecordsetObj::checkEmpty(recordset))
		{
			OmnScreen << "=========================get next recordset, size: " << recordset->size() << endl;
			mLock->unlock();
			return true;
		}
	}

	AosBuffDataPtr buffdata;
	rslt = mDataScanner->getNextBlock(buffdata, rdata);
	if (!rslt)
	{
		if (mTaskDocid)
		{
			AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
			aos_assert_r(task, false);
			task->setErrorType(AosTaskErrorType::eInPutError);
			task->taskFailed(rdata);
		}
		mLock->unlock();
		return true;
	}

	if (!buffdata)
	{
		//mScanDataset->setBuffFinished();
		mLock->unlock();
		return true;
	}

	aos_assert_rl(buffdata->dataLen() > 0, mLock, false);
	int64_t read_size = buffdata->dataLen() - buffdata->getCrtIdx();

	bool incomplete = false;
	int64_t incmp_size = read_size%sizeof(u64);
	if (incmp_size > 0)
	{
		incomplete = true;
	}
	buffdata->setCrtIdx(read_size - incmp_size);

	int64_t remain_size = buffdata->dataLen() - buffdata->getCrtIdx();
	aos_assert_rl(remain_size >= 0, mLock, false);

	int64_t size = read_size - remain_size;
	aos_assert_r(size>=0, false);

	rslt = mDataScanner->setRemainingBuff(buffdata, incomplete, size, rdata);
	aos_assert_rl(rslt, mLock, false);

	AosBuffPtr buff = buffdata->getBuff();
	buff->setDataLen(size);
	if (buff->dataLen() > 0)
	{
		mSetBuff = true;
		OmnScreen << "======================setValueBuff" << endl;
		buff->reset();
		mScanDataset->reset(rdata);
		mScanDataset->setValueBuff(buff, rdata);
		rslt = mScanDataset->sendStart(rdata);
		aos_assert_rl(rslt, mLock, false);
		rslt = mScanDataset->nextRecordset(rdata, recordset);
		aos_assert_rl(rslt, mLock, false);
		OmnScreen << "=====================get next recordset, size: " << recordset->size() << endl;
	}
	mLock->unlock();
	return true;
}


bool
AosDatasetByReadDoc::sendStart(const AosRundataPtr &rdata)
{
	aos_assert_r(mDataScanner, false);
	if (mDataScanner->getDiskError())
	{
		AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
		aos_assert_r(task, false);
		task->setErrorType(AosTaskErrorType::eInPutError);
		task->taskFailed(rdata);
		return true;
	}
	bool rslt = mDataScanner->startReadData(rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDatasetByReadDoc::sendFinish(const AosRundataPtr &rdata)
{
	return true;
}

AosJimoPtr
AosDatasetByReadDoc::cloneJimo()  const
{
	try
	{
		return OmnNew AosDatasetByReadDoc(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

bool
AosDatasetByReadDoc::getRecord(const OmnString &name, AosDataRecordObjPtr &record)
{
	aos_assert_r(mScanDataset, false);
	return mScanDataset->getRecord(name, record);
}

bool
AosDatasetByReadDoc::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_r(mScanDataset, false);
	return mScanDataset->getRecords(records);
}

