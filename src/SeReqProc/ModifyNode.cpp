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
// 09/30/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ModifyNode.h"

#include "EventMgr/EventMgr.h"
#include "SEInterfaces/DocClientObj.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/OnlineMgr.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosModifyNode::AosModifyNode(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_MODIFYNODE, 
		AosSeReqid::eModifyNode, rflag)
{
}


bool 
AosModifyNode::proc(const AosRundataPtr &rdata)
{
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
	
	OmnString sdoc_objid;
	AosParseArgs(args, "sdocobjid", sdoc_objid);
	
	if (sdoc_objid == "")
	{
		rdata->setError() << "Missing sdocobjid!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
		
	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDocByObjid(sdoc_objid, rdata);
	if(!sdoc)
	{
		rdata->setError() << "smdoc operate error!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	 AosXmlTagPtr objdef = root->getFirstChild("objdef");
	 if (objdef)
	 {
		 AosXmlTagPtr sourcedoc = objdef->getFirstChild();
		 if (!sourcedoc)
		 {
			rdata->setError() << "Missing objdef!";
		    AOSLOG_LEAVE(rdata);
		    return false;
		 }
		 AosXmlParser parser;
		 sourcedoc = parser.parse(sourcedoc->toString(), "" AosMemoryCheckerArgs);
		 rdata->setSourceDoc(sourcedoc, true);
		
		 AosXmlTagPtr targetdoc = objdef->getNextChild();
		 if (!targetdoc)
		 {
			rdata->setError() << "Missing objdef!";
		    AOSLOG_LEAVE(rdata);
		    return false;
		 }
		 AosXmlParser parser1;
		 targetdoc = parser1.parse(targetdoc->toString(), "" AosMemoryCheckerArgs);
		 rdata->setTargetDoc(targetdoc, true);
	 }
	
	AosSmartDocObj::procSmartdocsStatic(sdoc_objid, rdata);

	OmnString content = rdata->getResults();
	if (content != "")
	{
		OmnString contents ="<Contents><record>";
		contents << content
				<< "</record></Contents>";
		rdata->setResults(contents);
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;
	}
	
	AOSLOG_LEAVE(rdata);
	return false;
}
