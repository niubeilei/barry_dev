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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermAnd_h
#define Aos_Query_TermAnd_h

#include "Query/QueryTerm.h"
#include "SEUtil/Ptrs.h"


class AosTermAnd : public AosQueryTerm
{
private:
	enum
	{
		eMaxTerms = 30,
		eDftPsize = 20,
//		eMaxPsize = 10000,
		eDftBsizeSingle = 100,
		eDftBsize = 100,			// Default number of entries per block
		eMaxBsize = 20000000,
//		eDftEsize = 1000,
		eMaxCheckedTotal = 5000,
		eMaxWords = 40
	};

	AosQueryTermObjPtr		mTerms[eMaxTerms];
	AosQueryRsltObjPtr		mQueryData;
	AosQueryContextObjPtr	mQueryContext;
	AosBitmapObjPtr	mBitmap;
	i64						mNumTerms;
	bool					mDataLoaded;
	i64             		mPsize;
	i64						mStartIdx;
	i64						mExpectSize;
	i64						mBlockSize;

	// The following member data used to the statistcs.
	bool					mNeedOrder;
	i64						mRemovedNum;
	bool					mHasOrderTerm;		// Chen Ding, 2013/08/02

public:
	AosTermAnd();
	AosTermAnd(const bool regflag);

	virtual bool 	nextDocid(
						const AosQueryTermObjPtr &parent_term,
						u64 &docid, 
						bool &finished, 
						const AosRundataPtr &rdata);
	virtual bool 	nextDocid(
						const AosQueryTermObjPtr &parent_term,
						u64 &docid, 
						OmnString &key, 
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
	virtual AosQueryTermObjPtr clone(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	virtual bool	addTerm( 
						const AosQueryTermObjPtr &term,
						const AosRundataPtr &rdata);
	virtual AosQueryTermObjPtr addTerm( 
						const AosXmlTagPtr &term,
						const AosRundataPtr &rdata);
	virtual bool	query(
						const AosQueryRsltObjPtr &query_rslt,
				  	 	bool &finished, 
						const AosRundataPtr &rdata);
	virtual bool	loadData(const AosRundataPtr &rdata);
	virtual bool 	getRsltSizeFromIIL(const AosRundataPtr &rdata);
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual bool    collectInfo(const AosRundataPtr &rdata);
	virtual void	setQueryData(const AosQueryRsltObjPtr &query_rslt)
					{
						mQueryData = query_rslt;
					}
	virtual void	setQueryContext(const AosQueryContextObjPtr &query_context) 
					{
						mQueryContext = query_context;	
					}

	bool	runQuery(const AosRundataPtr &rdata);
	virtual AosQueryRsltObjPtr getQueryData() const {return mQueryData;}  // Lynch 2011/07/18
	//void    setQueryDataEmpty() { mQueryData->setEmpty();}
	virtual bool    setPagesize(const i64 &psize);
	virtual bool	setStartIdx(const i64 &start);
	void 	setNeedOrder(const bool flag){mNeedOrder = flag;}
	bool	getNeedOrder() const {return mNeedOrder;}
	AosXmlTagPtr getDoc(const u64 &docid, const AosRundataPtr &rdata);
	AosXmlTagPtr getDoc(const u64 &docid, const OmnString &value, const AosRundataPtr &rdata);
	bool	loadData2(const AosRundataPtr &rdata);
	bool	loadData3(const AosRundataPtr &rdata);

//	bool    setExpectSiz(const i64 &num);

	// Chen Ding, 01/02/2013
	AosQueryTermObjPtr *getTerms(i64 &num)
	{
		num = mNumTerms;
		return mTerms;
	}
	virtual i64		getNumTerms() const {return mNumTerms;}
	virtual bool	queryFinished();
	virtual bool	withValue() const; 
	bool	setNoMoreDocs()
	{
		mNoMoreDocs = true;
		return true;
	}
	virtual bool isCompoundQuery() const;

	// Chen Ding, 2014/01/29
	bool getTimeCond(
					const AosRundataPtr &rdata, 
					const OmnString &time_fname, 
					int &num_matched,
					int *start_days, 
					int *end_days);

	/*
	// Ketty 2014/02/13
	virtual bool findTermByStatKeyField(
				const OmnString &key_field, 
				bool &find,
				AosQueryTermObjPtr &term);
	*/
	virtual bool getCondTerms(vector<AosQueryTermObjPtr> & conds);
	virtual bool addInternalStatIdTerm(
				const AosRundataPtr &rdata,
				const OmnString &iil_name,
				const u32 stat_internal_id);
	virtual bool removeStatValueTerms(
				const AosRundataPtr &rdata,
				vector<AosQueryTermObjPtr> &value_terms);

private:
	bool	getTotal(const i64 &idx, const AosRundataPtr &rdata);
	bool 	parseEpochCond(
				const AosXmlTagPtr &term,
				const AosRundataPtr &rdata);
	bool 	procWordsForm1(const AosXmlTagPtr &term, const AosRundataPtr &rdata);
	bool 	procWordsForm2(const AosXmlTagPtr &term);
	void	parseFinished();
	void	releaseResource(const AosBitmapObjPtr &bitmap,const AosQueryRsltObjPtr &rslt);
	i64		adjustTermOrder(const AosRundataPtr &rdata);
	i64		adjustTermOrder3(const AosRundataPtr &rdata);

	void	switchTerm(const i64 &a,const i64 &b);
	void	countExpectBlockSize();
	void	countExpectBlockSizeSingle();
	bool 	moveToSingle(const i64 &startidx, const AosRundataPtr &rdata);

	// Chen Ding, 2014/01/30
	AosQueryTermObjPtr getJimoTerm(const AosRundataPtr &rdata, const AosXmlTagPtr &def);

	// Ketty 2014/04/10
	bool 	removeEachStatValue(
				const AosRundataPtr &rdata,
				const AosQueryTermObjPtr &term);
};
#endif

