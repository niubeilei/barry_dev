////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 08/29/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SQLApi/SqlApi.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"



AosSqlApi::AosSqlApi()
{
}


AosSqlApi::~AosSqlApi()
{
}


bool
AosSqlApi::login(
			const OmnString &username,
			const OmnString &passwd,
			const OmnString &ctnr_name,
			OmnString &ssid,
			u64 &urldocid,
			u64 &userid,
			AosXmlTagPtr &userdoc,
			const u32 siteid,
			const OmnString &cid,
			const OmnString &rootssid)	
{
	u32 trans_id = mTransId++;
	OmnString obj;
	obj << "<embedobj "
		<< "getsysurl" << "=\"" << "true"<<"\" "
		<< AOSTAG_USERNAME << "=\""<< username <<"\" "
		<< " opr=\"login\" "
		<< AOSTAG_CLOUDID << "=\"" << cid <<"\" "
		<< AOSTAG_HPVPD << "=\"yyy_room_frame\" "
		<< AOSTAG_LOGIN_VPD << "=\"yyy_login\" "
		<< AOSCONFIG_CTNR << "=\""<< ctnr_name <<"\"><" 
		<< AOSTAG_PASSWD <<">"<< passwd <<"</"<< AOSTAG_PASSWD
		<<"></embedobj>";

	OmnString req = "<request>";
	req << "<command>" << obj <<"</command>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
	 	<< "<item name='loginobj'>true</item>"
		<< "</request>";
	// <request>
	// <item name='zky_siteid'><![CDATA[mSiteid]]></item>
	// 		<item name='operation'><![CDATA[serverCmd]]></item>
	// 		<item name='loginobj'><![CDATA[true]]></item>
	// 		<item name='trans_id'><![CDATA[5]]></item>
	// 		<command>
	// 			<embedobj container="yunyuyan_account" zky_hpvpd="yyy_room_frame" 
	// 				rattrs="zky_category|sep418|zky_hpvpd|sep418|zky_objimg|sep418|zky_fans|sep418|zky_abmctnr|sep418|zky_realnm|sep418|zky_uname"
	// 				zky_lgnvpd="yyy_login" opr="login" zky_uname="yuhui"><zky_passwd>&lt;![CDATA[12345]]&gt;</zky_passwd>
	// 			</embedobj>
	// 		</command>
	// 		<zky_cookies>
	// 			<cookie zky_name="aos_userid_ck"><![CDATA[201074183101135610747320097]]></cookie>
	// 			<cookie zky_name="JSESSIONID"><![CDATA[999454DA3E45597FD08E8C9B0D6482C2]]></cookie>
	// 		</zky_cookies>
	// </request>

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	AosXmlTagPtr contents = resproot ->getFirstChild("Contents");
	AosXmlTagPtr record = contents->getFirstChild();
	ssid = record->getAttrStr(AOSTAG_SESSIONID);
	userid = record->getAttrU64(AOSTAG_DOCID, 0);

	AosXmlTagPtr doc = parser.parse(record->toString(), "" AosMemoryCheckerArgs);
	aos_assert_r(doc, false);
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid!="", false);
	OmnString ss = ssid;
	if (rootssid != "") ss = rootssid;
	urldocid = doc->getAttrU64("zky_sysurldocid", 0);
	userdoc = retrieveDocByObjid(siteid, ss, urldocid, objid, false);
	aos_assert_r(userdoc, false);
	userid = userdoc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(userid, false);
	return true;
}


bool
AosSqlApi::logout(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const AosRundataPtr &rdata)
{
	u32 trans_id = mTransId++;

	rdata->setOk();
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>logout</item>"
		<< "<item name='trans_id'>"<< trans_id << "</item>";

	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>"
			<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
			<< "</request>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>"
			<< "</request>";
	}
			
	OmnString errmsg;
	OmnString resp;
	if (!mConn)
	{
		rdata->setError() << "Missing connection to server!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		rdata->setError() << "Failed sending request to server: " << errmsg;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (resp == "")
	{
		rdata->setError() << "Failed receiving response";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// if (rdata->withLog()) rdata->appendLog(resp);

	AosXmlParser parser;
	AosXmlTagPtr resproot =  parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Failed parsing response!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool exist;
	rslt = (child->xpathQuery("status/code", exist, "") == "200");
	if (!rslt)
	{
		rdata->setError() << "Failed logging out: " 
			<< child->xpathQuery("status/code", exist, "");
		return false;
	}
	rdata->setOk();
	return true;
}


bool
AosSqlApi::getLoginObj(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		AosXmlTagPtr &loginobj)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">resolve_url</item>"
		<< "<item name=\"loginobj\">true</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << sessionId << "</item>";
	req << "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	AosXmlTagPtr status = child->getFirstChild("status");
	aos_assert_r(status, false);
	loginobj = status->getFirstChild("zky_lgnobj");
	return true;
}


bool
AosSqlApi::checkLogin(
		const u32 siteid,
		const OmnString &cid,
		const OmnString &ssid,
		const u64 &urldocid)
{
	u32 trans_id = mTransId++;
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>" << siteid << "</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>checklogin</item>"
		<< "<item name='trans_id'>" << trans_id << "</item>"
		<< "<objdef>"
		<< "<Contents>"
		<< "<ssid>" << ssid << "</ssid>"
		<< "<cid>" << cid << "</cid>"
		<< "</Contents>"
		<< "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return exist;
}
#endif
