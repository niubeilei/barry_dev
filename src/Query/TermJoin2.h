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
// 2013/12/04 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermJoin2_h
#define Aos_Query_TermJoin2_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "CounterUtil/Ptrs.h"
#include "CounterUtil/StatTypes.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"

class AosTermJoin2 : public AosTermIILType
{
public:
	enum Opr
	{
		eDftPsize = 20
	};

private:
	bool				mDataLoaded;
	AosQueryTermObjPtr	mLTerm;
	AosQueryTermObjPtr	mRTerm;
	bool				mLIsStat;
	bool				mRIsStat;
	i64					mLStatIdx;
	i64					mRStatIdx;
	OmnString			mLStatKey;
	OmnString			mRStatKey;
	OmnString			mLStatValue;
	OmnString			mRStatValue;

	bool				mIsSameOrder;

	bool				mNeedSpecificTotal;
	i64					mPsize;
	i64					mStartIdx;
	i64					mDocsNum;
	i64					mCrtIdx;
	i64					mCrtLIdx;
	i64					mCrtRIdx;
	vector<OmnString>	mDocs;

public:
	AosTermJoin2(const bool regflag);
	AosTermJoin2(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermJoin2();

	virtual bool	nextDocid(
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

	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const AosRundataPtr &rdata);

	virtual AosQueryTermObjPtr clone(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

	bool			loadData(const AosRundataPtr &rdata);


private:
	bool 			parse(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);
	AosQueryTermObjPtr createQueryTerm(
						const AosXmlTagPtr &conds, 
						const AosRundataPtr &rdata);
	AosXmlTagPtr	getNextDoc(
						const AosQueryTermObjPtr &term,
						bool &finished,
						const AosRundataPtr &rdata);
	bool			loadData2(const AosRundataPtr &rdata);

};
#endif

