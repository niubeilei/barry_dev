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

static u64 t5, t7, t8;

///////////////////////////////////////////////////////
//    Constructors/Destructors
///////////////////////////////////////////////////////
AosStatQryEngine::AosStatQryEngine(AosRundata* rdata, 
				 				AosStatQueryInfo *queryInfo) 
:
mRundata(rdata),
mVt2dList(0),
mKeyConn(0),
mFieldNameMap(0),
mQryInfo(queryInfo),
mHavingExpr(0),
mDistMeasure(""),
mMeasureInfoList(0),
mDistInMeasureList(0),
mOrderByMeasureValue(false),
mMeasureMapperList(0),
mTimeKeyPos(-1),
mHasAccumulate(false)
{
	mPageSize  =  mQryInfo->mPageSize  + mQryInfo->mPageOffset;
	mEasyRecordPtr = OmnNew AosStatRecord(AosMemoryCheckerArgsBegin);
	mEasyRecord = mEasyRecordPtr.getPtrNoLock();
}

AosStatQryEngine::~AosStatQryEngine()
{
#if 0
	//need to release memory
	map<AosStatRecord *,int>::iterator itr = mStatRecordMap.begin();
	for(; itr != mStatRecordMap.end(); )
	{
		if (mStatRecordList[i])
			delete mStatRecordList[i];

		mStatRecordList[i] = NULL;
	}

//	for (u32 i = 0; i < mStatRecordList.size(); i++)
//	{
//		if (mStatRecordList[i])
//			delete mStatRecordList[i];
//
//		mStatRecordList[i] = NULL;
//	}

	mStatRecordMap.clear();
#endif

	if (mMeasureMapperList)
	{
		mMeasureMapperList->clear();
		delete mMeasureMapperList;
	}

	if (mMeasureInfoList)
	{
		mMeasureInfoList->clear();
		delete mMeasureInfoList;
	}

	if (mDistInMeasureList)
	{
		mDistInMeasureList->clear();
		delete mDistInMeasureList;
	}

	if (mFieldNameMap)
	{
		mFieldNameMap->clear();
		delete mFieldNameMap;
	}

}

///////////////////////////////////////////////////////
//    init methods
///////////////////////////////////////////////////////
bool
AosStatQryEngine::basicInit()
{
	//translate queryMeasureInfo to measureInfo
	aos_assert_r(genMeasureInfo(), false);

	//generate ordered grpby field list based on order by list
	aos_assert_r(genOrderInfo(), false);

	//get the groupby key orders in stat key list when running data
	aos_assert_r(genGrpbyKeyOrder(), false);

	//generate the innitial mCubeRecordMap and mCubeParentMap
	aos_assert_r(genCubeInfo(), false);

	//This assumes no multi-threading
	//mEasyRecord.setTimeKeyPos(mTimeKeyPos);
	//mEasyRecord.setOrderIsAsc(mOrderIsAscList);
	initStatRecord(mEasyRecord);

	initCounters();
	return true;
}

bool
AosStatQryEngine::cubeInit()
{
	bool rslt;

	rslt = basicInit();
	aos_assert_r(rslt, false);

	//create measure mapper vectors
	if (mDistInMeasureList->empty())
	{
		rslt = genMeasureMappers();
		aos_assert_r(rslt, false);
	}
	else
	{
		//if distinct count statistics, cube side can only take care of one at a time
		aos_assert_r(mMeasureInfoList->size() == 1, false);
		mDistMeasure = (*mMeasureInfoList)[0].mName;
		aos_assert_r(genDistMeasureMappers(), false);

		//the last field is the distinct count field
		//remove the last key
		mQryInfo->mStatKeyFields.pop_back();
	}
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
//pass common properties to statRecord if needed
//
bool
AosStatQryEngine::initStatRecord(AosStatRecord *statRcd)
{
	aos_assert_r(statRcd, false);
	statRcd->setTimeKeyPos(mTimeKeyPos);
	statRcd->setGrpbyKeyNum(mOrderedGrpbyFieldList.size());
	statRcd->setTimeUnit(mQryInfo->mGrpbyTimeUnit);
	statRcd->setMeasureInfo(mMeasureInfoList);
	statRcd->setDistInMeasure(mDistInMeasureList);
	statRcd->setMeasureMappers(mMeasureMapperList);
	statRcd->setFieldNameMap(mFieldNameMap);
	statRcd->setKeyFieldIdxs(mQryInfo->mKeyIdxMap);
	statRcd->setOrderIsAscList(&mGrpbyFieldIsAscList);
	statRcd->setMeasurePosList(&mOrderedMeasurePosList);
	//arvin 2015-10-8
	statRcd->setGroupByKeyType(mGroupByKeyIdxType);
	if(mKeyIdxTypeMap.size()>0)
		statRcd->setKeyIdxTypeMap(mKeyIdxTypeMap);
	else
		statRcd->setKeyIdxTypeMap(mQryInfo->mKeyIdxTypeMap);

	return true;
}

//
//pass properties to vt2dRecord based on a Vt2d
//
bool
AosStatQryEngine::initVt2dRecord(AosVt2dRecord *vt2dRcd, u32 idx)
{
	vt2dRcd->clear();
	vt2dRcd->setVt2dIndex(idx);
	vt2dRcd->setTimeUnit((*mVt2dList)[idx]->getTimeUnit());
	vt2dRcd->setHasValidFlag((*mVt2dList)[idx]->hasValidFlag());
	vt2dRcd->setMeasureValueSize((*mVt2dList)[idx]->getMeasureValueSize());

	vt2dRcd->setMeasureInfo((*mVt2dList)[idx]->getMeasureInfos());
	return true;
}

//
//this method find the positions of the grpbyKeys
//in stat keys when running statistics
//
//if time key, the order is -1
//
bool
AosStatQryEngine::genGrpbyKeyOrder()
{
	OmnString grpbyKey;
	OmnString statKey;
	bool found;
	AosDataType::E keyType;

	mKeyOrderList.clear();
	mTimeKeyPos = -1;
	for (u32 i = 0; i < mOrderedGrpbyFieldList.size(); i++)
	{
		grpbyKey = mOrderedGrpbyFieldList[i]->mName;
		if (grpbyKey == mQryInfo->mTimeKeyField)
		{
			mKeyOrderList.push_back(-1);
			mTimeKeyPos = i;
			continue;
		}

		found = false;
		for (u32 j = 0; j < mQryInfo->mStatKeyFields.size(); j++)
		{
			statKey = mQryInfo->mStatKeyFields[j];
			keyType = mQryInfo->mKeyIdxTypeMap[j];	
			if (grpbyKey == statKey)
			{
				mKeyOrderList.push_back(j);
				mGroupByKeyIdxType.insert(make_pair(i,keyType));
				found = true;
				break;
			}
		}

		aos_assert_r(found, false);
	}

	return true;
}

//
//map queryMeasureInfo data to measureInfo data
//
bool
AosStatQryEngine::genMeasureInfo()
{
	//vector<AosQueryMeasureInfo> qryMeasureInfo = mQryInfo->mQryMeasures;
	vector<AosStatFieldPtr> & qryMeasureInfo = mQryInfo->mQryMeasures;
	MeasureInfo info;

	mDistMeasure = "";
	mMeasureInfoList = new vector<MeasureInfo>();
	mDistInMeasureList = new vector<u32>();
	for (u32 i = 0; i < qryMeasureInfo.size(); i++)
	{
		AosStatMeasureField * field = (AosStatMeasureField *)(qryMeasureInfo[i].getPtr());
		info.mName = field->getFieldName();
		info.mFieldName = field->mRawFname;
		info.mAggrFuncStr = field->mAggrFuncStr;
		//info.mDataType = AosDataType::eInt64;
		info.mDataType = field->mDataType;
		info.mNumber = field->mNumber;
		info.mCond = field->mCond;
		info.mCondMeasureName = field->mCondMeasureName;
		info.configFunc(mRundata);
		if (info.mIsAccumulate)
			mHasAccumulate = true;

		mMeasureInfoList->push_back(info);

		if (info.mAggrFuncStr.findSubString("dist_count", 0, false) >= 0)
		{
			//save the position in dist in measure list
			mDistInMeasureList->push_back(i);
		}
	}

	return true;
}

//
//generate orderedGrpbyFieldList based on grpby fields and orderBy
//fields. E.g:
//     group by a, b, c order by c, a
//ordered groupby list will be:
//	          c, a, b
//
//If order by list has measure value fields, such as:
//     group by a, b, c order by sum(f1), c, a
//
//measure value fields will be skipped and ordered group by
//list will still be:   c, a, b
//
bool
AosStatQryEngine::genOrderInfo()
{
	vector<AosStatFieldPtr> orderbyFieldList;
	vector<AosStatFieldPtr> grpbyFieldList;
	int pos;
	OmnString fieldStr;

	orderbyFieldList.clear();
	grpbyFieldList.clear();

	if (!mQryInfo->mRollupGrpbyFieldsGrp.empty())
	{
		//use the first group in rollup
		grpbyFieldList = mQryInfo->mRollupGrpbyFieldsGrp[0];
	}
	else if (!mQryInfo->mCubeGrpbyFields.empty())
	{
		//we need to support group by keys + cube keys, like:
		// group by k1, cube(k2, k3, k4) ....
		if (!mQryInfo->mGrpbyFields.empty())
			grpbyFieldList = mQryInfo->mGrpbyFields;

		for (u32 i = 0; i < mQryInfo->mCubeGrpbyFields.size(); i++)
			grpbyFieldList.push_back(mQryInfo->mCubeGrpbyFields[i]);
	}
	else
	{
		//for cube and rollup, order by list is ignored.
		//Only for normal group by, user configured orderby
		//list will be taken care of
		orderbyFieldList = mQryInfo->mOrderbyFields;
		grpbyFieldList = mQryInfo->mGrpbyFields;
	}

	//save the orderby fields into the ordered grp field
	//list firstly. Filter out measure fields
	for (u32 i = 0; i < orderbyFieldList.size(); i++)
	{
		if (findFieldByName(grpbyFieldList, orderbyFieldList[i]) >= 0)
		{
			mOrderedGrpbyFieldList.push_back(orderbyFieldList[i]);
			mGrpbyFieldIsAscList.push_back(mQryInfo->mOrderbyAscs[i]);
			//this position is not a mesure field
			mOrderedMeasurePosList.push_back(-1);
		}
		else
		{
			//the field is a measure value field, find the position
			//in measure value list
			//find the aggr func string firstly
			fieldStr = orderbyFieldList[i]->mName;
			pos = findMeasure(fieldStr);

			//it might be true that the orderby measure is not
			//in this statengine run
			//aos_assert_r(pos >= 0, false);
			mOrderedMeasurePosList.push_back(pos);

			if (!mOrderByMeasureValue && pos >= 0)
			{
				mOrderByMeasureValue = true;

				//
				//it might be true that:
				//		order by key, sum(v1)
				//
				//in this case, no need to full scan all records in
				//frontend side.  However, since there is value
				//measures, for simplicity, in engine side, all the
				//given docids will be handled
				//
				//aos_assert_r(mQryInfo->mNeedFullScan, false);
			}
			mGrpbyFieldIsAscList.push_back(mQryInfo->mOrderbyAscs[i]);
		}
	}

	//add the rest grpby fields into the ordered list
	for (u32 i = 0; i < grpbyFieldList.size(); i++)
	{
		if (findFieldByName(orderbyFieldList, grpbyFieldList[i]) < 0)
		{
			mOrderedGrpbyFieldList.push_back(grpbyFieldList[i]);
			//no order specification, default is ASC
			mGrpbyFieldIsAscList.push_back(true);
		}
	}

	return true;
}

//
//vt2d measures mapping to query measure
//
bool
AosStatQryEngine::genMeasureMappers()
{
	AosMeasureValueMapper mapper;
	vector<MeasureInfo> *infoList;
	OmnString measureName,qryMeasureName,noAccuMeasureName;
	int pos;

	vector<AosStatFieldPtr> qryMeasures = mQryInfo->mQryMeasures;
 	mMeasureMapperList = new vector<AosMeasureValueMapper>();
	for (u32 i = 0; i < mVt2dList->size(); i++)
	{
		mapper.clear();
		infoList = (*mVt2dList)[i]->getMeasureInfos();

		for(u32 k = 0; k  < qryMeasures.size(); k++)
		{
			qryMeasureName = qryMeasures[k]->mName;
			//qryMeasureName.toLower();
			noAccuMeasureName = qryMeasureName;
			noAccuMeasureName.toLower();
			//check if an accumulate aggregation
			if (qryMeasureName.hasPrefix("accu_"))
			{
				mHasAccumulate = true;
				//remove accu_ prefix
				noAccuMeasureName = qryMeasureName.substr(5, 0);
			}

			for (u32 j = 0; j < infoList->size(); j++)
			{
				measureName = (*infoList)[j].mName;
				measureName.toLower();
				//yang,fix count(*) query bug
/*				if((noAccuMeasureName.hasPrefix("count0x28cond") &&  measureName.hasPrefix("count0x28"))
						|| (noAccuMeasureName.hasPrefix("sum0x28cond") &&  measureName.hasPrefix("sum0x28")))
				{
					if(!seenCount)
						seenCount = true;
					else continue;
				}
*/
				if(noAccuMeasureName ==measureName)
				{
					if(qryMeasures[k]->mCond)
						qryMeasureName = qryMeasures[k]->mCondMeasureName;
					pos = findMeasure(qryMeasureName);
					if(pos < 0)
					{
						continue;
					}
					mapper.addIndex(j, k, (*mMeasureInfoList)[pos].mAggrFunc);
					break;
				}
			}
		}
		mMeasureMapperList->push_back(mapper);
	}

	return true;
}

//
//For dist_count, there will be only 1 vt2d useful and
// only the specified dist_count measure is concerned
//
bool
AosStatQryEngine::genDistMeasureMappers()
{
	AosMeasureValueMapper mapper;
	vector<MeasureInfo> *infoList;
	OmnString measureName;
	int pos;

	mMeasureMapperList = new vector<AosMeasureValueMapper>();
	for (u32 i = 0; i < mVt2dList->size(); i++)
	{
		mapper.clear();
		infoList = (*mVt2dList)[i]->getMeasureInfos();

		//find the vt2d with only 1 measure and dist_count measure
		if (infoList->size() == 1)
		{
			measureName = (*infoList)[0].mName;
			if (measureName == mDistMeasure)
			{
				pos = findMeasure(measureName);
				aos_assert_r(pos >= 0, false);
				mapper.addIndex(0, pos, (*mMeasureInfoList)[pos].mAggrFunc);
			}
		}

		mMeasureMapperList->push_back(mapper);
	}

	return true;
}

///////////////////////////////////////////////////////
//   Cube-used methods to generate the basic
//   statistics result
///////////////////////////////////////////////////////
//
//generate query result based on the docids
//
//In cube side, the stat records are sorted
//by mOrderedGrpbyFieldList. Even if measure
//fields are in order by list, its ordering
//will be taken care of in frontend side by
//calling sortStatRecord() method
//
bool 
AosStatQryEngine::runStat(
		const vector<u64> &docidList, 
		AosStatKeyConn* keyConn, 
		vector<AosVector2DPtr>   *vector2DList)
{
	// This function retrieves stat values for all the stat docs listed in
	// 'docidList'. One stat model may have multiple groups of measures. 
	// Each measure group is represented by one AosVector2DPtr. 
	// 'keyConn' is the class to read data. 
	//
	// This function assumes docids in 'docidList' are sorted. This is very
	// important because if it is not sorted, it will create lots of I/Os.
	u64	sdocid;
	OmnValueList	*keyValueList;
	bool			keyValueListUsed;
	OmnValueList	*timedKeyValueList;
	AosVt2dRecord	*vt2dRcd;
	AosStatRecord	*statRcd;
	i64 timeValue;
	bool rslt;

	bool mergeFlag;
//	vector<AosStatRecord *>::iterator rcdItr;
	StatRecordMap::iterator rcdItr;
	u64 tStart, tEnd;
	bool isInPage;
	OmnString distValue;
	
	//set member vars
	mKeyConn = keyConn;
	mVt2dList = vector2DList;

	//generate needed data structures
	//tStart = OmnGetTimestamp();
	aos_assert_r(cubeInit(), false);
	//tEnd = OmnGetTimestamp();
	//mInitCubeTime += tEnd - tStart;

	OmnStringList stat_key_values;
	vt2dRcd = new AosVt2dRecord();
	//read the stat docs
	for (u32 i = 0; i < docidList.size(); i++)
	{
		sdocid = docidList[i];

		//read in the docid's key values
		// Ketty 2014/12/02
		//keyValueList = readKeyValues(sdocid, distValue);
		//
		// 'readKeyValues(...)' reads the key fields that are grouped by
		// for the query.
		tStart = OmnGetTimestamp();
		keyValueList = readKeyValues(sdocid, distValue, stat_key_values);
		tEnd = OmnGetTimestamp();
		mReadKeyTime += tEnd - tStart;
		mReadKeyNum++;
		if (stat_key_values.size() <= 0)
			continue;

		aos_assert_r(stat_key_values.size(), false);
		keyValueListUsed = false;

		//no need to handle a vt2dRcd if not in the 
		//page to be generated
		tStart = OmnGetTimestamp();
		isInPage = inPage(mStatRecordMap, keyValueList, mergeFlag, rcdItr);
		tEnd = OmnGetTimestamp();
		mStatProcTime += tEnd - tStart;
		mStatProcNum++;
		if (!isInPage)
		{
			//release keyValueList
			if (keyValueList)
				delete keyValueList;

			continue;
		}

		for (u32 j = 0; j < vector2DList->size(); j++)
		{
			if ((*mMeasureMapperList)[j].getMeasureValueNum() <= 0)
			{
				//skip vt2ds with no measure values regarded in
				//the current query
				continue;
			}

			if (j != 0 && 
					(mQryInfo->mGrpbyTimeUnit == AosStatTimeUnit::eAllTime ||
					 mQryInfo->mGrpbyTimeUnit == AosStatTimeUnit::eInvalid) )
			{
				//reuse previous statRcd
				mergeFlag = true;
			}

			//init vt2DRecord
			//tStart = OmnGetTimestamp();
			initVt2dRecord(vt2dRcd, j);
			//tEnd = OmnGetTimestamp();
			//mInitVt2dRecordTime += tEnd - tStart;

			//read in the vt2d record
			//there could be multiple vector2d to read
			//handle one for now
			tStart = OmnGetTimestamp();
			rslt = (*vector2DList)[j]->retrieve(
					mRundata, vt2dRcd, sdocid, mQryInfo->mQryTimeConds);
			aos_assert_r(rslt, false);
			tEnd = OmnGetTimestamp();
			mReadStatDocTime += tEnd - tStart;
			mReadStatDocNum++;

			vt2dRcd->setStatDocid(sdocid);
			vt2dRcd->resetIndex();
			vt2dRcd->setDistMeasure(mDistMeasure);
			vt2dRcd->setDistValue(distValue);
			
			if((*vector2DList)[j]->getMeasureType() == AosVector2D::DIST_COUNT)
	        	vt2dRcd->setMeasureType(AosVt2dRecord::DIST_COUNT);
			else
				vt2dRcd->setMeasureType(AosVt2dRecord::NORMAL);

			
			while (!vt2dRcd->isIndexEnd())
			{
				//
				//If groupby time, timeValue need to be set back
				//to key values. At the beginning, timevalue in 
				//keyValueList vector is 0. So far, timevalue is put
				//at the end
				//
				if (mQryInfo->mGrpbyTimeUnit != AosStatTimeUnit::eAllTime &&
					mQryInfo->mGrpbyTimeUnit != AosStatTimeUnit::eInvalid) 
				{
					//timeValue is based on the original timeUnit. Need to
					//translate to be the query timeUnit
					//tStart = OmnGetTimestamp();
					timeValue = vt2dRcd->getCrtTimeValue(mQryInfo->mGrpbyTimeUnit); 
					//tEnd = OmnGetTimestamp();
					//mVt2dRcdGetTime += tEnd - tStart;

					aos_assert_r(mTimeKeyPos >= 0, false);
					//(*keyValueList)[mTimeKeyPos] = "";
					//(*keyValueList)[mTimeKeyPos] << timeValue;
					//tStart = OmnGetTimestamp();
					(*keyValueList)[mTimeKeyPos] = AosValueRslt(timeValue);
					//tEnd = OmnGetTimestamp();
					//mValueRsltTime += tEnd - tStart;

					//re-check if this timeValue is in the page
					tStart = OmnGetTimestamp();
					isInPage = inPage(mStatRecordMap, keyValueList, mergeFlag, rcdItr);
					tEnd = OmnGetTimestamp();
					mStatProcTime += tEnd - tStart;
					mStatProcNum++;
					if (!isInPage)
					{
						//tStart = OmnGetTimestamp();
						vt2dRcd->skipByTimeValue(mRundata, timeValue, mQryInfo->mGrpbyTimeUnit);
						//tEnd = OmnGetTimestamp();
						//mVt2dRcdSkipTime += tEnd - tStart;
						continue;	
					}
				}

				if (mergeFlag)
				{
				//	u64 myval;

					statRcd = rcdItr->first.getPtrNoLock();
					//merge vt2d values into the stat record
					tStart = OmnGetTimestamp();
					statRcd->setStatKeyValues(stat_key_values);
					vt2dRcd->statByTimeValue(mRundata, statRcd, timeValue,
							mQryInfo->mGrpbyTimeUnit, &mQryInfo->mQryTimeConds);
					tEnd = OmnGetTimestamp();
					mVt2dProcTime += tEnd - tStart;
					mVt2dProcNum++;

					//OmnScreen << "merge flag" << endl;
				//	myval = *((u64*)mStatRecordList[0]->getMeasureValues());
					//aos_assert_r(myval < 10000, false);

				} else {

					//OmnScreen << "enter create a new record" << endl; 

					//create a new statRcd and insert before itr
					tStart = OmnGetTimestamp();
					if (mQryInfo->mGrpbyTimeUnit == AosStatTimeUnit::eAllTime ||
						mQryInfo->mGrpbyTimeUnit == AosStatTimeUnit::eInvalid) 
					{
						//time irrelevant, use keyvaluelist directly
						statRcd = new AosStatRecord(keyValueList, 
								mMeasureInfoList, mDistInMeasureList AosMemoryCheckerArgs); 
						keyValueListUsed = true;
					}
					else 
					{
						//clone the keyValueList into the timeKeyValueList
						timedKeyValueList = new OmnValueList();
						for (u32 i = 0; i < keyValueList->size(); i++)
						{
							timedKeyValueList->push_back((*keyValueList)[i]);
						}

						//time relevant, use the timed keyvalueList directly
						statRcd = new AosStatRecord(timedKeyValueList, 
								mMeasureInfoList, mDistInMeasureList AosMemoryCheckerArgs); 
					}
					tEnd = OmnGetTimestamp();
					mCreateStatRcdTime += tEnd - tStart;
					mCreateStatRcdNum++;

					// Ketty 2014/12/02
					tStart = OmnGetTimestamp();
					statRcd->setStatKeyValues(stat_key_values);
					initStatRecord(statRcd);
					tEnd = OmnGetTimestamp();
					mSetStatKeyValueTime += tEnd - tStart;
					mSetStatKeyValueNum++;
					

					tStart = OmnGetTimestamp();
					statRcd->setHasValidFlag((*vector2DList)[j]->hasValidFlag());
					tEnd = OmnGetTimestamp();
					mSetStatRcdValueTime += tEnd - tStart;
					mSetStatRcdValueNum++;

					tStart = OmnGetTimestamp();
					mStatRecordMap[statRcd] = 1;
					//mStatRecordList.insert(rcdItr, statRcd);
					tEnd = OmnGetTimestamp();
					mProcStatRecordListTime += tEnd - tStart;
					mProcStatRecordListNum++;

					//merge vt2d values into the stat record
					tStart = OmnGetTimestamp();
					vt2dRcd->statByTimeValue(mRundata, statRcd, timeValue,
							mQryInfo->mGrpbyTimeUnit, &mQryInfo->mQryTimeConds);
					tEnd = OmnGetTimestamp();
					mVt2dProcTime += tEnd - tStart;
					mVt2dProcNum++;

					tStart = OmnGetTimestamp();
					//removeOutPageRecords();				
					tEnd = OmnGetTimestamp();
					t7 += tEnd - tStart;
				}
			}
		}

		//if the keyValueList not used, release it
		if (!keyValueListUsed)
			delete keyValueList;
	}

	delete vt2dRcd;
	
	mKeyConn->outputCounters();

	return true;
}

//
//read a keyvalue list of a stat doc id
//take out the group by key values
//
// Keys may have 2 formats
//  1. f1_f2  <--> group by f1, f2
//  2. f1_f2_f3 <---> dist_count(f3) group by f1, f2
//     in the 2nd format, there could be only 1 dist
//     count key and it is at the last position
//
// For mQryInfo's statKey, it always in 1st format, we
//     need to read the dist key at the end
//
OmnValueList *
AosStatQryEngine::readKeyValues(
		u64 sdocid,
		OmnString &distValue,
		OmnStringList &kvList)
{

	OmnString keys;
	//OmnStringList kvList;
	OmnValueList *grpbyList;
	OmnString timeValue = "0";
	int pos;

	kvList.clear();
	keys = mKeyConn->readKey(mRundata, sdocid);
	if(keys == "")
	{
		keys = mKeyConn->readKey(mRundata, sdocid);
	}
	//aos_assert_r(keys != "", NULL);
	if (keys == "")
		return NULL;

	// Ketty 2014/12/12
	AosStrSplit::splitStrBySubstr(keys, AOS_COUNTER_SEGMENT_SEP2, kvList, 0);

	pos = kvList.size();
	if (mDistMeasure.isEmpty())
	{
		//arvin 2015.08.09
		//JIMODB-157:kvList[0]=="_const" => "true"
		if (mQryInfo->mStatKeyFields.empty() &&
			pos == 1 &&
			kvList[0] == "true")
		{
			//do nothing for now
			//this means there is no group by keys
		}
		else
		{
			aos_assert_r(mQryInfo->mStatKeyFields.size() ==(u32) pos, NULL);
		}
	}
	else
	{
		aos_assert_r(mQryInfo->mStatKeyFields.size() + 1 ==(u32) pos, NULL);
		distValue =	kvList[pos - 1];
	}

	//set the default timeValue before reading v2d data
	timeValue = "0";
	if ( (mTimeKeyPos >= 0) && !mGrpbyFieldIsAscList[mTimeKeyPos])
	{
		//if desc order, set the default order to be a bigger number
		timeValue = "999999";   //a big number
	}

	//
	//need to keep groupby only fields (including time fields
	//if in groupby
	//
	grpbyList = new OmnValueList();
	AosValueRslt vv;
	bool rslt;
	for (u32 i = 0; i < mKeyOrderList.size(); i++)
	{
		pos = mKeyOrderList[i];
		if (pos == -1)
		{
			//set the field to be the default
			grpbyList->push_back(AosValueRslt(timeValue));
		}
		else
		{
			//performance issue: in the future, we may
			//consider to get rid of so many push_back
			rslt = convertStrToValueRslt(kvList[pos], i, vv); 
			aos_assert_r(rslt, NULL);
			grpbyList->push_back(vv);
		}
	}

	return grpbyList;
}


bool
AosStatQryEngine::convertStrToValueRslt(
		OmnString	 	keyValue,
		int 		 	idx,
		AosValueRslt	&vv)
{
	AosDataType::E type = mGroupByKeyIdxType[idx];
	switch(type)
	{
	case AosDataType::eU64:
		vv.setU64(keyValue.toU64(0));
		return true;
	case AosDataType::eString:
		vv.setStr(keyValue);
		return true;
	case AosDataType::eDouble:
		vv.setDouble(keyValue.toD64());
		return true;
	case AosDataType::eInt64:
		vv.setI64(keyValue.toI64());
		return true;
	case AosDataType::eDateTime:
		vv.setI64(keyValue.toI64());
		return true;
	default:
		OmnAlarm << "Invalid datatype : " << type << enderr;
		break;
	}
	return false;
}

bool 
AosStatQryEngine::compareStatKey(
		const AosStatRecordPtr &lhs, 
		const AosStatRecordPtr &rhs)
{
	int rslt =  lhs->cmpStatRecordKeys(
						rhs->getKeyValueList(),
						rhs->getOrderIsAsc());

	return (rslt > 0);
}

bool
AosStatQryEngine::serializeTo(
		const AosRundataPtr &rdata,
		AosBuff *buff)
{
	u64 tStart, tEnd;
	u32 num;
	OmnString name;
	AosStatRecord *rcd;
	u32 pos;

	//save measure names firstly
	num = mMeasureInfoList->size();
	buff->setU32(num);
	for (u32 i = 0; i < num; i++)
	{
		if((*mMeasureInfoList)[i].mCond)
			name = (*mMeasureInfoList)[i].mCondMeasureName;
		else
			name = (*mMeasureInfoList)[i].mName;
		buff->setOmnStr(name);
	}
	
	//save dist measure names
	num = mDistInMeasureList->size();
	buff->setU32(num);
	for (u32 i = 0; i < num; i++)
	{
		pos = (*mDistInMeasureList)[i];
		buff->setOmnStr((*mMeasureInfoList)[pos].mFieldName);
	}

	//save number of records
//	num = mStatRecordList.size();
//	buff->setU32(num);
//	for (u32 i = 0; i < num; i++)
//	{
//		rcd = mStatRecordList[i];
//		rcd->serializeTo(rdata, buff);
//	}

	aos_assert_r(mStatRecordList.empty(),false);
	
	//get the key of StatRecordMap into StatRecordList                            
	StatRecordMap::iterator itrmap;
	for(itrmap = mStatRecordMap.begin(); itrmap != mStatRecordMap.end(); itrmap++)
	{
		mStatRecordList.push_back(itrmap->first);
	}

	tStart = OmnGetTimestamp();
	sort(mStatRecordList.begin(), mStatRecordList.end(), compareStatKey);
	tEnd = OmnGetTimestamp();
	mSingleSortTime += tEnd - tStart;
	mSingleSortNum ++;	
	
	num = mStatRecordList.size();
	buff->setU32(num);
	for (u32 i = 0; i < num; i++)
	{
		rcd = mStatRecordList[i].getPtrNoLock();
		rcd->serializeTo(rdata, buff);
	}


	return true;
}


bool
AosStatQryEngine::serializeFrom(
		const AosRundataPtr &rdata,
		AosBuff *buff)
{
	u64 tStart, tEnd;
	u32 num;
	AosStatRecordPtr rcd;
	//mark where the measures in the asked-for measure info list
	//the records to be serialized from may have different measure
	//info list
	vector<int> measurePosList;
	vector<int> distMeasurePosList;
	OmnString measureName;
	int pos;

	//collect measure pos list
	num = buff->getU32(0);
	tStart = OmnGetTimestamp();
	for (u32 i = 0; i < num; i++)
	{
		measureName = buff->getOmnStr("");
		pos = findMeasure(measureName);
		measurePosList.push_back(pos);
	}
	tEnd = OmnGetTimestamp();
	mCollectMeasureTime += tEnd - tStart;
	mCollectMeasureNum ++;
	OmnScreen << "(Statistics counters : Engine (serializeFrom)) Collect measure pos list --- Time : "
		<< mCollectMeasureTime << ", Num: " << mCollectMeasureNum << endl;

	//collect dist measure pos list
	tStart = OmnGetTimestamp();
	num = buff->getU32(0);
	for (u32 i = 0; i < num; i++)
	{
		measureName = buff->getOmnStr("");
		pos = findDistMeasure(measureName);
		distMeasurePosList.push_back(pos);
	}
	tEnd = OmnGetTimestamp();
	mCollectDistMeasureTime += tEnd - tStart;
	mCollectDistMeasureNum ++;
	OmnScreen << "(Statistics counters : Engine (serializeFrom)) Collect dist measure pos list --- Time : "
		<< mCollectDistMeasureTime << ", Num: " << mCollectDistMeasureNum << endl;

	//get stat records
	num = buff->getU32(0);
	tStart = OmnGetTimestamp();
	for (u32 i = 0; i < num; i++)
	{
		rcd = new AosStatRecord(NULL, mMeasureInfoList, mDistInMeasureList AosMemoryCheckerArgs);
		initStatRecord(rcd.getPtrNoLock());
		rcd->serializeFrom(rdata, buff, measurePosList, distMeasurePosList);

	//	mStatRecordMap[rcd] = 1;
		mStatRecordList.push_back(rcd);
	}
	tEnd = OmnGetTimestamp();
	mGetStatRecordsTime += tEnd - tStart;
	mGetStatRecordsNum ++;
	OmnScreen << "(Statistics counters : Engine (serializeFrom)) Get Stat records --- Time : "
		<< mGetStatRecordsTime << ", Num: " << mGetStatRecordsNum << endl;


	return true;
}

///////////////////////////////////////////////////////
//   Helper methods used by both cube and frontend
///////////////////////////////////////////////////////
//
//find if a vt2dRcd with keyValueList is in a page of statRcd or not:
//  If not in the page, return false;
//  If in the page, return true and:
//     If need to merge into an existing statRcd, mergeFlag = true
//          and itr point to the statRcd to merge
//     If not belong to any existing statRecord, mergeFlag = false
//          and itr point to the statRcd to merge insert before
//
//InPage will be called in both cube and frontend sides. It will order
//stat records by mOrderedGrpbyFieldList order.
//
//In frontend side, if the records need to be sorted by orderby setting
//at the end, sortStatRecord() needs to be called
//
#if 0
bool
AosStatQryEngine::inPage(StatRecordMap &rcdList,
						 OmnValueList *keyValueList,
						 bool &mergeFlag,
						 StatRecordMap::iterator &itr)
{

	aos_assert_r(keyValueList, false);

	if (rcdList.size() > 0)
	{
		AosStatRecordPtr rcd = rcdList.end()->first;
		int rslt;
		rslt = rcd->cmpStatRecordKeys(keyValueList,mGrpbyFieldIsAscList);
		if (rslt <= 0)
		{
			//The new keyValueList needs to be included
			mEasyRecord->setKeyValueList(keyValueList);
			itr = mStatRecordMap.find(mEasyRecord);
			if (itr != rcdList.end())
			{
				//Fount the record with same keys, need to merge
				mergeFlag = true;
			}
			else
			{
				mergeFlag = false;
			}

			return true;
		}
	}

	//check if keyValueList need to be append to the end
	if (!noMoreRecordNeeded())
	{
		mergeFlag = false;
		return true;
	}

	return false;
}
#endif


bool                                                            
AosStatQryEngine::inPage(StatRecordList &rcdList,
		OmnValueList *keyValueList,
		bool &mergeFlag,
		StatRecordList::iterator &itr)
{
	AosStatRecord *statRcd;

  //if (mStatRecordList.size() < mPageSize)
  //	return true;

	aos_assert_r(keyValueList, false);

  //
  //the stat record list could be large, need to use binary
  //search method
  //
  //lowerBound will position itr to be the first statRecord
  //which keyValue >= keyValueList or the end
  //
	itr = lowerBound(rcdList, keyValueList, NULL);
	if (itr != rcdList.end())
	{
		statRcd = (*itr).getPtrNoLock();
		if (statRcd->cmpStatRecordKeys(
					keyValueList, mGrpbyFieldIsAscList) == 0)
		{
			//same keyValueList, need to merge
			mergeFlag = true;
		}
		else
		{
			//need to insert a new stat record
			mergeFlag = false;
		}

		return true;
	}

	//check if keyValueList need to be append to the end
	if (!noMoreRecordNeeded())
	{
		mergeFlag = false;
		return true;
	}

	return false;
}

//
//This is for unordered_map
//This assumes that all the docid needs to be read
//
bool                                                            
AosStatQryEngine::inPage(StatRecordMap &rcdList,
		OmnValueList *keyValueList,
		bool &mergeFlag,
		StatRecordMap::iterator &itr)
{
	aos_assert_r(keyValueList, false);

	mEasyRecord->setKeyValueList(keyValueList);
	mEasyRecord->setOrderIsAscList(&mGrpbyFieldIsAscList);
	itr = mStatRecordMap.find(mEasyRecord);
	// pay    JIMODB-1385    2015-12-09
	mEasyRecord->setKeyValueList(NULL);
	if (itr != rcdList.end())
	{
		mergeFlag = true;
	}
	else
	{
		mergeFlag = false;
	}
	return true;          
}

//if more than enough records, remove last several stat record
//so that all the records are still in a page
bool
AosStatQryEngine::removeOutPageRecords()
{
	int numToRemove = 0;
	AosStatRecord *rcd;

	//if not page full, no need to remove extra records
	if (!noMoreRecordNeeded())
		return true;

	StatRecordList::iterator itr = mStatRecordList.begin();
	u32 size = mStatRecordList.size();
	if(size > mPageSize)
	{
		u32 count = 0;
		for (; itr != mStatRecordList.end() ;)
		{
			count++;
			if(count > mPageSize)
			{
				mStatRecordList.erase(itr++);
			}
			else
			{
				itr++;
			}

		}
	}
	return true;
}

bool
AosStatQryEngine::removeOutPageRecords(StatRecordList & rcdList)
{
	int numToRemove = 0;
	AosStatRecord *rcd;

	//if not page full, no need to remove extra records
	if (!noMoreRecordNeeded())
		return true;


	StatRecordList::iterator itr = rcdList.begin();
	u32 size = rcdList.size();
	if(size > mPageSize)
	{
		u32 count = 0;
		for (; itr != rcdList.end() ;)
		{
			count++;
			if(count > mPageSize)
			{
				rcdList.erase(itr);
				itr++;
			}
			else
			{
				itr++;
			}

		}
	}
	return true;
}
//apply Having in the cube
bool 
AosStatQryEngine::applyHavingCond()
{
	if(!(mQryInfo->mHitShuffleFields))
		return true;
	bool rslt;
	AosValueRslt value;
	StatRecordMap::iterator itr;
	StatRecordMap rcdMap;
	AosStatRecordPtr rcd;

	if (!mHavingExpr)
		return true;

	itr = mStatRecordMap.begin();
	while(itr != mStatRecordMap.end())
	{
		rcd = itr->first;
		rslt = mHavingExpr->getValue(mRundata, rcd.getPtr(), value);
		aos_assert_r(rslt, false);
		if (!value.getBool())
		{
			//non-select record, release the space
		//	delete rcd;
		}
		else
		{
			//save the matching records
			rcdMap[rcd] = 1;
		}
		itr++;
	}
	mStatRecordMap = rcdMap;
	return true;

}

//
//For internal use. Tell if no more record needs to
//be included in the current round
//
bool
AosStatQryEngine::noMoreRecordNeeded()
{
	if (mOrderByMeasureValue ||
		!mQryInfo->mRollupGrpbyFieldsGrp.empty() ||
		!mQryInfo->mCubeGrpbyFields.empty() ||
		mQryInfo->mHavingCond != "" ) 
	{
		return false;
	}

	return pageFull();
}

//
//For external use.
//
//Tell cube or frontend if more data is needed
//
bool
AosStatQryEngine::pageFull()
{
	if (mQryInfo->mNeedFullScan)
		return false;

#if 0
	//if having condition, need to read all data
	if (mQryInfo->mHavingCond != "")
		return false;
#endif

	return (mStatRecordMap.size() >= mPageSize || mStatRecordList.size() >= mPageSize);
}

//
//-1: measureName not found
//Otherwise return the index found
//
//If accumulate stat, measureName has the
//accumulate prefix
//
int
AosStatQryEngine::findMeasure(const OmnString &measureName)
{
	MeasureInfo *measureInfo;
	OmnString funcStr;
	int pos;

	pos = measureName.findSubString("0x28", 0);
	aos_assert_r(pos > 0, -1);
	funcStr = measureName.subString(0, pos);
	OmnString meaName = measureName;

	funcStr.toLower();
	meaName.toLower();
	for (u32 i = 0; i < mMeasureInfoList->size(); i++)
	{
		measureInfo = &((*mMeasureInfoList)[i]);
		OmnString name;
		if(measureInfo->mCond)
			name = measureInfo->mCondMeasureName;
		else
			name = measureInfo->mName;
			
#if 0
		if(name.hasPrefix("accu_"))
			name = name.substr(5,0);
#endif

		if (measureName == name) return i; 
		name.toLower();		
//		if(meaName.hasPrefix("count0x28cond") || (meaName.hasPrefix("count0x280x2a0x29") && name.hasPrefix("count0x28cond")) )
//			continue;

		// Ketty 2014/10/22
//		if(funcStr == "count" && measureInfo->mAggrFuncStr == "count")	return i;
		if(funcStr == "accu_count" && measureInfo->mAggrFuncStr == "accu_count")	return i;
	}

	return -1;
}

//
//-1: measureName not found
//Otherwise return the index found
//
int
AosStatQryEngine::findDistMeasure(const OmnString &measureName)
{
	OmnString name;
	int pos;

	for (u32 i = 0; i < mDistInMeasureList->size(); i++)
	{
		pos = (*mDistInMeasureList)[i];
		aos_assert_r(pos >= 0, -1);
		name = (*mMeasureInfoList)[pos].mFieldName;
		if (name == measureName)
			return i;
	}

	return -1;
}

//find the postion of a string in a string list
//return -1 if not found
int
AosStatQryEngine::findFieldByName(
		const vector<AosStatFieldPtr> &fieldList,
		const AosStatFieldPtr &field)
{
	for (u32 i = 0; i < fieldList.size(); i++)
	{
		if (field->mName == fieldList[i]->mName) return i; 
	}

	return -1;
}

bool
AosStatQryEngine::getStringFromKeys(OmnStringList &keyStringList, OmnString sep, OmnString &str)
{
	//aos_assert_r(keyStringList, false);

	str = "";
	for (u32 i = 0; i < keyStringList.size(); i++)
	{
		if (str == "")
			str << keyStringList[i];
		else
			str << sep << keyStringList[i];
	}

	return true;
}

//
//This algo is borrowed from STL lower_bound lib and
//tailored for local use
//
//If keyValueList is not null, compare is based on 
//keyvalue list, otherwise we need to compare measure
//values and key based on order by setting
//
//return the lower_bound place, NULL if something wrong
//
StatRecordList::iterator
AosStatQryEngine::lowerBound(StatRecordList &rcdList,
							 OmnValueList *keyValueList,
							 AosStatRecord *newRcd)
{
	int first = 0, middle;
	int half, len;
	AosStatRecord *rcd;
	int cmpRslt;

	aos_assert_r(keyValueList || newRcd, rcdList.end());
	len = rcdList.size();
	while(len > 0) 
	{
		half = len >> 1;
		middle = first + half;

		rcd = rcdList[middle].getPtrNoLock();
		if (keyValueList)
		{
			//compare key values
			cmpRslt = rcd->cmpStatRecordKeys(keyValueList, mGrpbyFieldIsAscList);
		}
		else 
		{
			//compare measure value or measure+key
			//this must happen in frontend side
			cmpRslt = rcd->cmpStatRecord(newRcd, mOrderedMeasurePosList, 
						mQryInfo->mOrderbyAscs);
		}

		if (cmpRslt == 1)
		{     
			first = middle + 1;          

			//find in the right side
			len = len - half - 1;       
		}
		else
		{
			//find in the left side
			len = half;           
		}
	}

	return (rcdList.begin() + first);
}

///////////////////////////////////////////////////////
//    Counters
///////////////////////////////////////////////////////
void
AosStatQryEngine::initCounters()
{
	//init different counters
	mReadKeyTime = 0;
	mReadStatDocTime = 0;
	mVt2dProcTime = 0;
	mStatProcTime = 0;
	mMergeTime = 0;
	mSortTime = 0;
	mRollupTime = 0;
	mCubePrepareTime = 0;
	mCubeGenRecordTime = 0;
	mCubeCombineTime = 0;
	mHavingCondTime = 0;

	mReadKeyNum = 0;
	mReadStatDocNum = 0;
	mVt2dProcNum = 0;
	mStatProcNum = 0;
	mMergeNum = 0;
	mSortNum = 0;
	mRollupNum = 0;
	mCubePrepareNum = 0;
	mCubeGenRecordNum = 0;
	mCubeCombineNum = 0;
	mHavingCondNum = 0;

	//Pay  JIMODB-1221	2015-11-24
	mInitCubeTime = 0;
	mInitVt2dRecordTime = 0;
	mVt2dRcdGetTime = 0;
	mValueRsltTime = 0;
	mVt2dRcdSkipTime = 0;

	mCreateStatRcdTime = 0;
	mCreateStatRcdNum = 0;

	mSetStatKeyValueTime = 0;
	mSetStatKeyValueNum = 0;

	mSetStatRcdValueTime = 0;
	mSetStatRcdValueNum = 0;

	mProcStatRecordListTime = 0;
	mProcStatRecordListNum = 0;

	mSingleSortTime = 0;
	mSingleSortNum = 0;

	mCollectMeasureTime = 0;
	mCollectMeasureNum = 0;

	mCollectDistMeasureTime = 0;
	mCollectDistMeasureNum = 0;

	mGetStatRecordsTime = 0;
	mGetStatRecordsNum = 0;

	mRemoveOutPageRecordTime = 0;
	mRemoveOutPageRecordNum = 0;

	mRecordCompareTime = 0;
	mRecordCompareNum = 0;

	mPushRecordTime1= 0;
	mPushRecordNum1 = 0;

	mPushRecordTime2= 0;
	mPushRecordNum2 = 0;
}

void
AosStatQryEngine::outputCounters()
{
	OmnScreen << "(Statistics counters : Engine) Read key values --- Time : "
		<< mReadKeyTime << ", Num: " << mReadKeyNum << endl;

	OmnScreen << "(Statistics counters : Engine) Read stat docs --- Time : "
		<< mReadStatDocTime << ", Num: " << mReadStatDocNum << endl;

	OmnScreen << "(Statistics counters : Engine) Proc Vector2D record --- Time : "
		<< mVt2dProcTime << ", Num: " << mVt2dProcNum << endl;

	OmnScreen << "(Statistics counters : Engine) Proc Stat record --- Time : "
		<< mStatProcTime << ", Num: " << mStatProcNum << endl;

	OmnScreen << "(Statistics counters : Engine) Merge Stat record --- Time : "
		<< mMergeTime << ", Num: " << mMergeNum << endl;

	OmnScreen << "(Statistics counters : Engine) Merge Stat record (Compare) --- Time : "
			<< mRecordCompareTime << ", Num: " << mRecordCompareNum << endl;

	OmnScreen << "(Statistics counters : Engine) Merge Stat record (push1)--- Time : "
			<< mPushRecordTime1 << ", Num: " << mPushRecordNum1 << endl;

	OmnScreen << "(Statistics counters : Engine) Merge Stat record (push2)--- Time : "
			<< mPushRecordTime2 << ", Num: " << mPushRecordNum2 << endl;

	OmnScreen << "(Statistics counters : Engine) Merge Stat Remove Out Page record --- Time : "
			<< mRemoveOutPageRecordTime << ",Num: " << mRemoveOutPageRecordNum <<endl;


	OmnScreen << "(Statistics counters : Engine) Sort Stat record --- Time : "
		<< mSortTime << ", Num: " << mSortNum << endl;

	OmnScreen << "(Statistics counters : Engine) Generate rollup record --- Time : "
		<< mRollupTime << ", Num: " << mRollupNum << endl;

	OmnScreen << "(Statistics counters : Engine) Prepare cube info --- Time : "
		<< mCubePrepareTime << ", Num: " << mCubePrepareNum << endl;

	OmnScreen << "(Statistics counters : Engine) Generate cube record --- Time : "
		<< mCubeGenRecordTime << ", Num: " << mCubeGenRecordNum << endl;

	OmnScreen << "(Statistics counters : Engine) Combine cube record --- Time : "
		<< mCubeCombineTime << ", Num: " << mCubeCombineNum << endl;

	OmnScreen << "(Statistics counters : Engine) Having condition record --- Time : "
		<< mHavingCondTime << ", Num: " << mHavingCondNum << endl;

	OmnScreen << "(Statistics counters : Engine) Create stat record cost --- Time : "
		<< mCreateStatRcdTime << ", Num: " << mCreateStatRcdNum << endl;

	OmnScreen << "(Statistics counters : Engine) Set stat key value cost --- Time : "
		<< mSetStatKeyValueTime << ", Num: " << mSetStatKeyValueNum << endl;

	OmnScreen << "(Statistics counters : Engine) Set stat record value cost --- Time : "
		<< mSetStatRcdValueTime << ", Num: " << mSetStatRcdValueNum << endl;

	OmnScreen << "(Statistics counters : Engine) Insert record cost  --- Time : "
		<< mProcStatRecordListTime << ", Num: " << mProcStatRecordListNum << endl;

	OmnScreen << "(Statistics counters : Engine) Sort before merge --- Time : "
		<< mSingleSortTime << ", Num: " << mSingleSortNum << endl;

	OmnScreen << "(Statistics counters : Engine) Total Stat record in final result : "
		<< mStatRecordList.size() << endl;

//	OmnScreen << "(Statistics counters : Engine) Init cube cost  --- Time : "
//		<< mInitCubeTime << endl;
//
//	OmnScreen << "(Statistics counters : Engine) Init Vt2d record cost --- Time : "
//		<< mInitVt2dRecordTime << endl;
//
//	OmnScreen << "(Statistics counters : Engine) Get Vt2d record cost --- Time : "
//		<< mVt2dRcdGetTime << endl;
//
//	OmnScreen << "(Statistics counters : Engine) Value rslt cost  --- Time : "
//		<< mValueRsltTime << endl;
//
//	OmnScreen << "(Statistics counters : Engine) Skip bt time value cost --- Time : "
//		<< mVt2dRcdSkipTime << endl;
//
}

