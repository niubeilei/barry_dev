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
AosSqlApi::createObj(const u32 siteid, const OmnString &docstr)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << docstr << "</objdef></request>";
			
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
	OmnString docid = child->xpathQuery("Contents/zky_docid", exist, "");
	if (docid == "")
	{
		OmnAlarm << "Failed processing: " << resp << enderr;
		return false;
	}

	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		OmnScreen <<  child->toString() << endl;
	}
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


u64
AosSqlApi::createObj(
		const u32 siteid, 
		const AosXmlTagPtr &doc,
		const OmnString &ssid,
		const u64 &urldocid)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>";
//		<< "<item name=\"zky_ssid_" << urldocid << "\">" << ssid << "</item>"
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}

	req << "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << doc->toString() << "</objdef></request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	OmnString respcode = child->xpathQuery("status/code", exist, "");
	if(respcode != "200")
	{
		OmnScreen << "Fail to create obj:  " << respcode << endl;
	}
	OmnString docid = child->xpathQuery("Contents/zky_docid", exist, "");
	u64 did = (u64)atoll(docid.data());
	aos_assert_r(did > 0, false);
	return did;
}


bool
AosSqlApi::createObj(
		const u32 siteid, 
		const char *docstr,
		const OmnString &ssid,
		const u64 &urldocid)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	//<< "<item name=\"zky_ssid_" << urldocid << "\">" << ssid << "</item>"
	req << "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << docstr << "</objdef></request>";
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
	OmnString docid = child->xpathQuery("Contents/zky_docid", exist, "");
	//aos_assert_r(docid != "", false);
	OmnString respcode = child->xpathQuery("status/code", exist, "");
	//aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	if(respcode != "200")
	{
		OmnScreen << "Fail to create obj:  " << respcode << endl;
	}
	return true;
}


AosXmlTagPtr
AosSqlApi::createDoc(
		const AosXmlTagPtr &doc,
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &owner_cid,
		const OmnString &resolve_objid, 
		const OmnString &saveas,
		const AosRundataPtr &rdata)
{
	return createDoc(doc, rdata->getSiteid(), ssid, urldocid, 
				owner_cid, resolve_objid, saveas, rdata);
}


AosXmlTagPtr
AosSqlApi::createDoc(
		const AosXmlTagPtr &doc,
		const u32 siteid,
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &owner_cid,
		const OmnString &resolve_objid, 
		const OmnString &saveas,
		const AosRundataPtr &rdata)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << 	"<objdef>" <<doc->toString() <<"</objdef>"
		<< 	"<command>"
		<< 		"<cmd "
		<<			"opr=\"createcobj\" "
		<<			"res_objid=\"" << resolve_objid << "\" "
		<<			AOSTAG_SAVEAS << "=\"" << saveas << "\" "
		<<		"/>"
		<<	"</command>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverCmd</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"" << AOSTAG_OWNER_CID << "\">" << owner_cid << "</item>";

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
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(rdata->getSiteid(), "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;

	if (rdata->getArg1(AOSARG_TRACK) == "true")
	{
		rdata->setArg1(AOSARG_REQUEST_STR, req);
		rdata->setArg1(AOSARG_RESPONSE_STR, resp);
	}

	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = resproot->getFirstChild("Contents");
	aos_assert_r(con, 0);
	OmnString data = con->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}


AosXmlTagPtr
AosSqlApi::createUserCtnr(
		const u32 siteid, 
		const AosXmlTagPtr &doc,
		const OmnString &ssid,
		const u64 &urldocid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverCmd]]></item>
	//	    <item name='reqid'><![CDATA[crt_usrctnr]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_userid'><![CDATA[xxx]]></item>
	// 		<item name='zky_cloudid'><![CDATA[xxx]]></item>
	//	    <objdef>
	//	    	...
	//	    </objdef>
	//	</request>
	
	u32 trans_id = mTransId++;
	OmnString docstr = doc->toString();
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
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
	req	<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<command><cmd opr='createcobj' res_objid='true'/></command>"
		<< "<objdef>" << docstr << "</objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		return 0;
	}
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}
	AosXmlTagPtr con = root->getFirstChild("Contents");
	aos_assert_r(con, 0);
	OmnString data = con->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}


AosXmlTagPtr
AosSqlApi::createUserAcct(
		const AosXmlTagPtr &doc,
		const OmnString &ssid,
		const u64 &urldocid,
		const u32 siteid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverCmd]]></item>
	//	    <item name='reqid'><![CDATA[crt_usrctnr]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_id'><![CDATA[xxx]]></item>
	// 		<item name='zky_cloudid'><![CDATA[xxx]]></item>
	//	    <objdef>
	//	    	...
	//	    </objdef>
	//	</request>
	u32 trans_id = mTransId++;
	
	OmnString objdef = doc->toString();
	/*
	OmnString objdef = "<user ";
	objdef << AOSTAG_OTYPE << "=\"" << AOSOTYPE_USERACCT << "\" "
			<< AOSTAG_OBJID << "=\"" << objid << "\" "
			<< AOSTAG_USER_CTNR << "=\"" << user_ctnr << "\" "
			<< AOSTAG_USERNAME << "=\"" << objid << "\" "
			<< AOSTAG_HPCONTAINER <<"=\"" << user_ctnr << "\" "
			<< AOSTAG_SITEID << "=\"" << siteid << "\">"
			<< "</user>";
			*/

	OmnString req = "<request>";
	req << "<objdef>" << objdef <<"</objdef>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"reqid\">create_user</item>"
		<< "<item name=\"args\">null</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>";
		
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
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = root->getFirstChild("Contents");
	aos_assert_r(con, 0);
	OmnString data = con->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}


bool
AosSqlApi::createSuperUser(
			const OmnString &requester_passwd,
			AosXmlTagPtr &userdoc,
			const u32 siteid)
{
	u32 trans_id = mTransId++;
	OmnString obj;
	obj <<"<embedobj " << AOSTAG_PASSWD "=\""<<requester_passwd <<"\"/>";

	OmnString req = "<request>";
	req	<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"reqid\">create_super_user</item>"
		<< "<item name=\"args\">null</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<objdef>" << obj <<"</objdef>"
		<< "</request>";
	//<request>
	//	<objdef>
	//		<embedobj/> 
	//	</objdef>
	//	<item name="zky_siteid"><![CDATA[mSiteid]]></item>
	//	<item name="operation"><![CDATA[serverreq]]></item>
	//	<item name="reqid"><![CDATA[create_user]]></item>
	//	<item name="args"><![CDATA[null]]></item>
	//	<item name="trans_id"><![CDATA[13]]></item>
	//</request>

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
	userdoc = resproot->getFirstChild("Contents");
	aos_assert_r(userdoc, false);
	userdoc = userdoc->getFirstChild();
	aos_assert_r(userdoc, false);

	OmnString docstr = userdoc->toString();
	userdoc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(userdoc, false);
	return true;
}


bool
AosSqlApi::addVersion(
		const AosXmlTagPtr &doc,
		const u32 siteid)
{
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>addver</item>"
		<< "<objdef>" << doc->toString() << "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(resproot, false);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	AosXmlTagPtr respcontents = resproot->getFirstChild("Contents");
	aos_assert_r(respcontents, false);
	OmnString logid = respcontents->getNodeText();
	aos_assert_r(logid != "", false);
	return true;
}


AosXmlTagPtr 
AosSqlApi::createUserDomain(
		const AosXmlTagPtr &doc, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const u32 siteid, 
		const AosRundataPtr &rdata)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">createuserdomain</item>"
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
	req	<< 	"<objdef>" << doc->toString() <<"</objdef>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;

	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr con = resproot->getFirstChild("Contents");
	aos_assert_r(con, 0);
	OmnString data = con->toString();
	con = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(con, 0);
	return con;
}

#endif
