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



bool
AosSqlApi::deleteObj(
		const u32 siteid,
		const OmnString &objid,
		const u64 &docid, 
		const u64 &urldocid)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">rebuildDelObj</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"" << AOSTAG_OBJID << "\">" << objid << "</item>"
		<< "<item name=\"" << AOSTAG_DOCID << "\">" << docid << "</item>"
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
	return true;
}


bool
AosSqlApi::deleteMsgContainer(
			const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &cname,
			const OmnString &container)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"reqid\">" << "delete_msgcontainer" << "</item>"
		<< "<item name=\"args\">" << "inbox=" << container << ",cname=" << cname << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
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
	if(child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	return true;
}


bool
AosSqlApi::deleteMsg(
			const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &objid,
			const OmnString &container)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"reqid\">" << "delete_msg" << "</item>"
		<< "<item name=\"args\">" << "objid=" << objid << ",container=" << container << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
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
	if(child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	return true;
}


bool
AosSqlApi::removeDocFromServer(
			const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &objid,
		    const u64 &docid)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">delObject</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (objid != "")
	{
		req << "<item name=\"objid\"><![CDATA[" << objid << "]]></item>";
	}
	if (docid)
	{
		req << "<item name=\"" << AOSTAG_DOCID << "\">" << docid <<"</item>";
	}
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
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if(child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	return true;
}


bool
AosSqlApi::deleteDoc(
			const u32 &siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &docid)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">delObject</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req << "<item name=\"zky_docid\">" << docid <<"</item>"
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
	if(child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	return true;
}

#endif
