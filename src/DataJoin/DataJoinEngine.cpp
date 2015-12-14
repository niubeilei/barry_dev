////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2012/11/05	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/DataJoinEngine.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataJoin/DataJoinCtlr.h"
#include "DataRecord/DataRecord.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DataAssembler/DataAssembler.h"
#include "Groupby/GroupbyProc.h"
#include "Groupby/GroupbyType.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosDataJoinEngine::AosDataJoinEngine()
:
AosDataJoin(),
mNumScanners(0)
{
}


AosDataJoinEngine::AosDataJoinEngine(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mNumScanners(0)
{
}


AosDataJoinEngine::~AosDataJoinEngine()
{
}


bool
AosDataJoinEngine::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	if(!scanners)
	{
		AosSetErrorU(rdata, "missing scanner cfg:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int seqno = 0;
	AosIILScannerListenerPtr thisptr(this, false);	
	AosXmlTagPtr scanner_tag = scanners->getFirstChild();
	while (scanner_tag)
	{
		AosIILScannerObjPtr scanner = AosIILScannerObj::createIILScannerStatic(
			thisptr, seqno++, scanner_tag, rdata);
		aos_assert_r(scanner, false);
		mIILScanners.push_back(scanner);
		scanner_tag = scanners->getNextChild();
	}
	
	mNumScanners = mIILScanners.size();
	if(mNumScanners < 2)
	{
		AosSetErrorU(rdata, "low scanner num:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mIILRecord = mCtlr->cloneIILRecord(rdata);
	if(!mIILRecord)
	{
		AosSetErrorU(rdata, "missing iil record:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mIILAssembler = mCtlr->getIILAssembler();
	mDocAssembler = mCtlr->getDocAssembler();
	mDocRecord = mCtlr->cloneDocRecord(rdata);
	if(mDocAssembler && !mDocRecord)
	{
		AosSetErrorU(rdata, "missing doc record:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	vector<AosDataProcObjPtr> data_procs = mCtlr->getDataProcs();
	for(u32 i=0; i<data_procs.size(); i++)
	{
		mDataProcs.push_back(data_procs[i]->clone());
	}
	
	AosXmlTagPtr gbtag = def->getFirstChild(AOSTAG_GROUPBY_PROC);
	if (gbtag)
	{
		AosGroupbyType::E type = AosGroupbyType::toEnum(gbtag->getAttrStr(AOSTAG_TYPE));
		if (AosGroupbyType::isValid(type))
		{
			mGroupbyProc = AosGroupbyProc::createGroupbyProc(gbtag, rdata);
			aos_assert_r(mGroupbyProc, false);
		}
	}
	
	vector<AosConditionObjPtr> filters = mCtlr->getFilters();
	AosConditionObjPtr filter;
	for(u32 i=0; i<filters.size(); i++)
	{
		filter = filters[i]->clone();
		aos_assert_r(filter, false);
		mFilters.push_back(filter);
	}

	return true;
}


bool
AosDataJoinEngine::run()
{
	aos_assert_r(mStatus == eIdle, false);
	mStartTime = OmnGetSecond();
	OmnScreen << "join iil start , time:" << mStartTime << endl;

	AosRundataPtr rdata = mRundata;
	mLock->lock();
	mStatus = eActive;
	bool rslt = startJoin(rdata);
	if(rslt) rslt = runJoin(rdata);
	procGroupbyFinish(rdata);
	mStatus = eFinished;
	mLock->unlock();
	mFinished = true;
	mSuccess = rslt;
	
	mIILScanners.clear();
	mWorkingData.clear();
	mIILAssembler = 0;
	mDocAssembler = 0;
	if(mIILRecord) mIILRecord->clear();
	if(mDocRecord) mDocRecord->clear();
	mIILRecord = 0;
	mDocRecord = 0;
	mGroupbyProc = 0;
	mDataProcs.clear();
	mFilters.clear();

	mEndTime = OmnGetSecond();
	OmnScreen << "join iil finished, time:" << mEndTime
		<< ", spend:" << (mEndTime - mStartTime) << endl;
	
	AosDataJoinPtr thisptr(this, true);
	mCtlr->joinFinished(thisptr, rdata);
	return true;
}

	
bool
AosDataJoinEngine::startJoin(const AosRundataPtr &rdata)
{
	bool rslt = true;
	for (u32 i=0; i<mNumScanners; i++)
	{
		rslt = mIILScanners[i]->start(rdata);
		aos_assert_r(rslt, false);
	}
	
	for (u32 i=0; i<mNumScanners; i++)
	{
		rslt = getNextKey(i, rdata);
		aos_assert_r(rslt, false);
	}
		
	return true;
}


bool
AosDataJoinEngine::runJoin(const AosRundataPtr &rdata)
{
	list<WorkingData>::iterator itr;
	u32 idx = 0;
	bool rslt = true;
	bool rejected = false;
	while (1)
	{
		sortWorkData();

OmnString str;
str << "\nwork_data_size:" << mWorkingData.size();
itr = mWorkingData.begin();
while(itr != mWorkingData.end())
{
	str << "\nidx:" << (*itr).idx << ",key:" << (*itr).key << ",more:" << (*itr).has_more << ",small:" << (*itr).is_smallest;
	itr++;
}
//OmnScreen << str << endl;

		itr = mWorkingData.begin();
		if((*itr).key == "") return true;

		rejected = false;
		while (itr != mWorkingData.end())
		{
			idx = (*itr).idx;
			rejected = mIILScanners[idx]->isReject((*itr).is_smallest);
			if(rejected) break;
			itr++;
		}

		if(!rejected)
		{
			rslt = createRecord(rdata);
			aos_assert_r(rslt, false);
		}
	
		bool has_more = false;
		itr = mWorkingData.begin();
		while (itr != mWorkingData.end())
		{
			if((*itr).is_smallest)
			{
				has_more = (*itr).has_more;
				if(has_more) break;
			}
			itr++;
		}
	
		itr = mWorkingData.begin();
		vector<u32> idxs;
		while (itr != mWorkingData.end())
		{
			if((*itr).is_smallest && (!has_more || (*itr).has_more))
			{
				idxs.push_back((*itr).idx);
			}
			itr++;
		}

		for(u32 i=0; i<idxs.size(); i++)
		{
			rslt = getNextKey(idxs[i], rdata);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}


bool
AosDataJoinEngine::getNextKey(
		const u32 idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx < mNumScanners, false);
	OmnString key;
	bool has_more = true;
	AosDataProcStatus::E status = AosDataProcStatus::eOk;
	while(1)
	{
		//OmnScreen << "move next, idx:" << idx << endl;
		status = mIILScanners[idx]->moveNext(key, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
		 	 setWorkData(key, idx, has_more);
			 return true;

		case AosDataProcStatus::eRetrievingData:
			 mSem->wait();
		 	 if(!mDataRetrieveSuccess) return false;
		 	 break;

		case AosDataProcStatus::eNoMoreData:
		 	 key = "";
			 setWorkData(key, idx, false);
			 return true;

		default:
			 return false;
		}
	}
	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataJoinEngine::createRecord(const AosRundataPtr &rdata)
{
	u32 num;
	u32 idx;
	list<WorkingData>::iterator it = mWorkingData.begin();
	OmnString str = "create record,";
	while(it != mWorkingData.end())
	{
		idx = (*it).idx;
		if((*it).is_smallest)
		{
			num = mIILScanners[idx]->getMatchedSize();
			str << "key:" << (*it).key << ",idx:" << idx << ",num:" << num << ",";
		}
		it++;
	}
	OmnScreen << str << endl;

	list<WorkingData>::iterator itr = mWorkingData.begin();
	aos_assert_r((*itr).key != "" && (*itr).is_smallest, false);
	
	AosDataRecordObjPtr record = mIILRecord->clone(rdata.getPtrNoLock());
	aos_assert_r(record, false);

	record->clear();
	return createRecordRec(itr, record, rdata);
}


bool
AosDataJoinEngine::createRecordRec(
		list<WorkingData>::iterator &itr,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	if(itr == mWorkingData.end())
	{
		//OmnScreen << "create record" << endl;
		return procRecord(record, rdata);
	}

	u32 idx = (*itr).idx;
	bool rslt;
	list<WorkingData>::iterator next_itr = itr;
	next_itr++;
	if((*itr).is_smallest)
	{
		AosDataRecordObjPtr rec;
		bool has_more = true;
		mIILScanners[idx]->resetMatchedIdx();
		while(has_more)
		{
			rec = record->clone(rdata.getPtrNoLock());
			//OmnScreen << "set match, idx:" << idx << endl;
			rslt = mIILScanners[idx]->setMatchedValue(rec, has_more, rdata);
			aos_assert_r(rslt, false);

			rslt = createRecordRec(next_itr, rec, rdata);
			aos_assert_r(rslt, false);
		}
		return true;
	}
	else
	{
		//OmnScreen << "set not match, idx:" << idx << endl;
		rslt = mIILScanners[idx]->setNotMatchedValue(record, rdata);
		aos_assert_r(rslt, false);
		
		return createRecordRec(next_itr, record, rdata);
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataJoinEngine::procRecord(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	if (mGroupbyProc)
	{
		AosDataProcStatus::E status = mGroupbyProc->procRecord(record, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk : break;
		case AosDataProcStatus::eRejected : return true;
		default : return false;
		}
	}
	return addRecord(record, rdata);
}

	
bool
AosDataJoinEngine::procGroupbyFinish(const AosRundataPtr &rdata)
{
	if (!mGroupbyProc) return true;
		
	bool rslt = mGroupbyProc->procFinished(mIILRecord, rdata);
	aos_assert_r(rslt, false);
		
	return addRecord(mIILRecord, rdata);
}


bool
AosDataJoinEngine::addRecord(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	char * data = record->getData(rdata.getPtrNoLock());
	int len = record->getRecordLen();
	u64 docid = record->getDocid();
	if(!data || len <= 0) return true;

	AosValueRslt value_rslt;
	value_rslt.setKeyValue(data, len, false, docid);
		
	bool rslt;
	for(u32 i=0; i<mFilters.size(); i++)
	{
		rslt = mFilters[i]->evalCond(value_rslt, rdata);
		if(!rslt) return true;
	}
	
	if (mIILAssembler)
	{
		rslt = mIILAssembler->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}

	mDocRecord->clear();
	rslt = mDocRecord->setData(data, len, 0, 0);
	aos_assert_r(rslt, false);
	
	if (docid == 0) docid = getNextDocid(rdata);

	AosDataProcStatus::E status = processRecord(docid, rdata);
	switch (status)
	{
	case AosDataProcStatus::eContinue:
		 break;
		
	case AosDataProcStatus::eRecordFiltered:
		 return true;

	case AosDataProcStatus::eDataTooShort:
		 return true;

	case AosDataProcStatus::eError:
	 	 OmnAlarm << "failed proc data:" << rdata->getErrmsg() << enderr;
	 	 return false;

	default:
		 OmnAlarm << "Unrecognized status: " << status << enderr;
	 	 return false;
	}

	if (mDocAssembler)
	{
		AosValueRslt value;
		value.setKeyValue(mDocRecord->getData(rdata.getPtrNoLock()),
			mDocRecord->getRecordLen(), false, docid);
		rslt = mDocAssembler->appendEntry(value, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}

	return true;
}

	
u64
AosDataJoinEngine::getNextDocid(const AosRundataPtr &rdata)
{
	if (!mDocAssembler) return 0;
	return mDocAssembler->getNextDocid(rdata.getPtrNoLock());
}

	
AosDataProcStatus::E
AosDataJoinEngine::processRecord(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosDataProcStatus::E status;
	for(u32 i=0; i<mDataProcs.size(); i++)
	{
		status = mDataProcs[i]->procData(mDocRecord, docid, 0, rdata);
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


void
AosDataJoinEngine::sortWorkData()
{
	mWorkingData.sort(AosDataJoinEngine::compareWorkData);

	list<WorkingData>::iterator itr = mWorkingData.begin();
	(*itr).is_smallest = ((*itr).key == "") ? false : true;

	int rsltInt = 0;
	list<WorkingData>::iterator last_itr = itr;
	itr++;
	while(itr != mWorkingData.end())
	{
		if((*itr).key == "")
		{
			(*itr).is_smallest = false;
		}
		else
		{
			rsltInt = compareKey(*last_itr, *itr);
			if(rsltInt == 0) (*itr).is_smallest = (*last_itr).is_smallest;
			else (*itr).is_smallest = false;
		}
		last_itr = itr;
		itr++;
	}
}


void
AosDataJoinEngine::addWorkData(
		const OmnString &key,
		const u32 idx,
		const bool has_more)
{
	WorkingData work_data;
	work_data.key = key;
	work_data.idx = idx;
	work_data.has_more = has_more;
	work_data.is_smallest = false;
	mWorkingData.push_back(work_data);
}	


void
AosDataJoinEngine::removeWorkData(const u32 idx)
{
	list<WorkingData>::iterator itr = mWorkingData.begin();
	while(itr != mWorkingData.end())
	{
		if((*itr).idx == idx)
		{
			mWorkingData.erase(itr);
			return;
		}
		itr++;
	}
}


void
AosDataJoinEngine::setWorkData(
		const OmnString &key,
		const u32 idx,
		const bool has_more)
{
	//OmnScreen << "set work data idx:" << idx << endl;
	removeWorkData(idx);
	addWorkData(key, idx, has_more);
}


bool
AosDataJoinEngine::compareWorkData(
		AosDataJoinEngine::WorkingData A,
		AosDataJoinEngine::WorkingData B)
{
	if(A.key == "" && B.key == "") return (A.idx < B.idx); 
	if(A.key == "") return false;
	if(B.key == "") return true;

	int rsltInt = compareKey(A, B);
	if(rsltInt == 0) rsltInt = (A.idx < B.idx) ? -1 : 1;
	if(rsltInt < 0) return true;
	return false;
}


int
AosDataJoinEngine::compareKey(
		AosDataJoinEngine::WorkingData A,
		AosDataJoinEngine::WorkingData B)
{
	int rsltInt = 0;
	if(A.key.length() == B.key.length())
	{
		rsltInt = strncmp(A.key.data(), B.key.data(), A.key.length());
	}
	else if(A.key.length() < B.key.length())
	{
		rsltInt = strncmp(A.key.data(), B.key.data(), A.key.length());
		if(rsltInt == 0) rsltInt = -1;
	}
	else
	{
		rsltInt = strncmp(A.key.data(), B.key.data(), B.key.length());
		if(rsltInt == 0) rsltInt = 1;
	}
	return rsltInt;
}


bool
AosDataJoinEngine::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	bool rslt = true;
	for(u32 i=0; i<mNumScanners; i++)
	{
		rslt = mIILScanners[i]->setQueryContext(context, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


int
AosDataJoinEngine::getProgress() 
{
	if(mFinished)
	{
		mProgress = 100;
		return mProgress;
	}
	
	aos_assert_r(mNumScanners > 0, 0);
	int progress = 0;
	for (u32 i=0; i<mNumScanners; i++)
	{
		progress += mIILScanners[i]->getProgress();
	}
	mProgress = progress / mNumScanners;
	//OmnScreen << "join:[" << this << "], mProgress:" << mProgress;
	return mProgress;
}

	
AosDataJoinPtr
AosDataJoinEngine::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinEngine * join = OmnNew AosDataJoinEngine(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinEngine::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);

	AosXmlTagPtr scanner_tag = scanners->getFirstChild();
	aos_assert_r(scanner_tag, false);

	AosIILScannerObjPtr scanner;
	vector<AosIILScannerObjPtr>	iilScanners;
	while (scanner_tag)
	{
		scanner = AosIILScannerObj::createIILScannerStatic(0, 0, scanner_tag, rdata);
		aos_assert_r(scanner, false);
		iilScanners.push_back(scanner);
		scanner_tag = scanners->getNextChild();
	}
	
	u32 size = iilScanners.size();
	aos_assert_r(size >= 2, false);
	
	AosXmlTagPtr record_tag = def->getFirstChild(AOSTAG_RECORD);
	aos_assert_r(record_tag, false);
	
	AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(record_tag, task, rdata.getPtrNoLock());
	aos_assert_r(record, false);
	
	AosXmlTagPtr gbtag = def->getFirstChild(AOSTAG_GROUPBY_PROC);
	if (!gbtag) return true;
		
	AosGroupbyType::E type = AosGroupbyType::toEnum(gbtag->getAttrStr(AOSTAG_TYPE));
	if (!AosGroupbyType::isValid(type)) return true;
			
	AosGroupbyProcPtr groupbyProc = AosGroupbyProc::createGroupbyProc(gbtag, rdata);
	aos_assert_r(groupbyProc, false);

	return true;	
}

