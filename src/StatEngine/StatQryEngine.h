///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet QryEngineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet QryEngineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2014/09/30 Created by Phil 
// 
////////////////////////////////////////////////////////////////////////////

#ifndef StatEngine_StatQryEngine_h
#define StatEngine_StatQryEngine_h

#include "StatServer/Ptrs.h"
#include "StatServer/StatKeyConn.h"
#include "StatUtil/StatModifyInfo.h"
#include "StatUtil/StatQueryInfo.h"
#include "StatUtil/StatRecord.h"
#include "Vector2DQryRslt/Ptrs.h"
#include "Vector2DUtil/Vt2dInfo.h"
#include "Vector2DUtil/Vt2dRecord.h"
#include "Vector2DUtil/Vt2dModifyInfo.h"
#include "Vector2D/Ptrs.h"
#include "StatUtil/StatTimeUnit.h"

#include <vector>
#include <map>
#include <tr1/unordered_map>
#include <algorithm>
using namespace std;

struct statrecord_hash
{	
	inline size_t operator()(const AosStatRecordPtr rcd) const 
	{                                                    
			OmnString keyStr;
			bool rslt;
			rslt = rcd->getKeyStr(keyStr);
			aos_assert_r(rslt,false);
		    return __stl_hash_string(keyStr.getBuffer());       
	}                                                    
};

struct compare_statrecord
{
	bool operator() (const AosStatRecordPtr &lhs, const AosStatRecordPtr &rhs) const
	{
		int rslt =  lhs->cmpStatRecordKeys(
							rhs->getKeyValueList(),
							rhs->getOrderIsAsc());
		return (rslt == 0);
	}
};


typedef vector<AosStatRecordPtr> StatRecordList;
//typedef map<AosStatRecordPtr, int, compare_statrecord> StatRecordMap;
typedef tr1::unordered_map<AosStatRecordPtr, int, statrecord_hash, compare_statrecord> StatRecordMap;
typedef	hash_map<OmnString, StatRecordList, Omn_Str_hash, compare_str> StatRecordListMap;
typedef	hash_map<OmnString, OmnStringList, Omn_Str_hash, compare_str> OmnStringListMap;
typedef	hash_map<OmnString, bool, Omn_Str_hash, compare_str> BooleanMap;

class AosStatQryEngine: public OmnRCObject
{
	OmnDefineRCObject;

private:
	///////////////////////////////////////////////////////
	//         Basic members
	///////////////////////////////////////////////////////
	AosRundata* 				mRundata;
	vector<AosVector2DPtr>		*mVt2dList;
	AosStatKeyConn	 			*mKeyConn;
	u32  						mPageSize;
	//Contains final stat records
	//It is better to have less than 10000 record
	//in the list. Otherwise, the insertion/erase
	//operations for this many records will take 
	//very longer time:
	//10w: 0.5 second                     
	//30w: 4.5 seconds                    
	//100w: 50 seconds                    
	//It is proportional to the number of 
	//vector elements. ~ power(n)         
	StatRecordList		mStatRecordList;
	vector<StatRecordList *>		mMergeList;
	StatRecordMap 		mStatRecordMap;
	//map<AosStatRecordPtr, int, compare_measure> mStatMeasureMap;
	vector<AosStatRecordPtr>		mMissStatRecords;
	//map from field name to the index of field list
	OmnStringHashMap 			*mFieldNameMap;

	///////////////////////////////////////////////////////
	//         Query relevant info
	///////////////////////////////////////////////////////
	//contain all query info
	AosStatQueryInfo			*mQryInfo;
	//bool						mOrderIsAsc;
	//i64						mGrpbyTimeUnit;
	//vector<AosQueryMeasureInfo> mQryMeasures
	//vector<AosStatTimeArea> 	mQryTimeArea;
	//OmnStringList			mGrpbyFields;
	//OmnString					mHavingCond;
	AosExprObjPtr				mHavingExpr;
	OmnString					mDistMeasure;
	vector<MeasureInfo>			*mMeasureInfoList;
	vector<u32>					*mDistInMeasureList;
	vector<AosStatFieldPtr>		mOrderedGrpbyFieldList;
	vector<int>					mOrderedMeasurePosList;
	vector<bool>				mGrpbyFieldIsAscList;
	bool						mOrderByMeasureValue;
	//arvin 2015-10-8
	map<int,AosDataType::E>		mKeyIdxTypeMap;

	//mapper from vt2d to selected measure list
	vector<AosMeasureValueMapper>	*mMeasureMapperList;

	//group by keys in vt2d keys 
	vector<int>					mKeyOrderList;
	
	//define the order of the time field. -1 means no time grpby
	int						    mTimeKeyPos;  

	//tell if the query contains accumulate aggrestion
	bool						mHasAccumulate;
	map<int,AosDataType::E> 	mGroupByKeyIdxType;

	///////////////////////////////////////////////////////
	//         Cube/Rollup relevant info
	///////////////////////////////////////////////////////
	//key strings ---> cube stat record list
	StatRecordListMap mCubeRecordMap;

	//key strings ---> parent key strings list
	//a child's stat record values can be aggregated from its parent's.
	// E.g.  (k1, aggr_token) stat records can be aggregated from (k1, k2) stat records
	OmnStringListMap mCubeParentMap;

	//save split individual keys associated with cube key string for convenience
	OmnStringListMap mCubeKeyMap;

	//this map records which cube stat record lists are generated or displayed
	BooleanMap mCubeDoneMap;

	//the parent cube key string, normally concating all of the keys
	OmnString mCubeRootKey;

	//the cube key string with all the keys valued by the aggr token. This
	// is normally the aggration of all the record
	OmnString mCubeEndKey;

	///////////////////////////////////////////////////////
	//         different counters
	///////////////////////////////////////////////////////
	u64			mReadKeyTime;
	u64			mReadKeyNum;

	u64			mReadStatDocTime;
	u64			mReadStatDocNum;

	u64			mStatProcTime;
	u64			mStatProcNum;

	u64			mVt2dProcTime;
	u64			mVt2dProcNum;

	u64			mMergeTime;
	u64			mMergeNum;

	u64			mRemoveOutPageRecordTime;
	u64			mRemoveOutPageRecordNum;

	u64			mSortTime;
	u64			mSortNum;

	u64			mRollupTime;
	u64			mRollupNum;

	u64			mCubeGenRecordTime;
	u64			mCubeGenRecordNum;

	u64			mCubeCombineTime;
	u64			mCubeCombineNum;

	u64			mCubePrepareTime;
	u64			mCubePrepareNum;

	u64			mHavingCondTime;
	u64			mHavingCondNum;

	u64			mInitCubeTime;
	u64			mInitVt2dRecordTime;
	u64			mVt2dRcdGetTime;
	u64			mValueRsltTime;
	u64			mVt2dRcdSkipTime;

	u64			mCreateStatRcdTime;
	u64			mCreateStatRcdNum;

	u64			mSetStatKeyValueTime;
	u64			mSetStatKeyValueNum;

	u64			mSetStatRcdValueTime;
	u64			mSetStatRcdValueNum;

	u64			mProcStatRecordListTime;
	u64			mProcStatRecordListNum;

	u64			mSingleSortTime;
	u64			mSingleSortNum;

	u64			mCollectMeasureTime;
	u64			mCollectMeasureNum;

	u64			mCollectDistMeasureTime;
	u64			mCollectDistMeasureNum;

	u64			mGetStatRecordsTime;
	u64			mGetStatRecordsNum;

	u64			mRecordCompareTime;
	u64			mRecordCompareNum;

	u64			mPushRecordTime1;
	u64			mPushRecordNum1;

	u64			mPushRecordTime2;
	u64			mPushRecordNum2;

	//A stat record template for convenience
	AosStatRecordPtr	mEasyRecordPtr;
	AosStatRecord*		mEasyRecord;

public:
	///////////////////////////////////////////////////////
	//       Constructors/Destructors
	///////////////////////////////////////////////////////
	AosStatQryEngine(AosRundata* rdata, 
					 AosStatQueryInfo *queryInfo); 
	~AosStatQryEngine();

	///////////////////////////////////////////////////////
	//        init methods
	///////////////////////////////////////////////////////
	bool basicInit();
	bool cubeInit();
	bool frontInit();

	//generate different needed data structures
	bool 	genGrpbyKeyOrder();
	bool 	genMeasureInfo();
	bool 	genMeasureMappers();
	bool 	genDistMeasureMappers();
	bool 	genOrderInfo();
	bool	genFieldNameMap();

	//init stat record or vt2d record
	bool 	initStatRecord(AosStatRecord *statRcd);
	bool 	initVt2dRecord(AosVt2dRecord *vt2dRcd, u32 idx);

	///////////////////////////////////////////////////////
	//  Accumulate function interface					 //
	//////////////////////////////////////////////////////
	bool inSameAccumulateGroup(OmnValueList *keyValueList1,OmnValueList *keyValueList2);
	bool resolveQryTime(AosStatTimeArea &oriTimeArea, AosStatTimeUnit::E timeUnit);
	bool addOneRecord(AosStatRecord* &prevRcd);
	bool appendMissingRecord();
	AosStatRecord* createMissingRecord(AosStatRecord* rcd);
	///////////////////////////////////////////////////////
	//   Cube-used methods to generate the basic
	//   statistics result
	///////////////////////////////////////////////////////
	bool runStat(const vector<u64> &docidList,
					  AosStatKeyConn* keyConn,
					  vector<AosVector2DPtr>   *vector2DList);

	OmnValueList *readKeyValues(
				u64 sdocid,
				OmnString &distValue,
				OmnStringList &kvList);

	bool 	serializeTo(
					const AosRundataPtr &rdata,
					AosBuff *buff);

	bool 	serializeFrom(
					const AosRundataPtr &rdata,
					AosBuff * buff);

	///////////////////////////////////////////////////////
	//   Frontend-used methods to consolidate data from
	//   different cubes
	///////////////////////////////////////////////////////
	bool dataFinished(const AosRundataPtr &rdata);
	int sortStatRecord();
	bool filterByHavingCond();
	bool checkShuffle();
	bool applyDistCount();
	bool applyAccumulate();

	int mergeStatRecord();
	int mergeStatRecord(AosStatQryEngine *engine, bool inOrder);
	int mergeStatRecord(vector<StatRecordList *> recordList, bool inOrder);
	StatRecordList *mergeStatRecord(StatRecordList *rcdList1,StatRecordList *rcdList2,bool inOrder);
	//int mergeStatRecord(StatRecordList *recordList, bool inOrder);
	bool mergeStatRecord(AosStatRecord *record);

	StatRecordList *getStatData() { return &mStatRecordList; }
	u32		getRcdNum(){ return mStatRecordList.size(); };
	int 	getFieldIdx(const OmnString &name);
	bool	getFieldValue(  const u32 rcd_idx,
				const OmnString &field_name,
				AosValueRslt &value_rslt);
	bool 	getFieldValue(  const u32 rcd_idx, 
			        const int field_idx,
			        AosValueRslt &value_rslt);

	///////////////////////////////////////////////////////
	//        Rollup/Cube query function relevant methods
	///////////////////////////////////////////////////////
	OmnValueList *getRollupSubKeys(vector<AosStatFieldPtr> *keyList,
									 AosStatRecord *rcd);
	bool isRollupSubKeys(OmnValueList *subKeyValueList,
						    	OmnValueList *keyValueList);

	bool	genRollupRecord();

	//cube methods
	bool	genCubeInfo();
	bool	genCubeRecord();
	bool	genCubeRecord(OmnString cubeKeyStr);
	bool	genCubeRecordFromParent(OmnString cubeKeyStr,
			   						  OmnString parentStr);
	bool	combineCubeRecord();
	bool	combineCubeRecord(OmnString cubkeyStr);

	bool	isCubeSubKeys(OmnStringList *subKeyValueList,
						OmnStringList *keyValueList);
	OmnValueList 	*getCubeSubKeys(OmnStringList &keyStringList,
									AosStatRecord *rcd);
	void	resetCubeDone();
	bool 	checkCubeDone();
	void	outputCubeInfo();


	///////////////////////////////////////////////////////
	//   Helper methods
	///////////////////////////////////////////////////////
	bool inPage(StatRecordMap &rcdList,
				OmnValueList *keyValueList,
				bool &mergeFlag,
				StatRecordMap::iterator &itr);
	bool inPage(StatRecordList &rcdList,
				OmnValueList *keyValueList,
				bool &mergeFlag,
				StatRecordList::iterator &itr);
	bool applyHaving();
	bool applyOrderBy();
	bool applyHavingCond();
	bool pageFull();
	bool noMoreRecordNeeded();
	bool removeOutPageRecords();
	bool removeOutPageRecords(StatRecordList &rcdList);
	int collectRecord(AosStatQryEngine *engine,bool inOrder);

	int findFieldByName(
		const vector<AosStatFieldPtr> &fieldList,
		const AosStatFieldPtr &field);
	int findMeasure(const OmnString &measureName);
	int findDistMeasure(const OmnString &measureName);
	bool getStringFromKeys(OmnStringList &keyStringList, OmnString sep, OmnString &str);

	StatRecordList::iterator lowerBound( StatRecordList &rcdList,
												  OmnValueList *keyValueList,
												  AosStatRecord *newRcd);

	bool setKeyIdxTypeMap(const map<int,AosDataType::E> &keyIdxTypeMap);
	bool setGroupByKeyType(const map<int,AosDataType::E> &groupByKeyType);
	///////////////////////////////////////////////////////
	//        Counters methods
	///////////////////////////////////////////////////////
	void initCounters();
	void outputCounters();

	static bool compareMeasure(
			const AosStatRecordPtr &lhs, 
			const AosStatRecordPtr &rhs);

	static bool compareStatKey(
			const AosStatRecordPtr &lhs, 
			const AosStatRecordPtr &rhs);

private:
	bool convertStrToValueRslt(
			OmnString	 	keyValue,
			int 		 	idx,
			AosValueRslt	&vv);
};

#endif
