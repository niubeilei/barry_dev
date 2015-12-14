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
// 2011/01/12	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActUtil.h"

#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/XmlTag.h"


/*
AosXmlTagPtr
AosActUtil::getDoc(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// This function retrieves a doc based on the configuration 'def'
	// 'def' should be in the form:
	// 	<doc zky_docsrc="xxx"/>
	OmnString docsrc = def->getAttrStr(AOSTAG_DOCSRC);
	if (docsrc == "") return 0;

	AosSmartDocPtr smartdoc = rdata->getSmartdoc();
	if (!smartdoc)
	{
		rdata->setError() << "Missing smartdoc: " << rdata->toString(1);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (docsrc == "accessedobj")
	{
		return rdata->getRetrievedDoc();
	}

	if (docsrc == AOSTAG_DOCIDX) 
	{
		// 	<doc docsrc="xxx" AOSTAG_DOCIDX="xxx"/>
		int idx = def->getAttrInt(AOSTAG_DOCIDX, -1);
		aos_assert_r(idx >= 0, 0);
		aos_assert_r(idx < rdata->numDocs(), 0);
		return rdata->getDoc(idx);
	}

	if (docsrc == AOSTAG_ACCESSED_DOCOBJID)
	{
		// The object is retrieved from database. The objid is specified
		// by 'def'.
		OmnString objid = def->getAttrStr(AOSTAG_ACCESSED_DOCOBJID);
		aos_assert_r(objid != "", 0);
		return AosDocMgr::getSelf()->getDoc(rdata->getSiteid(), objid);
	}
	
	//Zky2742 Linda 2011/02/19
	if (docsrc == "sourcedoc")
	{
		// retrieved Rundata SourceDoc 
		return rdata->getSourceDoc(); 
	}

	//Linda 2011/04/22
	if (docsrc == "retrieved")
	{
		//retrieved Rundata mRetrievedDoc
		return rdata->getRetrievedDoc();
	}

	//Zky3293, Linda, 2011/04/25
	if (docsrc == "actiondoc")
	{
		AosXmlTagPtr adoc;
		AosSmartDocPtr smartdoc = rdata->getSmartdoc();
		if (smartdoc)
		{
			 adoc= smartdoc->getDoc();
		}
		return adoc; 
	}
	
	if (docsrc == "received")
	{
		AosXmlTagPtr request = rdata->getReceivedDoc();
		if (!request)
		{
			rdata->setError()<< "Request incorrect!!" ;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		return request->getFirstChild();
	}

	//end

	OmnAlarm << "Unrecognized objsrc: " << docsrc << enderr;
	return 0;
}
*/

