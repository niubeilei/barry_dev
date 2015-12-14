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
// 2013/08/12  Andy Zhang     
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermJoin_h
#define Aos_Query_TermJoin_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "CounterUtil/Ptrs.h"
#include "CounterUtil/StatTypes.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"

class AosTermJoin : public AosTermIILType
{
public:
	enum Opr
	{
		eDftPsize = 20,
		eMapContentMax = 1000 * 1000 * 100
	};

private:
	Opr											mOpr;
	bool                                        mDataLoaded;
	i64                                         mDocMapSize;
	AosQueryTermObjPtr							mOrTerm;
	vector<AosQueryTermObjPtr>					mOrTerms;
	map<OmnString, AosXmlTagPtr> 				mDocsMap;       
	map<OmnString, bool> 						mIsJoined;
	vector<OmnString>							mKeyVec;
	i64               							mPsize;
	i64                 						mStartIdx;
	i64                 						mDocsNum;
	i64											mCrtIdx; 
	OmnString                                   mSep;
	OmnString									mJoinKey;
	vector<OmnString>							mJoinValues;


	bool                                        mReverseOrder;
	OmnString                                   mOrderContainer;
	OmnString                                   mOrderFname;

public:
	AosTermJoin(const bool regflag);
	AosTermJoin(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermJoin();

	virtual bool 	nextDocid(
					const AosQueryTermObjPtr &parent_term,
					u64 &docid, 
					bool &finished, 
					const AosRundataPtr &rdata);
	virtual bool	checkDocid(const u64 &docid, const AosRundataPtr &rdata);

	virtual bool 	getDocidsFromIIL(
					const AosQueryRsltObjPtr &query_rslt,
					const AosBitmapObjPtr &query_bitmap, 
					const AosQueryContextObjPtr &query_context, 
					const AosRundataPtr &rdata);

	virtual i64		getTotal(const AosRundataPtr &rdata);

	virtual void	reset(const AosRundataPtr &rdata);

	virtual bool 	moveTo(
					const i64 &pos, 
					const AosRundataPtr &rdata);

	virtual void	toString(OmnString &str);

	virtual AosXmlTagPtr    getDoc(
					const u64 &docid,
					const AosRundataPtr &rdata);

	virtual AosQueryTermObjPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	bool loadData(const AosRundataPtr &rdata);


private:
	bool 			parse(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	AosXmlTagPtr	createRecord(
					i64 &index, 
					const AosRundataPtr &rdata);

	AosXmlTagPtr	processRecordByOneDime(
					i64 &index, 
					const AosRundataPtr &rdata);

	AosXmlTagPtr 	processRecordByTwoDime(
					i64 &index, 
					const AosRundataPtr &rdata);

	bool 			processDataByOneDime(const AosRundataPtr &rdata);

	bool 			processDataByTwoDime(const AosRundataPtr &rdata);

	bool 			getCounterNames(
					const u16 &entry_type, 
					const OmnString &str_header, 
					const AosRundataPtr &rdata);

	bool 			loadTimedData(const AosRundataPtr &rdata);

	bool 			findColumnKeys(
					map<OmnString, i64> &column, 
					i64 &num_row_key, 
					const AosRundataPtr &rdata);

	bool 			retrieveCounterIds(const AosXmlTagPtr &term);

	bool			retrieveAggregateConf(
					const AosXmlTagPtr &term,  
					i64 &fieldindex);

	bool 			retrieveTwoDimeConf(
					const AosXmlTagPtr &term, 
					const i64 &fieldindex);

	bool 			retrieveCounterTimeConf(
					const AosXmlTagPtr &term,  
					const AosRundataPtr &rdata);

	void 			retrieveBuffValue(
					OmnString &cname, 
					i64 &value, 
					const bool &timeorder,	
					const AosRundataPtr &rdata);

	bool			processStatistics(
					const AosQueryTermObjPtr &cond, 
					const AosXmlTagPtr &stat_tag,
	   				const AosRundataPtr &rdata);

 	vector<AosXmlTagPtr> doJoin(
			        const vector<AosXmlTagPtr> leftVec,
					const vector<AosXmlTagPtr> rightVec,
					const AosRundataPtr &rdata); 

	bool setDocsNum();
};
#endif

