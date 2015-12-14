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
// 07/31/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermIIL_h
#define Aos_Query_TermIIL_h

#include "Query/TermIILType.h"
#include "QueryUtil/QueryCondInfo.h"
#include "Util/Opr.h"


class AosTermIIL : public AosTermIILType
{
private:
	i64				mStartIdx;

public:
	AosTermIIL(const bool flag);
	AosTermIIL(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosTermIIL(
		const OmnString &ctnr_objid,
		const OmnString &fname, 
		const OmnString &value, 
		const AosOpr opr,
		const bool reverse,
		const bool order,
		const AosRundataPtr &rdata);
	AosTermIIL(const AosQueryCondInfo &cond, const AosRundataPtr &rdata);

	AosTermIIL(
		const u64 &iilid, 
		const OmnString &value, 
		const AosOpr opr,
		const bool reverse,
		const bool order,
		const AosRundataPtr &rdata);

	// Chen Ding, 2013/02/07
	AosTermIIL(
		const OmnString &iilname, 
		const AosValueRslt &value1, 
		const AosValueRslt &value2, 
		const AosOpr opr,
		const bool reverse,
		const bool order,
		const bool flag,
		const AosRundataPtr &rdata);

	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

