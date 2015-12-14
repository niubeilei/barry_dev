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
#ifndef Aos_Query_TermIILType_h
#define Aos_Query_TermIILType_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"


class AosTermIILType : public AosQueryTerm
{

protected:
	// u64					mIILId;	// Chen Ding, 2014/01/30
	// AosQueryCondPtr		mCond;	// Chen Ding, 2014/01/30
	bool				mDataLoaded;
	AosQueryRsltObjPtr	mQueryRslt;
	AosQueryContextObjPtr	mQueryContext;
	OmnString			mFieldname;
	OmnString 			mCtnrObjid;
	AosOpr				mOpr;
	OmnString			mValue;
	bool				mIsSuperIIL;
	OmnString			mValue2;
	OmnString			mFormat1;		// Chen Ding, 2014/02/02
	OmnString			mFormat2;		// Chen Ding, 2014/02/02
	vector<AosQueryFilterObjPtr>	mFilters;	// Ken Lee, 2014/08/19

public:
	AosTermIILType(const bool flag);
	AosTermIILType(
			const OmnString &name, 
			const AosQueryType::E type, 
			const bool regflag);
	AosTermIILType(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermIILType();

	// AosTermIILType Interface
	virtual bool nextDocid(
					const AosQueryTermObjPtr &parent_term,
					u64 &docid, 
					bool &finished, 
					const AosRundataPtr &rdata);
	virtual bool	checkDocid(const u64 &docid, const AosRundataPtr &rdata);

	virtual bool 	getRsltSizeFromIIL(const AosRundataPtr &rdata);
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt, 
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual bool 	moveTo(const i64 &startidx, const AosRundataPtr &rdata);
	virtual i64		getTotal(const AosRundataPtr &rdata);
	virtual bool 	setOrder(
						const OmnString &container, 
						const OmnString &name, 
						const bool reverse, 
						const AosRundataPtr &);
	virtual void	reset(const AosRundataPtr &rdata);
	virtual void	toString(OmnString &str);
	virtual bool	collectInfo(const AosRundataPtr &rdata);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool	retrieveContainers(
						const AosXmlTagPtr &def, 
						AosHtmlCode &code, 
						const AosRundataPtr &rdata);

	// Chen Ding, 01/03/2012
	virtual bool canUseBitmapQuery() const {return true;}
	virtual AosQueryContextObjPtr getQueryContext() const;

	virtual bool	setPagesize(const i64 &psize){return false;}
	virtual bool	setStartIdx(const i64 &num){return false;}

	// Chen Ding, 2014/01/29
	virtual bool getTimeCond(
						const AosRundataPtr &rdata, 
						const OmnString &time_fname, 
						int &num_matched,
						int *start_days, 
						int *end_days);

	// Ketty 2014/04/14
	//virtual bool initStatTimeCond(const AosRundataPtr &rdata, AosStatQueryCondInfo &cond);

protected:
	bool loadData(const AosRundataPtr &rdata);

	// Chen Ding, 2013/02/07
	virtual bool setQueryContext(const AosQueryContextObjPtr &context) const;
};
#endif

