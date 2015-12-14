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
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermVersionDocs_h
#define Aos_Query_TermVersionDocs_h

#include "Query/TermIILType.h"


class AosTermVersionDocs : public AosTermIILType
{
private:
	OmnString		mObjid;
	AosXmlTagPtr	mRetrievedVers;
	AosXmlTagPtr	mCrtRecord;
	i64				mCrtIdx;
	u64 			mCtnrDocid;

public:
	AosTermVersionDocs(const bool flag);
	AosTermVersionDocs(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermVersionDocs();

	virtual bool nextDocid(
			        	const AosQueryTermObjPtr &parent_term,
						u64 &docid,
						bool &finished,
						const AosRundataPtr &rdata);
	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt, 
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual bool needHandleNextDocid() const {return true;}
	virtual AosXmlTagPtr	getDoc(const u64 &docid, const AosRundataPtr &rdata);

private:
	bool parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool initIILName(const OmnString &objid, const AosRundataPtr &rdata);
	bool loadData(const AosQueryRsltObjPtr &query_rslt, const AosRundataPtr &rdata);
};
#endif

