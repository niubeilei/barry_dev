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
//     Rollup relevant methods
////////////////////////////////////////////////////////////////
//aggr string is used to mark an aggregated field name or key value
//static OmnString AGGR_TOKEN = "+!AGGR!+";

//
//This method generate rollup records which are mostly
//level1, level2 ...... statistics data based on level0
//statistics data
//
//This method is called in frontend side when all the cube's statistics
//result is already merged into one sorted list
//
bool
AosStatQryEngine::genRollupRecord()
{
	vector<AosStatFieldPtr> *preRollupGrp, *curRollupGrp;
	StatRecordList::iterator itr;
	AosStatRecord *rcd, *newRcd;
	//OmnStringList *subKeyValueList;
	OmnValueList *subKeyValueList;
	u64 tStart, tEnd;

	if (mQryInfo->mRollupGrpbyFieldsGrp.empty())
		return true;

	tStart = OmnGetTimestamp();
	//
	//The first rollup grpby list is the same as group field list
	//which is the level0 keys list. Rollup actions start from the
	//second grpby list in rollup group
	//
	preRollupGrp = &mQryInfo->mRollupGrpbyFieldsGrp[0];
	for (u32 i = 1; i < mQryInfo->mRollupGrpbyFieldsGrp.size(); i++)
	{
		curRollupGrp = &mQryInfo->mRollupGrpbyFieldsGrp[i];
		newRcd = NULL;
		itr = mStatRecordList.begin();
		while (itr != mStatRecordList.end())
		{
			//create and init a new next-level stat record:
			//keys come from next cur-level stat record
			//measures set to be all-zeros
			rcd = (*itr).getPtrNoLock();
			if (!newRcd || !isRollupSubKeys(newRcd->getKeyValueList(), rcd->getKeyValueList()))
			{
				if (newRcd)
				{
					//insert the current newRcd into the record list
					itr = mStatRecordList.insert(itr, newRcd);
					itr++;
				}

				subKeyValueList = getRollupSubKeys(curRollupGrp, rcd);
				newRcd = new AosStatRecord(subKeyValueList, mMeasureInfoList, mDistInMeasureList AosMemoryCheckerArgs);
				initStatRecord(newRcd);
				newRcd->setStatKeyValues(rcd->getStatKeyValues());
			}

			//accumulate the previous level of statistics value
			//into the newRcd
			if (rcd->getKeyValueList()->size() == preRollupGrp->size())
			{
				newRcd->addValue(mRundata, rcd);
				mRollupNum++;
			}

			itr++;
		}

		//save the last unsaved rollup record if any
		if (newRcd)
			mStatRecordList.insert(mStatRecordList.end(), newRcd);

		preRollupGrp = curRollupGrp;
	}

	tEnd = OmnGetTimestamp();
	mRollupTime += tEnd - tStart;
	return true;
}

//
//check if the 1st keyValueList is a subset of keyValueList
//
//So far, we only support the following rollup key group sequence:
// 			(f1, f2, f3), (f1, f2), (f1) ()
//The latter key group is a subset of the former and
//    i. The subset is from the beginning part
//    ii. The subset is continuous in the former
//
bool
AosStatQryEngine::isRollupSubKeys(OmnValueList *subKeyValueList,
							OmnValueList *keyValueList)
{
	aos_assert_r(subKeyValueList && keyValueList, false);

	if (subKeyValueList->size() >= keyValueList->size())
		return false;

	for (u32 i = 0; i < subKeyValueList->size(); i++)
	{
		// by andy 
		if ((*subKeyValueList)[i].isNull() || (*keyValueList)[i].isNull()) return false;
		AosDataType::E type = AosDataType::autoTypeConvert((*subKeyValueList)[i].getType(), (*keyValueList)[i].getType());
		if (AosValueRslt::doComparison(eAosOpr_ne, type, (*subKeyValueList)[i], (*keyValueList)[i])) //if ((*subKeyValueList)[i] != (*keyValueList)[i])
			return false;
	}

	return true;
}

//
//get a subset of the record's key values based on keys group
//
OmnValueList *
AosStatQryEngine::getRollupSubKeys(vector<AosStatFieldPtr> *keyList,
							 AosStatRecord *rcd)
{
	aos_assert_r(keyList && rcd, false);
	OmnValueList *keyValueList = rcd->getKeyValueList();
	OmnValueList *subKeyValueList = new OmnValueList();
	for (u32 i = 0; i < keyList->size(); i++)
	{
		subKeyValueList->push_back((*keyValueList)[i]);
	}

	return subKeyValueList;
}

