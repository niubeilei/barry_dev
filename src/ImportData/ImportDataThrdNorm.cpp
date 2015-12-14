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
#if 0
#include "ImportData/ImportDataThrdNorm.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataAssembler/DataAssembler.h"
#include "ImportData/ImportDataCtlrNorm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "Util/ReadFile.h"
#include "Util/UtUtil.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/BuffData.h"


AosImportDataThrdNorm::AosImportDataThrdNorm(
		const AosDataCacherObjPtr &cacher,
		const AosImportDataCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosImportDataThrd(cacher, ctlr, rdata),
mProcLen(0),
mProcTotal(0)
{
}


AosImportDataThrdNorm::~AosImportDataThrdNorm()
{
}


bool
AosImportDataThrdNorm::config(const AosRundataPtr &rdata)
{
	aos_assert_r(mCtlr, false);
	aos_assert_r(mDataCacher, false);

	AosImportDataCtlrNorm * ctlr = (AosImportDataCtlrNorm *)(mCtlr.getPtr());
	mDataRecord = ctlr->cloneDataRecord(rdata);
	aos_assert_r(mDataRecord, false);
	
	mDocAssembler = ctlr->getDocAssembler();
	if(mDocAssembler)
	{
		mOutputRecord = ctlr->cloneOutputRecord(rdata);
		aos_assert_r(mOutputRecord, false);
	}

	mDataProcs.clear();
	vector<AosDataProcObjPtr> procs = ctlr->getDataProcs();
	for(u32 i=0; i<procs.size(); i++)
	{
		mDataProcs.push_back(procs[i]->clone());
	}

	return true;
}

	
AosDataProcStatus::E
AosImportDataThrdNorm::processRecord(
		u64 &docid,
		const AosRundataPtr &rdata)
{
	AosDataProcStatus::E status;
	for(u32 i=0; i<mDataProcs.size(); i++)
	{
		if (mDataProcs[i]->needDocid() && docid == 0) docid = getNextDocid(rdata);	
		status = mDataProcs[i]->procData(mDataRecord, docid, mOutputRecord, rdata);
		switch (status)
		{
		case AosDataProcStatus::eContinue:
			 break;
		
		case AosDataProcStatus::eRecordFiltered:
			 // The record is filtered out. 
			 return status;

		case AosDataProcStatus::eDataTooShort:
			 // The data is too short. Do nothing. Just return.
			 return status;

		case AosDataProcStatus::eError:
			 AosSetEntityError(rdata, "importdatathrdnorm_process_record_error", 
				"Import", "ImportThrdNorm");
			 return status;

		default:
			 AosSetErrorU(rdata, "unrecog_dataproc_status:") << status;
			 OmnAlarm << rdata->getErrmsg() << enderr;
			 return status;
		}
	}
	return AosDataProcStatus::eContinue;
}

	
bool
AosImportDataThrdNorm::run()
{
	// This function reads in N number of records, parses them, 
	// and then create them as docs.
	AosRundataPtr rdata = mRundata;
	aos_assert_r(mDataCacher, false);
	mStartTime = OmnGetSecond();

	int len;
	char * data = 0;
	bool finished = false;
	bool rslt = true;
	u64	docid = 0;
	AosDataProcStatus::E status;
	AosBuffDataPtr metaData;

	mProcLen = 0;

	while(1)
	{
		docid = 0;
		rslt = mDataCacher->nextValue(&data, len, docid, metaData, rdata);
		if (metaData)
		{
			if (metaData->getDiskError())
			{
OmnScreen << "=========================disk error: " << endl;
				mCtlr->setInputError();
				mEndTime = OmnGetSecond();
				mSuccess = false;
				mFinished = true;
				AosImportDataThrdPtr thisptr(this, true);
				mCtlr->thrdFinished(thisptr, rdata);
				clear();
				return true;
			}
		}
		if (!data || !rslt) break;
		mProcLen += len;
		mProcTotal++;

		int record_len = mDataRecord->getRecordLen();
		char tmp[2000] = {' '};
		if (len < record_len)
		{
			aos_assert_r(record_len < 2000, false);
			memcpy(tmp, data, len);
			if (tmp[len - 1] == '\0') tmp[len - 1] = ' ';
			tmp[record_len - 1] = '\0';
			data = tmp;
			len = record_len;
		}

		mDataRecord->clear();
		// Chen Ding, 2013/11/29
		// rslt = mDataRecord->setData(data, len, metaData, false);
		rslt = mDataRecord->setData(data, len, metaData.getPtr(), 0);
		if (!rslt) break;

		status = processRecord(docid, rdata);
		switch (status)
		{
		case AosDataProcStatus::eContinue:
			 if (mDocAssembler)
		 	 {
				// JACKIE-HADOOP
				if (mDocAssembler->isHadoop())
				{
					// It should construct the following entry:
					// 		[fileid, offset, length]
					// 'fileid' set to 0
					char data[20];
					*(u16 *)&data[0] = 0;
					int64_t offset = metaData->getOffset() - len;
					*(u64 *)&data[2] = offset;
					*(u32 *)&data[10] = len;

					AosValueRslt value;
					value.setKeyValue(data, sizeof(u16) + sizeof(u64) + sizeof(u32), false, docid);
		   			rslt = mDocAssembler->appendEntry(value, rdata.getPtr());
					if (!rslt)
					{
						finished = true;
						OmnAlarm << "failed append entry: " << rdata->getErrmsg() << enderr;
					}
				}
				else
				{
					char * d = mOutputRecord->getData(rdata.getPtr());
					int l = mOutputRecord->getRecordLen();
					AosValueRslt value;
					value.setKeyValue(d, l, false, docid);
		   			rslt = mDocAssembler->appendEntry(value, rdata.getPtr());
					if (!rslt)
					{
						finished = true;
						OmnAlarm << "failed append entry: " << rdata->getErrmsg() << enderr;
					}
				}
		 	 }
		 	 break;
		
		case AosDataProcStatus::eRecordFiltered:
			 break;

		case AosDataProcStatus::eDataTooShort:
			 finished = true;
			 break;

		case AosDataProcStatus::eError:
			 {
				 AosSetEntityError(rdata, "importdatathrdnorm_failed_converting_data", 
						 "Import", "ImportThrdNorm");
				 mEndTime = OmnGetSecond();
				 mSuccess = false;
				 mFinished = true;
				 AosImportDataThrdPtr thisptr(this, true);
				 mCtlr->thrdFinished(thisptr, rdata);
				 clear();
				 return true;
			 }
		default:
			 OmnAlarm << "Unrecognized status: " << status << enderr;
			 finished = true;
			 break;
		}
		if (finished) break;
	}
		
	mEndTime = OmnGetSecond();
	mSuccess = true;
	mFinished = true;
	
	AosImportDataThrdPtr thisptr(this, true);
	mCtlr->thrdFinished(thisptr, rdata);

	clear();
	return true;
}


u64
AosImportDataThrdNorm::getNextDocid(const AosRundataPtr &rdata)
{
	if (!mDocAssembler) return 0;
	return mDocAssembler->getNextDocid(rdata.getPtrNoLock());
}

	
bool
AosImportDataThrdNorm::procFinished()
{
	return true;
}


void
AosImportDataThrdNorm::clear()
{
	mCtlr = 0;
	mDataCacher = 0;
	
	if(mDataRecord) mDataRecord->clear();
	if(mOutputRecord) mOutputRecord->clear();
	mDataRecord = 0;
	mOutputRecord = 0;
	mDocAssembler = 0;
	mDataProcs.clear();
}


AosImportDataThrdPtr
AosImportDataThrdNorm::createImportDataThrd(
		const AosDataCacherObjPtr &cacher,
		const AosImportDataCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosImportDataThrdNorm * thrd = OmnNew AosImportDataThrdNorm(
		cacher, ctlr, rdata);
	bool rslt = thrd->config(rdata);
	aos_assert_r(rslt, 0);
	return thrd;
}
#endif
