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
// 02/27/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermCounterAll_h
#define Aos_Query_TermCounterAll_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "CounterUtil/Ptrs.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/Ptrs.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"


class AosTermCounterAll : public AosTermIILType
{
public:
	enum
	{
		eMaxNumCounterId = 50 
	};

private:
    bool                	mDataLoaded;
	i64						mCrtIdx;
	i64						mNumValues;
	AosCounterTimeInfoPtr	mTime;
	vector<AosStatType::E>	mStatTypes;
	vector<OmnString>		mKeys;
	vector<OmnString>		mCounterIds;
	double 					mFactor;
	bool					mUseIILName;
	AosRecordFormatPtr		mRecordFormat;
	AosCounterParameterPtr	mParm;

public:
	AosTermCounterAll(const bool regflag);
	AosTermCounterAll(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermCounterAll();

	virtual bool nextDocid(
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
	virtual bool 	moveTo(const i64 &pos, const AosRundataPtr &rdata);
	virtual void	toString(OmnString &str);
	virtual AosXmlTagPtr    getDoc(const u64 &docid, const AosRundataPtr &rdata);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool loadData(const AosRundataPtr &rdata);


private:
	bool parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosXmlTagPtr createRecord(i64 &index, const AosRundataPtr &rdata);

	bool parseCond(const AosXmlTagPtr &term, const AosRundataPtr &rdata);

	bool parseMiddleResult(const AosXmlTagPtr &term, const AosRundataPtr &rdata);
	bool parseResult(const AosXmlTagPtr &term, const AosRundataPtr &rdata);

	bool retrieveDataTypeConf(const AosXmlTagPtr &term, const AosRundataPtr &rdata);
	bool retrieveFillingConf(const AosXmlTagPtr &term, const AosRundataPtr &rdata);
	bool retrieveRecordFormatConf(const AosXmlTagPtr &term, const AosRundataPtr &rdata);

	bool getCounterNames(const OmnString &cname, const u16 &entry_type, const OmnString &str_header, const AosRundataPtr &rdata);

	bool retrieveCounterIds(const AosXmlTagPtr &term);

	bool retrieveAggregateConf(const AosXmlTagPtr &term,  const AosRundataPtr &rdata);
	bool retrieveSortConf(const AosXmlTagPtr &term,  const AosRundataPtr &rdata);

};
#endif

