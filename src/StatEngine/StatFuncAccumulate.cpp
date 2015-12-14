////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/09/30 Created by Phil 
////////////////////////////////////////////////////////////////////////////

#include "StatEngine/StatQryEngine.h"

#include "Thread/Sem.h"
#include "SEInterfaces/RlbFileType.h"
#include "StatServer/Vt2dModifyReq.h"
#include "StatServer/Vt2dReadReq.h"
#include "StatUtil/StatTimeUnit.h"
#include "Thread/ThreadPool.h"
#include "Util/StrSplit.h"
#include "Vector2DQryRslt/Vt2dQryRslt.h"
#include "Vector2DQryRslt/Vt2dQryRsltProc.h"
#include "Vector2D/Vector2D.h"
#include "CounterUtil/CounterUtil.h"  
#include "Debug/Debug.h"

////////////////////////////////////////////////////////////////
//     Accumulate Statistics functions
//  Accumulate means latter statistics values collect previous
//  statistics values as a total accumulation.
//
//  E.g:
//  Normal count(*) group by _day()
//  01-01: 100
//  01-02: 50
//  01-03: 20
//
//  Accumulate count(*) group by _day()
//  01-01: 100
//  01-02: 150 
//  01-03: 170 
//
//  This also apply to sum, max, min, etc
////////////////////////////////////////////////////////////////

//
// Generate accumulated aggregation values
//
bool
AosStatQryEngine::applyAccumulate()
{
	bool rslt;
	if (!mHasAccumulate)
		return true;

	//if (mStatRecordList.size() <= 1)
	//	return true;

	if (mOrderedGrpbyFieldList.size() < 1)
		return true;
	if(mStatRecordList.size() <= 0)
		return true;

	AosStatRecord* rcd;
	AosStatRecord* prevRcd;
	prevRcd = mStatRecordList[0].getPtr();
	for (u32 i = 1; i < mStatRecordList.size(); i++)
	{
		rcd = mStatRecordList[i].getPtr();
		//compare groupby keys
		if (inSameAccumulateGroup(prevRcd->getKeyValueList(), 
					rcd->getKeyValueList()) )
		{
			//handle accumulate measures
			rcd->addValue(mRundata, prevRcd, true);
		}

		prevRcd = rcd;
	}

	//add missing entry to the results
	int num = mStatRecordList.size();
	mMissStatRecords.resize(num * 3);
	mMissStatRecords.clear();

	rslt = appendMissingRecord();
	aos_assert_r(rslt,false);

	mStatRecordList = mMissStatRecords;
	//for(size_t i = 0; i < mMissStatRecords.size(); i++)
	//{
	//	mStatRecordList.push_back(mMissStatRecords[i]);
	//}

	//int num = sortStatRecord();
	
	aos_assert_r(num >= 0, false);

	return true;
}

//
//check if the 1st keyValueList is the same as 2nd
//key value list except the inner-most groupby key.
//
//Accumulation will be based on the inner-most groupby 
//key which is "time" in the following example
//
//	k1  k2  time  sum(f1)
//	k11 k21 01-01 5
//	k11 k21 01-02 6
//	k11 k21 01-03 7
//Above 3 belongs to 1st group of accumulation
//
//	k11 k22 01-01 6
//	k11 k22 01-01 6
//	k11 k22 01-01 6
//Above 3 belongs to 2nd group of accumulation
//
bool
AosStatQryEngine::inSameAccumulateGroup(OmnValueList *keyValueList1,
							OmnValueList *keyValueList2)
{
	aos_assert_r(keyValueList1 && keyValueList2, false);

	aos_assert_r(keyValueList1->size() == keyValueList2->size(), false);

	//compare the key values except the inner-most 
	//group by key
	for (u32 i = 0; i < keyValueList1->size() - 1; i++)
	{
		// by andy
		if ((*keyValueList1)[i].isNull() || (*keyValueList2)[i].isNull()) return false;
		AosDataType::E type = AosDataType::autoTypeConvert((*keyValueList1)[i].getType(), (*keyValueList2)[i].getType());
		if (AosValueRslt::doComparison(eAosOpr_ne, type, (*keyValueList1)[i], (*keyValueList2)[i]))
			return false;
	}

	return true;
}


//for example
//e.g
//select msi, ot, dt, count(accumulate *)
//from cdr
//	where (ot>=0601 and ot<=0604) and (dt>=0601 and dt<=0604)
//		group by msi, ot, _day(dt);
//
//msi  ot   dt 		count(accumulate *)
//3600 0601 0601 	0 
//3600 0601 0602 	0 
//3600 0601 0603 	3 
//3600 0601 0604 	5
//
//3700 0602 0601 	0 
//3700 0602 0602 	1 
//3700 0602 0603 	1 
//3700 0602 0604 	3
//
//We need to make sure that for each (msi, ot), there should be
//  4 entries, that is, the last key needs to spread from 0601 to
//  0604 as specified in the time field condition:
//  (dt>=0601 and dt<=0604)
//If some entries not listed, we need to add from previous entry. For 
//example:
//3700 0602 0602 1 
//3700 0602 0604 3
//We know that 0601 and 0603 are missing, then we can add them:
//3700 0602 0601 0 <---- create one and set the aggr value to be 0 
//3700 0602 0602 1 
//3700 0602 0603 1 <---- create one and set the aggr value to be 1, same as the previous one 0602's
//3700 0602 0604 3
bool 
AosStatQryEngine::appendMissingRecord()
{
	//only when there have timeField
	if(mQryInfo->mQryTimeConds.size() <= 0) return true;
	i64 startTime = 0, endTime = 0;

	AosRundataPtr rdata = OmnApp::getRundata();
	//now we assume that there is one timeCondition
	aos_assert_r(mQryInfo->mQryTimeConds.size() == 1, false);
	for(size_t i = 0; i < mQryInfo->mQryTimeConds.size(); i++)
	{
		AosStatTimeUnit::E statTimeUnit = mQryInfo->mStatTimeUnit;
		startTime = AosStatTimeUnit::parseTimeValue(mQryInfo->mQryTimeConds[i].start_time, statTimeUnit, mQryInfo->mGrpbyTimeUnit);
		endTime = AosStatTimeUnit::parseTimeValue(mQryInfo->mQryTimeConds[i].end_time, statTimeUnit, mQryInfo->mGrpbyTimeUnit);
		//resolveQryTime(mQryInfo->mQryTimeConds[i], mQryInfo->mGrpbyTimeUnit);			
		//startTime = mQryInfo->mQryTimeConds[i].start_time;
		//endTime = mQryInfo->mQryTimeConds[i].end_time;
		aos_assert_r(startTime != -1 || endTime != -1, false);		
	}
	AosStatRecord *prevRcd = mStatRecordList[0].getPtr();

	i64 timeFieldPriv = 0;
	i64 timeFieldNext = 0;
	bool notEndFlag = false;
	i64 timeValue = startTime;
	timeFieldPriv = prevRcd->getTimeFieldValue();

	if(mStatRecordList.size() == 1)
	{
		while(startTime < timeFieldPriv)
		{
			AosStatRecord* newRecord = createMissingRecord(prevRcd);
			newRecord->setTimeFieldValue(startTime);
			newRecord->resetAccumulate();
			//addOneRecord(newRecord);
			mMissStatRecords.push_back(newRecord);	
			startTime++;
		}
		mMissStatRecords.push_back(mStatRecordList[0]);
		while(timeFieldPriv < endTime)
		{
			AosStatRecord* newRecord = createMissingRecord(prevRcd);
			timeFieldPriv++;
			newRecord->setTimeFieldValue(timeFieldPriv);
			//addOneRecord(newRecord);
			mMissStatRecords.push_back(newRecord);
		}
		return true;
	}

	for (u32 i = 1; i < mStatRecordList.size(); i++)
	{
		timeFieldNext = mStatRecordList[i]->getTimeFieldValue();
		if(inSameAccumulateGroup(prevRcd->getKeyValueList(),
					mStatRecordList[i]->getKeyValueList()))
		{
			notEndFlag = true;
			if(startTime + 1 <= timeFieldPriv && timeValue == startTime)
			{
				while(startTime < timeFieldPriv)
				{
					AosStatRecord* newRecord = createMissingRecord(prevRcd);
					newRecord->setTimeFieldValue(startTime);
					newRecord->resetAccumulate();
					//addOneRecord(newRecord);
					mMissStatRecords.push_back(newRecord);
					startTime++;
				}
				mMissStatRecords.push_back(prevRcd);
				if(i == mStatRecordList.size() -1)
				{
					//mMissStatRecords.push_back(mStatRecordList[i-1]);
					while(timeFieldPriv < timeFieldNext -1)
					{
						AosStatRecord* newRecord = createMissingRecord(prevRcd);
						timeFieldPriv++;
						newRecord->setTimeFieldValue(timeFieldNext);
						//addOneRecord(newRecord);
						mMissStatRecords.push_back(newRecord);
					}
					mMissStatRecords.push_back(mStatRecordList[i]);
					while(timeFieldNext < endTime)
					{
						AosStatRecord* newRecord = createMissingRecord(mStatRecordList[i].getPtr());
						timeFieldNext++;
						newRecord->setTimeFieldValue(timeFieldNext);
						//addOneRecord(newRecord);
						mMissStatRecords.push_back(newRecord);
					}
					break;
				}
				if(startTime + 1 < timeFieldNext)
				{
					startTime++;
					while(startTime < timeFieldNext)
					{
						AosStatRecord* newRecord = createMissingRecord(prevRcd);
						newRecord->setTimeFieldValue(startTime);
						newRecord->resetAccumulate();
						mMissStatRecords.push_back(newRecord);
						startTime++;
					}
				}
			}
			else if(timeFieldPriv + 1 == timeFieldNext)
			{
				mMissStatRecords.push_back(mStatRecordList[i-1]);
				//mMissStatRecords.push_back(mStatRecordList[i]);
				timeFieldPriv = timeFieldNext;
				prevRcd = mStatRecordList[i].getPtr();
				startTime = timeFieldNext;
				//the last record not
				if(i == mStatRecordList.size() -1)
				{
					mMissStatRecords.push_back(mStatRecordList[i]);
					while(timeFieldNext < endTime)
					{
						AosStatRecord* newRecord = createMissingRecord(prevRcd);
						timeFieldNext++;
						newRecord->setTimeFieldValue(timeFieldNext);
						//addOneRecord(newRecord);
						mMissStatRecords.push_back(newRecord);
					}
					break;
				}
				continue;
			}
			else 
			{
				timeFieldPriv++;
				mMissStatRecords.push_back(mStatRecordList[i-1]);
				while(timeFieldPriv < timeFieldNext)
				{
					AosStatRecord* newRecord = createMissingRecord(prevRcd);
					newRecord->setTimeFieldValue(timeFieldPriv);
					//addOneRecord(newRecord);
					mMissStatRecords.push_back(newRecord);
					timeFieldPriv++;
				}
				if(i == mStatRecordList.size() -1)
				{
					mMissStatRecords.push_back(mStatRecordList[i]);
					while(timeFieldNext < endTime)
					{
						AosStatRecord* newRecord = createMissingRecord(mStatRecordList[i].getPtr());
						timeFieldNext++;
						newRecord->setTimeFieldValue(timeFieldNext);
						//addOneRecord(newRecord);
						mMissStatRecords.push_back(newRecord);
					}
					break;
				}
			}
			startTime = timeFieldPriv;
		}
		else
		{
			//this mean timeFieldPriv < endTime, wo should append all missing record whose timeField between timeFieldPriv and endTime
			i64 tempTime;
			if(notEndFlag) tempTime = timeFieldPriv;
			else tempTime = startTime;

			while(tempTime < timeFieldPriv)
			{
				AosStatRecord* newRecord = createMissingRecord(prevRcd);
				newRecord->setTimeFieldValue(tempTime);
				newRecord->resetAccumulate();
				//addOneRecord(newRecord);
				mMissStatRecords.push_back(newRecord);
				tempTime++;
			}
			mMissStatRecords.push_back(mStatRecordList[i-1]);
			while(timeFieldPriv < endTime)
			{
				AosStatRecord* newRecord = createMissingRecord(prevRcd);
				timeFieldPriv++;
				newRecord->setTimeFieldValue(timeFieldPriv);
				//addOneRecord(newRecord);
				mMissStatRecords.push_back(newRecord);
			}
			notEndFlag = false;
			startTime = timeValue;
		}

		//the last record not
		if(i == mStatRecordList.size() -1 && !notEndFlag)
		{
			prevRcd = mStatRecordList[i].getPtr();

			while(startTime < timeFieldNext)
			{
				AosStatRecord* newRecord = createMissingRecord(prevRcd);
				newRecord->setTimeFieldValue(startTime);
				newRecord->resetAccumulate();
				//addOneRecord(newRecord);
				mMissStatRecords.push_back(newRecord);
				startTime++;
			}

			mMissStatRecords.push_back(prevRcd);

			while(timeFieldNext < endTime)
			{
				AosStatRecord* newRecord = createMissingRecord(prevRcd);
				timeFieldNext++;
				newRecord->setTimeFieldValue(timeFieldNext);
				//addOneRecord(newRecord);
				mMissStatRecords.push_back(newRecord);
			}
			break;
		}
		timeFieldPriv = timeFieldNext;
		prevRcd = mStatRecordList[i].getPtr();
	}
	return true;
}

bool
AosStatQryEngine::resolveQryTime(AosStatTimeArea &oriTimeArea,AosStatTimeUnit::E timeUnit)
{
	AosStatTimeUnit::init();
	int64_t start_time = -1, end_time = -1;
	aos_assert_r(oriTimeArea.start_time != -1 || oriTimeArea.end_time != -1, false);

	if(oriTimeArea.start_time != -1)                                   
	{                                                                         
		start_time = AosStatTimeUnit::parseTimeValue(oriTimeArea.start_time,  
				oriTimeArea.time_unit, timeUnit);          
		aos_assert_r(start_time >=0, false);                                  
	}                                                                         

	if(oriTimeArea.end_time != -1)                                     
	{
		end_time = AosStatTimeUnit::parseTimeValue(oriTimeArea.end_time,
				oriTimeArea.time_unit, timeUnit);          
		aos_assert_r(end_time >=0, false);                                    
	}

	AosStatTimeArea new_area(start_time, end_time, timeUnit); 
	oriTimeArea = new_area;		

	return true;                                                                  
}
	
bool 
AosStatQryEngine::addOneRecord(AosStatRecord* &prevRcd)
{
	mMissStatRecords.push_back(prevRcd);
	return true;
}

AosStatRecord*
AosStatQryEngine::createMissingRecord(
			AosStatRecord *statRcd)
			//OmnStringList * keyValueList,
			//OmnStringList statKeyValueList)
{
	OmnValueList *keyValueList = statRcd->getKeyValueList();
	OmnValueList* keyValueListNew = OmnNew OmnValueList();
	if(keyValueList)
		(*keyValueListNew).insert((*keyValueListNew).begin(),(*(keyValueList)).begin(), (*(keyValueList)).end());	
	OmnStringList statKeyValueList = statRcd->getStatKeyValues();

	vector<MeasureInfo> *measureInfoList = new vector<MeasureInfo>();
		(*measureInfoList).insert((*measureInfoList).begin(),(*(mMeasureInfoList)).begin(), (*(mMeasureInfoList)).end());
	AosStatRecord *rcd = new AosStatRecord(keyValueListNew, measureInfoList AosMemoryCheckerArgs);

	//clone keyValueList and StatKeyValueList
	//The accumulation group by field need to
	//be modified for both of them
	rcd->setStatKeyValues(statKeyValueList);
	initStatRecord(rcd);
	//(mStatKeyValueList).insert((mStatKeyValueList).begin(),((statRcd.mStatKeyValueList)).begin(), ((statRcd.mStatKeyValueList)).end());	

	//clone measure values and change the accumulate value
	u32	len = mMeasureInfoList->size() * 8;
	//mMeasureValues = (char *)calloc(len+1, sizeof(char));
		if(statRcd->getMeasureValues())
			memcpy(rcd->getMeasureValues(), statRcd->getMeasureValues(), len + 1);

	return rcd;
}

