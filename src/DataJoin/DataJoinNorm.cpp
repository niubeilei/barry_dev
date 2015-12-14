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
#include "DataJoin/DataJoinNorm.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataJoin/DataJoinCtlr.h"
#include "DataRecord/DataRecord.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DataAssembler/DataAssembler.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosDataJoinNorm::AosDataJoinNorm()
:
AosDataJoin(),
mPrefixLen(-1),
mNumScanners(0)
{
}


AosDataJoinNorm::AosDataJoinNorm(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mPrefixLen(-1),
mNumScanners(0)
{
}


AosDataJoinNorm::~AosDataJoinNorm()
{
}


bool
AosDataJoinNorm::config(
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
	
	// Retrieve Prefix Length
	mPrefixLen = def->getAttrInt(AOSTAG_PREFIX_LENGTH, -1);

	// Retrieve IIL Scanners
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
		AosSetErrorU(rdata, "low sanner num:") << def->toString();
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
		// Check whether it is a valid tag. 
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
AosDataJoinNorm::run()
{
	// This task joins two or more IILs to create a new table. The
	// IILs are specified in mIILScanners. 
	// 1. Issue a request to copy the data for all the IILs in mIILScanners.
	// 2. It waits until all responses are received.
	// 3. Once all responses are received, it starts the join process.
	//
	// Join Process:
	// 1. Every IIL has a cursor, which sets to the first entry.
	// 2. Starting from left, it retrieves the current 'Join Key', 
	//    passes it on to the next IIL with a 'Record To be Created'.
	//    If the next IIL accepts the value, it sets its value to 
	//    the record, and passes the 'Join Key' to the next IIL, 
	//    if any. 
	//
	//    If any IIL rejects the 'Join Key', the record is dropped, 
	//    and the control goes back to the first IIL. It moves the cursor
	//    to the next one, if any, and repeats the above process.
	//
	//    If all IILs accept the 'Join Key', the record is created and 
	//    inserted into the table.
	//
	// 3. During the above process, if any IIL consumed all its data but
	//    there are still remaining data to process, it will pause the process,
	//    fetch the data, and then resume the process.
	//
	// This join will creates at most one record for each 'Join Key' 
	// in the first IIL.

	// 1. Issue a request to retrieve data for every IIL in 'mIIL'. 
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
	mNewData.clear();
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
AosDataJoinNorm::startJoin(const AosRundataPtr &rdata)
{
	// This is the thread to process the join. It uses an array of 'mWorkingValues'.
	// Values in mWorkingValues are sorted. The first one is the smallest one. 
	// 1. At the beginning, it retrieves one value from every IIL. The values are 
	//    sorted. 
	// 2. The first value is the smallest one. It uses that value to get values 
	//    from all the IILs. At mean time, if the IIL has the same value (which means
	//    the smallest value), it gets the matched values and moves the cursor
	//    (i.e., retrieves the next value). If it has different value (which
	//    means its value is bigger than the current smallest value), it gets
	//    the not-matched-values and do nothing. 
	bool rslt;
	for (int i=0; i<mNumScanners; i++)
	{
		rslt = mIILScanners[i]->start(rdata);
		aos_assert_r(rslt, false);
		
		WorkingData work_data;
		work_data.key = 0;
		work_data.len = 0;
		work_data.idx = i;
		work_data.is_smallest = false;
		mWorkingData.push_back(work_data);
		mNewData.push_back(work_data);
	}
	
	AosDataProcStatus::E status;
	const char *key = 0;
	int num_working_data = 0;
	int len;
	
	// This action is retrieving data. It checks
	// whether it has received all the data. If not, do nothing. 
	// Otherwise, it moves on to the next state.
	// It has received all the data. Need to retrieve a value from every IIL.
		 
	for (int i=0; i<mNumScanners; i++)
	{
		status = mIILScanners[i]->moveNext(key, len, -1, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
		 	 rslt = addToWorkingData(mWorkingData, num_working_data, key, len, i);
		 	 break;

		case AosDataProcStatus::eRetrievingData:
			 mSem->wait();
		 	 if(!mDataRetrieveSuccess) return false;
		 	 i--;
		 	 break;

		case AosDataProcStatus::eNoMoreData:
		 	 rslt = addToWorkingData(mWorkingData, num_working_data, 0, 0, i);
		 	 break;

		default:
	 	 	 return false;
		}
		if(!rslt) return rslt;
	}
			
	for (int i=1; i<num_working_data; i++)
	{
		setSmallestFlag(i);
		if (!mWorkingData[i].is_smallest) break;
	}
	return true;
}


bool
AosDataJoinNorm::runJoin(const AosRundataPtr &rdata)
{
	// This is the main process logic. It loops over all the scanner, 
	// finding the smallest value, and then asking all others to generate
	// values for the smallest one. After that, it creates a record, appended
	// it to the table, and then move the smallest one to the next, and then
	// repeat this process.
		
	AosDataProcStatus::E status;
	const char *new_key = 0;
	int num_new_data = 0;
	int len;
	bool rslt;
	while (1)
	{
		// The first values for all IILs have been retrieved into 'mWorkingValues'.
		// The first value is the smallest one.
		// There are two groups of data in 'mWorkingValues': the ones with 
		// the smallest values and others. It will retrieve the 'matched-values'
		// from all the IILs with the smallest value once. After that it 
		// retrieves values from the remaining IILs.
		//
		// When retrieving matched-values from the IILs with the smallest 
		// values, the IILs will move to the next value. It uses the 'next
		// value' to create a new working value array. 
		if (!mWorkingData[0].is_smallest || !mWorkingData[0].key)
		{
			// Finished all the processing.
	 	 	return true;
		}

		// Step 1: Set the matched values
		// 'WorkingData' holds the next data to be processed for all
		// the scanners, values are sorted. If the value is the smallest,
		// its flag 'is_smallest' is marked to be true. The following
		// loops processes the smallest values first.
		int smallestIdx = 0;
		mIILRecord->clear();

		while (mWorkingData[smallestIdx].is_smallest && smallestIdx < mNumScanners)
		{
			// It is the smallest one. Let the IIL scanner check whether the 
			// value matches. 
			if (!mWorkingData[smallestIdx].key)
			{
			 	return false;
			}

			// If not match, it needs to move to the next. 
			// If all 'smallest' scanners match the value, an entry may (or may
			// not be created depending on all the 'not-matched'. 
			int idx = mWorkingData[smallestIdx].idx;
			bool has_more;
			rslt = mIILScanners[idx]->setMatchedValue(mIILRecord, has_more, rdata);
			aos_assert_r(rslt, false);
			smallestIdx++;
		}

		// Step 2: Move all the smallest forward.
		// If there is only one smallest and the smallest match type is 
		// mapper, it should move the cursor only. No record shall be generated.
		num_new_data = 0;
		rslt = true;
		for (int i=0; i<smallestIdx; i++)
		{
			int idx = mWorkingData[i].idx;
			status = mIILScanners[idx]->moveNext(new_key, len, smallestIdx, rdata);
			switch (status)
			{
			case AosDataProcStatus::eOk:
				 rslt = addToWorkingData(mNewData, num_new_data, new_key, len, idx);
				 break;
				
			case AosDataProcStatus::eRetrievingData:
			 	 mSem->wait();
				 if(!mDataRetrieveSuccess) return false;
				 i--;
				 break;

			case AosDataProcStatus::eNoMoreData:
			 	 rslt = addToWorkingData(mNewData, num_new_data, 0, 0, idx);
				 break;

			default:
				 return false;
			}
			if(!rslt) return false;
		}

		// Step 3: Set not-matched values
		// It finished processing the smallest values. Need to Process the 
		// remaining IILs.
		bool rejected = false;
		for (int i=smallestIdx; i<mNumScanners; i++)
		{
			int idx = mWorkingData[i].idx;
			if(mIILScanners[idx]->isReject(false))
			{
				rejected = true;
				break;
			}
			
			rslt = mIILScanners[idx]->setNotMatchedValue(mIILRecord, rdata);
			aos_assert_r(rslt, false);
		}

		if (!rejected)
		{
			bool rslt = addRecord(rdata);
			aos_assert_r(rslt, false);
		}

		if (num_new_data <= 0)
		{
		 	return false;
		}

		// Merge new_data to working_data.
		// 'new_data' holds only the values that used to be the smallest. 
		mergeData(num_new_data);
	}
	return true;
}


bool
AosDataJoinNorm::addRecord(const AosRundataPtr &rdata)
{
	if (mGroupbyProc)
	{
		//OmnScreen << "append into group record:" 
		// << mIILRecord->getData() << ":" << mIILRecord->getRecordLen() 
		// << ":" << mIILRecord->getDocid() << endl;
		AosDataProcStatus::E status = mGroupbyProc->procRecord(mIILRecord, rdata);
		switch(status)
		{
		case AosDataProcStatus::eOk:break;
		case AosDataProcStatus::eRejected:return true;
		default:return false;								  
		}
	}
	
	char * data = mIILRecord->getData(rdata.getPtrNoLock());
	int len = mIILRecord->getRecordLen();
	u64 docid = mIILRecord->getDocid();
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
AosDataJoinNorm::getNextDocid(const AosRundataPtr &rdata)
{
	if (!mDocAssembler) return 0;
	return mDocAssembler->getNextDocid(rdata.getPtrNoLock());
}

	
AosDataProcStatus::E
AosDataJoinNorm::processRecord(
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


bool
AosDataJoinNorm::addToWorkingData(
		vector<AosDataJoinNorm::WorkingData> &data, 
		int &num_values,
		const char *key,
		const int len,
		const u32 idx)
{
	// If 'data' is null, it means the corresponding scanner has no more
	// data. It appends the data to the end. 
	aos_assert_r(num_values < mNumScanners, false);
	if (!key)
	{
		data[num_values].key = 0;
		data[num_values].len = 0;
		data[num_values].idx = idx;
		data[num_values].is_smallest = false;
		num_values++;
		return true;
	}
	
	for (int i=0; i<num_values; i++)
	{
		if (!data[i].key)
		{
			// This is the spot to insert
			for (int k=num_values; k>i; k--)
			{
				data[k] = data[k-1];
			}
			data[i].key = key;
			data[i].len = len;
			data[i].idx = idx;
			data[i].is_smallest = (i == 0);
			num_values++;
			return true;
		}

		int rsltInt = 0;
		if(len == data[i].len)
		{
			// Chen Ding, 06/11/2012
			if (mPrefixLen > 0 && mPrefixLen < data[i].len)
			{
				rsltInt = strncmp(key, data[i].key, mPrefixLen);
			}
			else
			{
				rsltInt = strncmp(key, data[i].key, len);
			}

			if(rsltInt == 0)
			{
				rsltInt = ((int)idx < data[i].idx) ? -1 : 1;
			}
		}
		else if(len < data[i].len)
		{
			if (mPrefixLen > 0 && mPrefixLen < len)
			{
				rsltInt = strncmp(key, data[i].key, mPrefixLen);
			}
			else
			{
				rsltInt = strncmp(key, data[i].key, len);
			}
			if(rsltInt == 0) rsltInt = -1;
		}
		else
		{
			if (mPrefixLen > 0 && mPrefixLen < data[i].len)
			{
				rsltInt = strncmp(key, data[i].key, mPrefixLen);
			}
			else
			{
				rsltInt = strncmp(key, data[i].key, data[i].len);
			}
			if(rsltInt == 0) rsltInt = 1;
		}

		if (rsltInt == 0)
		{
			// The values are the same. Insert the new data after it.
			// If it is already the last one, append it.
			if (i == num_values-1)
			{
				// Already the last one. Append it.
				data[num_values].key = key;
				data[num_values].len = len;
				data[num_values].idx = idx;
				data[num_values].is_smallest = data[i].is_smallest;
				num_values++;
				return true;
			}

			// It is not the last one. Insert it.
			for (int k=num_values; k>i+1; k--)
			{
				data[k] = data[k-1];
			}

			data[i+1].key = key;
			data[i+1].len = len;
			data[i+1].idx = idx;
			data[i+1].is_smallest = data[i].is_smallest;
			num_values++;
			return true;
		}

		if (rsltInt < 0)
		{
			// data < working_data[i].data. Need to insert it in front 
			for (int k=num_values; k>i; k--)
			{
				data[k] = data[k-1];
			}

			data[i].key = key;
			data[i].len = len;
			data[i].idx = idx;
			data[i].is_smallest = (i == 0);
			num_values++;
			return true;
		}
	}

	// It is the largest one. Append it.
	data[num_values].key = key;
	data[num_values].len = len;
	data[num_values].idx = idx;
	data[num_values].is_smallest = (num_values == 0);
	num_values++;
	return true;
}


bool
AosDataJoinNorm::mergeData(const int num_new_data)
{
	// This function merges mNewData to mWorkingData. There are 'mNumScanners'
	// entries in mWorkingData, but the first 'num_new_data' are invalid. 
	// mNewData holds 'num_new_data'. The purpose of this function is to 
	// merge mNewData[0, num_new_data] with mWorkingData[num_new_data, mNumScanners-1]. 
	// Note that null strings are always put at the end.
	int rsltInt;
	int cursor = 0;
	int new_idx = 0;
	int working_idx = num_new_data;
	while (cursor < mNumScanners)
	{
		if (new_idx >= num_new_data)
		{
			// There are no more data in 'mNewData'. Copy all the remaining
			// data from 'mWorkingData'.
			while (cursor < mNumScanners)
			{
				mWorkingData[cursor] = mWorkingData[working_idx];
				setSmallestFlag(cursor);
				cursor++;
				working_idx++;
			}
			goto sort_idx;
		}

		if (working_idx >= mNumScanners)
		{
			// There are no more data in 'mWorkingData'. Copy all the 
			// remaining data from 'mNewData'.
			while (cursor < mNumScanners)
			{
				mWorkingData[cursor] = mNewData[new_idx];
				setSmallestFlag(cursor);
				cursor++;
				new_idx++;
			}
			goto sort_idx;
		}

		// Pick the smaller one of working_data[working_idx] and 
		// new_data[new_idx].
		if (!mWorkingData[working_idx].key)
		{
			// There are no more valid data in 'working_data'. 
			// Make room for the remaining of 'new_data'.
			while (cursor < mNumScanners && new_idx < num_new_data)
			{
				mWorkingData[cursor] = mNewData[new_idx];

				setSmallestFlag(cursor);
				cursor++;
				new_idx++;
			}
			goto sort_idx;
		}

		if(mNewData[new_idx].key == 0)
		{
			rsltInt = 1;
		}
		else
		{
			if(mNewData[new_idx].len == mWorkingData[working_idx].len)
			{
				if (mPrefixLen > 0 && mPrefixLen < mNewData[new_idx].len)
				{
					rsltInt = strncmp(mNewData[new_idx].key, 
						mWorkingData[working_idx].key, mPrefixLen);
				}
				else
				{
					rsltInt = strncmp(mNewData[new_idx].key, 
						mWorkingData[working_idx].key, mNewData[new_idx].len);
				}
				if(rsltInt == 0)
				{
					rsltInt = (mNewData[new_idx].idx < mWorkingData[working_idx].idx) ? -1 : 1;
				}
			}
			else if(mNewData[new_idx].len < mWorkingData[working_idx].len)
			{
				if (mPrefixLen > 0 && mPrefixLen < mNewData[new_idx].len)
				{
					rsltInt = strncmp(mNewData[new_idx].key, 
						mWorkingData[working_idx].key, mPrefixLen);
				}
				else
				{
					rsltInt = strncmp(mNewData[new_idx].key, 
						mWorkingData[working_idx].key, mNewData[new_idx].len);
				}
				if(rsltInt == 0) rsltInt = -1;
			}
			else
			{
				if (mPrefixLen > 0 && mPrefixLen < mWorkingData[working_idx].len)
				{
					rsltInt = strncmp(mNewData[new_idx].key, 
						mWorkingData[working_idx].key, mPrefixLen);
				}
				else
				{
					rsltInt = strncmp(mNewData[new_idx].key, 
						mWorkingData[working_idx].key, mWorkingData[working_idx].len);
				}
				if(rsltInt == 0) rsltInt = 1;
			}
		}

		if (rsltInt < 0)
		{
			// new data < working data
			mWorkingData[cursor] = mNewData[new_idx++];
		}
		else if (rsltInt == 0)
		{
			mWorkingData[cursor] = mNewData[new_idx++];
		}
		else
		{
			mWorkingData[cursor] = mWorkingData[working_idx++];
		}
		setSmallestFlag(cursor);
		cursor++;
	}

sort_idx:
	// It sorts all the 'smallest' entries based on 'idx'.
	for (int i=0; i<mNumScanners; i++)
	{
		if (!mWorkingData[i].is_smallest) return true;

		// Find the smallest idx
		int small_idx = mWorkingData[i].idx;
		int pos = i;
		for (int k=i+1; k<mNumScanners; k++)
		{
			if (!mWorkingData[k].is_smallest) break;
			if (mWorkingData[k].idx < small_idx)
			{
				small_idx = mWorkingData[k].idx;
				pos = k;
			}
		}

		if (pos != i)
		{
			WorkingData ww = mWorkingData[i];
			mWorkingData[i] = mWorkingData[pos];
			mWorkingData[pos] = ww;
		}
	}
	return true;
}


bool
AosDataJoinNorm::setSmallestFlag(const u32 nn)
{
	// Need to set the 'is_smallest' flag. If it is the first entry
	// (i.e., nn == 0), set it to true. Otherwise, if the previous
	// is smallest and the current one is the same as the previous
	// one, set it to true.
	if (nn == 0)
	{
		mWorkingData[nn].is_smallest = true;
		return true;
	}
		
	mWorkingData[nn].is_smallest = false;
	if (!mWorkingData[nn-1].is_smallest) return true;
	if (!mWorkingData[nn].key) return true;
	if (mWorkingData[nn-1].len != mWorkingData[nn].len) return true;

	int rsltInt;
	if (mPrefixLen > 0 && mPrefixLen < mWorkingData[nn-1].len)
	{
		rsltInt = strncmp(mWorkingData[nn-1].key, mWorkingData[nn].key, mPrefixLen);
	}
	else
	{
		rsltInt = strncmp(mWorkingData[nn-1].key, mWorkingData[nn].key, mWorkingData[nn-1].len);
	}
	if (rsltInt != 0) return true;
	mWorkingData[nn].is_smallest = true;
	return true;
}


bool
AosDataJoinNorm::procGroupbyFinish(const AosRundataPtr &rdata)
{
	if (!mGroupbyProc) return true;
		
	bool rslt = mGroupbyProc->procFinished(mIILRecord, rdata);
	aos_assert_r(rslt, false);
			
	if (mIILRecord->getRecordLen() <= 0) return true;
	
	AosValueRslt value_rslt;
	value_rslt.setKeyValue(mIILRecord->getData(rdata.getPtrNoLock()),
		mIILRecord->getRecordLen(), false, mIILRecord->getDocid());

	if (mIILAssembler)
	{
		rslt = mIILAssembler->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}		
	if (mDocAssembler)
	{
		rslt = mDocAssembler->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
	}
	return true;
}


void
AosDataJoinNorm::printWorkingData(const int num_new_data)
{
	//OmnScreen << "New Data: " << endl;
//	for (int i=0; i<num_new_data; i++)
//	{
//		OmnScreen << "Entry: " << i << ":" << mNewData[i].key
//			<< ":" << mNewData[i].len
//			<< ":" << mNewData[i].idx << endl;
//	}

//	OmnScreen << "Working Data: " << endl;
//	for (int i=0; i<mNumScanners; i++)
//	{
//		OmnScreen << "Entry: " << i << ":" << mWorkingData[i].key
//			<< ":" << mWorkingData[i].len
//			<< ":" << mWorkingData[i].idx << endl;
//	}
}

	
bool
AosDataJoinNorm::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	bool rslt = true;
	for(int i=0; i<mNumScanners; i++)
	{
		rslt = mIILScanners[i]->setQueryContext(context, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


int
AosDataJoinNorm::getProgress() 
{
	if(mFinished)
	{
		mProgress = 100;
		return mProgress;
	}
	
	aos_assert_r(mNumScanners > 0, 0);
	int progress = 0;
	for (int i=0; i<mNumScanners; i++)
	{
		progress += mIILScanners[i]->getProgress();
	}
	mProgress = progress / mNumScanners;
	//OmnScreen << "join:[" << this << "], mProgress:" << mProgress;
	return mProgress;
}

	
AosDataJoinPtr
AosDataJoinNorm::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinNorm * join = OmnNew AosDataJoinNorm(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinNorm::checkConfig(
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
	
	int size = iilScanners.size();
	aos_assert_r(size >= 2, false);
	
	// Create the Record
	AosXmlTagPtr record_tag = def->getFirstChild(AOSTAG_RECORD);
	aos_assert_r(record_tag, false);
	
	AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(record_tag, task, rdata.getPtrNoLock());
	aos_assert_r(record, false);
	
	AosXmlTagPtr gbtag = def->getFirstChild(AOSTAG_GROUPBY_PROC);
	if (!gbtag) return true;
		
	// Check whether it is a valid tag. 
	AosGroupbyType::E type = AosGroupbyType::toEnum(gbtag->getAttrStr(AOSTAG_TYPE));
	if (!AosGroupbyType::isValid(type)) return true;
			
	AosGroupbyProcPtr groupbyProc = AosGroupbyProc::createGroupbyProc(
		gbtag, rdata);
	aos_assert_r(groupbyProc, false);

	return true;	
}

