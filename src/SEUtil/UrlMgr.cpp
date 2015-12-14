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
// 07/29/2010: Created by James Kong
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to SEModules. Chen Ding, 12/22/2011



#include "SEUtil/UrlMgr.h"

#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/TimeOfDay.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Objid.h"
#include "SearchEngine/Ptrs.h"
#include "SEServer/SeReqProc.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosUrlMgrSingleton,
                 AosUrlMgr,
                 AosUrlMgrSelf,
                "AosUrlMgr");


AosUrlMgr::AosUrlMgr()
:
mIsStopping(false)
{
}


AosUrlMgr::~AosUrlMgr()
{
}


bool      	
AosUrlMgr::start()
{
	return true;
}


bool        
AosUrlMgr::stop()
{
	mIsStopping = true;
	return true;
}


bool
AosUrlMgr::config(const AosXmlTagPtr &def)
{
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid("100");
	rdata->setUserid(301);
	bool url_used = false;
	AosXmlTagPtr urlTag = def->getFirstChild("UrlMgr");
	if(urlTag)
	{
		AosXmlTagPtr objdef = urlTag->getFirstChild("objdef");
		if(objdef)
		{
			createUrl(rdata, objdef, url_used);
		}
	}
	return true;
}


bool	
AosUrlMgr::createUrl(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &objdef,
		bool &url_used) 
{
	// This function tries to create a URL. 
	// All published URLs are indexed by the following IIL:
	// 	AOSZTG_URL + siteid
	OmnString siteid = rdata->getSiteid();
	AosSessionPtr sdoc = rdata->getSession();
	OmnString errmsg;
	AosXmlRc errcode;

	// 1. Retrieve the URL
	u64 userid = 0;
	if (sdoc) userid = sdoc->getUserid();
	errcode = eAosXmlInt_General;
	OmnString url = objdef->getNodeText(AOSTAG_URL);
	if (url == "")
	{
		errmsg = "Missing URL";
		rdata->setError() << errmsg;
		return true;
	}
	OmnString url_objid = AosObjid::createUrlObjid(url);

	// Chen Ding, 07/01/2011
	AosXmlTagPtr thedoc = objdef->clone(AosMemoryCheckerArgsBegin);
	aos_assert_rr(thedoc, rdata, false);
	thedoc ->setAttr(AOSTAG_OBJID, url_objid);
	thedoc->setAttr(AOSTAG_OTYPE, AOSOTYPE_URL);
	thedoc->setAttr(AOSTAG_PARENTC, "url");
	thedoc->setAttr(AOSTAG_SITEID, siteid);
	AosXmlTagPtr olddoc = AosDocClientObj::getDocClient()->getDocByObjid(url_objid, rdata);
	if (olddoc)
	{
		//if url doc is exist.do nothing.
		url_used = true;
		rdata->setOk();
		return true;
	}

	AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe3(rdata, thedoc, 
			"", "", true, false, false, false, true, true, true);
	if (!dd)
	{
		// The user has been used by someone else
		errcode = eAosXmlInt_ObjidNotUnique;
		rdata->setError(errcode);
		return false;
	}
	rdata->setOk();
	return true;
}


//--Ketty
/*bool
AosUrlMgr::overrideUrl(
		const AosXmlTagPtr &root,
		const OmnString &siteid, 
		const AosSessionPtr &sdoc, 
		const AosXmlTagPtr &objdef,
		AosXmlRc &errcode, 
		OmnString &errmsg)*/
bool
AosUrlMgr::overrideUrl(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &objdef)
{
	AosSessionPtr sdoc = rdata->getSession();
	// Overriding means modify
	OmnString url = objdef->getNodeText(AOSTAG_URL);
	OmnString url_objid = AosObjid::createUrlObjid(url);
	objdef->setAttr(AOSTAG_OBJID, url_objid);
	
	u64 userid;
	if (sdoc) userid = sdoc->getUserid();
	//AosXmlDocPtr header;  //Linda, header
	//int ttl = 0;
	//bool rslt = AosDocServer::getSelf()->modifyObj(siteid, AOSAPPNAME_SYSTEM, 
	//		userid, root, objdef, 0, header, "", false, errcode, errmsg, ttl);
	// Chen Ding, 2011/01/27
	// bool rslt = AosDocServer::getSelf()->modifyObj(siteid, AOSAPPNAME_SYSTEM, 
	// 		userid, root, objdef, 0, "", false, errcode, errmsg, ttl);
	//--Ketty
	//bool rslt = AosDocServer::getSelf()->modifyObj(siteid, AOSAPPNAME_SYSTEM, 
	//		userid, root, objdef, "", false, errcode, errmsg, ttl);
	bool rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, objdef, "", false);

	aos_assert_r(rslt, false);
	rdata->setOk();
	return true;
}
        

bool
AosUrlMgr::resolveUrl(
		const OmnString &siteid,
		const OmnString &url, 
		OmnString &vpdname, 
		OmnString &objname,
		OmnString &svpd, 
		const AosRundataPtr &rdata)
{
	OmnString newurl = url;
	OmnString url_objid = AosObjid::createUrlObjid(newurl);
	//AosXmlTagPtr urldoc = AosIILClientObj::getIILClient()->getDoc(siteid, url_objid);
	AosXmlTagPtr urldoc = AosDocClientObj::getDocClient()->getDocByObjid(url_objid, rdata);
	if (!urldoc)
	{
OmnScreen << "UrlMgr :: Faild to retrieve doc :: " << url_objid  << endl; 
		vpdname = "";
		return true;
	}

	// 	<doc ...>
	// 		<vpdname>xxx</vpdname>
	// 		<objname>xxx</objname>
	// 		...
	// 	</doc>
	vpdname = urldoc->getNodeText(AOSTAG_VPDNAME);
	objname = urldoc->getNodeText(AOSTAG_OBJNAME);
	svpd = urldoc->getNodeText(AOSTAG_SVPD);
	return true;
}

#endif
