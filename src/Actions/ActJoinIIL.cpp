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
// 05/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActJoinIIL.h"
/*
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataRecord/DataRecord.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocProc/DocAssembler.h"
#include "DocProc/DocAssemblerType.h"
#include "DataAssembler/DataAssembler.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEUtil/GroupbyProc.h"
#include "SEUtil/GroupbyType.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/DataTable.h"
#include "Util/TableAssembler.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosActJoinIIL::AosActJoinIIL(const bool flag)
:
AosSdocAction(AOSACTTYPE_JOINIIL, AosActionType::eJoinIIL, flag),
mStatus(eIdle),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin)),
mNumScanners(-1),
mDataRetrieveSuccess(true),
mPrefixLen(-1)
{
}


AosActJoinIIL::AosActJoinIIL(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_JOINIIL, AosActionType::eJoinIIL, false),
mStatus(eIdle),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mNumScanners(-1),
mDataRetrieveSuccess(true),
mPrefixLen(-1)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosActJoinIIL::~AosActJoinIIL()
{
}


AosActionObjPtr
AosActJoinIIL::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActJoinIIL(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActJoinIIL::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
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
	aos_assert_r(rdata && rdata->getSiteid() > 0, false);
	aos_assert_rr(def, rdata, false);
	
	// Chen Ding, 06/11/2012
	// Retrieve Prefix Length
	mPrefixLen = def->getAttrInt(AOSTAG_PREFIX_LENGTH, -1);

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	if (!scanners)
	{
		AosSetErrorU(rdata, "missing_iil_scanners") << ": " << def->toString();
		return false;
	}

	AosXmlTagPtr scanner_tag = scanners->getFirstChild();
	int seqno = 0;
	AosIILScannerListenerPtr thisptr(this, true);	
	while (scanner_tag)
	{
		AosIILScannerObjPtr scanner = AosIILScannerObj::createIILScannerStatic(
				thisptr, seqno++, scanner_tag, rdata);
		aos_assert_rr(scanner, rdata, false);
		mIILScanners.push_back(scanner);
		// mMatchTypes.push_back(scanner->getMatchType());
		scanner_tag = scanners->getNextChild();
	}	
	
	mNumScanners = mIILScanners.size();
	if (mNumScanners <= 0)
	{
		AosSetErrorU(rdata, "missing_iil_scanners") << ": " << def->toString();
		return false;
	}
	
	// Create the Table Assembler
	// AosXmlTagPtr table_tag = def->getFirstChild(AOSTAG_TABLE);
	// if(!table_tag)
	// {
	// 	AosSetErrorU(rdata, "missing_table") << ": " << def->toString();
	// 	return false;
	// }
	//
	// try
	// {
	// 	mTableAssembler = OmnNew AosTableAssembler(table_tag, rdata);
	// }
	// 
	// catch (...)
	// {
	// 	AosSetErrorU(rdata, "missing_table") << ": " << def->toString();
	// 	return false;
	// }

	// Create the Record
	AosXmlTagPtr record_tag = def->getFirstChild(AOSTAG_RECORD);
	if (!record_tag)
	{
		AosSetErrorU(rdata, "missing_record_conf") << ": " << def->toString();
		return false;
	}
	
	mRecord = AosDataRecordObj::createDataRecordStatic(record_tag, rdata);
	if (!mRecord)
	{
		AosSetErrorU(rdata, "missing_record") << ": " << def->toString();
		return false;
	}

	// Create IILAssembler
	AosXmlTagPtr assembler = def->getFirstChild("iilassember");
	if (assembler)
	{
		mIILAssembler = AosDataAssembler::getDataAssembler(
			mTask, assembler, rdata AosMemoryCheckerArgs);
		aos_assert_rr(mIILAssembler, rdata, false);
	}

	// Create DocAssembler
	assembler = def->getFirstChild("docassembler");
	if (assembler)
	{
		// Chen Ding, 06/19/2012
		// Check whether it is a valid tag. 
		AosDocAssemblerType::E type = AosDocAssemblerType::toEnum(
				assembler->getAttrStr(AOSTAG_TYPE));
		if (AosDocAssemblerType:.isNull(type))
		{
			try
			{
				mDocAssembler = OmnNew AosDocAssembler(
					mTask, 0, mRecord, assembler, rdata AosMemoryCheckerArgs);
			}
	
			catch (...)
			{
				OmnAlarm << "Failed creating doc assembler" << enderr;
				return false;
			}
		}
	}

	// Chen Ding, 05/29/2012
	AosXmlTagPtr gbtag = def->getFirstChild(AOSTAG_GROUPBY_PROC);
	if (gbtag)
	{
		// Chen Ding, 06/19/2012
		// Check whether it is a valid tag. 
		AosGroupbyType::E type = AosGroupbyType::toEnum(gbtag->getAttrStr(AOSTAG_TYPE));
		if (AosGroupbyType:.isNull(type))
		{
			try
			{
				mGroupbyProc = OmnNew AosGroupbyProc(gbtag, rdata);
			}

			catch (...)
			{
				AosSetErrorU(rdata, "failed_create_groupbyproc") << ": " << def->toString();
				return false;
			}
		}
	}

	return true;
}


bool 
AosActJoinIIL::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
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
	for (u32 i=0; i<mIILScanners.size(); i++)
	{
		bool rslt = mIILScanners[i]->start(rdata);
		if (!rslt)
		{
			OmnAlarm << "failed: " << rdata->getErrmsg() << ":" << toString() << enderr;
			actionFailed();
			return false;
		}
	}
	
	if (mThread)
	{
		AosSetErrorU(rdata, "internal_error");
		return false;
	}

	mStatus = eIdle;
	OmnThreadedObjPtr thisptr(this, true);
	mThread = OmnNew OmnThread(thisptr, "joiniil", 0, false, true);
	mThread->start();
	return true;
}
	

bool
AosActJoinIIL::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	mTask = task;
	if(mIILAssembler) mIILAssembler->setTask(task);
	if(mDocAssembler) mDocAssembler->setTask(task);
	return run(sdoc, rdata);
}


bool
AosActJoinIIL::noMoreData(const AosIILScannerObjPtr &scanner, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosActJoinIIL::dataRetrieved(const AosIILScannerObjPtr &scanner, const AosRundataPtr &rdata)
{
	// aos_assert_rr(scanner, rdata, false);
	// int idx = scanner->getSeqno();
	// aos_assert_rr(idx >= 0 && (u32)idx < mIILScanners.size(), rdata, false);
	// aos_assert_rr(mIILScanners[idx], rdata, false);
	mLock->lock();
	mDataRetrieveSuccess = true;
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool	
AosActJoinIIL::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
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
	AosDataProcStatus::E status;
	
	const char *key = 0, *new_key = 0;
	
	mStartTime = OmnGetSecond();
	OmnScreen << "join iil start , time:" << mStartTime << endl;

	for (int i=0; i<mNumScanners; i++)
	{
		WorkingData work_data;
		work_data.key = 0;
		work_data.len = 0;
		work_data.idx = i;
		work_data.is_smallest = false;
		mWorkingData.push_back(work_data);
		mNewData.push_back(work_data);
	}
	
	bool rslt;
	if(mIILAssembler)
	{
		rslt = mIILAssembler->sendStart(mRundata);
		if(!rslt)
		{
			state = OmnThrdStatus::eExit;
			actionFailed();
			return false;
		}
	}
	
	if (mDocAssembler)
	{
		rslt = mDocAssembler->sendStart(mRundata);
		if(!rslt)
		{
			state = OmnThrdStatus::eExit;
			actionFailed();
			return false;
		}
	}

	int num_working_data = 0;
	int num_new_data = 0;
	int len;
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		switch (mStatus)
		{
		case eIdle:
			 // This action is retrieving data. It checks
		 	 // whether it has received all the data. If not, do nothing. 
		 	 // Otherwise, it moves on to the next state.

			 // It has received all the data. Need to retrieve a value from 
			 // every IIL.
			 num_working_data = 0;
		 	 rslt = true;
			 for (int i=0; i<mNumScanners; i++)
		 	 {
				 status = mIILScanners[i]->moveNext(key, len, -1, mRundata);
				 switch (status)
				 {
			 	 case AosDataProcStatus::eOk:
				 	  rslt = addToWorkingData(mWorkingData, num_working_data, key, len, i);
					  break;

				 case AosDataProcStatus::eRetrievingData:
					  mCondVar->wait(mLock);
					  if(!mDataRetrieveSuccess) rslt = false;
					  i--;
					  break;

				 case AosDataProcStatus::eNoMoreData:
				 	  rslt = addToWorkingData(mWorkingData, num_working_data, 0, 0, i);
					  break;

				 default:
			 	 	  rslt = false;
					  break;
			 	 }
				 
				 if(!rslt)
				 {
					 state = OmnThrdStatus::eExit;
					 mLock->unlock();
					 actionFailed();
		 	 		 return false;
				 }
		 	 }
			 // Chen Ding, 06/11/2012
			 for (int i=1; i<num_working_data; i++)
			 {
			 	setSmallestFlag(i);
				if (!mWorkingData[i].is_smallest) break;
			 }
		 	 mStatus = eActive;

		case eActive:
		 	 break;

		case eFinished:
			 state = OmnThrdStatus::eExit;
			 mLock->unlock();
		 	 return actionFinished();

		default:
			 state = OmnThrdStatus::eExit;
			 mLock->unlock();
			 actionFailed();
			 return false;
		}

		// This is the main process logic. It loops over all the scanner, 
		// finding the smallest value, and then asking all others to generate
		// values for the smallest one. After that, it creates a record, appended
		// it to the table, and then move the smallest one to the next, and then
		// repeat this process.
		
		AosValueRslt value_rslt;
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
			 	state = OmnThrdStatus::eExit;
				mStatus = eFinished;
				mLock->unlock();
		 	 	return actionFinished();
			}

			// Step 1: Set the matched values
			// 'WorkingData' holds the next data to be processed for all
			// the scanners, values are sorted. If the value is the smallest,
			// its flag 'is_smallest' is marked to be true. The following
			// loops processes the smallest values first.
			int smallestIdx = 0;
			mRecord->reset(mRundata);

			while (mWorkingData[smallestIdx].is_smallest && smallestIdx < mNumScanners)
			{
				// It is the smallest one. Let the IIL scanner check whether the 
				// value matches. 
				if (!mWorkingData[smallestIdx].key)
				{
			 		state = OmnThrdStatus::eExit;
					mLock->unlock();
			 		actionFailed();
			 		return false;
				}

				// If not match, it needs to move to the next. 
				// If all 'smallest' scanners match the value, an entry may (or may
				// not be created depending on all the 'not-matched'. 
				int idx = mWorkingData[smallestIdx].idx;
				status = mIILScanners[idx]->setMatchedValue(mRecord, -1, mRundata);
				switch (status)
				{
				case AosDataProcStatus::eOk:
					 break;

				case AosDataProcStatus::eError:
				default:
			 		 state = OmnThrdStatus::eExit;
					 mLock->unlock();
			 		 actionFailed();
			 		 return false;
				}
				smallestIdx++;
			}

			// Step 2: Move all the smallest forward.
			// Chen Ding, 05/22/2012
			// If there is only one smallest and the smallest match type is 
			// mapper, it should move the cursor only. No record shall be generated.
			num_new_data = 0;
			rslt = true;
			bool rejected = false;
			int len;
			for (int i=0; i<smallestIdx; i++)
			{
				int idx = mWorkingData[i].idx;
				status = mIILScanners[idx]->moveNext(new_key, len, smallestIdx, mRundata);
				switch (status)
				{
				case AosDataProcStatus::eOk:
					 rslt = addToWorkingData(mNewData, num_new_data, new_key, len, idx);
					 break;
				
				case AosDataProcStatus::eRetrievingData:
					 mCondVar->wait(mLock);
					 if(!mDataRetrieveSuccess) rslt = false;
					 i--;
					 break;

				case AosDataProcStatus::eNoMoreData:
				 	 rslt = addToWorkingData(mNewData, num_new_data, 0, 0, idx);
					 break;

				case AosDataProcStatus::eRejected:
					 rejected = true;
					 break;

				default:
					 rslt = false;
					 break;
				}
				
				if(!rslt)
				{
			 		state = OmnThrdStatus::eExit;
					mLock->unlock();
			 		actionFailed();
			 		return false;
				}
			}

			// Step 3: Set not-matched values
			// It finished processing the smallest values. Need to Process the 
			// remaining IILs.
			if (!rejected)
			{
				for (int i=smallestIdx; i<mNumScanners; i++)
				{
					int idx = mWorkingData[i].idx;
					if(mIILScanners[idx]->isReject())
					{
						rejected = true;
						break;
					}
				
					status = mIILScanners[idx]->setNotMatchedValue(mRecord, mRundata);
					switch (status)
					{
					case AosDataProcStatus::eOk:
						 break;
	
					default:
			 			 state = OmnThrdStatus::eExit;
						 mLock->unlock();
			 			 actionFailed();
			 			 return false;
					}
				}
			}

			if (!rejected)
			{
				if (mGroupbyProc)
				{
					//OmnScreen << "append into group record:" 
					// << mRecord->getData() << ":" << mRecord->getRecordLen() 
					// << ":" << mRecord->getDocid() << endl;
					AosDataProcStatus::E status = mGroupbyProc->procRecord(mRecord, mRundata);
					value_rslt.setKeyValue(mRecord->getData(), mRecord->getRecordLen(), 
								 false, mRecord->getDocid());
					switch (status)
					{
					case AosDataProcStatus::eOk:
						 if (mIILAssembler)
						 {

							 // Chen Ding, 07/08/2012
							 // rslt = mIILAssembler->appendEntry(mRecord->getData(),
							 // 	mRecord->getRecordLen(), mRecord->getDocid(), mRundata);
							 rslt = mIILAssembler->appendEntry(value_rslt, mRundata);
							 if(!rslt)
							 {
			 			 		state = OmnThrdStatus::eExit;
						 		mLock->unlock();
			 			 		actionFailed();
			 			 		return false;
							 }
						 }

						 if (mDocAssembler)
						 {
							 // rslt = mDocAssembler->appendEntry(mRecord->getData(),
							 // 	mRecord->getRecordLen(), mRecord->getDocid(), mRundata);
							 rslt = mDocAssembler->appendEntry(value_rslt, mRundata);
							 if(!rslt)
							 {	
			 			 		state = OmnThrdStatus::eExit;
						 		mLock->unlock();
			 			 		actionFailed();
			 			 		return false;
							 }
						 }
						 break;

					case AosDataProcStatus::eRejected:
						 // There is no need to append the record
						 break;

					default:
			 			 state = OmnThrdStatus::eExit;
						 mLock->unlock();
			 			 actionFailed();
			 			 return false;
					}
				}
				else
				{
					value_rslt.setKeyValue(mRecord->getData(), mRecord->getRecordLen(), 
								 false, mRecord->getDocid());
					if (mIILAssembler)
					{
						// Chen Ding, 07/08/2012
						// rslt = mIILAssembler->appendEntry(mRecord->getData(),
						// 	mRecord->getRecordLen(), mRecord->getDocid(), mRundata);	
						rslt = mIILAssembler->appendEntry(value_rslt, mRundata);
						if (!rslt)
						{
			 				state = OmnThrdStatus::eExit;
						 	mLock->unlock();
			 			 	actionFailed();
			 			 	return false;
						}
					}

					if (mDocAssembler)
					{
						// Chen Ding, 07/08/2012
						// rslt = mDocAssembler->appendEntry(mRecord->getData(),
						// 	mRecord->getRecordLen(), mRecord->getDocid(), mRundata);
						rslt = mDocAssembler->appendEntry(value_rslt, mRundata);
						if(!rslt)
						{
			 				state = OmnThrdStatus::eExit;
						 	mLock->unlock();
			 			 	actionFailed();
			 			 	return false;
						}
					}
				}
			}

			if (num_new_data <= 0)
			{
			 	state = OmnThrdStatus::eExit;
				mLock->unlock();
			  	actionFailed();
			 	return false;
			}

			// Merge new_data to working_data.
			// 'new_data' holds only the values that used to be the smallest. 
			mergeData(num_new_data);
		}
	}
	
	// Should never come here;
	state = OmnThrdStatus::eExit;
	return true;
}


bool	
AosActJoinIIL::signal(const int threadLogicId)
{
	return true;
}


bool    
AosActJoinIIL::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosActJoinIIL::addToWorkingData(
		vector<AosActJoinIIL::WorkingData> &data, 
		int &num_values,
		const char *key,
		const int len,
		const u32 idx)
{
	// If 'data' is null, it means the corresponding scanner has no more
	// data. It appends the data to the end. 
	aos_assert_r(num_values < mNumScanners, false);
	// Chen Ding, 08/01/2012
	// if (!key)
	if (!key || len <= 0)
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
AosActJoinIIL::mergeData(const int num_new_data)
{
	// This function merges mNewData to mWorkingData. There are 'mNumScanners'
	// entries in mWorkingData, but the first 'num_new_data' are invalid. 
	// mNewData holds 'num_new_data'. The purpose of this function is to 
	// merge mNewData[0, num_new_data] with mWorkingData[num_new_data, mNumScanners-1]. 
	// Note that null strings are always put at the end.
	int rsltInt;
	u32 cursor = 0;
	int new_idx = 0;
	int working_idx = num_new_data;
	while (cursor < (u32)mNumScanners)
	{
		if (new_idx >= num_new_data)
		{
			// There are no more data in 'mNewData'. Copy all the remaining
			// data from 'mWorkingData'.
			while (cursor < (u32)mNumScanners)
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
			while (cursor < (u32)mNumScanners)
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
			while (cursor < (u32)mNumScanners && new_idx < num_new_data)
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
AosActJoinIIL::setSmallestFlag(const u32 nn)
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


OmnString 
AosActJoinIIL::toString() const
{
	OmnNotImplementedYet;
	return "";
}


bool 
AosActJoinIIL::dataRetrieveFailed(
		const AosIILScannerObjPtr &scanner, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mDataRetrieveSuccess = false;
	mCondVar->signal();
	mLock->unlock();
	return false;
}


bool
AosActJoinIIL::actionFinished()
{
	bool rslt = sendAll();
	aos_assert_r(rslt, false);

	AosActionObjPtr thisptr(this, true);
	if(mTask) mTask->actionFinished(thisptr, mRundata);
			
	mEndTime = OmnGetSecond();
	OmnScreen << "join iil finished, end time:" << mEndTime << endl;
	OmnScreen << "join iil spend:" << (mEndTime - mStartTime) << endl;
	return true;
}


bool
AosActJoinIIL::actionFailed()
{
	bool rslt = sendAll();
	aos_assert_r(rslt, false);
	
	AosActionObjPtr thisptr(this, true);
	if(mTask) mTask->actionFailed(thisptr, mRundata);
		 	 
	AosSetErrorU(mRundata, "action failed") << ": " << toString();

	mEndTime = OmnGetSecond();
	OmnScreen << "join iil failed, end time:" << mEndTime << endl;
	OmnScreen << "join iil spend:" << (mEndTime - mStartTime) << endl;
	return true;
}


bool
AosActJoinIIL::sendAll()
{
	bool rslt;
	if (mGroupbyProc)
	{
		rslt = mGroupbyProc->procFinished(mRecord, mRundata);
		aos_assert_r(rslt, false);
		
		AosValueRslt value_rslt;
		value_rslt.setKeyValue(mRecord->getData(), mRecord->getRecordLen(), 
					 false, mRecord->getDocid());
		if (mRecord->getRecordLen() > 0)
		{
			if (mIILAssembler)
			{
				// Chen Ding, 07/08/2012
				// rslt = mIILAssembler->appendEntry(mRecord->getData(),
				// 	mRecord->getRecordLen(), mRecord->getDocid(), mRundata);
				rslt = mIILAssembler->appendEntry(value_rslt, mRundata);
				if(!rslt)
				{
					OmnAlarm << "record error, " << mRecord->getData() 
						<< ":" << mRecord->getRecordLen() 
						<< ":" << mRecord->getDocid() << enderr; 
				}
			}		
			
			if (mDocAssembler)
			{
				// Chen Ding, 07/08/2012
				// rslt = mDocAssembler->appendEntry(mRecord->getData(),
				// 	mRecord->getRecordLen(), mRecord->getDocid(), mRundata);
				rslt = mDocAssembler->appendEntry(value_rslt, mRundata);
				if(!rslt)
				{
					OmnAlarm << "record error, " 
						<< mRecord->getData() << ":" << mRecord->getRecordLen() 
						<< ":" << mRecord->getDocid() << enderr; 
				}
			}
		}
	}

	if (mIILAssembler)
	{
		rslt = mIILAssembler->sendFinish(mRundata);
		aos_assert_r(rslt, false);
	}

	if (mDocAssembler)
	{
		rslt = mDocAssembler->sendFinish(mRundata);
		aos_assert_r(rslt, false);
	}
	return true;
}


void
AosActJoinIIL::printWorkingData(const int num_new_data)
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
*/
