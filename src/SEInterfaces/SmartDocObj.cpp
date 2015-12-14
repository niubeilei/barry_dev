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
#include "SEInterfaces/SmartDocObj.h"

#include "Rundata/Rundata.h"


AosSmartDocObjPtr AosSmartDocObj::smSmartDoc;

bool 
AosSmartDocObj::procSmartdocsStatic(const OmnString &sdoc_objids, const AosRundataPtr &rdata)
{
	aos_assert_rr(smSmartDoc, rdata, false);
	return smSmartDoc->procSmartdocs(sdoc_objids, rdata);
}


bool 
AosSmartDocObj::runSmartdocStatic(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	aos_assert_rr(smSmartDoc, rdata, false);
	return smSmartDoc->runSmartdoc(sdoc, rdata);
}


bool 
AosSmartDocObj::procSmartdocStatic(const u64 &sdocid, const AosRundataPtr &rdata)
{
	aos_assert_rr(smSmartDoc, rdata, false);
	return smSmartDoc->runSmartdoc(sdocid, rdata);
}


bool 
AosSmartDocObj::runSmartdocsStatic(const OmnString &sdoc_objids, const AosRundataPtr &rdata)
{
	aos_assert_rr(smSmartDoc, rdata, false);
	return smSmartDoc->runSmartdocs(sdoc_objids, rdata);
}
