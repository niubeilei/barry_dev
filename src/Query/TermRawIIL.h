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
#ifndef Aos_Query_TermRawIIL_h
#define Aos_Query_TermRawIIL_h

#include "Query/TermIILType.h"
#include "Util/Opr.h"
#include "SEUtil/Ptrs.h"


class AosTermRawIIL : public AosTermIILType
{
	OmnString		mQueryRsltOpr;

public:
	AosTermRawIIL(const bool regflag);
	AosTermRawIIL(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata);
	AosTermRawIIL(
		const OmnString &aname, 
		const OmnString &rhs,
		const bool order,
		const bool reverse, 
		const AosOpr opr,
		const OmnString &ctnr_objid,
		const AosRundataPtr &rdata);
	
	~AosTermRawIIL();

	virtual bool	needHandleNextDocid() const {return false;}
	virtual void	toString(OmnString &str);

	virtual AosQueryTermObjPtr clone(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const OmnString &key,
						const AosRundataPtr &rdata);
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt, 
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual bool	nextDocid(
			        	const AosQueryTermObjPtr &parent_term,
						u64 &docid,
						bool &finished,
						const AosRundataPtr &rdata);
	virtual bool	withValue() const {return true;}

private:
	bool 	parse(const AosRundataPtr &rdata);

};
#endif

