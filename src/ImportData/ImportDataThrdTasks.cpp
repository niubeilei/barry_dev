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
#include "ImportData/ImportDataThrdTasks.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataAssembler/DataAssembler.h"
#include "ImportData/ImportDataCtlrTasks.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "Util/ReadFile.h"
#include "Util/UtUtil.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/BuffData.h"


AosImportDataThrdTasks::AosImportDataThrdTasks(
		const AosDataCacherObjPtr &cacher,
		const AosImportDataCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosImportDataThrd(cacher, ctlr, rdata),
mProcLen(0)
{
}


AosImportDataThrdTasks::~AosImportDataThrdTasks()
{
}


bool
AosImportDataThrdTasks::config(const AosRundataPtr &rdata)
{
	aos_assert_r(mCtlr, false);
	aos_assert_r(mDataCacher, false);

	AosImportDataCtlrTasks * ctlr = (AosImportDataCtlrTasks *)(mCtlr.getPtr());
	mKeyIdx = ctlr->getKeyIdx();
	mKeyLen = ctlr->getKeyLen();
	map<OmnString, procGroup> mp = ctlr->getProcGroup();
	map<OmnString, procGroup>::iterator itr = mp.begin();
	
	mMap.clear();
	mProcTotal.clear();

	while(itr != mp.end())
	{
		procGroup group;
			
		aos_assert_r(itr->second.mDataRecord, false);
		group.mDataRecord = itr->second.mDataRecord->clone(rdata.getPtr());
		aos_assert_r(group.mDataRecord, false);

		group.mDocAssembler = itr->second.mDocAssembler;
		if (group.mDocAssembler)
		{
			aos_assert_r(itr->second.mOutputRecord, false);
			group.mOutputRecord = itr->second.mOutputRecord->clone(rdata.getPtr());
			aos_assert_r(group.mOutputRecord, false);
		}
		
		for(u32 i=0; i<itr->second.mDataProcs.size(); i++)
		{
			group.mDataProcs.push_back(itr->second.mDataProcs[i]->clone());
		}
		
		mMap[itr->first] = group;
		mProcTotal[itr->first] = 0;
		itr++;
	}

	return true;
}

	
AosDataProcStatus::E
AosImportDataThrdTasks::processRecord(
		procGroup &group,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	AosDataProcStatus::E status;
	for(u32 i=0; i<group.mDataProcs.size(); i++)
	{
		if (group.mDataProcs[i]->needDocid() && docid == 0) docid = getNextDocid(group, rdata);	
		status = group.mDataProcs[i]->procData(group.mDataRecord, docid, group.mOutputRecord, rdata);
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
			 OmnAlarm << rdata->getErrmsg() << enderr;
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
AosImportDataThrdTasks::run()
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
	map<OmnString, procGroup>::iterator itr;

	mProcLen = 0;

	while(1)
	{
		docid = 0;
		rslt = mDataCacher->nextValue(&data, len, docid, metaData, rdata);
		if (metaData)
		{
			if (metaData->getDiskError())
			{
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

		if (!data || !rslt)
		{
			break;
		}
		mProcLen += len;

		if (len < mKeyIdx + mKeyLen) break;
		
		OmnString key(&data[mKeyIdx], mKeyLen);
		key << "_" << len;
		itr = mMap.find(key);
		if (itr == mMap.end())
		{
			key.assign(&data[mKeyIdx], mKeyLen);
			key << "_" << (len + 40);
			itr = mMap.find(key);
		}

		if (itr == mMap.end())
		{
			key = "UnKnow__";
			key.append(&data[mKeyIdx], mKeyLen);
			key << "_" << len;
			if (mProcTotal.find(key) == mProcTotal.end())
			{
				mProcTotal[key] = 0;
			}
			u64 v = mProcTotal[key];
			mProcTotal[key] = ++v;
			continue;
		}
	
		u64 v = mProcTotal[key];
		mProcTotal[key] = ++v;

		int record_len = itr->second.mDataRecord->getRecordLen();
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

		itr->second.mDataRecord->clear();
		// Chen Ding, 2013/11/29
		// rslt = itr->second.mDataRecord->setData(data, len, metaData, false);
		rslt = itr->second.mDataRecord->setData(data, len, metaData.getPtr(), 0);
		if (!rslt) break;
	
		status = processRecord(itr->second, docid, rdata);
		switch (status)
		{
		case AosDataProcStatus::eContinue:
			 if (itr->second.mDocAssembler)
		 	 {
				char * d = itr->second.mOutputRecord->getData(rdata.getPtr());
				int l = itr->second.mOutputRecord->getRecordLen();
				AosValueRslt value;
				value.setKeyValue(d, l, false, docid);
		   		rslt = itr->second.mDocAssembler->appendEntry(value, rdata.getPtr());
				if (!rslt)
				{
					finished = true;
					OmnAlarm << "failed append entry: " << rdata->getErrmsg() << enderr;
				}
		 	 }
		 	 break;
		
		case AosDataProcStatus::eRecordFiltered:
			 break;

		case AosDataProcStatus::eDataTooShort:
			 finished = true;
			 break;

		case AosDataProcStatus::eError:
		 	 OmnAlarm << "failed converting data: " << rdata->getErrmsg() << enderr;
		 	 finished = true;
		 	 break;

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
AosImportDataThrdTasks::getNextDocid(
		procGroup &group,
		const AosRundataPtr &rdata)
{
	AosDataAssemblerObjPtr doc_asm = group.mDocAssembler;
	if (!doc_asm) return 0;
	return doc_asm->getNextDocid(rdata.getPtrNoLock());
}

	
bool
AosImportDataThrdTasks::procFinished()
{
	return true;
}


void
AosImportDataThrdTasks::clear()
{
	mCtlr = 0;
	mDataCacher = 0;
	
	map<OmnString, procGroup>::iterator itr = mMap.begin();
	while(itr != mMap.end())
	{
		if (itr->second.mDataRecord) itr->second.mDataRecord->clear();
		if (itr->second.mOutputRecord) itr->second.mOutputRecord->clear();
		itr->second.mDataRecord = 0;
		itr->second.mOutputRecord = 0;
		itr->second.mDocAssembler = 0;
		itr->second.mDataProcs.clear();
		itr++;
	}
	mMap.clear();
}


AosImportDataThrdPtr
AosImportDataThrdTasks::createImportDataThrd(
		const AosDataCacherObjPtr &cacher,
		const AosImportDataCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosImportDataThrdTasks * thrd = OmnNew AosImportDataThrdTasks(
		cacher, ctlr, rdata);
	bool rslt = thrd->config(rdata);
	aos_assert_r(rslt, 0);
	return thrd;
}
#endif
