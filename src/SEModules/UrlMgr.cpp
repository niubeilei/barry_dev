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
#include "SEModules/UrlMgr.h"

#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "MultiLang/LangTermIds.h"
#include "Porting/TimeOfDay.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Objid.h"
#include "SEUtil/ValueDefs.h"
#include "SEUtil/Docid.h"
#include "SearchEngine/Ptrs.h"
#include "SEServer/SeReqProc.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Security/Session.h"
#include "SEModules/ObjMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/SecurityMgrObj.h"

const OmnString sgParmName_Vpd = "v";
const OmnString sgParmName_Obj = "o";
const OmnString sgParmName_Svpd = "s";

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
	return true;
}

bool
AosUrlMgr::createSysUrl(const AosRundataPtr &rdata)
{
	// This function tries to create a URL. 
	// All published URLs are indexed by the following IIL:
	// 	AOSZTG_URL + siteid
	u32 siteid = rdata->getSiteid();
	rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));	// test
	
	OmnString url_objid = AOSTAG_SYS_URL;
	AosXmlTagPtr thedoc = AosDocClientObj::getDocClient()->getDocByObjid(url_objid, rdata);

	if (thedoc)
	{
		aos_assert_r(thedoc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_URL, false);
		return true;
	}

	OmnString docstr = "<url ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_URL
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_PARENTC << "=\"" << "url"
		<< "\" " << AOSTAG_OBJID << "=\"" << url_objid
		<< "\" " << AOSTAG_CTNR_PUBLIC << "=\"" << "true"
		<< "\" />";

	AosXmlParser parser;
	thedoc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_rr(thedoc, rdata, false);

	u64 docid = AOSDOCID_SYSURL;
	thedoc->setAttr(AOSTAG_DOCID, docid);
	rdata->setReceivedDoc(thedoc, true);
	AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe3(rdata, thedoc, 
			"", "", true, false, false, false, true, true, true);
	if (!dd)
	{
		// The user has been used by someone else
		return false;
	}
	rdata->setOk();
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
	u32 siteid = rdata->getSiteid();
	AosSessionObjPtr sdoc = rdata->getSession();

	// 1. Retrieve the URL
	OmnString url = objdef->getNodeText(AOSTAG_URL);
	if (url == "")
	{
		AosSetError(rdata, AOSLT_MISSING_URL);
		return false;
	}
	OmnString url_objid = AosObjid::createUrlObjid(url);

	// Chen Ding, 07/01/2011
	AosXmlTagPtr thedoc = objdef->clone(AosMemoryCheckerArgsBegin);
	aos_assert_rr(thedoc, rdata, false);
	thedoc->setAttr(AOSTAG_OBJID, url_objid);
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

	// Check whether it is allowed
	if (!AosSecurityMgrObj::getSecurityMgr()->checkCreateUrl(url, rdata))
	{
		// It is not allowed.
		return false;
	}

	AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe3(rdata, thedoc, 
			"", "", true, false, false, false, true, true, true);
	if (!dd)
	{
		// The user has been used by someone else
		return false;
	}
	rdata->setOk();
	return true;
}


bool
AosUrlMgr::overrideUrl(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &objdef)
{
	AosSessionObjPtr sdoc = rdata->getSession();
	// Overriding means modify
	OmnString url = objdef->getNodeText(AOSTAG_URL);
	OmnString url_objid = AosObjid::createUrlObjid(url);
	objdef->setAttr(AOSTAG_OBJID, url_objid);
	
	// u64 userid;
	// if (sdoc) userid = sdoc->getUserid();
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
		const OmnString &url, 
		const OmnString &full_url,
		const OmnString &query_str,
		OmnString &vpdname, 
		OmnString &objname,
		OmnString &svpd, 
		AosXmlTagPtr &urldoc,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 12/17/2011
	// 'url' can be in the form:
	// 	xxx.xxx.xxx/xxx/xxx/...?xxx
	vector<OmnString> names, values;
	vpdname = "";
	OmnString url_home = full_url;
	parseUrl(query_str, names, values);
	OmnString local_url = url;
	OmnString url_objid = AosObjid::createUrlObjid(local_url);
	urldoc = AosDocClientObj::getDocClient()->getDocByObjid(url_objid, rdata);
	if (!urldoc)
	{
		AosSetError(rdata, AOSLT_URL_UNDEFINED);
		OmnAlarm << rdata->getErrmsg() << ". URL: " << url 
			<< ", objid: " << url_objid << enderr;
		return false;
	}
	
	// Add By Brian Zhang 06/01/2011
	OmnString type = urldoc->getAttrStr("type");
	if (names.size() > 0 && values.size() >0)
	{
		if (type != AOSVALUE_PARMED_URL)
		{
			AosSetError(rdata, AOSLT_URL_TYPE_MISMATCH);
			OmnAlarm << rdata->getErrmsg() << ". URL: " << url 
				<< ", objid: " << url_objid << enderr;
			return false;
		}
	}
	// 	<doc urltype="xxx">
	// 		<vpdname>xxx</vpdname>
	// 		<objname>xxx</objname>
	// 		...
	// 	</doc>
	if (type == "" || type == AOSVALUE_NORMAL_URL)
	{
		// This is a backward compatible URL. In the future, there shall 
		// not be such URLs.
		vpdname = urldoc->getNodeText(AOSTAG_VPDNAME);
		objname = urldoc->getNodeText(AOSTAG_OBJNAME);
		svpd = urldoc->getNodeText(AOSTAG_SVPD);
		return true;
	}

	if (type == AOSVALUE_MULTIHOME_URL)
	{
		bool rslt = checkHomes(urldoc, url_home, vpdname, objname, svpd, rdata);
		if (rslt) return true;

		// Did not find it. Try the full one
		AosSetError(rdata, AOSLT_URL_UNDEFINED);
		OmnAlarm << rdata->getErrmsg() << ". URL: " << url << enderr;
		return false;
	}

	if (type == AOSVALUE_PARMED_URL)
	{
		if (names.size() != values.size())
		{
			AosSetError(rdata, AOSLT_INTERNAL_ERROR);
			OmnAlarm << rdata->getErrmsg() << ": " << url << enderr;
		}
			
		bool rslt = checkHomes(urldoc, url_home, vpdname, objname, svpd, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AOSLT_INVALID_URL);
			OmnAlarm << rdata->getErrmsg() << ". URL: " << url << enderr;
			return false;
		}

		// The home matched. 
		// The url assumes the following form:
		// 	xxx.xxx.xxx/xxx/xxx/...?v=vpdname&o=objid
		for (u32 i=0; i<names.size(); i++)
		{
			if (names[i] == sgParmName_Vpd)
			{
				vpdname = values[i];
			}
			else if (names[i] == sgParmName_Obj)
			{
				objname = values[i];
			} 
			else if (names[i] == sgParmName_Svpd)
			{
				svpd = values[i];
			}
		}

		if (vpdname != "")
		{
			return true;
		}

		vpdname = urldoc->getNodeText(AOSTAG_VPDNAME);
		objname = urldoc->getNodeText(AOSTAG_OBJNAME);
		svpd = urldoc->getNodeText(AOSTAG_SVPD);

		if (vpdname != "") return true;

		AosSetError(rdata, AOSLT_FAILED_RESOLVE_URL);
		return false;
	}

	// Did not find it. Try the full one, if not yet
	AosSetError(rdata, AOSLT_INVALID_URL_TYPE);
	OmnAlarm << rdata->getErrmsg() << ": " << url << ":" << full_url << ":" << type << enderr;
	return false;
}


// Chen Ding, 12/17/2011
bool
AosUrlMgr::parseUrl(
		const OmnString &query_str, 
		vector<OmnString> &names,
		vector<OmnString> &values)
{
	// 'url' can be in the form:
	// 	xxx.xxx.xxx/xxx/xxx/...?xxx
	// In the current implementations, we only support parmed URLs in the
	// following format:
	// 	xxx.xxx.xxx/xxx/xxx/...?xxx
	// where the portion before '?' is a publiced URL and the portion after
	// '?' is the parms. In the future, we may want to support the format:
	// 	xxx.xxx.xxx/xxx/
	
	names.clear();
	values.clear();

	if (query_str.length() <= 0) return true;

	// Decode the contents
	OmnString new_query_str = decodeUrl(query_str);
	aos_assert_r(new_query_str.length() > 0, false);
	const char *data1 = new_query_str.data();

	// The format is:
	// 	name[=[value]]&...
	vector<OmnString> pairs;
	bool finished=false;
	const int nn = AosStrSplit::splitStrByChar(data1, "&", pairs, eMaxParms, finished);
	aos_assert_r(finished, false);
	if (nn <= 0) return true;

	for (int i=0; i<nn; i++)
	{
		if (pairs[i] != "")
		{
			vector<OmnString> pp;
			int mm = AosStrSplit::splitStrByChar(pairs[i].data(), "=", pp, 2, finished);
			switch (mm)
			{
			case 1:
				 names.push_back(pp[0]);
				 values.push_back("");
				 break;

			case 2:
				 names.push_back(pp[0]);
				 values.push_back(pp[1]);
				 break;
			}
		}
	}
	
	return true;
}


OmnString
AosUrlMgr::decodeUrl(const OmnString &url)
{
	return url;
}


bool
AosUrlMgr::checkHomes(
		const AosXmlTagPtr &urldoc, 
		const OmnString &url_home, 
		OmnString &vpdname, 
		OmnString &objname,
		OmnString &svpd, 
		const AosRundataPtr &rdata)
{
	// The URL supports multiple homes. The doc should be in the form:
	// 	<urldoc ...>
	// 		<homes>
	// 			<home ...>home</home>
	// 			<home ...>home</home>
	// 			...
	// 		</homes>
	// 	</urldoc>
	AosXmlTagPtr homes = urldoc->getFirstChild(AOSTAG_HOMES);
	if (!homes) return true;

	AosXmlTagPtr record = homes->getFirstChild();
	while (record)
	{
		OmnString home = record->getNodeText();
		if (home == "")
		{
			OmnAlarm << "Invalid URL doc: " << urldoc->toString() << enderr;
		}
		else
		{
			if (home == url_home)
			{
				// Found it. 
				vpdname = urldoc->getNodeText(AOSTAG_VPDNAME);
				objname = urldoc->getNodeText(AOSTAG_OBJNAME);
				svpd = urldoc->getNodeText(AOSTAG_SVPD);
				return true;
			}
		}
		record = homes->getNextChild();
	}

	return false;
}


// Chen Ding, 2013/12/07
/*
AosXmlTagPtr
AosUrlMgr::getUrlMgrDoc(
		const AosRundataPtr &rdata, 
		const OmnString &url)
{
	// For access control reasons, we need a doc for URLs and the doc
	// must have a container. The container can define the access
	// controls. There will be one and only one URL Management Doc
	// for each database. If the doc does not exist, it will
	// create it automatically.
	OmnString database = rdata->getDatabase();
	if (database == "")
	{
		// The user has not logged in yet. It should return the default
		// URL doc. If it is not there yet, it will create the doc.
		return getDefaultUrlMgmtDoc(rdata, url);
	}

	OmnString objid = AosComposeUrlMgmtDocObjid(database);
	aos_assert_rr(objid != "", rdata, 0);
	AosXmlTagPtr doc = AosGetDocByObjid(rdata, objid);
	if (doc) return doc;

	// The doc does not exist yet. Need to Create it.
	// First, check whether the container exists.
	OmnString ctnr_objid = AosComposeUrlMgmtCtnrObjid(database);
	aos_assert_rr(ctnr_objid != "", rdata, 0);
	AosXmlTagPtr ctnr = AosGetDocByObjid(rdata, ctnr_objid);
	if (!ctnr)
	{
		// The container does not exist yet. Create it.
		ctnr = AosCreateUrlMgmtDocCtnr(rdata, database, ctnr_objid);
		aos_assert_rr(ctnr, rdata, false);
	}

	return AosCreateUrlMgmtDoc(rdata, database, objid, ctnr);
}
*/

