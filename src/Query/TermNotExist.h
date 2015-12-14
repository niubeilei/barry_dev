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
#ifndef Aos_Query_TermNotExist_h
#define Aos_Query_TermNotExist_h

#include "Query/QueryTerm.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"


class AosTermNotExist : public AosQueryTerm
{
private:
	AosOpr 	mOpr;
	bool			mIsByDocid;
	bool			mDocidRetrieved;
	u64				mDocid;
	OmnString		mFieldname;

	static const i64 	mMaxDocs;
public:
	AosTermNotExist(const bool flag);
	AosTermNotExist(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosTermNotExist(const OmnString &lhs, const AosRundataPtr &rdata);

	virtual bool 	nextDocid(
						const AosQueryTermObjPtr &and_term,
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
						const AosRundataPtr &rdata) {return true;}
	virtual bool    collectInfo(const AosRundataPtr &rdata);

	static bool convertToXml(
				const OmnString &cond_str, 
				const AosXmlTagPtr &conds_tag, 
				const OmnString fields[5],
				const i64 &nn,
				const AosRundataPtr &rdata);

	static bool convertQuery(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata);
private:
	bool 	parse(
				const OmnString &lhs,
				const AosRundataPtr &rdata);
};
#endif

