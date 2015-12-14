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
// 04/11/2011 Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermDate_h
#define Aos_Query_TermDate_h

#include "Query/TermIILType.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include <iostream>

using namespace boost::gregorian;
using namespace boost::posix_time;

class AosTermDate : public AosTermIILType
{
private:
	u64 			mStartTime;
	u64				mEndTime;
	OmnString		mAttrName;

public:
	AosTermDate(const bool regflag);
	AosTermDate(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata);
	AosTermDate(
			const OmnString &ctnr_objid,
			const OmnString &aname,
			const bool reverse,
			const bool order,
			const AosRundataPtr &rdata);

	virtual void toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt, 
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);

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

