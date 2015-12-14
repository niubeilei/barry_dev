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
#include "SeReqProc/ModifyAttr.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosModifyAttr::AosModifyAttr(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_MODIFY_ATTR, 
		AosSeReqid::eModifyAttr, rflag)
{
}


bool 
AosModifyAttr::proc(const AosRundataPtr &rdata)
{
	//  <request ...>
	//      <item name="args">objid:xxx, attrname:xxx,
	//          value_unique:xxx, docid_unique:xxx</item>
	//      <item name="value">value</item>
	//      <item name="default">value</item>
	//  </request>
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString contents;
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		rdata->setError() << "Missing args!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString objid, attrname, value_unique, docid_unique;
	AosParseArgs(args, "objid", objid, "attrname", attrname, 
			"value_unique", value_unique, "docid_unique", docid_unique);

	if (objid == "")
	{
		rdata->setError() << "Missing objid";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (attrname == "")
	{
		rdata->setError() << "Missing attribute name"; 
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (!doc)
	{
		rdata->setError() << "Object not found: " << objid;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString value = root->getChildTextByAttr("name", "value");
	OmnString dft = root->getChildTextByAttr("name", "default");

	AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
		 doc->getAttrU64(AOSTAG_DOCID, 0), objid, 
		attrname, value, (value_unique == "true"), 
		(docid_unique == "true"), true);
	AOSLOG_LEAVE(rdata);
	return true;
}

