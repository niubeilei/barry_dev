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
// 06/22/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermAllowance_h
#define Aos_Query_TermAllowance_h

#include "Query/QueryTerm.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include <time.h>


class AosTermAllowance : public AosQueryTerm
{

private:
	OmnString		mSdocObjid;

public:
	AosTermAllowance(const bool flag);
	AosTermAllowance(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosTermAllowance(
		const OmnString &siteid,
		const OmnString &sdoc_objid, 
		const bool order, 
		const AosRundataPtr &rdata);

	virtual bool 	nextDocid(
						const AosQueryTermObjPtr &parent_term,
						u64 &docid, 
						bool &finished, 
						const AosRundataPtr &rdata);
	virtual bool	addTerm(const AosQueryTermObjPtr &t);
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

	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata){return true;}
	virtual AosXmlTagPtr	getDoc(const u64 &docid, const AosRundataPtr &rdata);
	bool getTime(time_t tt, OmnString &stm);

	static bool convertToXml(
				const OmnString &cond_str, 
				const AosXmlTagPtr &conds_tag, 
				const OmnString fields[5],
				const i64 &nn,
				const AosRundataPtr &rdata);

	static bool convertQuery(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata);
};
#endif

