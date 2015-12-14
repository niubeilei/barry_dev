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
#if 0
#include "SmartDoc/SmartDocImpl.h"

#include "SmartDoc/SmartDoc.h"


AosSmartDocImpl::AosSmartDocImpl()
{
}


AosSmartDocImpl::~AosSmartDocImpl()
{
}


bool 
AosSmartDocImpl::runSmartdocs(const OmnString &sdoc_objids, const AosRundataPtr &rdata)
{
	return AosSmartDoc::runSmartdocs(sdoc_objids, rdata);
}


bool 
AosSmartDocImpl::runSmartdoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	return AosSmartDoc::runSmartdoc(sdoc, rdata);
}

AosXmlTagPtr 
AosSmartDocImpl::createDoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	return AosSmartDoc::createDoc(sdoc, rdata);
}

#endif
