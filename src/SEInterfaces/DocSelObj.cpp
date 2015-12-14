////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 01/30/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DocSelObj.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/DLLObj.h"


AosDocSelObjPtr AosDocSelObj::smDocSelector;


AosXmlTagPtr
AosDocSelObj::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const OmnString &tagname,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, 0);
	aos_assert_rr(tagname != "", rdata, 0);
	AosXmlTagPtr tag = sdoc->getFirstChild(tagname);
	if (!tag)
	{
		AosSetErrorUser(rdata, "doc_selector_tag_not_found")
			<< AOSDICTERM("tagname", rdata.getPtr()) << ": " << tagname 
			<< AOSDICTERM("worker_doc", rdata.getPtr()) << ": " << sdoc->toString() << enderr;
		return 0;
	}

	return selectDoc(tag, rdata);
}

