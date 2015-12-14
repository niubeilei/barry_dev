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
// Modification History:
// 07/15/2011	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "AppVar/WebAppMgr.h"

#include "alarm_c/alarm.h"
#include "AppVar/WebApp.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "HtmlServer/HtmlRetrieveSites.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Objid.h"
#include "SEUtil/SysTags.h"
#include "Util/StrSplit.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

#include <set>
using namespace std;

int AosWebAppMgr::smNum = 100;

bool 
AosWebAppMgr::createApp(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	// This function creates a Web App. Creating a web app means:
	// 1. Check whether the requester has the right to create an application;
	// 2. Set the doc otype to AOSOTYPE_WEBAPP
	// 3. If the doc does not have a parent container, put the doc into
	//    AOSTAG_LOSTnFOUND.
	// 4. Attach the following tags:
	// 		AOSSYSTAG_WEBAPP
	//		
	//	<doc ...
	//		AOSTAG_TAG="xxx,xxx,..."
	//		...>
	//		...
	//	</doc>
	// 
	// 5. search all the vpds ,build to a xml,and save it 
	//    get the docid to the attr AOSTAG_XMLDOCID
	//
	// 6. cope the smart docs and acccess control  to the ctnr
	//    
	// 7. Create the doc. 
	
	// 1. Check Security
	/*if (!AosSecurityMgr::getSelf()->checkCreateApp(doc, rdata))
	{
		return false;
	}*/
    
	// 2. set AOSTAG_OTYPE
	doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_WEBAPP);

    // 3. set zky_pctrs , zky_objid
	OmnString ctnr_name = doc->getAttrStr(AOSTAG_PARENTC);
	if(ctnr_name == "")
	{
		ctnr_name = AOSTAG_CTNR_LOSTFOUND;
		doc->setAttr(AOSTAG_CTNR_DEFAULT_APPS, ctnr_name);
	}
 	
	OmnString vpdname = doc->getAttrStr("app_name");
	if (vpdname == "")
	{
		rdata->setError() << "Missing VPD Name";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	doc->setAttr(AOSTAG_OBJID, vpdname);

	// 4. attach the tag AOSSYSTAG_WEBAPP
	OmnString tags = doc->getAttrStr(AOSTAG_TAG);
	OmnString newtags;
	if(tags == "")
	{
		newtags = AOSSYSTAG_WEBAPP;
	}
	else
	{
		set<OmnString>  tagset;
		AosStrSplit split;
		bool finished, haswebapp;
		OmnString pairs[smNum];
	    int num = split.splitStr(tags.data(), ",", pairs, smNum, finished);
		for (int i=0; i<num; i++)
		{
			if(pairs[i] == AOSSYSTAG_WEBAPP)
			{
			    haswebapp = true;
				break;
			}
		}
		if(!haswebapp)
		{
			newtags << tags << "," << AOSSYSTAG_WEBAPP;
		}
	}
	doc->setAttr(AOSTAG_TAG, newtags);

	// 5. search the vpds ,and create the xmldoc 
	//    get the docid to doc attr AOSTAG_XMLDOCID
    OmnString homepage = doc->getAttrStr("zky_homepage");
	aos_assert_r(homepage != "", false);
	OmnString xmlStr, errmsg;
	AosHtmlRetrieveSites sites;
	bool rslt = sites.retrieveAllSites(xmlStr, homepage, errmsg, rdata);
	aos_assert_r(rslt, false);
	AosXmlParser parser;
	AosXmlTagPtr xmlDoc = parser.parse(xmlStr, "" AosMemoryCheckerArgs);
	xmlDoc->setAttr(AOSTAG_HPCONTAINER, ctnr_name);
	xmlDoc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	xmlDoc->setAttr(AOSTAG_PUBLIC_DOC, "true");
	AosXmlTagPtr createDoc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, xmlDoc->toString(),
			          rdata->getCid(), "", true, true, false, false, false, true);
	//AosXmlTagPtr createDoc = AosDocClient::getSelf()->createDocSafe3(rdata, xmlDoc, "", "",
	//		     true, false, false, false, false, true, true);
	aos_assert_r(createDoc, false);
	OmnString zky_docid = createDoc->getAttrStr(AOSTAG_DOCID);
	aos_assert_r(zky_docid != "", false);
	doc->setAttr(AOSTAG_XMLDOCID, zky_docid);
	return true;
}

bool 
AosWebAppMgr::deleteApp(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	// this function is to delete the app
	// delete app has two cases
	// 1. really delete
	// 2. temporary delete
	aos_assert_r(doc, false);
	OmnString inst_objid = doc->getChildTextByAttr("name", "objid");
	aos_assert_r(inst_objid != "", false);
	AosXmlTagPtr inst_doc = AosDocClientObj::getDocClient()->getDocByObjid(inst_objid, rdata);
	aos_assert_r(inst_doc, false);
	OmnString url = inst_doc->getNodeText("zky_url");
	aos_assert_r(url != "", false);
	OmnString url_objid = AosObjid::createUrlObjid(url);
	aos_assert_r(url_objid != "", false);
	AosXmlTagPtr urldoc = AosDocClientObj::getDocClient()->getDocByObjid(url_objid, rdata);
	aos_assert_r(urldoc, false);
	OmnString delinfo = doc->getChildTextByAttr("name", "dmid");
	aos_assert_r(delinfo != "", false);
	bool rslt;
	if (delinfo == "false")
	{
		urldoc->setAttr("zky_delapp", "true");
		rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, urldoc, "", false);
		//aos_assert_r(rslt, false);
		//rslt = AosWebApp::deleteApp(url, delinfo, rdata);
	}
	else
	{
		OmnString docid = urldoc->getAttrStr(AOSTAG_DOCID);
		OmnString objid = urldoc->getAttrStr(AOSTAG_OBJID);
		rslt = AosDocClientObj::getDocClient()->deleteObj(rdata, docid, objid, "", true);
		if (rslt)
		{
			docid = inst_doc->getAttrStr(AOSTAG_DOCID);
			objid = inst_doc->getAttrStr(AOSTAG_OBJID);
			rslt = AosDocClientObj::getDocClient()->deleteObj(rdata, docid, objid, "", true);
		}
		//aos_assert_r(rslt, false);
		//rslt = AosWebApp::deleteApp(url, delinfo, rdata);
	}
	
	return rslt;
}

bool
AosWebAppMgr::resetApp(const OmnString &url, const AosRundataPtr &rdata)
{
	aos_assert_r(url != "", false);
	AosXmlTagPtr inst_doc = AosDocClientObj::getDocClient()->getDocByObjid(url, rdata);
	aos_assert_r(inst_doc, false);
	OmnString newurl = inst_doc->getNodeText("zky_url");
	aos_assert_r(newurl != "", false);
	OmnString url_objid = AosObjid::createUrlObjid(newurl);
	aos_assert_r(url_objid != "", false);
	AosXmlTagPtr urldoc = AosDocClientObj::getDocClient()->getDocByObjid(url_objid, rdata);
	aos_assert_r(urldoc, false);
	urldoc->setAttr("zky_delapp", "false");
	bool rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, urldoc, "", false);
	//bool rslt = AosWebApp::resetApp(url, rdata);	
	return rslt;
}
