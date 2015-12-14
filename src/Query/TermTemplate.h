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
// 08/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermTemplate_h
#define Aos_Query_TermTemplate_h

#include "Query/QueryTerm.h"


class AosTermTemplate : public AosQueryTerm
{
private:

public:
	AosTermTemplate(const bool flag);
	AosTermTemplate(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata);
	~AosTermTemplate();

	virtual bool 	nextDocid(
						const AosQueryTermObjPtr &and_term,
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
	virtual i64		getTotal(const AosRundataPtr &rdata);
	virtual void	reset(const AosRundataPtr &rdata);
	virtual bool 	moveTo(const i64 &pos, const AosRundataPtr &rdata);
	virtual void	toString(OmnString &str);
	virtual bool 	setOrder(
						const OmnString &container, 
						const OmnString &name, 
						const bool reverse, 
						const AosRundataPtr &);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool    collectInfo(const AosRundataPtr &rdata);

private:
};
#endif

