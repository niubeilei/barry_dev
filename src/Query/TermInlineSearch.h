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
#ifndef Aos_Query_TermInlineSearch_h
#define Aos_Query_TermInlineSearch_h

#include "Query/TermIILType.h"
#include "Util/Opr.h"
#include "SEUtil/Ptrs.h"


class AosTermInlineSearch : public AosTermIILType
{
	OmnString		mCtnrObjid;
	OmnString		mSeqId;

public:
	AosTermInlineSearch(const bool regflag);
	AosTermInlineSearch(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata);

	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	
	virtual AosXmlTagPtr	getDoc(const u64 &docid, const AosRundataPtr &rdata);

private:
	bool 	parse(const AosRundataPtr &rdata);
};
#endif

