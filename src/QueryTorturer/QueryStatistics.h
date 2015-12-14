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
// 2014/05/14	Created by Fei Pei
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryTorturer_QueryStatistics_h
#define Aos_QueryTorturer_QueryStatistics_h

#include "QueryTorturer/Ptrs.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include "QueryTorturer/QueryFieldKey.h"
#include "QueryTorturer/QueryFieldMeasure.h"
#include "QueryTorturer/QueryFieldTime.h"
#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "SEInterfaces/QueryProcCallback.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;

//class AosSmartQueryTester;
class AosSmartStatisticsTester;

class AosQueryStatistics : public AosQueryProcCallback
{
	OmnDefineRCObject;
	enum
	{
		eErrorMarginPercent = 5,
		eErrorMargin = 5,
		eCloseEnough = 10000,
		eMaxMoveToTime = 10,

		eDataFullCombination = 1000,
		eDataRandom = 1001,
		eDataDocId = 1002,
	};
public:
	typedef hash_map<const u64, u64, u64_hash, u64_cmp> map_t;
	typedef hash_map<const u64, u64, u64_hash, u64_cmp>::iterator mapitr_t;
	typedef hash_map<const u64, u8, u64_hash, u64_cmp> smap_t;
	typedef hash_map<const u64, u8, u64_hash, u64_cmp>::iterator smapitr_t;

private:
	vector<AosQueryTesterUtil::CondDef>	mConds;
	u64					mGroupStartTime;
	u64					mGroupEndTime;
	u64					mQueryStartTime;
	u64					mPageMaxTime;
	u64					mNumDocsQueried;
	AosSmartStatisticsTester*  	mTester;
	u64					mNumMatchedDocs;
	u64					mGroupId;
	AosBitmapObjPtr		mSeeds;
	vector<u64>			mSeedVec;
	vector<OmnString>	mQuery_in;
	vector<OmnString>   mSelectKeyList_k;
	vector<vector<OmnString> >   mSelectKeyList;
	vector<OmnString>   mSelectMeasureList;
	vector<OmnString>   mSelectTimeList;
	vector<OmnString>   mQueryValueList;
	vector<OmnString>   mQueryAggrList;
	vector<int>  		mCountKeyList_k;
	vector<vector<int> >  		mCountKeyList;
	vector<int>  		mSumKeyList_k;
	vector<vector<int> >  		mSumKeyList;
	vector<vector<int> >  		mRestCountKeyList;
	vector<int>  		mRestCountKeyList_k;
	vector<int>  		mMaxKeyList;
	vector<int>  		mMinKeyList;
	u64					mQueryTotalTime;
	int					mNumPagesQueried;
	int					mTimeNum;
	int					mKey_n;
	AosRundata*			mRundata;
	AosRundataPtr		mRundataPtr;
	OmnString			mStatFilename;
	OmnString			mStat;
	i64					mCrtQueryPos;
	u64					mDocidCycleSize;
	u64					mNumDocsInGroup;
	vector<AosQueryFieldPtr>	mFields;
	int					mSortingFieldIdx;
	u64					mStartDocid;
	u64					mEndDocid;
	int					mNumEqualConds;
	int					mSumValue;
	u64					mDocidRangeSize;
	u64					mSampleDocidStart;
	smap_t				mSampleDocids;
	vector<u64>			mMissedDocids;
	int					mTotalBlockLoadTime;
	int					mTotalNumQueried;
	int					mTotalFullQuery;
	int					mTotalBlocksTried;
	int 				mTotalExpected;
	int					mOtherTotalExpected;
	int 				mCountExpected;
	int 				mVerified;
	int					mMaxBlockLoadTime; 
	int					mMaxNumBlocksTried;
	int					mExpectMinValue;
	int					mExpectMaxValue;
	AosBitmapObjPtr		mExpectBitmap;
	AosBitmapObjPtr		mActualBitmap;
	AosBitmapObjPtr		mActualBitmap2;
	AosQueryFieldPtr	mSortingField;
	OmnString			mCrtValue;
	u64					mCrtU64Value;
	bool				mIsMovingTo;
	bool				mNumAlpha;
	bool				mReverse;
	int					mMoveToTimes;
	OmnString			mSQLStmt;
	OmnString			mDatabase;
	int					mDataType;
	OmnString			mTableName;
	int					mPageSize;
	OmnString			mSortFieldName;
	u64					mLastSortValue;
	
	static u64			smSampleDocidSize;
	static u32 			smRangeFreq;
	static u32 			smSeed;

	//define statistics torture members
	vector<vector<AosQueryFieldKey*> > 		mKeyList;
	vector<vector<AosQueryFieldMeasure*> > 	mMeasureList;
	vector<AosQueryFieldTime*> 		mTimeField;

	//query statistics members
	vector<vector<AosQueryFieldKey*> > 		mQueryKeyList;
	vector<vector<AosQueryFieldMeasure*> > 	mQueryMeasureList;
	vector<AosQueryFieldMeasure*> 	mAggrList;
	vector<AosQueryFieldTime*> 		mQueryTimeField; //"" means no timefield

public:
	AosQueryStatistics(AosSmartStatisticsTester *tester, 
					const u64 group_id, 
					const OmnString &table_name);
	~AosQueryStatistics();

	// QueryProcCallback interface
	virtual bool queryFailed(
					const AosRundataPtr &rdata,
					const OmnString &msg);

	virtual bool queryFinished(
					const AosRundataPtr &rdata,  
					const AosQueryRsltObjPtr &results,
					const AosQueryProcObjPtr &proc);

	bool setStrMinMax(const int field_idx, const u64 value);
	bool setU64MinMax(const int field_idx, const u64 value);
	bool createQuery();
	bool docQueried(const OmnString &value, const u64 docid);
	bool startQuery();
	OmnString getQueryStat();
	bool allDocsQueried();
	int determineNumConds(vector<int> &field_idxs);
	bool runQuery();
	u64 getGroupId() const {return mGroupId;}
	static void setShowLogLevel1(const bool b);
	static void setShowLogLevel2(const bool b);
	void setFields(vector<AosQueryFieldPtr> fields);
	void setPageSize(int pageSize);
	void setDatabase(OmnString db) { mDatabase = db; }
	void setDataType(int type) { mDataType = type; }

	bool verifyDataDocId(OmnString results);
	bool verifyDataCombination(OmnString results);
	bool verifyRecordCombination(const AosXmlTagPtr &rec);	
	OmnString createKeyList();	
	OmnString createTimeField();	
	OmnString createValueList();	

	bool createQueryKeyField();
	OmnString createQueryTimeField();	
	bool createQueryMeasureField();	

	virtual bool queryFinished(
			const AosRundataPtr &rdata,
			const AosQueryRsltObjPtr &results,
			const AosBitmapObjPtr &bitmap,
			const AosQueryProcObjPtr &proc) {return false;}  


	void setKeyList(vector<vector<AosQueryFieldKey*> > keyList) { mKeyList = keyList;}
	void setMeasureList(vector<vector<AosQueryFieldMeasure*> > measureList) { mMeasureList = measureList;}
	void setAggrList(vector<AosQueryFieldMeasure*> aggrList) { mAggrList = aggrList;}
	void setTimeField(vector<AosQueryFieldTime*> timeField) { mTimeField = timeField;}


private:
	bool createGroup();
	bool saveQueryStat();
	bool pageFinished(const OmnString &results);
	bool groupQueryFinished();
	bool groupQueryFailed();
	int  getDigitLen(const u64 value);
	int determineSortingIIL();
	void printStat(const AosQueryRsltObjPtr &results, const bool flag);
	void dumpQueryGroup();
	OmnString getFieldValues(const u64 docid);
	inline bool isSampleDocid(const u64 docid)
	{
		return (docid >= mSampleDocidStart && docid < mSampleDocidStart + smSampleDocidSize);
	}
	inline bool isDocInRange(const u64 docid)
	{
		return ((docid / mDocidRangeSize % smRangeFreq) == 0);
	}
	bool analyzeQuery();
	void dumpMissingDocids();
	bool verifyDocidsForOneField(
					const AosBitmapObjPtr &bitmap, 
					const AosQueryFieldPtr &field);
	bool verifyDocidsForQuery(
					const AosBitmapObjPtr &bitmap);
	bool verifyMultipleFields();
	bool verifySorting(AosXmlTagPtr rec_xml);
	bool verifyFirstPage(const OmnString &value, const u64 docid);
	bool verifyMoveTo(const OmnString &value, const u64 docid);
	bool verifyNormal(const OmnString &value, const u64 docid);

	bool isValid(const u64 &value);
	bool isValidDocid(const u64 &docid);

	bool before(const u64 &a, const u64 &b);
	bool checkOrder(const u64 pre, const u64 next);
	void next(u64 &v);

	bool checkMoveToRange(const u64 &real_distance,
									const u64 &crt_pos);
	bool moveToPageFinished(
			const AosQueryRsltObjPtr &results);
	bool chooseMoveToPos();
	bool runQueryPriv();
	OmnString createSelectExpr();
	int createInitPageSize();
	int createInitStartIdx();
	bool getTotalExpect();
	bool createCountKeyList();
	bool getTotalExpectCount();
	bool getOtherTotalExpect();
	bool getCountExpect();
	bool createSelectKeyField();
	bool createMinMaxValue();
	i64 getSumValue(map<OmnString, int> keyValueMap, OmnString measureField);
	i64 getCountValue(map<OmnString, int> keyValueMap, OmnString measureField);
	i64 getMaxValue(map<OmnString, int> keyValueMap, OmnString measureField);
	i64 getMinValue(map<OmnString, int> keyValueMap, OmnString measureField);
	i64 getDistinctValue(map<OmnString, int> keyValueMap, OmnString measureField);

	vector<vector<AosQueryFieldKey*> > 		getKeyList() { return mKeyList;}
	vector<vector<AosQueryFieldMeasure*> > 	getMeasureList() { return mMeasureList;}
	vector<AosQueryFieldMeasure*> 	getAggrList() { return mAggrList;}
	vector<AosQueryFieldTime*>		getTimeField() { return mTimeField;}
};
#endif

