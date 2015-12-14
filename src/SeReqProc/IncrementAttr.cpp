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
#include "SeReqProc/IncrementAttr.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosIncrementAttr::AosIncrementAttr(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_INCREMENT_ATTR, 
		AosSeReqid::eIncrementAttr, rflag)
{
}


bool 
AosIncrementAttr::proc(const AosRundataPtr &rdata)
{
	// It increments the value of an attribute of a specified object
	// by one.
	// 'args' are in the form:
	//      "objid=xxx,attrname=xxx"
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	OmnString objid, aname;
	AosParseArgs(args, "objid", objid, "attrname", aname);
	if (objid == "")
	{
		rdata->setError() << "Missing objid!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (aname == "")
	{
		rdata->setError() << "Missing Attr!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (!doc)
	{
		rdata->setError() << "Missing obj!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	int64_t value = doc->getAttrInt64(aname, 0);
	value++;
	OmnString vstr;
	vstr << value;
	AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, doc->getAttrU64(AOSTAG_DOCID, 0), 
			objid, aname, vstr, false, false, true);
	OmnString contents = "<Contents>";
	contents << "<record " << AOSTAG_OBJID << "=\"" << objid << "\" "
		    << "value=\"" << value << "\"/>"
			<< "</Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

