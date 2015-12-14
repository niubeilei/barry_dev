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
// 05/27/2011	Created by Jackie Zhao
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/GetLanguage.h"

#include "SearchEngine/DocServerCb.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosGetLanguage::AosGetLanguage(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GET_LANGUAGE, 
		AosSeReqid::eGetLanguage, rflag)
{
}


bool 
AosGetLanguage::proc(const AosRundataPtr &rdata)
{
	// This function moves a message from one container to another.
	//<request reqid = "" >
	//		<item name='zky_siteid'><![CDATA[100]]></item>
	//		<item name='operation'><![CDATA[serverreq]]></item>
	//		<item name='trans_id'><![CDATA[8]]></item>
	//		<item name='zky_ssid'><![CDATA[xxx]]></item>
	//		<item name='reqid'><![CDATA[get_language]]></item>
	//		<item name='args'><![CDATA[languagetype="Chinese" ]]></item>
	//</request>
	
	AOSLOG_ENTER_R(rdata, false);

	map<OmnString, OmnString> cookies; 
	if(!rdata->getCookies(cookies))
	{
		rdata->setError() << "Missing Cookies";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	//1.get parameters
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args != "")
	{
		OmnString languagetype;
		AosParseArgs(args, "languagetype", languagetype);
		AosSessionObjPtr session = rdata->getSession();
		if(languagetype != "" && session)
		{
			session->setLanguageCode(rdata, languagetype);
		}
	}

	OmnString results;
	results << "<language>" << rdata->getLocale() << "</language>"; 
	rdata->setResults(results);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

