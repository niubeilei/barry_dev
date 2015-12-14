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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/Counter.h"

#include "EventMgr/EventMgr.h"
#include "SEInterfaces/DocClientObj.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "SEInterfaces/SmartDocObj.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosCounter::AosCounter(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_COUNTER, 
		AosSeReqid::eCounter, rflag)
{
}


bool 
AosCounter::proc(const AosRundataPtr &rdata)
{
	// This function is created by Tank 2010/12/17
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args =="")
	{
		rdata->setError() << "Missing args!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString objid, sdoc_objids, subType;
	AosParseArgs(args, "objid", objid, "sdocobjid", sdoc_objids, "subtype", subType);
	
	if (sdoc_objids == "")
	{
		rdata->setError() << "Missing objid!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
		
	if (subType == "")
	{
		rdata->setError() << "Missing type!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
		
	if (objid != "")
	{
		AosXmlTagPtr doc;
		doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
		if(!doc)
		{
			rdata->setError() << "doc not find!";
			AOSLOG_LEAVE(rdata);
			return false;
		}

		// if (mSession && doc) mSession->setAccessedObj(doc);
		rdata->setRetrievedDoc(doc, true);
		OmnString ctnr_objid = doc->getAttrStr(AOSTAG_OBJID);
		if (ctnr_objid != "")
		{
			AosEventMgr::getSelf()->procEvent(eAosHookCounterByCtnr,
			eAosHkptCounter1, ctnr_objid, rdata);
		}
	}
	
	// Chen Ding, 2011/01/21
	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDocByObjid(sdoc_objids, rdata);
	if(!sdoc)
	{
		rdata->setError() << "smdoc operate error!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	//Chen Ding, 2011/01/18
	//Why modify the attribute???
	AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
			sdoc->getAttrU64(AOSTAG_DOCID,0), sdoc->getAttrStr(AOSTAG_OBJID),
			"zky_subcnttype", subType, false, false, true);

	AosEventMgr::getSelf()->procEvent(eAosHookCounterBySdocIDs,
			   eAosHkptCounter2, sdoc_objids, rdata);
	AosSmartDocObj::procSmartdocsStatic(sdoc_objids, rdata);

	OmnString content = rdata->getResults();
	if (content != "")
	{
		OmnString contents ="<Contents><record ";
		content << "zky_count"<<"=\"" << content <<"\""
				<< "/></Contents>";
		rdata->setResults(contents);
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;
	}
	
	AosSetError(rdata, "internal_error");
	AOSLOG_LEAVE(rdata);
	return false;
}
