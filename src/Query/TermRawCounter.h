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
#ifndef Aos_Query_TermRawCounter_h
#define Aos_Query_TermRawCounter_h

#include "Query/TermIILType.h"
#include "Util/Opr.h"
#include "SEUtil/Ptrs.h"


class AosTermRawCounter : public AosTermIILType
{
	OmnString		mQueryRsltOpr;
	bool			mSaveToFile;
	OmnFilePtr		mFile;
	OmnString		mKey;

public:
	AosTermRawCounter(const bool regflag);
	AosTermRawCounter(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata);
	AosTermRawCounter(
		const OmnString &aname, 
		const OmnString &rhs,
		const bool order,
		const bool reverse, 
		const AosOpr opr,
		const OmnString &ctnr_objid,
		const AosRundataPtr &rdata);
	
	~AosTermRawCounter();

	virtual bool	needHandleNextDocid() const {return false;}
	virtual void	toString(OmnString &str);

	virtual AosQueryTermObjPtr clone(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const OmnString &k,
						const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
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

	virtual bool	nextDocid(
			        	const AosQueryTermObjPtr &parent_term,
						u64 &docid,
						OmnString &value,
						bool &finished,
						const AosRundataPtr &rdata);

	virtual void setQueryData(const AosQueryRsltObjPtr &query_data) { mQueryRslt = query_data; }

	virtual bool	withValue() const {return true;}
						

private:
	bool 	parse(const AosRundataPtr &rdata);

	OmnFilePtr createFile(const AosRundataPtr &rdata);

};
#endif

