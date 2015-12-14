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
// 07/06/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryContextObj_h
#define Aos_SEInterfaces_QueryContextObj_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "IILUtil/IILUtil.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryDistrMapObj.h"
#include "SEInterfaces/QueryFilterObj.h"
#include "QueryUtil/Ptrs.h"
#include "Util/Opr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosIILIdx;

class AosQueryContextObj : virtual public OmnRCObject
{
public:
	enum GroupFilterType
	{
	 	eInvalidGroupFilterType = 0,
	 
		eGroupFilterKey = 1,
		eGroupFilterFirstStrKey = 2,
		eGroupFilterSecondStrKey = 3,
		eGroupFilterFirstU64Key = 4,
		eGroupFilterSecondU64Key = 5,
	 
		eMaxGroupFilterType,
	};

	enum QueryKeyGroupingType
	{
		eFullKey = 1,
		eFirstKey = 2
	};

	enum QueryValueGroupingType
	{
		eFirstEntry = 1,
		eLastEntry = 2,
		eMaxEntry = 3,
		eMinEntry = 4,
		eSum = 5,
		eAvg = 6
	};

	enum SplitValueType
	{
		eKey = 0,
		eKeyValue = 1,
		eFirstField = 2,
		eFirstSecondField = 3
	};

protected:
	bool					mReverse;
	i64						mIndex;
	i64						mIILIndex;
	i64						mPageSize;
	i64						mBlockSize;
	AosOpr					mOpr;
	OmnString				mStrValue;	
//	u64						mU64Value;
	OmnString				mStrValue2;	
//	u64						mU64Value2;
	u64						mCrtDocid;
	OmnString				mCrtValue;
//	u64						mCrtU64Value;
	bool					mFinished;
	bool					mCheckRslt;
	bool					mMoveOnly;
	i64						mNumDocChecked;
	i64						mTotalDocInIIL;
	i64						mTotalDocInRslt;
	
	// This num is only used by bitmapQuery, and it doesn't need to send back to server
	i64						mNumInRslt;				
	
	bool					mGrpFirstEntry;
	OmnString				mGrpCmpKey;
	OmnString				mGrpCurKey;
	u64						mGrpCmpU64Key;
	u64						mGrpCurU64Key;
	u64						mGrpCrtValue;
	u64						mGrpCrtNum;
	OmnString				mSaperator;
	bool					mGrouping;	
	QueryKeyGroupingType	mKeyGroupingType;
	QueryValueGroupingType	mValueGroupingType;
	SplitValueType			mSplitValueType;
	OmnString				mSplitValueStr;

	OmnString               mParalIILName;
	OmnString               mLimitStrValue;
	u64						mLimitU64Value;
	u64                     mLimitDocid;
	bool                    mDetect;
	bool					mIsAlphaNum;
	bool                    mEstimate;
	u64						mQueryCursor;
	AosQueryDistrMapObjPtr	mDistrMap;
	AosIILType 				mIILType;
//	u64						mPageStart;
public:
	static AosQueryContextObjPtr		smQueryContextObj;
	
	void		setIILType(const AosIILType &type){mIILType = type;} 
	AosIILType	getIILType()const{return mIILType;}

	void		setIILIndex(const i64 &iilidx){mIILIndex = iilidx;}
	void		setIndex(const i64 &idx){mIndex = idx;}
	void 		setReverse(const bool reverse){mReverse = reverse;}
	void 		setFinished(const bool finished){mFinished = finished;}
	void 		setOpr(const AosOpr opr){mOpr = opr;}
//	void		setU64Value(const u64 &value){mU64Value = value;}
	void		setStrValue(const OmnString &value){mStrValue = value;}
//	void		setU64Value2(const u64 &value2){mU64Value2 = value2;}
	void		setStrValue2(const OmnString &value2){mStrValue2 = value2;}
	void 		setPageSize(const i64 &p) {mPageSize = p;}
	void 		setBlockSize(const i64 &b) {mBlockSize = b;}
	void 		setCrtDocid(const u64 &docid){mCrtDocid = docid;}
	void		setCrtValue(const OmnString &value){mCrtValue = value;}
//	void		setCrtU64Value(const u64 &value){mCrtU64Value = value;}
	void		setGrouping(const bool grouping){mGrouping = grouping;}
	void 		setKeyGroupingType(const QueryKeyGroupingType t){mKeyGroupingType = t;}
	void		setValueGroupingType(const QueryValueGroupingType t){mValueGroupingType = t;}
	void		setSplitValueType(const SplitValueType t){mSplitValueType = t;}
	void		setSplitValueStr(const OmnString &s){mSplitValueStr = s;}
//	void		setPageStart(const u64 &s){mPageStart = s;}
	bool		isGrouping()const{return mGrouping;}

	void		setNumDocChecked(const i64 &num){mNumDocChecked = num;}
	void		incNumDocChecked(const i64 &num){mNumDocChecked += num;}
	void		setTotalDocInIIL(const i64 &total){mTotalDocInIIL = total;}
	void		setTotalDocInRslt(const i64 &total){mTotalDocInRslt = total;}
	void		setNumInRslt(const i64 &num_rslt){mNumInRslt = num_rslt;}

	void		setCheckRslt(const bool checkrslt){mCheckRslt = checkrslt;}
	void		setMoveOnly(const bool moveonly){mMoveOnly = moveonly;}
	bool		isReverse()const{return mReverse;}
	bool 		finished()const{return mFinished;}
	AosOpr		getOpr()const{return mOpr;}
	bool 		isCheckRslt()const{return mCheckRslt;}
	bool 		isMoveOnly()const{return mMoveOnly;}
//	u64			getU64Value()const{return mU64Value;}
	OmnString	getStrValue()const{return mStrValue;}
//	u64			getU64Value2()const{return mU64Value2;}
	OmnString	getStrValue2()const{return mStrValue2;}
	i64 		getPageSize()const{return mPageSize;}
	i64			getIILIndex()const{return mIILIndex;}
	i64			getIndex()const{return mIndex;}
	OmnString 	getCrtValue()const{return mCrtValue;}
//	u64 		getCrtU64Value()const{return mCrtU64Value;}
	i64			getNumDocChecked()const{return mNumDocChecked;}
	i64			getTotalDocInIIL()const{return mTotalDocInIIL;}
	i64			getTotalDocInRslt()const{return mTotalDocInRslt;}
	i64			getNumInRslt()const{return mNumInRslt;}

	u64			getCrtDocid()const{return mCrtDocid;}
	SplitValueType	getSplitValueType()const{return mSplitValueType;}
	OmnString		getSplitValueStr()const{return mSplitValueStr;}
	void 		clean(){clear();}
	OmnString	getParalIILName()const{return mParalIILName;}
	OmnString	getLimitStrValue()const{return mLimitStrValue;}
	u64			getLimitU64Value()const{return mLimitU64Value;}
	i64			getLimitI64Value()const{return (i64)mLimitU64Value;}
	d64			getLimitD64Value()const{return (d64)mLimitU64Value;}
	u64			getLimitDocid()const{return mLimitDocid;} 
	bool		isDetect()const{return mDetect;}
//	u64			getPageStart()const{return mPageStart;} 
	bool		isAlphaNum()const{return mIsAlphaNum;}
	bool		isEstimate()const{return mEstimate;}
	u64			getQueryCursor()const{return mQueryCursor;}
	AosQueryDistrMapObjPtr	getDistrMap()const{return mDistrMap;}

	void		setParalIILName(const OmnString &name){mParalIILName = name;}
	void		setLimitStrValue(const OmnString &value){mLimitStrValue = value;}
	void		setLimitU64Value(const u64 &value){mLimitU64Value = value;}
	void		setLimitI64Value(const i64 &value){mLimitU64Value = (u64)value;}
	void		setLimitD64Value(const d64 &value){mLimitU64Value = (u64)value;}
	void		setLimitDocid(const u64 &did){mLimitDocid = did;}
	void		setDetect(const bool &flag){mDetect = flag;}
	void		setAlphaNum(const bool &flag){mIsAlphaNum = flag;}

	void		setEstimate(const bool &flag){mEstimate = flag;}
	void		setQueryCursor(const u64 &cursor){mQueryCursor= cursor;}
	void		setDistrMap(vector<OmnString> &values,  
								vector<u64> &docids,
								vector<u64> &num_entries);
	void		setDistrMap(vector<u64> &values,  
								vector<u64> &docids,
								vector<u64> &num_entries);
	void		setDistrMap(vector<i64> &values,  
								vector<u64> &docids,
								vector<u64> &num_entries);
	void		setDistrMap(vector<d64> &values,  
								vector<u64> &docids,
								vector<u64> &num_entries);
	void		setDistrMap(const AosQueryDistrMapObjPtr &distrmap){mDistrMap = distrmap;}

	virtual void setFieldFilterObj(const AosFieldFilterPtr &b) = 0;
	virtual bool isFieldFilterValid() = 0; 
	virtual bool filterFieldCond(const OmnString &key) = 0;
	virtual u32	 getNumMultiCond() = 0;

	virtual bool retrieveMultiCond(const u32 &idx) = 0;
	virtual bool updateMultiCond(const u32 &idx) = 0;
	virtual void addMultiCond(
					const AosOpr &opr,
					const OmnString &strvalue,
					const u64 &u64value,
					const OmnString &strvalue2,
					const u64 &u64value2) = 0;

	virtual AosQueryContextObjPtr createQueryContext() = 0;
	virtual void returnQueryContext(const AosQueryContextObjPtr &context) = 0;
	
	virtual i64	getBlockSize() = 0; 
	virtual bool copyFrom(const AosQueryRsltObjPtr &query_rslt) = 0;
	virtual bool copyTo(const AosQueryRsltObjPtr &query_rslt) = 0;
	virtual bool isFull(const u64 &num) const = 0;
	virtual void resetIILIndex2() = 0;
	virtual AosIILIdx getIILIndex2()const = 0;
	
	virtual void appendGroupEntryStrStart(const AosQueryRsltObjPtr &queryRslt) = 0;
	virtual void appendGroupEntryStrFinish(const AosQueryRsltObjPtr &queryRslt) = 0;
	virtual void appendGroupEntryU64Start(const AosQueryRsltObjPtr &queryRslt) = 0;
	virtual void appendGroupEntryU64Finish(const AosQueryRsltObjPtr &queryRslt) = 0;
	virtual void appendGroupEntryI64Start(const AosQueryRsltObjPtr &queryRslt) = 0;
	virtual void appendGroupEntryI64Finish(const AosQueryRsltObjPtr &queryRslt) = 0;
	virtual void appendGroupEntryD64Start(const AosQueryRsltObjPtr &queryRslt) = 0;
	virtual void appendGroupEntryD64Finish(const AosQueryRsltObjPtr &queryRslt) = 0;
	
	virtual void appendGroupEntry(
					const u64 &key, 
					const u64 &value, 
					const AosIILIdx &curIdx,
					const AosQueryRsltObjPtr &queryRslt) = 0;

	virtual void appendGroupEntry(
					const OmnString &key, 
					const u64 &value, 
					const AosIILIdx &curIdx,
					const AosQueryRsltObjPtr &queryRslt, 
					const bool num_alpha) = 0;
	
	virtual void clear() = 0;
	virtual bool serializeToXml(AosXmlTagPtr &xml, const AosRundataPtr &rdata) = 0;
	virtual bool serializeFromXml(const AosXmlTagPtr &xml, const AosRundataPtr &rdata) = 0;
	virtual void setSaperator(const OmnString &saperator) = 0;
	virtual void setIILIndex2(const AosIILIdx &iilidx) = 0;
	virtual void setIILIndex2(const int level, const i64 &idx) = 0;
	virtual AosQueryContextObjPtr clone() = 0;

	// This function only copy the infomation which set by IIL back to the original context.
	virtual bool setBackward(const AosQueryContextObjPtr &new_context); 
	virtual void addGroupFilter(
					const GroupFilterType type,
					const AosOpr opr, 
					const OmnString &value1,
					const OmnString &value2,
					const bool createDefault) = 0; 
	
	virtual void addGroupFilter(
					const GroupFilterType type,
					const AosOpr opr, 
					const u64 &value1,
					const u64 &value2,
					const bool createDefault) = 0;

	bool copyMemberData(AosQueryContextObj * const) const;
	static AosQueryContextObjPtr createQueryContextStatic()
	{
		if (!smQueryContextObj)
		{
			OmnAlarm << "QueryContextObj not set yet. Please create a QueryContext"
				<< " object in main() and call AosQueryContextObj::setQueryContextObj(...)"
				<< enderr;
			return 0;
		}
		return smQueryContextObj->createQueryContext();
	}

	static void returnQueryContextStatic(const AosQueryContextObjPtr &context)
	{
		if (!smQueryContextObj)
		{
			OmnAlarm << "QueryContextObj not set yet. Please create a QueryContext"
				<< " object in main() and call AosQueryContextObj::setQueryContextObj(...)"
				<< enderr;
			return;
		}
		smQueryContextObj->returnQueryContext(context);
	}

protected:
	bool copyMemberData();

public:
	static void setQueryContextObj(const AosQueryContextObjPtr &d) {smQueryContextObj = d;}
	static AosQueryContextObjPtr getQueryContext() {return smQueryContextObj;}

	static OmnString 				GroupFilterType_toString(const GroupFilterType type);
	static GroupFilterType 			GroupFilterType_toEnum(const OmnString &type);
	static QueryKeyGroupingType 	QueryKeyGroupingType_toEnum(const OmnString &type);
	static OmnString 				QueryKeyGroupingType_toString(const QueryKeyGroupingType type);
	static QueryValueGroupingType	QueryValueGroupingType_toEnum(const OmnString &type);
	static OmnString 				QueryValueGroupingType_toString(const QueryValueGroupingType type);
	static SplitValueType			SplitValueType_toEnum(const OmnString &type);
	static OmnString 				SplitValueType_toString(const SplitValueType type);

	// Chen Ding, 2013/02/15
	virtual bool	serializeToStr(OmnString &str, const AosRundataPtr &rdata) = 0;

	// Ken Lee, 2013/04/03
	virtual bool	merge(const AosQueryContextObjPtr &obj);
	virtual void	increaseIndex(const int index) {mIndex += index;}

	// Ken Lee, 2014/08/19
	virtual bool	hasFilter() = 0;
	virtual void	addFilter(const AosQueryFilterObjPtr &filter) = 0;
	virtual void	clearFilter() = 0;
	virtual bool	evalFilter(
						const OmnString &key,
						const u64 &value,
						const AosRundataPtr &rdata) const = 0;
	virtual bool	evalFilter(
						const u64 &key,
						const u64 &value,
						const AosRundataPtr &rdata) const = 0;
};
#endif
