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
#ifndef Aos_Query_TermTag_h
#define Aos_Query_TermTag_h

#include "Query/TermHitType.h"
#include "Util/Opr.h"


class AosTermTag : public AosTermHitType
{

private:
	OmnString		mCtnrObjid;
	OmnString		mTags;

public:
	AosTermTag(const bool flag);
	AosTermTag(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosTermTag(
			const OmnString &ctnr_objid, 
			const OmnString &tags, 
			const bool reverse, 
			const bool order, 
			const AosRundataPtr &rdata);
	~AosTermTag() {}

	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	static bool convertToXml(
				const OmnString &cond_str, 
				const AosXmlTagPtr &conds_tag, 
				const OmnString fields[5],
				const i64 &nn,
				const AosRundataPtr &rdata);

	static bool convertToXml(
			    const AosXmlTagPtr &conds_tag,
				const OmnString &tags,
				const AosRundataPtr &rdata);

	static bool convertQuery(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata);

private:
	bool parse(const OmnString &ctnr_objid, const OmnString &tags, const AosRundataPtr &rdata);
};
#endif

