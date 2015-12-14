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
//
// Modification History:
// 08/15/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/AddVersion.h"

#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "VersionServer/VersionServer.h"
#include "XmlUtil/XmlTag.h"


AosAddVersion::AosAddVersion(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_ADDVERSION, AosSeReqid::eAddVersion, rflag)
{
}


bool 
AosAddVersion::proc(const AosRundataPtr &rdata)
{
	AOSLOG_ENTER_R(rdata, false);		
	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr xmldoc = root->getFirstChild("objdef");
	if (!xmldoc)
	{
		rdata->setError() << "Missing object";
		OmnAlarm <<  rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr doc = xmldoc->getFirstChild();
	aos_assert_r(doc, false);

	// root should be this form:
	// <item name="args"><![CDATA[XXX]]></item>
	// <doc zky_log_ctnr_objid="xxx"
	// 		<contents><[CDATA[XXX]]></contents>
	// </doc>

	bool rslt = AosVersionServer::getSelf()->addVersionObj(doc, rdata);
	if (!rslt)
	{
	    rdata->setError() << "Faild to add version!";
	    OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
	    return false;
	}

	AOSSYSLOG_CREATE(rslt, AOSREQIDNAME_ADDVERSION, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}

