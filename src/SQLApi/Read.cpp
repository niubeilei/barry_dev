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
#include "SQLApi/SqlApi.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"



AosXmlTagPtr
AosSqlApi::readDoc(
		const u32 siteid, 
		const OmnString &objid,
		const OmnString &ssid,
		const u64 &urldocid, 
		const OmnString &isInEditor,
		const AosXmlTagPtr &cookies)
{
	// This function retrieves a the doc whose objid is 'objid'. If 'ssid'
	// is not empty, it uses the ssid. Otherwise, it uses its own ssid.
	OmnString req = "<request><item name='zky_siteid'><![CDATA[";
	req << siteid << "]]></item>"
		<< "<item name='operation'><![CDATA[retrieve]]></item>"
		<< "<item name='username'><![CDATA[nonameyet]]></item>";

	if (urldocid!= 0)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	req	<< "<item name=\"zky_isineditor\">" << isInEditor << "</item>"
		<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
		<< "<item name='objid'><![CDATA[" 
		<< objid << "]]></item>"
		<< "<item name='trans_id'><![CDATA[53]]></item>";

	if (cookies)
	{
	    req << cookies->toString();
	}
	req << "</request>";
	
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(resp, "" AosMemoryCheckerArgs);
	return doc;
}


/*
bool
AosSqlApi::readDoc(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &username, 
		const OmnString &passwd,
		const OmnString &objid, 
		AosXmlTagPtr &xmlroot)
{
	// This function retrieves the object '[siteid, objid]'. If found, 
	// the object is return through 'xmlroot'. Otherwise, false is returned.
	// The requester should have logged in and a session ID is passed 
	// on in case the access requires security check. 
	//
	// 	<request>
	// 		<item name='operation'><![CDATA[retrieve]]></item>
	// 		<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	// 		<item name='objid'><![CDATA[xxx]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='trans_id'><![CDATA[xxx]]></item>
	// 		<item name='zky_uname'><![CDATA[xxx]]></item>
	// 		<item name='zky_passwd'><![CDATA[xxx]]></item>
	// 	</request>
	//
	// IMPORTANT: This is a blocking call. It will not return until
	// it receives response. 
	//
	// Future Enhancements: need to improve so that we can set a timer.

	OmnString req = "<request>";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"objid\">" << objid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
	//	<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"" << AOSTAG_USERNAME << "\">" << username << "</item>"
		<< "<item name=\"" << AOSTAG_PASSWD << "\">" << passwd << "</item>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosSqlApi::readDocs(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &username,
		const OmnString &passwd,
		const int start_idx,
		const bool reverse,
		const int pagesize,
		const OmnString &fnames,
		const OmnString &order_fname,
		const OmnString &query, 
		const bool getTotal,
		AosXmlTagPtr &xmlroot)
{
	// This function retrieves a list of objects. The result is in the form:
	// 	<Contents>
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 	</Contents>
	//
	// It is passed back through 'xmlroot'.
	//
	// Returns:
	// Upon success, it returns true and the results are returned through 'xmlroot'. 
	// Otherwise, it returns false.
	//
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverCmd]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_uname'><![CDATA[xxx]]></item>
	// 		<item name='zky_passwd'><![CDATA[xxx]]></item>
	//	    <command>
	//	    	<cmd start_idx="xxx" 
	//	             reverse="true|false" 
	//	             psize="xxx" 
	//	             fnames="xxx" 
	//	             order="xxx" 			// Order field name
	//	             query="xxx" 
	//	             get_total="true|false" 
	//	             opr="retlist"/>
	//	    </command>
	//	</request>
	

	OmnString reverseStr = (reverse)?"true":"false";
	OmnString totalStr = (getTotal)?"true":"false";
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

	req	<< "<item name=\"" << AOSTAG_USERNAME << "\">" << username << "</item>"
		<< "<item name=\"" << AOSTAG_PASSWD << "\">" << passwd << "</item>"
		<< "<command>"
		<< 	   "<cmd start_idx=\"" << start_idx << "\""
		<<			"reverse=\"" << reverseStr << "\""
		<<			"psize=\"" << pagesize << "\""
		<<			"fnames=\"" << fnames << "\""
		<<			"order=\"" << order_fname << "\""
		<<			"get_total=\"" << totalStr << "\""
		<<			"opr=\"retlist\"/>"
		<< "</command>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	return true;
}


bool
AosSqlApi::readDocs(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &username,
		const OmnString &passwd,
		const OmnString &command,
		AosXmlTagPtr &xmlroot)
{
	// This function retrieves a list of objects. The result is in the form:
	// 	<Contents>
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 	</Contents>
	//
	// It is passed back through 'xmlroot'.
	//
	// Returns:
	// Upon success, it returns true and the results are returned through 'xmlroot'. 
	// Otherwise, it returns false.
	//
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverCmd]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_uname'><![CDATA[xxx]]></item>
	// 		<item name='zky_passwd'><![CDATA[xxx]]></item>
	//	    <command>
	//	    	<cmd start_idx="xxx" 
	//	             reverse="true|false" 
	//	             psize="xxx" 
	//	             fnames="xxx" 
	//	             order="xxx" 			// Order field name
	//	             query="xxx" 
	//	             get_total="true|false" 
	//	             opr="retlist"/>
	//	    </command>
	//	</request>
	

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
	//	<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"" << AOSTAG_USERNAME << "\">" << username << "</item>"
		<< "<item name=\"" << AOSTAG_PASSWD << "\">" << passwd << "</item>"
		<< command 
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	xmlroot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(xmlroot, false);
	AosXmlTagPtr child = xmlroot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}
	return true;
}


AosXmlTagPtr 
AosSqlApi::retrieveDocByObjid(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &objid,
		const bool needLock)
{
	OmnString resp;
	return retrieveDocByObjid(siteid, ssid, urldocid, objid,resp, needLock);
}
	

AosXmlTagPtr 
AosSqlApi::retrieveDocByObjid(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &objid,
		OmnString &resp,
		const bool needLock)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString req = "<request >";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>"
		<< "<item name=\"objid\">" << objid << "</item>"
		<< "<item name=\"zky_editor\">true</item>";
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
	if (!mConn)
	{
		OmnAlarm << "Failed conn!" <<enderr;
		return 0;
	}
	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		OmnAlarm << "Failed conn!" <<enderr;
		return 0;
	}

	if(resp == "")
	{
		OmnAlarm << "Missing Resp!" <<enderr;
		return 0;
	}

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	if(!child)
	{
		OmnAlarm << "Missing Child Tag!" <<enderr;
		return 0;
	}
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") 
	{
		return 0;
	}

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	if(!child1)
	{
		OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}
	AosXmlTagPtr child2 = child1->getFirstChild();
	if(!child2)
	{
		OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


AosXmlTagPtr
AosSqlApi::retrieveDocByDocid(const u32 siteid, const u64 &docid)
{
	return retrieveDocByDocid(siteid, ssid, docid, true);
}


AosXmlTagPtr 
AosSqlApi::retrieveDocByDocid(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid,
		const u64 &docid,
		const bool if_error)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString req = "<request >";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>"
		<< "<item name=\"zky_docid\">" << docid << "</item>"
		<< "<item name=\"zky_editor\">true</item>";
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
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (if_error)
	{
		if(child->xpathQuery("status/code", exist, "") != "200")
		{
			return 0;	
		}
	}
	else
	{
		aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	}

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	if (!child1) 
	{
		return 0;
	}

	AosXmlTagPtr child2 = child1->getFirstChild();
	if (!child2) 
	{
		return 0;
	}

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


AosXmlTagPtr 
AosSqlApi::retrieveDocByDocid(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid, 
		const u64 &docid,
		OmnString &resp1)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString req = "<request >";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"zky_docid\">" << docid << "</item>"
		<< "<item name=\"zky_editor\">true</item>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);
	resp1 = resp;

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	if (!child1) 
	{
		return 0;
	}

	AosXmlTagPtr child2 = child1->getFirstChild();
	if (!child2) 
	{
		return 0;
	}

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


AosXmlTagPtr
AosSqlApi::getAccessRcd(
		const u32 siteid, 
		const u64 &docid,
		const OmnString &ssid,
		const u64 &urldocid, 
		const bool create_flag, 
		const bool get_parent)
{
	u32 trans_id = mTransId++;

	OmnString args = "docid=";
	args << docid << ",create=";
	if (create_flag) args << "true";
	else args << "false";
	args << ",parent=";
	if (get_parent) args << "true";
	else args << "false";

	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='args'>"<< args <<"</item>"
		<< "<item name='reqid'>arcd_get</item>"
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

	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);


	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(resp, "" AosMemoryCheckerArgs);
	if (!doc) 
	{
		return 0;
	}

	AosXmlTagPtr child1 = doc ->getFirstChild("Contents");
	AosXmlTagPtr child2 = child1 ->getFirstChild();
	
	if (!child2) 
	{
		return 0;
	}
	OmnString data = child2->toString();
	AosXmlParser parser1;
	AosXmlTagPtr redoc = parser1.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


AosXmlTagPtr 
AosSqlApi::retrieveVersionByObjid(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const OmnString &objid, 
		const OmnString &version)
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="subopr">verobj</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="objid">xxx</item>
	//  </request>
	u32 trans_id = mTransId++;
	OmnString req = "<request >";
	req << "<item name=\"operation\">retrieve</item>"
		<< "<item name=\"subopr\">verobj</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"zky_objid\">" << objid <<"</item>"
		<< "</request>";
		//<< "<item name=\"zky_objid\">" << objid << ":" << version << "</item>"

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
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	aos_assert_r(child1, 0);
	AosXmlTagPtr child2 = child1->getFirstChild();
	aos_assert_r(child2, 0);

	OmnString data = child2->toString();
    AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
    return redoc;
}


bool
AosSqlApi::getDomain(OmnString &domain) 
{

	OmnString req = "<request>";
	req	<< "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"zky_siteid\">100</item>"
		<< "<item name=\"reqid\">getdomain</item>"
		<< "<item name=\"args\">dftdomain</item>"
		<< "</request>"; 
			
	OmnString errmsg;
	OmnString resp;
	u32 siteid = 100; 
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	// The response should be in the following format:
	// 	<Contents>
	// 		<vpd ...>
	// 		...
	// 		</vpd>
	//		...
	//		</obj>
	//	</Contents>
	child = root->getFirstChild("Contents");
	aos_assert_r(child, false);
	domain = child->getNodeText();
	return true;
}


OmnString
AosSqlApi::retrieveCloudid(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &userid)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">getcid</item>"
		<< "<item name=\"args\">userid=" << userid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid << "</item>"
	req	<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, "");
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), "");
	aos_assert_r(resp != "", "");

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, "");
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") return "";

	child = resproot->getNextChild();
	if (!child) return "";
	OmnString cid = child->getAttrStr(AOSTAG_CLOUDID);
	return cid;
}


AosXmlTagPtr 
AosSqlApi::retrieveDocByCloudid(
		 const u32 siteid, 
		 const OmnString &cid, 
		 const u64 &urldocid) 
{
	// It retrieves the doc from the server. 
	//  <request ...>
	//      <item name="operation">retrieve</item>
	//      <item name="siteid">xxx</item>
	//      <item name="zky_ssid">xxx</item>
	//      <item name="docid">xxx</item>
	//  </request>
	OmnString req = "<request >";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"zky_siteid\">" << siteid << "</item>"
		<< "<item name=\"reqid\">getuserdocbycloudid</item>"
		<< "<item name=\"zkyurldocdid\">" << urldocid<< "</item>"
		<< "<item name=\"cid\">" << cid << "</item>"
		<< "</request>";

	OmnString errmsg;
	OmnString resp;
	if (!mConn)
	{
		OmnAlarm << "Failed conn!" <<enderr;
		return 0;
	}
	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		OmnAlarm << "Failed conn!" <<enderr;
		return 0;
	}

	if (resp == "")
	{
		OmnAlarm << "Missing Resp!" <<enderr;
		return 0;
	}

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	if(!child)
	{
		OmnAlarm << "Missing Child Tag!" <<enderr;
		return 0;
	}
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200") 
	{
		return 0;
	}

	AosXmlTagPtr child1 = child->getFirstChild("Contents");
	if(!child1)
	{
		OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}
	AosXmlTagPtr child2 = child1->getFirstChild();
	if(!child2)
	{
		OmnAlarm << "Missing Xml!" <<enderr;
		return 0;
	}

	OmnString data = child2->toString();
	AosXmlTagPtr redoc = parser.parse(data, "" AosMemoryCheckerArgs);
	return redoc;
}


bool 
AosSqlApi::getLanguageType(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const OmnString &sendltype, 
		OmnString &receiveltype)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[100]]></item>
	//	    <item name='operation'><![CDATA[serverreq]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='reqid'><![CDATA[get_language]]></item>
	// 		<item name='args'><![CDATA[languagetype="Chinese" ]]></item>
	//	</request>
	
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>";
	if (urldocid)
	{
		req << "<item name=\"zky_ssid"<<"_"<< urldocid << "\">"<< ssid << "</item>";
	}
	else
	{
		req << "<item name=\"zky_ssid\">"<< ssid<<"</item>";
	}
		//<< "<item name=\"zky_ssid\">" << ssid<< "</item>"
	req	<< "<item name=\"reqid\">get_language</item>";
			
	if(sendltype == "")
	{
		req << "</request>";
	}
	else
	{
		req	<< "<item name=\"args\"><![CDATA[languagetype=" << sendltype << "]]></item>"
			<< "</request>";
	}
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg),false);
	aos_assert_r(resp != "", false);

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return false;
	}

	// The response should be in the following format:
	// 	<Contents>
	// 		<language>Chinese</language>
	//	</Contents>
	receiveltype = root->getNodeText("language");
	return true;
}
*/
