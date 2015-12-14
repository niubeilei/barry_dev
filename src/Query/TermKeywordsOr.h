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
#ifndef Aos_Query_TermKeywordsOr_h
#define Aos_Query_TermKeywordsOr_h

#include "Query/QueryTerm.h"
#include <vector>


class AosTermKeywordsOr : public AosQueryTerm 
{
private:
	enum
	{
		eMaxWords = 10
	};

	OmnString			mContainer;
	OmnString			mAttrname;
	vector<OmnString>	mIILNames;
	bool				mDataLoaded;
	AosQueryRsltObjPtr  mQueryRslt;
	AosBitmapObjPtr   mBitmap;
	AosQueryContextObjPtr  mQueryContext;
	AosCondInfo			mCondInfo;

public:
	AosTermKeywordsOr(const bool regflag);
	AosTermKeywordsOr(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermKeywordsOr();

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
	virtual bool 	getRsltSizeFromIIL(const AosRundataPtr &rdata);
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual bool 	collectInfo(const AosRundataPtr &rdata);

	static bool convertToXml(
				const OmnString &cond_str, 
				const AosXmlTagPtr &conds_tag, 
				const OmnString fields[5],
				const int nn,
				const AosRundataPtr &rdata);

	static bool convertQuery(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata);
private:
	bool 	parse(
				const OmnString &words, 
				const AosRundataPtr &rdata);

	bool loadData(const AosRundataPtr &rdata);
};
#endif

