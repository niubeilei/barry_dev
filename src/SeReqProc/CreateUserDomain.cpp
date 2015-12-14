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
#include "SeReqProc/CreateUserDomain.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosCreateUserDomain::AosCreateUserDomain(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATE_USERDOMAIN, 
		AosSeReqid::eCreateUserDomain, rflag)
{
}


bool 
AosCreateUserDomain::proc(const AosRundataPtr &rdata)
{
	// It creates a user domain. A user domain is defined by an XML doc. 
	// The input is in the form:
	// 	<request ...>
	// 		<objdef .../>
	// 	</request>
	//

	AOSLOG_ENTER_R(rdata, false);     

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	//OmnString cid = AosCloudidSvr::getCloudid(mUserid);
	//OmnString cid = rdata->getCid();

	AosXmlTagPtr xmldoc = root->getFirstChild("objdef");
	if (!xmldoc)
	{
		rdata->setError() << "Missing objdef";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	AosXmlTagPtr doc = xmldoc->getFirstChild();
	if (!doc)
	{
		rdata->setError() << "Missing doc";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	rdata->setRetrievedDoc(doc, false);
	//retrieve the smart docs from args;
	OmnString sobjids = root->getChildTextByAttr("name", "args");
	if (sobjids != "")
	{
		OmnStrParser1 parser(sobjids, ",");
		OmnString sdocid;
		while((sdocid = parser.nextWord()) != "")
		{
			AosSmartDocObj::procSmartdocsStatic(sdocid, rdata);
		}
	}

	// Set the doc
	doc->setAttr(AOSTAG_PARENTC, AOSCTNR_USERDOMAIN);
	doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_USERDOMAIN);
	doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	doc->setAttr(AOSTAG_PUBLIC_DOC, "true");

	OmnString docstr = doc->toString();
	
	AosXmlTagPtr newdoc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, docstr, "",
								"", true, false,false, false, false, false);

	if (!newdoc)
	{
		rdata->setError() << "Fail To Create doc";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString contents = "<Contents>";
		contents << "<root sengt_docid=\"" << newdoc->getAttrU64("sengt_docid", AOS_INVDID) << "\""
				 << " zky_docid=\"" << newdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) << "\""
				 << " zky_objid=\"" << newdoc->getAttrStr(AOSTAG_OBJID, AOS_INVDID) << "\""
				 << "/>";
		contents << "</Contents>";

	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}	
