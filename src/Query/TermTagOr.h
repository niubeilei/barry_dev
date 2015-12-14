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
#ifndef Aos_Query_TermTagOr_h
#define Aos_Query_TermTagOr_h

#include "Query/QueryTerm.h"
#include "Util/Opr.h"
#include <vector>


class AosTermTagOr : public AosQueryTerm
{

private:
	enum
	{
		eMaxTags = 10
	};

	OmnString			mContainer;
	OmnString			mAttrname;
	vector<OmnString>	mIILNames;
	bool				mDataLoaded;
	AosQueryRsltObjPtr	mQueryRslt;
	AosBitmapObjPtr	mBitmap;
	AosQueryContextObjPtr	mQueryContext;

public:
	AosTermTagOr(const bool flag);
	AosTermTagOr(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	// AosTermTagOr(
	// 		const OmnString &ctnr_objid, 
	// 		const OmnString &attrname,
	// 		const OmnString &tags, 
	// 		const bool reverse, 
	// 		const bool order, 
	// 		const AosRundataPtr &rdata);
	~AosTermTagOr();

	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool nextDocid(
					const AosQueryTermObjPtr &parent_term,
					u64 &docid, 
					bool &finished, 
					const AosRundataPtr &rdata);
	virtual bool 	moveTo(const i64 &startidx, const AosRundataPtr &rdata);
	virtual i64		getTotal(const AosRundataPtr &rdata);
	virtual bool 	setOrder(
						const OmnString &container, 
						const OmnString &name, 
						const bool reverse, 
						const AosRundataPtr &);
	virtual bool 	checkDocid(const u64 &docid, const AosRundataPtr &rdata);
	virtual bool 	getRsltSizeFromIIL(const AosRundataPtr &rdata);
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual void	reset(const AosRundataPtr &rdata);
	virtual bool	collectInfo(const AosRundataPtr &rdata);

private:
	bool parse(const OmnString &tags, const AosRundataPtr &rdata);
	bool loadData(const AosRundataPtr &rdata);
};
#endif

