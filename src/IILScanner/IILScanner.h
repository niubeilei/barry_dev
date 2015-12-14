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
// 05/10/2012 Created by Chen Ding
// 2012/11/12 Moved Form IILUtil By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILScanner_IILScanner_h
#define Aos_IILScanner_IILScanner_h

#include "IILUtil/IILEntrySelType.h"
#include "SEUtil/IILIdx.h"
#include "IILScanner/IILMatchType.h"
#include "IILScanner/IILScanValue.h"
#include "IILScanner/IILValueType.h"
#include "IILScanner/Ptrs.h"
#include "QueryUtil/QueryContext.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEInterfaces/DataProcStatus.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Thread/ThreadedObj.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
using namespace std;

class AosIILScanner : virtual public OmnThreadedObj,
					  virtual public AosIILScannerObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxComposorStrLen = 10000,
		eDftPagesize = 10000,
		eDftIgnoreMatchErrorMaxNum = 10000000
	};

	struct ValueCond
	{
		OmnString	mValueType;
		OmnString	mValue1;
		OmnString	mValue2;
		AosOpr		mOpr;
		
		bool		mIsGroup;
		AosQueryContext::QueryKeyGroupingType	mKeyGroupType;
		AosQueryContext::QueryValueGroupingType	mValueGroupType;

		bool		mNeedFilter;
		AosQueryContextObj::GroupFilterType	mFilterType;
		OmnString	mFilterValueType;
		// Chen Ding, 2014/02/26
		OmnString	mFilterValue1;
		OmnString	mFilterValue2;
		AosOpr		mFilterOpr;

		ValueCond()
		{
			mValueType = "str";
			mValue1 = "*";
			mOpr = eAosOpr_an;
			mIsGroup = false;
			mNeedFilter = false;
		}
	};

	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	OmnThreadPtr				mThread;
	AosRundataPtr				mRundata;
	
	AosIILScannerListenerPtr	mCaller;
	OmnString					mIILName;				
	int							mSeqno;
	
	AosDataProcStatus::E 		mStatus;
	bool						mNoMoreData;
	
	bool						mCouldSetQueryContext;
	AosQueryContextObjPtr		mQueryContext;
	ValueCond					mValueCond;				
	int							mPagesize;				
	
	AosIILMatchType::E			mMatchType;			
	int							mMatchedFieldIdx;
	OmnString					mFieldSep;				
	int							mStartPos;
	int							mEndPos;
	
	int							mProgress;
	u64							mTotalRecordNum;
	u64							mProcRecordNum;
	
	bool						mIsControllingScanner;
	bool						mMatchReject;
	bool						mNoMatchReject;
	bool						mIgnoreMatchError;				
	u32							mIgnoreMatchErrorMaxNum;
	u32							mIgnoreMatchErrorNum;
	vector<AosIILSelector>		mSelectors;				
	
	AosQueryRsltObjPtr			mCrtData;
	AosQueryRsltObjPtr			mNextData;
	u64							mNumDocs;
	vector<OmnString>			mCrtKeys;
	vector<u64>					mCrtValues;
	OmnString					mCrtValueStr;
	int							mCrtIdx;
	int							mCrtEndIdx;	
	
	AosIILScanValuePtr			mScanValue;
	vector<OmnString>			mCrtMatchKeys;
	vector<u64>					mCrtMatchValues;
	int							mCrtMatchIdx;

public:
	AosIILScanner();
	AosIILScanner(
			const AosIILScannerListenerPtr &caller, 
			const int seqno,
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);
	~AosIILScanner();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	virtual AosIILMatchType::E getMatchType() const {return mMatchType;}
	virtual bool isControllingScanner() const{return mIsControllingScanner;}
	virtual void setListener(const AosIILScannerListenerPtr &listener);
	virtual OmnString getIILName()const {return mIILName;}
	virtual bool isReject(const bool is_match) const;
	virtual u64 getCrtValue() const;
	virtual int getProgress();
	virtual bool start(const AosRundataPtr &rdata);
	
	int		getSeqno() const {return mSeqno;}
	bool	isFinished() const;

	virtual AosDataProcStatus::E moveNext(
						const char *&value, 
						int &len,
						const int total_matched,
						const AosRundataPtr &rdata);
	virtual AosDataProcStatus::E moveNext(
						OmnString &data,
						bool &has_more,
						const AosRundataPtr &rdata);
	virtual u32 getMatchedSize();
	virtual void resetMatchedIdx();
	virtual bool setMatchedValue(
						const AosDataRecordObjPtr &record, 
						bool &has_more,
						const AosRundataPtr &rdata);
	virtual bool setNotMatchedValue(
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);

	virtual AosQueryContextObjPtr getQueryContext() const;
	virtual bool setQueryContext(
						const AosQueryContextObjPtr &context,
						const AosRundataPtr &rdata);

	virtual bool reloadData(
						const AosQueryContextObjPtr &context,
						const AosRundataPtr &rdata); 

	virtual void clear();

	virtual AosIILScannerObjPtr createIILScanner(
						const AosIILScannerListenerPtr &caller,
						const int seqno,
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	bool			parseValCond(
						const AosXmlTagPtr &val_cond,
						const AosRundataPtr &rdata);
	bool			retrieveData();
	bool			getCrtEndIdx(const AosRundataPtr &rdata);
	AosDataProcStatus::E setupNewData(const AosRundataPtr &rdata);

};
#endif
