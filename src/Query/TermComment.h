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
#ifndef Aos_Query_TermComment_h
#define Aos_Query_TermComment_h

#include "Query/TermIILType.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include <time.h>


class AosTermComment : public AosTermIILType 
{
private:
	u64			mCmtDocid;
	OmnString 	mCmtobjid;

public:
	AosTermComment(const bool flag);
	AosTermComment(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	AosTermComment(
			const OmnString &cmt_docid,
			const OmnString &cmt_objid,
			const bool order, 
			const AosRundataPtr &rdata);

	virtual void	toString(OmnString &str);
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
};
#endif

