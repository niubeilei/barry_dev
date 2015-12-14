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
// 08/11/2011	created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEModules/SeRundata.h"

#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SEModules/SiteMgr.h"
#include "Security/Session.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/Locale.h"


/*
AosLocale::E
AosSeRundata::getLocale(const AosRundataPtr &rdata) 
{
	// 1. If no session, return the default language code from the site. 
	//    If the site does not have a default language code, use the 
	//    system default language code. 
	// 2. Ask the session to return the language code.
	AosSessionPtr session = rdata->getSession();
	if (!session)
	{
		return AosSiteMgr::getSelf()->getLocale(rdata);
	}
	
	return session->getLocale();
}


OmnString 
AosSeRundata::getSmartdocObjid() const
{
	if (!mSmartdoc) return "";
	return mSmartdoc->getSdocObjid();
}
*/


AosXmlTagPtr 
AosSeRundata::getSdoc(const AosRundataPtr &rdata)
{
	AosXmlTagPtr sdoc = rdata->getSdoc();
	if (sdoc) return sdoc;
	OmnString objid = rdata->getSdocObjid();
	if (objid == "") return 0;
	sdoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (sdoc)
	{
		rdata->setSdoc(sdoc);
	}
	return sdoc;
}

#endif
