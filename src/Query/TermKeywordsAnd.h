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
#ifndef Aos_Query_TermKeywordsAnd_h
#define Aos_Query_TermKeywordsAnd_h

#include "Query/TermHitType.h"
#include "SEUtil/Ptrs.h"


class AosTermKeywordsAnd : public AosTermHitType 
{
private:
	enum
	{
		eMaxWords = 10
	};

	OmnString		mCtnrObjid;
	OmnString		mAname;

public:
	AosTermKeywordsAnd(const bool regflag);
	AosTermKeywordsAnd(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosTermKeywordsAnd(
			const OmnString &ctnr_objid, 
			const OmnString &aname,
			const OmnString &keywords, 
			const bool reverse, 
			const bool order, 
			const AosRundataPtr &rdata);

	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	static AosQueryTermObjPtr addTerms(
					const AosQueryTermObjPtr &andterm, 
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

private:
	bool 	parse(
				const OmnString &ctnr_objid, 
				const OmnString &keywords, 
				const AosRundataPtr &rdata);
};
#endif

