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
// 08/01/2011	by Jackie Zhao
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocUtil_DocLink_h
#define AOS_DocUtil_DocLink_h

#include "util_c/types.h"
#include "XmlUtil/Ptrs.h"


struct AosDocLink
{
	u64				docId;
	AosXmlTagPtr	doc;
	AosDocLink		*p;
	AosDocLink		*n;

	AosDocLink()
	:
	docId(0),
	doc(0),
	p(0),
	n(0)
	{
	}

	AosDocLink(const u64 &docid, const AosXmlTagPtr &doc)
	:
	docId(docid),
	doc(doc),
	p(0),
	n(0)
	{
	}
};
#endif
