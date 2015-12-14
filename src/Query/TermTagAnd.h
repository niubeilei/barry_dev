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
#ifndef Aos_Query_TermTagAnd_h
#define Aos_Query_TermTagAnd_h

#include "Query/TermIILType.h"
#include "Util/Opr.h"


class AosTermTagAnd : public AosTermIILType
{

private:
	enum
	{
		eMaxTags = 10
	};

	OmnString		mContainer;

public:
	AosTermTagAnd(const bool flag);
	AosTermTagAnd(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosTermTagAnd(
			const OmnString &ctnr_objid, 
			const OmnString &aname, 
			const OmnString &tags, 
			const bool reverse, 
			const bool order, 
			const AosRundataPtr &rdata);
	~AosTermTagAnd() {}

	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	static AosQueryTermObjPtr addTerms(
					const AosQueryTermObjPtr &andterm, 
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

private:
	bool parse(const OmnString &ctnr_objid, const OmnString &tags, const AosRundataPtr &rdata);
};
#endif

