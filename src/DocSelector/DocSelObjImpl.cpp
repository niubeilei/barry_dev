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
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/DocSelObjImpl.h"

#include "DocSelector/DocSelector.h"
#include "XmlUtil/XmlTag.h"


AosDocSelObjImpl::AosDocSelObjImpl()
{
}


AosDocSelObjImpl::~AosDocSelObjImpl()
{
}


AosXmlTagPtr 
AosDocSelObjImpl::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	return AosDocSelector::selectDocStatic(sdoc, rdata);
}


/*
AosXmlTagPtr 
AosDocSelObjImpl::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const OmnString &name,
		const AosRundataPtr &rdata)
{
	return AosDocSelector::selectDoc(sdoc, name, rdata);
}
*/
