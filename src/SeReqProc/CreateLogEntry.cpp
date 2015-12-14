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
#include "SeReqProc/CreateLogEntry.h"

#include "API/AosApi.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SmartDoc/SMDMgr.h"
#include "SeLogUtil/LogNames.h"
#include "SEInterfaces/SmartDocObj.h"
#include "XmlUtil/XmlTag.h"


AosCreateLogEntry::AosCreateLogEntry(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATELOGENTRY, AosSeReqid::eCreateLogEntry, rflag)
{
}


bool 
AosCreateLogEntry::proc(const AosRundataPtr &rdata)
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
	if (args != "")
	{
		OmnString sdoc_objid;
		AosParseArgs(args, "objid", sdoc_objid);
		if (sdoc_objid != "")
		{
		    bool rslt = AosSmartDocObj::procSmartdocsStatic(sdoc_objid, rdata);
			aos_assert_r(rslt, false);
			AOSLOG_LEAVE(rdata);
			return true;
		}
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

	OmnString ctnr_objid = doc->getAttrStr(AOSTAG_LOG_CTNROBJID);
	if (ctnr_objid == "")
	{
		rdata->setError() << "Missing container objid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString logname = doc->getAttrStr(AOSTAG_LOGNAME);
	if (logname == "")
	{
		logname = doc->getAttrStr(AOSTAG_LOGTYPE, "");
		if (logname == "") logname = AOSTAG_DFT_LOGNAME;
	}
	
	bool needResp = doc->getAttrBool(AOSTAG_NEEDRESP, false);
	if (needResp)
	{
		//u64 logid = AosSeLogClientObj::getLogClient()->addLogWithResp(
				//ctnr_objid, logname, doc, rdata);

		u64 logid = 1;
		if (logid == 0)
		{
			rdata->setError() << "Failed to add log!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		OmnString resp = "<Contents>";
		resp << logid << "</Contents>";
		rdata->setResults(resp);
	}
	else
	{
		bool rslt = AosAddLog(rdata, ctnr_objid, logname, doc->toString());
		if (!rslt)
		{
	   		rdata->setError() << "Faild to add log!";
	   	 	OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
	    	return false;
		}
	}
	
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

