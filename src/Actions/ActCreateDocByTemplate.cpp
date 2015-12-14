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
// 12/27/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActCreateDocByTemplate.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocUtil/DocProcUtil.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlUtil.h"


AosActCreateDocByTemplate::AosActCreateDocByTemplate(const bool flag)
:
AosSdocAction(AOSACTTYPE_CREATE_DOC_BY_TEMPLATE, AosActionType::eCreateDocByTemplate, flag)
{
}


bool 
AosActCreateDocByTemplate::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function retrieves a doc based on a doc selector. If the doc 
	// can be retrieved, it returns the doc. Otherwise, it retrieves a 
	// template. If the template is retrieved, it uses the template to 
	// create a new doc. 
	// 'sdoc' should be in the following format:
	// 	<sdoc ...>
	// 		<AOSTAG_RETRIEVED_DOC .../>		This is a doc selector
	//
	// If the doc is retrieved/created, it is set to rdata as Created Doc
	aos_assert_rr(sdoc, rdata, false);
	// AOSMONITORLOG_ENTER(rdata);		
	// rdata->getLog() << "->create_doc_by_template";

	// 1. Retrieve the doc. 
	AosXmlTagPtr retrieve_doc_sdoc = sdoc->getFirstChild(AOSTAG_RETRIEVED_DOC);
	if (!retrieve_doc_sdoc)
	{
		AosSetError(rdata, AOSLT_MISSING_DOC_SELECTOR);
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr doc = AosRunDocSelector(rdata, retrieve_doc_sdoc);
	if (doc) 
	{
		if (!doc->isRootTag())
		{
			doc = doc->clone(AosMemoryCheckerArgsBegin);
		}
		rdata->setCreatedDoc(doc, false);
		return true;
	}

	// The doc is not there yet. Need to create it.
	AosXmlTagPtr tmpl_sdoc = sdoc->getFirstChild(AOSTAG_TEMPLATE_SDOC);
	if (!tmpl_sdoc)
	{
		AosSetError(rdata, AOSLT_MISSING_TEMPLATE_SDOC);
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr tmpl_doc = AosRunDocSelector(rdata, tmpl_sdoc);
	if (!tmpl_doc)
	{
		AosSetError(rdata, AOSLT_MISSING_TEMPLATE_DOC);
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	AosXmlUtil::removeMetaAttrs(tmpl_doc);
	if (!tmpl_doc->isRootTag())
	{
		tmpl_doc = tmpl_doc->clone(AosMemoryCheckerArgsBegin);
	}
	rdata->setCreatedDoc(tmpl_doc, false);

	// Run actions on tmpl_doc
	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_PRE_ACTIONS);
	if (actions)
	{
		if (!AosSdocAction::runAction(actions, rdata)) return false;
	}

	bool is_public = sdoc->getAttrBool(AOSTAG_ISPUBLIC, false);
	OmnString docstr = doc->toString();
	AosXmlTagPtr user_opr = AosDocClientObj::getDocClient()->createDocSafe1(rdata, docstr,
		rdata->getCid(), "", is_public, true, false, false, true, true);
	if (!user_opr) return false;

	AosXmlTagPtr post_actions = sdoc->getFirstChild(AOSTAG_POST_ACTIONS);
	if (post_actions)
	{
		AosSdocAction::runAction(post_actions, rdata);
	}

	return true;
}


AosActionObjPtr
AosActCreateDocByTemplate::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActCreateDocByTemplate(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


