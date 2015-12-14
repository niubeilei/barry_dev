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
// 08/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermContainer_h
#define Aos_Query_TermContainer_h

#include "Query/TermIILType.h"
#include "Util/Opr.h"


class AosTermContainer : public AosTermIILType
{
private:

public:
	AosTermContainer(const bool regflag);
	AosTermContainer(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosTermContainer(
			const OmnString &ctnr_objid, 
			const OmnString &aname, 
			const bool reverse, 
			const bool order, 
			const AosRundataPtr &rdata);

	virtual void toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	static bool convertToXml(
			const OmnString &cond_str, 
			const AosXmlTagPtr &conds, 
			const OmnString fields[5],
			const i64 &nn,
			const AosRundataPtr &rdata);

	static bool convertToXml(
	        const AosXmlTagPtr &conds_tag,
	        const OmnString &ctnrs,
	        const AosRundataPtr &rdata);

	static bool convertQuery(
			const AosXmlTagPtr &term, 
			const AosRundataPtr &rdata);
};
#endif

