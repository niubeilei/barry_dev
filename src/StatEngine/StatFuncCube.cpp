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
//     Cube relevant methods
////////////////////////////////////////////////////////////////
//aggr string is used to mark an aggregated field name or key value
//static OmnString AGGR_TOKEN = "+!AGGR!+";

//
// Generate the following cube record list information:
// 1. init mCubeRecordMap: including level0, level1, ..... statRecord list
// 2. generate mCubeParentMap: each levelN stat record list has multiple
//             parents. All the parents' keys string are added as the map's
//             second value
//
bool
AosStatQryEngine::genCubeInfo()
{
	u64 tStart, tEnd;
	StatRecordList emptyRcdList;
	OmnStringList emptyParentList;
	StatRecordListMap::iterator itr;

	OmnString key;
	OmnString cubeKeyStr;      //all the keys concated into a string
	OmnStringList keyStringList;  //with all the keys saved in a vector
	OmnStringList cubeKeyStringList;  //save all of the cube key strings
	u32 normalGrpbyFieldNum = mQryInfo->mGrpbyFields.size();

	tStart = OmnGetTimestamp();

	//init all the vectors and maps
	emptyRcdList.clear();
	emptyParentList.clear();
	mCubeRecordMap.clear();
	mCubeParentMap.clear();

	//generate the 1st cube key string
	//use mOrderedGrpbyFieldList = grpbyFieldList + cubeGrpbyFieldList
	keyStringList.clear();
	for (u32 i = 0; i < mOrderedGrpbyFieldList.size(); i++)
		keyStringList.push_back(mOrderedGrpbyFieldList[i]->mName);

	getStringFromKeys(keyStringList, AOS_COUNTER_SEGMENT_SEP2, mCubeRootKey);

	//generate cube stat record map
	mCubeRecordMap[mCubeRootKey] = emptyRcdList;
	mCubeParentMap[mCubeRootKey] = emptyParentList;
	mCubeKeyMap[mCubeRootKey] = keyStringList;
	mCubeDoneMap[mCubeRootKey] = true; //the 1st parent cube is done by runStat()

	//generate different types of cube maps
	//Skip normal groupby fields since they will not be reduced
	//to next level
	for (u32 i = normalGrpbyFieldNum; i < mOrderedGrpbyFieldList.size(); i++)
	{
		key = mOrderedGrpbyFieldList[i]->mName;

		//get new keys strings when a new key is added
		cubeKeyStringList.clear();
		itr = mCubeRecordMap.begin();
		while (itr != mCubeRecordMap.end())
		{
			//for each cube key string, add a sub key string with the i-th
			//token replaced with the aggration token
			cubeKeyStr = itr->first;
			keyStringList = mCubeKeyMap[cubeKeyStr];
			aos_assert_r(keyStringList.size() == mOrderedGrpbyFieldList.size(), false);

			keyStringList[i] = AGGR_TOKEN;
			getStringFromKeys(keyStringList, AOS_COUNTER_SEGMENT_SEP2, cubeKeyStr);
			cubeKeyStringList.push_back(cubeKeyStr);

			//set cubeKeyStr --> keyStringList directly
			mCubeKeyMap[cubeKeyStr] = keyStringList;

			//move to next cube stat record list
			itr++;
		}

		//add new key strings to stat record map
		for (u32 i = 0; i < cubeKeyStringList.size(); i++)
		{
			//add the initial stat record list for the new cubeKeyStr
			cubeKeyStr = cubeKeyStringList[i];
			mCubeRecordMap[cubeKeyStr] = emptyRcdList;

			//add the inital parent list for the new cubeKeyStr
			mCubeParentMap[cubeKeyStr] = emptyParentList;

			//set record done value to be false since record not generated yet
			mCubeDoneMap[cubeKeyStr] = false;
		}
	}

	//generate cube parent map
	OmnString oneKey;
	OmnString childKeyStr;
	OmnStringListMap::iterator keyMapItr;
	keyMapItr = mCubeKeyMap.begin();
	while (keyMapItr != mCubeKeyMap.end())
	{
		cubeKeyStr = keyMapItr->first;
		keyStringList = keyMapItr->second;
		aos_assert_r(keyStringList.size() == mOrderedGrpbyFieldList.size(), false);

		for (u32 i = normalGrpbyFieldNum; i < keyStringList.size(); i++)
		{
			if (keyStringList[i] == AGGR_TOKEN)
				continue;

			//for a given keyStringList, replace any one key in the list with the aggr token, a
			// sub keyStringList will be genreated. This subKeyStringList's cubeKeyStr's parent
			// is the current cubeKeyStr. E.g. for (k1, k2, k3), replace k1, (+!aggr!+, k2, k3)'s
			// parent is (k1, k2, k3)
			oneKey = keyStringList[i];
			keyStringList[i] = AGGR_TOKEN;
			aos_assert_r(getStringFromKeys(keyStringList,
							AOS_COUNTER_SEGMENT_SEP2, childKeyStr), false);

			//recover keyStringList value
			keyStringList[i] = oneKey;

			mCubeParentMap[childKeyStr].push_back(cubeKeyStr);
		}

		keyMapItr++;
	}

	//Generate mCubeEndKey which is the combination of all the aggr tokens
	//But normal groupby fields should not be replaced with aggr_token
	mCubeEndKey = "";
	for (u32 i = 0; i < mOrderedGrpbyFieldList.size(); i++)
	{
		if (mCubeEndKey != "")
			mCubeEndKey <<  AOS_COUNTER_SEGMENT_SEP2;

		if (i < normalGrpbyFieldNum)
			mCubeEndKey << mOrderedGrpbyFieldList[i]->mName;
		else
			mCubeEndKey << AGGR_TOKEN;
	}

	//check if cube end key with all keys valued as the aggr token is created
	itr = mCubeRecordMap.find(mCubeEndKey);
	aos_assert_r(itr != mCubeRecordMap.end(), false);

	tEnd = OmnGetTimestamp();
	mCubePrepareTime += tEnd - tStart;
	mCubePrepareNum++;

	//output Cube relevant info
	outputCubeInfo();
	return true;
}

bool
AosStatQryEngine::genCubeRecord()
{
	if (mQryInfo->mCubeGrpbyFields.empty())
		return true;

	resetCubeDone();
	aos_assert_r(genCubeRecord(mCubeEndKey), false);
	aos_assert_r(checkCubeDone(), false);

	return true;
}

//
//This method generate different levels of cube stat record list
//from the original stat record list (level 0)
//
// LevenN (n > 0) record list normally generated from its parent
// stat record list with least record number.
// E.g.
//      level0 keys:  (k1, k2 k3)
//      level1 keys:  (k1, k2, aggr_token), (k1, aggr_token, k3),
//					  (aggr_token, k2, k3)
//      level2 keys:  (k1, aggr_token, aggr_token),
//					  (aggr_token, k2, aggr_token),
//					  (aggr_token, aggr_token, k3)
//
// Use level2 keys: (aggr_token, k2, aggr_token) as the example. It has 2 parents
//                  (k1, k2, aggr_token), (aggr_token, k2, k3)
// if (k1, k2, aggr_token)'s record num is 100, (aggr_token, k2, k3)'s record
//      number is 20, then (aggr_token, k2, k3)'s record list will be used to
//      generate (aggr_token, k2, aggr_token)'s stat record list
//
// We use recursive approach to generate cube records:
//  1. A cube stat record list can be generated only when all of its
//     parent cube stat record list are generated
//  2. the cube stat record list will chose the shortest parent stat
//     record list to generate its own stat record list
//
bool
AosStatQryEngine::genCubeRecord(OmnString cubeKeyStr)
{
	u64 tStart, tEnd;
	OmnString parentStr;
	StatRecordListMap::iterator recordItr;
	OmnStringListMap::iterator parentItr;
	BooleanMap::iterator doneItr;

	if (mQryInfo->mCubeGrpbyFields.empty())
		return true;

	//For the root cube key string, runStat will generate the records.
	//There is no need to generate cube record again
	if (cubeKeyStr == mCubeRootKey)
	{
		mCubeDoneMap[cubeKeyStr] = true;
		return true;
	}

	//check if records of this cube key string are already generated
	doneItr = mCubeDoneMap.find(cubeKeyStr);
	aos_assert_r(doneItr != mCubeDoneMap.end(), false);
	if (doneItr->second)
		return true;   //records already generated

	//find the cube key's parent with shortest stat record list
	parentItr = mCubeParentMap.find(cubeKeyStr);
	aos_assert_r(parentItr != mCubeParentMap.end(), false);

	OmnStringList &parentList = parentItr->second;
	OmnString minParentStr = "";
	u64 minRecordNum = 999999999999;  //just a big number
	for (u32 i = 0; i < parentList.size(); i++)
	{
		parentStr = parentList[i];
		doneItr = mCubeDoneMap.find(parentStr);
		aos_assert_r(doneItr != mCubeDoneMap.end(), false);

		if (!doneItr->second)
		{
			//genereate the stat record list for the parent firstly
			aos_assert_r(genCubeRecord(parentStr), false);
		}

		if (minRecordNum > mCubeRecordMap[parentStr].size())
		{
			minRecordNum = mCubeRecordMap[parentStr].size();
			minParentStr = parentStr;
		}
	}

	//use the shortest parent record list to generate stat record list
	tStart = OmnGetTimestamp();
	genCubeRecordFromParent(cubeKeyStr, minParentStr);
	mCubeDoneMap[cubeKeyStr] = true;

	tEnd = OmnGetTimestamp();
	mCubeGenRecordTime += tEnd - tStart;
	mCubeGenRecordNum++;
	return true;
}

bool
AosStatQryEngine::genCubeRecordFromParent(OmnString cubeKeyStr,
											  OmnString parentStr)
{
	StatRecordList *curList, *parentList;
	StatRecordListMap::iterator itr;
	//OmnStringList *subKeyValueList;
	OmnValueList *subKeyValueList;

	itr = mCubeRecordMap.find(cubeKeyStr);
	aos_assert_r(itr != mCubeRecordMap.end(), false);
	curList = &(itr->second);

	if (parentStr == mCubeRootKey)
	{
		//for the root cube stat record list, just use mStatRecordList
		parentList = &mStatRecordList;
	}
	else
	{
		itr = mCubeRecordMap.find(parentStr);
		aos_assert_r(itr != mCubeRecordMap.end(), false);
		parentList = &(itr->second);
	}

	AosStatRecord *parentRcd = NULL;
	AosStatRecord *rcd = NULL;
	bool mergeFlag;
	StatRecordList::iterator rcdItr;
	for (u32 i = 0; i < parentList->size(); i++)
	{
		parentRcd = (*parentList)[i].getPtrNoLock();
		subKeyValueList = getCubeSubKeys(mCubeKeyMap[cubeKeyStr], parentRcd);
		aos_assert_r(inPage(*curList, subKeyValueList, mergeFlag, rcdItr), false);
		if (mergeFlag)
		{
			//merge the parent record's measure values
			rcd = (*rcdItr).getPtrNoLock();
		}
		else
		{
			//create a new record to aggreate new values
			rcd = new AosStatRecord(subKeyValueList, mMeasureInfoList, mDistInMeasureList AosMemoryCheckerArgs);
			initStatRecord(rcd);
			rcd->setStatKeyValues(parentRcd->getStatKeyValues());

			//insert the new record
			curList->insert(rcdItr, rcd);
		}
		
		//aggregate the parent record measure values
		rcd->addValue(mRundata, parentRcd);
	}

	//output the stat records' size
	OmnScreen << "Cube Statistics: generated " << curList->size() << 
		" stat records for " << cubeKeyStr << " from parent " << parentStr << endl;

	return true;
}

//
//combine cube records to mStatRecordList in the order from
//most number of grpby keys to least number of cube grpby 
//keys. 
//
//Using simplied BFS algorith to traverse mCubeKeyMap
//
bool
AosStatQryEngine::combineCubeRecord()
{
	if (mQryInfo->mCubeGrpbyFields.empty())
		return true;
	
	//add counters
	u64 tStart, tEnd;
	tStart = OmnGetTimestamp();

	//save cube keys to be traverse parents
	OmnStringList visitKeyList;

	//save cube keys to combine in the reverse order
	OmnStringList reverseKeyList;

	//starting cub key
	OmnString startCubeKey = mCubeEndKey;
	OmnString cubeKey, parentKey;
	OmnStringList parentList;

	//clear lists and init visit status to be all false
	visitKeyList.clear();
	reverseKeyList.clear();
	resetCubeDone();

	//add the start cube key in visitKeyList
	u32 index = 0;
	visitKeyList.push_back(startCubeKey);
	mCubeDoneMap[startCubeKey] = true;
	while (true)
	{
		//save into reverseKeyList 
		cubeKey = visitKeyList[index];
		reverseKeyList.push_back(cubeKey);
		parentList = mCubeParentMap[cubeKey];
		for (u32 i = 0; i < parentList.size(); i++)
		{
			parentKey = parentList[i];
			if (!mCubeDoneMap[parentKey])
			{
				visitKeyList.push_back(parentKey);
				mCubeDoneMap[parentKey] = true;
			}
		}

		index++;
		if (index >= visitKeyList.size())
			break;
	}
	aos_assert_r(checkCubeDone(), false);

	//traverse all the cubekeys in reverseKeyList to combine records
	StatRecordListMap::iterator recordItr; 	
	StatRecordList *rcdList;
	AosStatRecord *rcd;
	
	//
	//the reverseKeyList should be like:
	// [ (), (k1), (k2), (k3), (k1, k2), (k1, k3), (k2, k3), (k1, k2, k3) ]
	// we need to combine records in reverse order and skip the last
        // cubekey which should be mCubeRootKey since it is mStatRecordList
	//
	resetCubeDone();
	mCubeDoneMap[mCubeRootKey] = true;
	for (int i = reverseKeyList.size() - 2; i >= 0 ; i--)
	{
		cubeKey = reverseKeyList[i];
		if (cubeKey == mCubeRootKey) continue;

		//combine the cube key's record into mStatRecordList
		recordItr = mCubeRecordMap.find(cubeKey);
		aos_assert_r(recordItr != mCubeRecordMap.end(), false);

		rcdList = &(recordItr->second);
		for (u32 i = 0; i < rcdList->size(); i++)
		{
			rcd = (*rcdList)[i].getPtrNoLock();
			mStatRecordList.push_back(rcd);
		}

		mCubeDoneMap[cubeKey] = true;
		mCubeCombineNum++;
	}
	
	//verify all the cubekey's records are combined
	aos_assert_r(checkCubeDone(), false);
	tEnd = OmnGetTimestamp();
	mCubeCombineTime += tEnd - tStart;
	return true;
}

/*
bool
AosStatQryEngine::combineCubeRecord()
{
	if (mQryInfo->mCubeGrpbyFields.empty())
		return true;

	resetCubeDone();
	aos_assert_r(combineCubeRecord(mCubeEndKey), false);
	aos_assert_r(checkCubeDone(), false);

	return true;
}
*/

bool
AosStatQryEngine::combineCubeRecord(OmnString cubeKeyStr)
{
	u64 tStart, tEnd;
	OmnString parentStr;
	StatRecordListMap::iterator recordItr;
	OmnStringListMap::iterator parentItr;
	BooleanMap::iterator doneItr;

	if (mQryInfo->mCubeGrpbyFields.empty())
		return true;

	//mCubeRootKey's stat record list is mStatRecordList and no
	//need to combine
	if (cubeKeyStr == mCubeRootKey)
	{
		mCubeDoneMap[cubeKeyStr] = true;
		return true;
	}

	//check if records of this cube key string are already generated
	doneItr = mCubeDoneMap.find(cubeKeyStr);
	aos_assert_r(doneItr != mCubeDoneMap.end(), false);
	if (doneItr->second)
		return true;   //records already combined

	//its parent cube stat record list needs to be combined firstly
	parentItr = mCubeParentMap.find(cubeKeyStr);
	aos_assert_r(parentItr != mCubeParentMap.end(), false);
	OmnStringList &parentList = parentItr->second;
	for (u32 i = 0; i < parentList.size(); i++)
	{
		parentStr = parentList[i];
		doneItr = mCubeDoneMap.find(parentStr);
		aos_assert_r(doneItr != mCubeDoneMap.end(), false);

		if (!doneItr->second)
		{
			//genereate the stat record list for the parent firstly
			aos_assert_r(combineCubeRecord(parentStr), false);
		}
	}

	tStart = OmnGetTimestamp();
	//append current cube stat records at the end mStatRecordList
	StatRecordList *rcdList;
	AosStatRecord *rcd;
	recordItr = mCubeRecordMap.find(cubeKeyStr);
	aos_assert_r(recordItr != mCubeRecordMap.end(), false);

	rcdList = &(recordItr->second);
	for (u32 i = 0; i < rcdList->size(); i++)
	{
		rcd = (*rcdList)[i].getPtrNoLock();
		mStatRecordList.push_back(rcd);
	}

	mCubeDoneMap[cubeKeyStr] = true;
	tEnd = OmnGetTimestamp();

	mCubeCombineTime += tEnd - tStart;
	mCubeCombineNum++;
	return true;
}

//
//check if the subkeyValueList is a direct subset of keyValueList
// with the same key orders:
//    (f1, f2, f3) may have the following subsets:
//    (f1, aggr_token, f3), (aggr_token, f2, f3), (f1, f2, aggr_token)
//
// Extra is the place where parent has the key while child doesn't
//
bool
AosStatQryEngine::isCubeSubKeys(OmnStringList *subKeyValueList,
								OmnStringList *keyValueList)
{
	aos_assert_r(subKeyValueList && keyValueList, false);
	aos_assert_r(subKeyValueList->size() == keyValueList->size(), false)

	for (u32 i = 0; i < subKeyValueList->size(); i++)
	{
		if ((*subKeyValueList)[i] == AGGR_TOKEN)
			continue;

		if ((*subKeyValueList)[i] != (*keyValueList)[i])
			return false;
	}

	return true;
}

//
//get a subKeyValueList based on keys group
//
OmnValueList *
AosStatQryEngine::getCubeSubKeys(OmnStringList &keyStringList,
							 AosStatRecord *rcd)
{
	//aos_assert_r(keyStringList && rcd, false);
	aos_assert_r(rcd, false);
	OmnValueList *keyValueList = rcd->getKeyValueList();

	OmnValueList *subKeyValueList = new OmnValueList();
	for (u32 i = 0; i < keyStringList.size(); i++)
	{
		if (keyStringList[i] == AGGR_TOKEN)
			subKeyValueList->push_back(AosValueRslt(AGGR_TOKEN));
		else
			subKeyValueList->push_back((*keyValueList)[i]);
	}

	return subKeyValueList;
}

//
//reset done map for following group actions
//
void
AosStatQryEngine::resetCubeDone()
{
	BooleanMap::iterator itr;

	itr = mCubeDoneMap.begin();
	while (itr != mCubeDoneMap.end())
	{
		itr->second = false;
		itr++;
	}
}

//
//check if all the entries in cubeDoneMap are marked done
//
bool
AosStatQryEngine::checkCubeDone()
{
	BooleanMap::iterator itr;

	itr = mCubeDoneMap.begin();
	while (itr != mCubeDoneMap.end())
	{
		if (!itr->second)
			return false;

		itr++;
	}

	return true;
}

//
//output parepared cube info
//
void
AosStatQryEngine::outputCubeInfo()
{
	OmnStringListMap::iterator itr;

	OmnScreen << "Cube Statistics: mCubeRecordMap size is "
		<< mCubeRecordMap.size() << endl;

	OmnScreen << "Cube Statistics: mCubeDoneMap size is "
		<< mCubeDoneMap.size() << endl;

	OmnScreen << "Cube Statistics: mCubeKeyMap size is "
		<< mCubeKeyMap.size() << endl;

	u32 totalParentNum = 0;
	itr = mCubeParentMap.begin();
	while (itr != mCubeParentMap.end())
	{
		totalParentNum += itr->second.size();
		itr++;
	}

	OmnScreen << "Cube Statistics: mCubeParentMap size is " << mCubeParentMap.size() 
		<< " and total parent num is " << totalParentNum << endl;
}
