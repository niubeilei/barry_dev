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
// 2013/09/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryTorturer_QueryField_h
#define Aos_QueryTorturer_QueryField_h

#include "QueryTorturer/Ptrs.h"
#include "QueryTorturer/QueryTesterUtil.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosSmartQueryTester;
class AosSmartStatisticsTester;

class AosQueryField : public OmnRCObject 
{
	enum
	{
		eMaxDigitLen = 10,
		eMaxAddContentBatch = 30000000
	};
	struct Entry
	{
		u64 value;// file id
		u64 docid;// file size
		Entry()
		:
		value(0),
		docid(0)
		{
		}

		Entry(const u64 v, const u64 d)
		:
		value(v),
		docid(d)
		{
		}
	};

	typedef hash_map<const OmnString, vector<Entry>, Omn_Str_hash, compare_str> emap_t;
	typedef hash_map<const OmnString, vector<Entry>, Omn_Str_hash, compare_str>::iterator eitr_t;

	typedef hash_map<const OmnString, Entry, Omn_Str_hash, compare_str> rmap_t;
	typedef hash_map<const OmnString, Entry, Omn_Str_hash, compare_str>::iterator ritr_t;

	typedef hash_map<const OmnString, OmnString, Omn_Str_hash, compare_str> lmap_t;
	typedef hash_map<const OmnString, OmnString, Omn_Str_hash, compare_str>::iterator litr_t;

public:
	struct FieldInfo
	{
		int			mFieldIdx;
		OmnString	mFieldName;
		u64			mMinValue;
		u64			mMaxValue;
		bool		mIsNumAlpha;
		u64			mStartDocid;

		bool serializeTo(const AosBuffPtr &buff);
		bool serializeFrom(const AosBuffPtr &buff);
	};

	//for time unit
	struct TimeInfo
	{
		OmnString	mFieldName;

		//bool serializeTo(const AosBuffPtr &buff);
		//bool serializeFrom(const AosBuffPtr &buff);
	};



	struct SwapEntry
	{
		int		from[3];
		int		to[3];

		SwapEntry()
		{
		}

		SwapEntry(const int from1, 
				const int to1, 
				const int from2, 
				const int to2, 
				const int from3, 
				const int to3)
		{
			from[0] = from1;
			from[1] = from2;
			from[2] = from3;
			to[0] = to1;
			to[1] = to2;
			to[2] = to3;
		}
	};

	struct Pattern
	{
		int 	right;
		u32		pattern;
		int		left;
		int		max;
	};

	enum
	{
		eMaxKnownDocid = 10000,
		eMaxParal = 5
	};

protected:
	OmnMutexPtr		mLock;

	//k1, k2, k3....
	int				mFieldIdx;
	OmnString		mFieldName;
	//key, time or value range
	u64				mMinValue;
	u64				mMaxValue;
	OmnString		mAggrFunc;

	u64 			mMinDocid;
	u64 			mMaxDocid;
	bool			mIsSortingField;
	bool			mIsCondField;
	bool			mIsSelectField;
	AosOpr			mOpr;
	bool			mIsNumAlpha;
	u64				mStartDocid;
	u64				mGroupSeedSize;
	vector<u32>		mGroupSeeds;
	vector<u64>		mKnownDocids;
	AosBitmapObjPtr	mQueriedBitmap;
	emap_t			mFileIdMap;
	rmap_t			mResultFileIdMap;
	bool			mIsTimeField;
	u64				mCondValue1;
	u64				mCondValue2;
	AosQueryTesterUtil::CondDef mCond;

	vector<Pattern>	mPatterns;
	
	static AosSmartQueryTester*				smTester1;
	static AosSmartStatisticsTester*		smTester;
	static vector<SwapEntry>				smFieldSwaps;

	//add members needed by statistics torturer
	OmnString	   mAggrFuncStr; //sum, count, max, min, dist_count, for measures
	OmnString	   mTimeUnit;   //day, hour, etc, for time field

public:
	AosQueryField(const int field_idx, const bool is_time_field);
	~AosQueryField();

	virtual bool setMinMax(const u64 docid) = 0;
	virtual bool setRandomMinMax() = 0;
	virtual OmnString getFieldIILName() = 0;
	virtual bool createCond(AosQueryTesterUtil::CondDef &cond) = 0;
	virtual bool createAnyCond(AosQueryTesterUtil::CondDef &cond) = 0;
	virtual bool isInRange(const u64 docid, u64 &value) {return false; }
	virtual bool isInRange(u64 &value) { return (value <= mMaxValue && value >= mMinValue); }
	virtual AosQueryFieldPtr clone() = 0;
	virtual bool initField(const FieldInfo &info) = 0;
	virtual FieldInfo getFieldInfo() = 0;
	virtual	void setFieldName(OmnString name) = 0;
	virtual bool serializeToBuff(const AosBuffPtr &buff);
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual u64 docid2U64Value(const u64 docid) const;
	virtual OmnString toString() const;
	virtual u64 getMinDocid() const{return mMinDocid;}
	virtual u64 getMaxDocid() const{return mMaxDocid;}
	virtual OmnString getMinValue() const = 0;
	virtual OmnString getMaxValue() const = 0;
	virtual OmnString getFieldname() const = 0;
	bool isNumAlpha() const {return mIsNumAlpha;}
	void setIsNumAlpha(bool isNumAlpha) {mIsNumAlpha = isNumAlpha;}
	static void setTester(AosSmartQueryTester *tester);
	static void setTester(AosSmartStatisticsTester *tester);

	int getNumDigits(const u64 value);
	int getFieldIdx() const {return mFieldIdx;}
	//void setFieldName(OmnString name) { mFieldName = name;}
	void setSortingField(const bool b) {mIsSortingField = b;}
	bool isSortingField() const {return mIsSortingField;}
	void setCondField(const bool b) {mIsCondField = b;}
	bool isCondField() const {return mIsCondField;}
	bool isSelectField() const {return mIsSelectField;}
	void setSelectField(const bool b) {mIsSelectField = b;}
	AosOpr getOpr() const {return mOpr;}
	void setQueriedBitmap(const AosBitmapObjPtr &b) {mQueriedBitmap = b;}
	AosBitmapObjPtr getQueriedBitmap() {return mQueriedBitmap;}
	void setTimeField(const bool b) {mIsTimeField = b;}
	void setPattern(const vector<Pattern> &pattern) {mPatterns = pattern;}

	void initMinMax()
	{
		mMinValue = 99999;
		mMaxValue = 0;
	}

	static AosQueryFieldPtr serializeFromBuffStatic(const AosBuffPtr &buff);

	u64 getMinU64Value() const{return mMinValue;}
	u64 getMaxU64Value() const{return mMaxValue;}
	void setMinU64Value(u64 val) { mMinValue = val;}
	void setMaxU64Value(u64 val) { mMaxValue = val;}
	void setAggrFunc(OmnString aggrfunc) {mAggrFunc = aggrfunc;}
	
protected:
	bool init();
	static bool initStatic();
	static bool createFieldSwappers();
	OmnString getStrIILName(const int field_idx);
	OmnString getU64IILName(const int field_idx);
	bool setU64MinMax(const int field_idx, const u64 value);
	bool isInRange(const int field_idx, const bool str_field, const u64 value);
	u64 getValueByHash(const u64 docid) const;
	u64 getFieldValueBySwapping(const u64 docid) const;
	int docid2EpochDay(const u64 docid);

public:
	// Chen Ding, 2013/10/26
	bool addFileId(const OmnString &iilname, const u64 fileid, const u64 size);
	bool setResultFile(
					const OmnString &iilname, 
					const u64 fileid, 
					const u64 filesize);

	bool		isDocValid(const u64 &docid)const;

	OmnString getNextValue(const OmnString &crt_value, u64 &docid) const;
	OmnString convertFieldValue(const u64 docid) const;
	u64 value2Docid(const u64 value) const;

private:
	// Chen Ding, 2014/02/07
	bool pickGEValue(AosQueryTesterUtil::CondDef &cond);
	bool pickGTValue(AosQueryTesterUtil::CondDef &cond);
	bool pickLTValue(AosQueryTesterUtil::CondDef &cond);
	bool pickLEValue(AosQueryTesterUtil::CondDef &cond);
	bool pickEQValue(AosQueryTesterUtil::CondDef &cond);
	bool pickNEValue(AosQueryTesterUtil::CondDef &cond);
	bool pickANValue(AosQueryTesterUtil::CondDef &cond);
	bool pickR1Value(AosQueryTesterUtil::CondDef &cond);
	bool pickR2Value(AosQueryTesterUtil::CondDef &cond);
	bool pickR3Value(AosQueryTesterUtil::CondDef &cond);
	bool pickR4Value(AosQueryTesterUtil::CondDef &cond);

	u64 pickValue();
	OmnString getFieldValueStr(const u64 value) const;
	u64 getNextValue(const u64 value) const;
	u64 getFieldValueByBitPattern(const u64 docid) const;
	u64 strValue2U64Value(const OmnString &value) const;
	OmnString u64Value2StrValue(const u64 value) const;
	
	bool	convertIILName(
				const AosRundataPtr &rdata,
				const int epoch_day,
				const OmnString &iilname,
				bool  &converted,
				OmnString &new_iilname);
};
#endif

