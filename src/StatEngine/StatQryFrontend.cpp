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

///////////////////////////////////////////////////////
//   Frontend-used methods to consolidate data from
//   different cubes
///////////////////////////////////////////////////////
//init qry engine for frontend daemon
bool
AosStatQryEngine::frontInit()
{
	aos_assert_r(basicInit(), false);

	aos_assert_r(genFieldNameMap(), false);

	//init the having condition expression
	OmnString havingStr = mQryInfo->mHavingCond;
	mHavingExpr = 0;
	if (havingStr != "")
	{
		havingStr.unescape();

		// Chen Ding, 2015/02/11
		// mHavingExpr = AosParseJQL(havingStr, false, mRundata);
		OmnString errmsg;
		mHavingExpr = AosParseExpr(havingStr, errmsg, mRundata);
		aos_assert_r(mHavingExpr , false);
	}
	
	return true;
}

//
//Called from frontend side to merge results from different
// cubes
//
int 
AosStatQryEngine::mergeStatRecord(AosStatQryEngine *engine,
								 bool inOrder)
{
	int merged  = 0;
	StatRecordList * rcdList;
	u64 tStart, tEnd;

	tStart = OmnGetTimestamp();
	aos_assert_r(engine, -1);
	rcdList = engine->getStatData();
	//merged = mergeStatRecord(rcdList, inOrder);

	//erase stat records from the engine which records
	//are merged into curent engine
	rcdList->clear();
	tEnd = OmnGetTimestamp();
	//mMergeTime += tEnd - tStart;
	mMergeNum++;

	OmnScreen << "(Statistics counters : Engine) Total stat records after merging: " << 
		mStatRecordList.size() << " ,cost time : " << (tEnd - tStart) <<endl;
	return merged;
}

// JIMODB-1368   Pay	2015-12-13
int 
AosStatQryEngine::mergeStatRecord()
{
	int merged  = 0;
	merged = mergeStatRecord(mMergeList, true);

	return merged;
}

int
AosStatQryEngine::mergeStatRecord(vector<StatRecordList* > recordList,
								bool inOrder)
{
	//JIMODB-1426
	//arvin 2015.12.14
	if(recordList.empty())
		return 0;
	u64 tStart,tEnd;
	int merged = 0;
	vector<StatRecordList *> mergeList ;
	StatRecordList *targetList;
	AosStatRecord *rcd;
	tStart = OmnGetTimestamp();
	while(recordList.size()>1)
	{
		u32 size = recordList.size();
		for(u32 i = 0; i < size; i+=2)
		{
			if((i+1) < size)
			{
				targetList = mergeStatRecord(recordList[i],recordList[i+1],inOrder);
				mMergeNum++;
				mergeList.push_back(targetList);
			}
			else
			{
				targetList = new StatRecordList;
				*targetList = *recordList[i];
				mergeList.push_back(targetList);
			}
		}
		vector<StatRecordList *>::iterator itr = recordList.begin();
		for(; itr != recordList.end();itr++)
		{
			aos_assert_r(*itr, 0);
			delete *itr;
		}

		recordList = mergeList;
		mergeList.clear();
	}
	StatRecordList::iterator itr = recordList[0]->begin();
	for(;itr != recordList[0]->end(); itr++)
	{
		rcd = itr->getPtrNoLock();
		initStatRecord(rcd);
		mStatRecordList.push_back(rcd);
	}
	
	tEnd = OmnGetTimestamp();
	mMergeTime += tEnd - tStart;

	return merged;
}

StatRecordList*
AosStatQryEngine::mergeStatRecord(StatRecordList *rcdList1,
								StatRecordList *rcdList2,
								bool inOrder)
{
	u64 tStart, tEnd;
	int cmpRslt;
	int numLocal = 0;
	int num = 0;

	StatRecordList::iterator itr1, itr2;
	//merge mStatRecordList and recordList into targetList
	StatRecordList *targetList = new StatRecordList;
	AosStatRecord *rcd1, *rcd2;

	itr1 = rcdList1->begin();
	itr2 = rcdList2->begin();

	while (itr1 != rcdList1->end() &&
			itr2 != rcdList2->end())
	{
		tStart = OmnGetTimestamp();
		rcd1 = itr1->getPtrNoLock();
		rcd2 = itr2->getPtrNoLock();
		cmpRslt = rcd1->cmpStatRecordKeys(
				rcd2->getKeyValueList(), mGrpbyFieldIsAscList);
		if (cmpRslt == 0)
		{
			initStatRecord(rcd1);
			rcd1->addValue(mRundata, rcd2);
			targetList->push_back(rcd1);

			//advance iterators
			itr1++;
			itr2++;

			//for debugging
			numLocal++;
			num++;
		}
		else if (cmpRslt == 1)
		{
			//local is before the new data, advance local
			initStatRecord(rcd1);
			targetList->push_back(rcd1);

			itr1++;

			//for debugging
			numLocal++;
		}
		else {
			//local is after the new data, need to insert
			//before local position
			initStatRecord(rcd2);
			targetList->push_back(rcd2);
			//localItr needs to advance by one to its original
			//record before the insertion

			//advance the new iterator
			itr2++;

			//for debugging
			num++;
		}
		tEnd = OmnGetTimestamp();
		mRecordCompareTime += tEnd - tStart;
		mRecordCompareNum++;
	}  

	//append the rest records in the new record list
	while (itr1 != rcdList1->end() && !noMoreRecordNeeded())
	{
		tStart = OmnGetTimestamp();
		rcd1 = itr1->getPtrNoLock();
		initStatRecord(rcd1);

		//local is after the new data, need to insert
		//before local position
		targetList->push_back(rcd1);

		//advance the new iterator
		itr1++;
		tEnd = OmnGetTimestamp();
		mPushRecordTime1+= tEnd - tStart;
		mPushRecordNum1++;
	}

	//append the local records in the new record list
	tStart = OmnGetTimestamp();
	while (itr2 != rcdList2->end() && !noMoreRecordNeeded())
	{
		tStart = OmnGetTimestamp();
		rcd2= itr2->getPtrNoLock();
		initStatRecord(rcd2);

		//local is after the new data, need to insert
		//before local position
		targetList->push_back(rcd2);

		//advance the new iterator
		itr2++;
		tEnd = OmnGetTimestamp();
		mPushRecordTime2+= tEnd - tStart;
		mPushRecordNum2++;
	}

	return targetList;
}

//
//using merge sort algorithm to merge
//
//return merged records, that is equal value records of
//the two record list
//
#if 0
int 
AosStatQryEngine::mergeStatRecord(StatRecordList *recordList, 
								 bool inOrder)
{
	int numLocal = 0;
	int num = 0;

	if (!recordList)
		return -1;

	//currently only take care of ordered list
	aos_assert_r(inOrder, -1); 

	StatRecordList::iterator localItr, itr;
	//merge mStatRecordList and recordList into targetList
	StatRecordList targetList;
	AosStatRecord *localRcd, *rcd;
	int cmpRslt;
	int merged  = 0;
	
	localItr = mStatRecordList.begin();
	itr = recordList->begin();
	
	StatRecordMap::iterator itr, cmpItr;
	AosStatRecord *localRcd, *rcd;

	itr = recordMap->begin();
	while (itr !=recordMap->end())
	{
		rcd = itr->first.getPtrNoLock();
		cmpItr = mStatRecordMap.find(rcd);
		if(cmpItr != mStatRecordMap.end())
		{
			localRcd = cmpItr->first.getPtrNoLock();
			localRcd->addValue(mRundata, rcd);
			merged++;

			numLocal++;        
			num++;             
		}
		else
		{
			mStatRecordMap.insert(make_pair(rcd,1));
			initStatRecord(rcd);

			num++;
		}
		itr++;
	}
	while (localItr != mStatRecordMap.end() &&
			itr != recordMap->end())
	{
		localRcd = localItr->first.getPtrNoLock();
		rcd = itr->first.getPtrNoLock();
		cmpRslt = localRcd->cmpStatRecordKeys(
				rcd->getKeyValueList(), mGrpbyFieldIsAscList);
		if (cmpRslt == 0)
		{
			localRcd->addValue(mRundata, rcd);
			merged++;

			//advance iterators
			localItr++;
			itr++;             
			                   
			//for debugging    
			numLocal++;        
			num++;             
		}
		else if (cmpRslt == 1)
		{
			localItr++;
			numLocal++;
		}
		else {
			localItr = mStatRecordMap.insert(make_pair(rcd,1)).first;
			localItr++;
			initStatRecord(rcd);

			itr++;


			num++;
		}
	}
	while (itr != recordMap->end() && !noMoreRecordNeeded())
	{
		rcd = itr->first.getPtrNoLock();

		//local is after the new data, need to insert
		//before local position
		mStatRecordMap[rcd] = 1;
		initStatRecord(rcd);		

		itr++;
		
	}

	StatRecordMap::iterator itr;
	//merge mStatRecordList and recordList into targetList
	StatRecordMap targetList;
	AosStatRecord *localRcd, *rcd;
	int merged  = 0;

	itr = recordList->begin();

	StatRecordMap::iterator cmpItr;

	while ( itr != recordList->end() && !noMoreRecordNeeded())
	{
		rcd = itr->first.getPtr();

		cmpItr = mStatRecordMap.find(rcd);
		if (cmpItr != mStatRecordMap.end())
		{
			localRcd = cmpItr->first.getPtr();
			localRcd->addValue(mRundata, rcd);
			merged++;

			//advance iterators
			itr++;             
			                   
			//for debugging    
			numLocal++;        
			num++;             
			
		}
		else
		{
			mStatRecordMap[rcd] = 1;

			itr++;

			numLocal++;
			num++;
		}
	}
	while (localItr != mStatRecordList.end() &&
			itr != recordList->end())
	{  
		localRcd = localItr->getPtrNoLock();
		rcd = itr->getPtrNoLock();
		cmpRslt = localRcd->cmpStatRecordKeys(
				rcd->getKeyValueList(), mGrpbyFieldIsAscList);
		if (cmpRslt == 0)
		{
			localRcd->addValue(mRundata, rcd);
			targetList.push_back(localRcd);
			merged++;

			//advance iterators
			localItr++;
			itr++;

			//for debugging
			numLocal++;
			num++;
		}
		else if (cmpRslt == 1)
		{
			//local is before the new data, advance local
			targetList.push_back(localRcd);
			localItr++;

			//for debugging
			numLocal++;
		}
		else {
			//local is after the new data, need to insert
			//before local position
			//localItr = mStatRecordList.insert(localItr, rcd);
			targetList.push_back(rcd);
			//localItr needs to advance by one to its original
			//record before the insertion
			initStatRecord(rcd);

			//advance the new iterator
			*itr = NULL; //this record is moved out
			itr++;

			//for debugging
			num++;
		}
	}  

	//append the rest records in the new record list
	while (itr != recordList->end() && !noMoreRecordNeeded())
	{
		rcd = itr->getPtrNoLock();

		//local is after the new data, need to insert
		//before local position
		targetList.push_back(rcd);
		initStatRecord(rcd);
		
		//advance the new iterator
		*itr = NULL; //this record is moved out
		itr++;
	}

	//append the local records in the new record list
	while (localItr != mStatRecordList.end() && !noMoreRecordNeeded())
	{
		localRcd = localItr->getPtrNoLock();

		//local is after the new data, need to insert
		//before local position
		targetList.push_back(localRcd);
		
		//advance the new iterator
		localItr++;
	}

	mStatRecordList.clear();
	bool rslt = removeOutPageRecords(targetList);
	aos_assert_r(rslt,false);
	mStatRecordList = targetList;

	targetList.clear();

	return merged;
}
#endif

//
//merge one stat record into mStatRecordList
//True if merged.
//False if not merged because out of a page
//
//This is normally called in frontend side. The records
//are sorted by mOrderedGrpbyFieldList, but not the final
//orderby list since merging is based on groupby fields.
//
//Frontend need to call sortStatRecord() for final 
//sorting
//
bool 
AosStatQryEngine::mergeStatRecord(AosStatRecord *record)
{
	StatRecordList::iterator rcdItr;
	bool mergeFlag;
	AosStatRecord *statRcd;

	aos_assert_r(record, false);

	//call inPage() to insert stat records to existing
	//stat record list in keys' order
	if (!inPage(mStatRecordList, record->getKeyValueList(), mergeFlag, rcdItr))
	{
		return false;
	}

	if (mergeFlag)
	{
		statRcd = rcdItr->getPtrNoLock();
		//merge vt2d values into the stat record
		statRcd->addValue(mRundata, record);

	} else {
//		mStatRecordMap[record] = 1;
		mStatRecordList.insert(rcdItr, record);
		removeOutPageRecords();				

		//
		//use the current engine's member data if needed
		//old data could be released with the old engine gone
		//
		initStatRecord(record);
	}

	return true;
}

//
//collect  all recordList
int 
AosStatQryEngine::collectRecord(AosStatQryEngine *engine,
								 bool inOrder)
{
	int merged  = 0;
	StatRecordList * rcdList = new StatRecordList;

	aos_assert_r(engine, -1);
	*rcdList = (*engine->getStatData());

	mMergeList.push_back(rcdList);

	return merged;
}

//
//check if having records reach a page
//This needs to be called in frontend process
//
bool
AosStatQryEngine::applyHaving()
{
	bool rslt;
	u64 tStart, tEnd;

	if (!mHavingExpr)
		return true;

	//complete dist count handling
	rslt = applyDistCount();
	aos_assert_r(rslt, false);

	//filter out stat records by having condition
	tStart = OmnGetTimestamp();
	rslt = filterByHavingCond();
	tEnd = OmnGetTimestamp();
	mHavingCondTime += tEnd - tStart;
	mHavingCondNum++;
	aos_assert_r(rslt, false);
	return true;
}

//
// frontend call this method to wrap up all the records to
// generate the final results to JQL
//
// Please notice:  the following option should not mix
//       1. order by
//       2. rollup
//       3. cube
//
// This means, only one of above features can exist in
// one query
//
bool
AosStatQryEngine::dataFinished(const AosRundataPtr &rdata)
{
	bool rslt;

	//do basic data operations needed by all upper level
	//operations
	
	//complete dist count handling
	rslt = applyDistCount();
	aos_assert_r(rslt, false);

	if (!mQryInfo->mRollupGrpbyFieldsGrp.empty())
	{
		//generate rollup result if configured
		//when there is rollup configuration, the result ordering
		//should be the same as ordered by groupby keys
		rslt = genRollupRecord();
		aos_assert_r(rslt, false);
	}
	else if (!mQryInfo->mCubeGrpbyFields.empty())
	{
		//generate cube record list if needed
		rslt = genCubeRecord();
		aos_assert_r(rslt, false);

		//consolidate all the cube records into one list
		rslt = combineCubeRecord();
		aos_assert_r(rslt, false);
	}
	else
	{
		//sorting the records if needed
		int num = sortStatRecord();
		aos_assert_r(num >= 0, false);

		//complete dist count handling
		rslt = applyAccumulate();
		aos_assert_r(rslt, false);

		//filter out stat records by having condition
		rslt = filterByHavingCond();
		aos_assert_r(rslt, false);
	}

	//output all types of counters
	outputCounters();
	return true;
}


bool 
AosStatQryEngine::compareMeasure (
		const AosStatRecordPtr &lhs, 
		const AosStatRecordPtr &rhs)
{
	int rslt =  lhs->cmpStatRecord(
			rhs.getPtrNoLock(),
			rhs->getMeasurePosList(),
			rhs->getOrderIsAsc());

	return (rslt > 0);
}

//
//If need to order by measure values, an extra sort is needed
//at the end. Normally in frontend side
//
//If no measure value ordering request, no need to re-order since
//all the stat records are already sorted by mOrderedGrpbyFieldList.
//Otherwise, the records will be sorted based on some measure
//values and grpby fields
//
//return value:
// 	-1: something is wrong
// 	otherwise: sorted records
//
//1. This sorting method uses binary sort algo
//2. This method assume that all the merging action done
//   already. Sort should be after all the merging
//
int
AosStatQryEngine::sortStatRecord()
{
//	aos_assert_r(mStatRecordList.empty(),false);
//
//	//get the key of StatRecordMap into StatRecordList
//	StatRecordList::iterator itrmap;
//	for(itrmap = mStatRecordMap.begin(); itrmap != mStatRecordMap.end(); itrmap++)
//	{
//		mStatRecordList.push_back(itrmap->first);
//	}

	u32 total = mStatRecordList.size();
	StatRecordList rcdList;
	AosStatRecord *rcd;
	u64 tStart, tEnd;
	StatRecordList::iterator itr;

	//no records, no sorting
	if (total <= 0)
		return 0;

	//if rollup, no sorting since data is already sorted
	if (!mQryInfo->mRollupGrpbyFieldsGrp.empty())
		return 0;

	//if orderby fields don't contain measure
	//fields, no need to sort
	if (!mOrderByMeasureValue /*&& !mHasAccumulate*/) return 0;

	//for debug
	OmnScreen << "(Statistics counters : Engine) Total stat records to sort: " << total << endl;

	tStart = OmnGetTimestamp();

#if 0
	for (u32 i = 0; i < total; i++)
	{
		rcdList.push_back(mStatRecordList[i].getPtr());
	}
	mStatRecordList.clear();

	//sort records from rcdList to mStatRecordList
	//one by one
	for (u32 i = 0; i < total; i++)
	{
		//
		//lowerBound will position itr to be the first statRecord
		//which keyValue >= keyValueList or the end
		//
		rcd = rcdList[i].getPtr();
		itr = lowerBound(mStatRecordList, NULL, rcd);
		if (itr != mStatRecordList.end())
		{
			//insert the new record
			mStatRecordList.insert(itr, rcd);
			if (mStatRecordList.size() > mPageSize)
			{
				//remove the last record
				rcd = mStatRecordList.back().getPtr();
				mStatRecordList.pop_back();
			}
		}
		else
		{
			//if has enough record, no appending
			if (mStatRecordList.size() < mPageSize)
			{
				mStatRecordList.push_back(rcd);
				rcdList[i] = NULL;
			}
		}
	}

	//clear records in rcdList not needed
	for (u32 i = 0; i < rcdList.size(); i++)
	{
		rcd = rcdList[i].getPtr();
		if (rcd) delete rcd;
	}
	rcdList.clear();
#endif

	if(mStatRecordList.size() > mPageSize)
	partial_sort(mStatRecordList.begin(), mStatRecordList.begin() + mPageSize, 
			mStatRecordList.end(), compareMeasure);
	else
		sort(mStatRecordList.begin(), mStatRecordList.end(), compareMeasure);
	tEnd = OmnGetTimestamp();
	mSortTime += tEnd - tStart;
	mSortNum++;
	//the first numToSort records is the result
	return 0;
}

//
// filtering the final statistics result based on having condition
//
bool
AosStatQryEngine::filterByHavingCond()
{
	//AosDataRecordObjPtr dataRecord;
	bool rslt;
	AosValueRslt value;
	StatRecordList::iterator itr;
	StatRecordList rcdList;
	AosStatRecordPtr rcd;
	//AosStatRecord *rcd;

	if (!mHavingExpr)
		return true;

	itr = mStatRecordList.begin();
	while(itr != mStatRecordList.end())
	{
		rcd = *itr;
		rslt = mHavingExpr->getValue(mRundata, rcd.getPtr(), value);
		aos_assert_r(rslt, false);
		//dataRecord->->setFieldValue(cname, value_rslt, outofmem, rdata.getPtrNoLock());
		if (!value.getBool())
		{
			//non-select record, release the space
		//	delete rcd;
		}
		else
		{
			//save the matching records
			rcdList.push_back(rcd);
		}
		itr++;
	}
	mStatRecordList = rcdList;

	return true;
}

//
//To finalize dist count data, all the distinct values are
//counted as a total number
//
bool
AosStatQryEngine::applyDistCount()
{
	StatRecordList::iterator itr;


	for (u32 i = 0; i < mStatRecordList.size(); i++)
		mStatRecordList[i]->reduceDistCount();

	return true;
}

bool
AosStatQryEngine::getFieldValue(
		const u32 rcd_idx, 
		const OmnString &field_name,
		AosValueRslt &value_rslt)
{
	OmnString name;
	int field_idx;

	aos_assert_r(rcd_idx < mStatRecordList.size(), false);
	field_idx = getFieldIdx(field_name); 
	bool rslt=getFieldValue(rcd_idx, field_idx, value_rslt);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosStatQryEngine::getFieldValue(
		const u32 rcd_idx, 
		const int field_idx,
		AosValueRslt &value_rslt)
{
	OmnString name;
	AosStatRecord *rcd;

	aos_assert_r(rcd_idx < mStatRecordList.size(), false);
	rcd = mStatRecordList[rcd_idx].getPtrNoLock();
	aos_assert_r(rcd->getFieldValue(field_idx, value_rslt, true, mRundata), false);

	return true;
}

int
AosStatQryEngine::getFieldIdx(const OmnString &name)
{
	int idx;
	OmnStringHashMap::iterator itr;

	//if count(xxx) field, change to be count(*)
/*	if (name.hasPrefix("count0x28") && !name.hasPrefix("count0x28cond"))
	{
		itr = mFieldNameMap->find("count0x280x2a0x29");
	}
*/
	if (name.hasPrefix("accu_count0x28") && !name.hasPrefix("accu_count0x28cond"))
	{
		itr = mFieldNameMap->find("accu_count0x280x2a0x29");
	}
	else
	{
		itr = mFieldNameMap->find(name);
	}

	//find the idx of the field_name in key/measure lists
	if (itr != mFieldNameMap->end())
	{
		idx = itr->second;
		return idx;
	}

	return -1;
}

//
//generate the hashmap from field or measure name to the idx
//in field or measure list
//
bool
AosStatQryEngine::genFieldNameMap()
{
	mFieldNameMap = new OmnStringHashMap();
	MeasureInfo *measureInfo;
	OmnString name;

	u32 grpby_key_num =  mOrderedGrpbyFieldList.size();
	//grpby fields
	for (u32 i = 0; i < grpby_key_num; i++)
	{
		name = mOrderedGrpbyFieldList[i]->mName;
		(*mFieldNameMap)[name] = i;
	}

	u32 measure_num =  mMeasureInfoList->size();
	//measures
	for (u32 i = 0; i < measure_num; i++)
	{
		measureInfo = &((*mMeasureInfoList)[i]);
		//arvin 2015.09.16
		//JIMODB-763
		//if this is a measure_condition,it will get mCondMeasureName,else it will use original name
		if(measureInfo->mCond)
			name = measureInfo->mCondMeasureName;
		else
			name = measureInfo->mName;
		(*mFieldNameMap)[name] = i + grpby_key_num;
	}

	//stat key fields
	u32 stat_key_num = mQryInfo->mStatKeyFields.size();
	bool found;
	for (u32 i = 0; i < stat_key_num; i++)
	{
		name = mQryInfo->mStatKeyFields[i];
		found = false;
		for (u32 j = 0; j < grpby_key_num; j++)
		{
			if (name == mOrderedGrpbyFieldList[j]->mName)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			//only when stat key field is not in group list, will
			//this stat key be included in the map
			(*mFieldNameMap)[name] = i + grpby_key_num + measure_num;
		}
	}

	
	return true;
}

bool 
AosStatQryEngine::setKeyIdxTypeMap(const map<int,AosDataType::E> &keyIdxTypeMap)
{
	mKeyIdxTypeMap = keyIdxTypeMap;
	return true;
}


bool 
AosStatQryEngine::setGroupByKeyType(const map<int,AosDataType::E> &groupByKeyType)
{
	mGroupByKeyIdxType = groupByKeyType;
	return true;
}
