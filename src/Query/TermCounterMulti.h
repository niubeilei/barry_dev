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
// Modification History:
// 06/22/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermCounterMulti_h
#define Aos_Query_TermCounterMulti_h

#include "CounterUtil/StatTypes.h"
#include "Query/QueryTerm.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Opr.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"
#include "Rundata/Ptrs.h"

class AosTermCounterMulti : public AosQueryTerm
{
public:
//	enum CounterType
//	{
//		eInvalid,
//
//		eNormal,
//		eYearly,
//		eMonthly,
//		eDaily,
//
//		eMax
//	};

private:
	i64								mStartTime;
	i64								mEndTime;
	AosTimeGran::E          		mTimeGran;
	AosTime::TimeFormat     		mTimeFormat;

	i64					 		 	mNumValues;
	AosRundataPtr 					mRdata;
	AosXmlTagPtr					mCNameTmp;
	AosQueryTermObjPtr				mOrTerm;
	bool							mDataLoaded;
	i64								mCrtIdx;
	vector<OmnString>				mCnames;
	vector<AosStatType::E>  		mStatTypes;
	OmnString 						mCounterId;
	AosBuffPtr              		mBuff;
	i64								mDftValue;
	i64                 			mAccumulates[AosStatType::eMaxEntry];
	double							mFactor;
	bool                    		mAverage;

public:
	AosTermCounterMulti(const bool regflag);
	AosTermCounterMulti(const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

	AosTermCounterMulti(const AosXmlTagPtr &def, 
						const AosQueryTermObjPtr &orTerm,
						const AosRundataPtr &rdata);

	~AosTermCounterMulti();

	virtual bool nextDocid(
					const AosQueryTermObjPtr &parent_term,
					u64 &docid, 
					bool &finished, 
					const AosRundataPtr &rdata);
	virtual i64		getTotal(const AosRundataPtr &rdata);
	virtual bool	checkDocid(const u64 &docid, const AosRundataPtr &rdata);
	virtual void	reset(const AosRundataPtr &rdata);
	virtual bool 	moveTo(const i64 &startidx, const AosRundataPtr &rdata);
	virtual void	toString(OmnString &str);
	virtual bool    setOrder(
						const OmnString &container, 
						const OmnString &name, 
						const bool reverse, 
						const AosRundataPtr &);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool 	getRsltSizeFromIIL(const AosRundataPtr &rdata)
					{
						OmnShouldNeverComeHere;
						return false;
					}
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
					{
						OmnShouldNeverComeHere;
						aos_assert_r(query_context,false);
						{
							query_context->setFinished(true);
						}
						return false;
					}
	virtual AosXmlTagPtr	getDoc(const u64 &docid, const AosRundataPtr &rdata);
	virtual bool    collectInfo(const AosRundataPtr &rdata);

	bool loadData(const AosRundataPtr &rdata);

	//OmnString createValue(bool &finished);
	bool parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata);

	// Chen Ding, 2014/01/29
	virtual bool getTimeCond(
						const AosRundataPtr &rdata, 
						const OmnString &time_fname, 
						bool &match,
						i32 &start_day, 
						i32 &end_day)
	{
		match = false;
		return true;
	}

private:
	//OmnString	getCounterCname(const AosXmlTagPtr &cnametmp);
	bool		getCounterNames(const AosRundataPtr &rdata);
	//bool createRecord(const i64 &numCnames);
	AosXmlTagPtr createRecord(const i64 &index, const AosRundataPtr &rdata);

	bool loadTimedData(const AosRundataPtr &rdata);
	AosXmlTagPtr createTimedRecord(const i64 &index, const AosRundataPtr &rdata); 

	double calculateAverages(const vector<i64> *values, const i64 &index);

	double calculateAverages(const i64 values[AosStatType::eMaxEntry]);

};
#endif

