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
#ifndef Aos_Query_TermInrange_h
#define Aos_Query_TermInrange_h

#include "Query/TermIILType.h"
#include "QueryCond/Ptrs.h"
#include "Util/Opr.h"


class AosTermInrange : public AosTermIILType
{

private:

public:
	AosTermInrange(const bool flag);
	AosTermInrange(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual void    toString(OmnString &str);

	// These are temporary functions for converting queries.
	static bool convertQuery(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata);

	static bool convertToXml(
				const OmnString &cond_str, 
				AosXmlTagPtr &conds, 
				const AosRundataPtr &rdata);
};
#endif

