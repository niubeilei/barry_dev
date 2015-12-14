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
#ifndef Aos_Query_TermOr_h
#define Aos_Query_TermOr_h

#include "Query/QueryTerm.h"


class AosTermOr : public AosQueryTerm
{
	enum
	{
		eMaxAndTerms = 10
	};

	AosQueryTermObjPtr		mTerms[eMaxAndTerms];
	i64					mNumTerms;
	AosQueryTermObjPtr 	mHelpTerm;
	AosQueryTermObjPtr 	mDocRetriever;
	bool				mIgnore;

public:
	AosTermOr();
	~AosTermOr();

	virtual bool	addTerm(
						const AosQueryTermObjPtr &term,
						const AosRundataPtr &rdata);
	virtual bool	nextDocid(
						const AosQueryTermObjPtr &parent_term,
						u64 &docid, 
						bool &finished, 
						const AosRundataPtr &rdata);
	virtual bool	nextDocid(
						const AosQueryTermObjPtr &parent_term,
						u64 &docid, 
						OmnString &value, 
						bool &finished, 
						const AosRundataPtr &rdata);
	virtual bool	checkDocid(
						const u64 &docid,
						const AosRundataPtr &rdata);
	virtual bool 	getRsltSizeFromIIL(const AosRundataPtr &rdata);
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual bool 	moveTo(
						const i64 &startidx,
						const AosRundataPtr &rdata);
	virtual i64		getTotal(const AosRundataPtr &rdata);
	virtual bool	setOrder(
						const OmnString &container, 
						const OmnString &name, 
						const bool reverse, 
						const AosRundataPtr &);
	virtual void	reset(const AosRundataPtr &rdata);
	virtual bool    collectInfo(const AosRundataPtr &rdata);
	virtual void	toString(OmnString &ss);
	virtual bool	setPagesize(const i64 &psize);
	virtual bool	setStartIdx(const i64 &num);
	virtual bool	loadData(const AosRundataPtr &rdata);
	virtual bool	addStatTerm(
						const AosXmlTagPtr &statTerm,
						const AosRundataPtr &rdata);
	virtual void	setDocRetriever(const AosQueryTermObjPtr &term){ mDocRetriever = term; }
	virtual bool	runQuery(const AosRundataPtr &rdata);
	virtual AosQueryRsltObjPtr getQueryData() const;

	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const AosRundataPtr &rdata); 
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const OmnString &value,
						const AosRundataPtr &rdata); 
	virtual bool	withValue() const; 
	virtual AosQueryTermObjPtr * getTerms(i64 &num)
	{
		num = mNumTerms;
		return mTerms;
	}
	virtual i64		getNumTerms() const {return mNumTerms;}

	virtual AosQueryTermObjPtr clone(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	virtual bool	queryFinished();
	bool setNoMoreDocs()
	{
		if(!mTerms[0]) return true;
		return mTerms[0]->setNoMoreDocs();
	}
	//void 	setQueryDataEmpty();
	virtual bool isCompoundQuery() const;

	// Chen Ding, 2014/01/29
	virtual bool getTimeCond(
						const AosRundataPtr &rdata, 
						const OmnString &time_fname, 
						int &num_matched,
						int *start_day, 
						int *end_day);

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
	
};
#endif

