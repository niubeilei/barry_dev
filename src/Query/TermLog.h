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
#ifndef Aos_Query_TermLog_h
#define Aos_Query_TermLog_h

#include "Query/TermIILType.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"


class AosTermLog : public AosTermIILType
{

private:
	bool		mDataRetrieved;
	OmnString	mCtnrObjid;
	OmnString 	mLogName;
	i64			mNumber;
	OmnString 	mIILName;
	u64 		mIILId;
public:
	AosTermLog(const bool regflag);
	AosTermLog(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosTermLog(
		const OmnString &ctnr_objid, 
		const OmnString &logtype, 
		const bool reverse, 
		const bool order,
		const AosOpr opr,
		const i64 &logtime,
		const AosRundataPtr &rdata);
	~AosTermLog();

	virtual void toString(OmnString &str);
	virtual bool getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt, 
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getDoc(const u64 &docid, const AosRundataPtr &rdata);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

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
};
#endif
