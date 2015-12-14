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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SmartDoc/SMDMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "Security/Session.h"
#include "SeLogClient/SeLogClient.h"
#include "SmartDoc/SdocIds.h"
#include "SmartDoc/SdocTypes.h"
#include "SmartDoc/SmartDoc.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"


AosSMDMgr::AosSMDMgr()
{
}


AosSMDMgr::~AosSMDMgr()
{
}


bool
AosSMDMgr::procSmartdocs(
		const OmnString &sdoc_objids, 
		const AosRundataPtr &rdata)
{
	OmnStrParser1 parser(sdoc_objids, ", ");
	OmnString sdoc_objid;
	while ((sdoc_objid = parser.nextWord()) != "")
	{
		AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDocByObjid(sdoc_objid, rdata);
		if (!sdoc)
		{
			AosSetError(rdata, "failed_retrieve_sdoc") << ": " << sdoc_objid;
			continue;
		}
		sdoc = sdoc->clone(AosMemoryCheckerArgsBegin);	
		aos_assert_rr(sdoc, rdata, false);
		
		OmnString type = sdoc->getAttrStr(AOSTAG_SMARTDOC_TYPE);
		AosSmartDocObjPtr smartdoc = AosSmartDoc::getSmartDoc(type);
		if (!smartdoc)
		{
			AosSetError(rdata, "invalid_sdoc_type") << ": " << type;
			continue;
		}
		smartdoc->setSdoc(sdoc);
		
		OmnString status = sdoc->getAttrStr(AOSTAG_INTERRUPT);
		rdata->setSdocReturnStaus(status);

		// rdata->setCrtSmartdoc(smartdoc);
		bool rslt = smartdoc->run(sdoc, rdata);
		AOSSYSLOG_CREATE(rslt, type, rdata); 
		//TSK0070, Linda, 2011/05/12  Container Member Verifications
		if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
		{
			if (!rdata->isOk()) return false; 
		}
		if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_BREAK)
		{
			if (!rdata->isOk()) return true;
		}
	}
	return true;
}


bool
AosSMDMgr::runSmartdoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	OmnString type = sdoc->getAttrStr(AOSTAG_SMARTDOC_TYPE);
	AosSmartDocObjPtr smartdoc = AosSmartDoc::getSmartDoc(type);
	if (!smartdoc)
	{
		AosSetError(rdata, "invalid_sdoc_type") << ": " << type;
		return false;
	}
	// rdata->setCrtSmartdoc(smartdoc);
	rdata->setSdoc(sdoc);
	bool rslt = smartdoc->run(sdoc, rdata);
	AOSSYSLOG_CREATE(rslt, type, rdata); 

	return true;
}


bool
AosSMDMgr::procSmartdoc(
		const u64 &sdocid, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdocid, false);
		
	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDocByDocid(sdocid, rdata);
	if (!sdoc)
	{
		AosSetError(rdata, "failed_retrieve_sdoc") << ": " << sdocid;
		return false;
	}
	
	sdoc = sdoc->clone(AosMemoryCheckerArgsBegin);	
	aos_assert_rr(sdoc, rdata, false);
		
	OmnString type = sdoc->getAttrStr(AOSTAG_SMARTDOC_TYPE);
	AosSmartDocObjPtr smartdoc = AosSmartDoc::getSmartDoc(type);
	if (!smartdoc)
	{
		AosSetError(rdata, "invalid_sdoc_type") << ": " << type;
		return false;
	}
	smartdoc->setSdoc(sdoc);
		
	OmnString status = sdoc->getAttrStr(AOSTAG_INTERRUPT);
	rdata->setSdocReturnStaus(status);

	// rdata->setCrtSmartdoc(smartdoc);
	bool rslt = smartdoc->run(sdoc, rdata);
	AOSSYSLOG_CREATE(rslt, type, rdata); 
	//TSK0070, Linda, 2011/05/12  Container Member Verifications
	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_RETURN)
	{
		if (!rdata->isOk()) return false; 
	}
	if (rdata->getSdocReturnStaus() == AOSTAG_STATUS_BREAK)
	{
		if (!rdata->isOk()) return true;
	}
	return true;
}
#endif
