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
AosSqlApi::modifyMod(
		const u32 siteid, 
		const char *docstr)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	OmnString req = "<request>";
	req << "<item name=\"operation\">modifyObj</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<xmlobj>" << docstr << "</xmlobj>"
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
AosSqlApi::modifyDoc(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid, 
		const OmnString &docstr, 
		const AosRundataPtr &rdata,
		const bool check_error)
{
	// Now the doc has been read into 'data'. Need to send a request
	// to the server to create it.
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">modifyObj</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"rename\"><![CDATA[false]]></item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<xmlobj>" << docstr << "</xmlobj>";

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
	rdata->setOk();
	if (!mConn)
	{
		rdata->setError() << "No connection to server!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		rdata->setError() << "Failed processing the request. Errmsg: "
			<< errmsg;
		OmnAlarm << rdata->getErrmsg();
		return false;
	}

	if (resp == "")
	{
		rdata->setError() << "Failed receiving response!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child = resproot->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Invalid response: " << resp;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool exist;
	if (!check_error)
	{
		if (child->xpathQuery("status/code", exist, "") != "200")
		{
			return false;
		}
	}

	if (child->xpathQuery("status/code", exist, "") != "200") 
	{
		if (child->xpathQuery("status/code", exist, "") != "200") 
		{
			rdata->setError() << "Failed processing request. Error code: "
				<< child->xpathQuery("status/code", exist, "");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	rdata->setOk();
	return true;
}


bool
AosSqlApi::modifyAccessRcd(
		const u32 siteid, 
		const AosXmlTagPtr &doc,
		const u64 &owndocid,
		const OmnString &ssid,
		const u64 &urldocid)
{
	//	<request>
	//	   	<item name='zky_siteid'><![CDATA[mSiteid]]></item>
	//	    <item name='operation'><![CDATA[serverreq]]></item>
	//	    <item name='reqid'><![CDATA[arcd_mod]]></item>
	//	    <item name='trans_id'><![CDATA[8]]></item>
	// 		<item name='zky_ssid'><![CDATA[xxx]]></item>
	// 		<item name='zky_userid'><![CDATA[xxx]]></item>
	// 		<item name='zky_cloudid'><![CDATA[xxx]]></item>
	//	    <objdef>
	//	    	...
	//	    </objdef>
	//	</request>
	
	aos_assert_r(owndocid, false);
	u32 trans_id = mTransId++;
	
	OmnString docstr = doc->toString();

	OmnString req = "<request>";
	req << "<objdef>" << docstr <<"</objdef>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"reqid\">arcd_mod</item>"
		<< "<item name=\"args\"> owndocid="<<owndocid <<" </item>";
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
		<< "<item name=\"zky_ssid\">" << ssid << "</item>"
		<< "</request>";
			
	OmnString errmsg;
	OmnString resp;
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

	aos_assert_r(child, false);
	return true;
}


bool
AosSqlApi::modifyIILTypeAttr(OmnString &str)
{
	static OmnString replacesrc[5] = {
		"zky_cloudid", 
		"zky_ctmepo", 
		"zky_mtmepo", 
		"zky_ver",
		"zky_passwd"};
	static OmnString replacedsc[5] = {
		"zky_cloudid__a", 
		"zky_ctmepo__d", 
		"zky_mtmepo__d",
		"zky_ver__a", 
		"zky_passwd__n"};
	static OmnString replaceinsert[5] = {
		"__a", 
		"__d", 
		"__d", 
		"__a", 
		"__n"};

	for (int i=0; i<5 ;i++)
	{
		int pos = 0;
		while(1)
		{
			pos = str.findSubString(replacesrc[i], pos);
			if (pos == -1 && pos < str.length())
			{
				break;
			}
			if (strncmp(replacedsc[i].data(), str.data()+pos, replacedsc[i].length()))
			{
				str.insert(replaceinsert[i],pos+replacesrc[i].length());
			}
			pos++;
		}
	}
	return true;

}


bool
AosSqlApi::modifyAttr(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &objid, 
		const OmnString &aname,
		const OmnString &value,
		const OmnString &dft,
		const OmnString &value_unique, 
		const OmnString &docid_unique)
{
	OmnString oid = objid;
	oid.replace("=", "#61", true);

	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">modattr</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<item name=\"args\">objid=" << oid 
		<< ",attrname=" << aname 
		<< ",value_unique=" << value_unique
		<< ",docid_unique=" << docid_unique
		<< "</item>"
		<< "<item name=\"value\"><![CDATA[" << value<< "]]></item>"
		<< "<item name=\"default\"><![CDATA[" << dft<< "]]></item>";
		
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
	if (child->xpathQuery("status/code", exist, "") != "200") return false;
	return true;
}


bool
AosSqlApi::manualOrder(
		const u32 siteid,
		const OmnString &reqid,
		const OmnString &args,
		OmnString &rslt) 
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"reqid\">" << reqid << "</item>"
		<< "<item name=\"args\">" << args << "</item>";
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

	bool exist;
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", 0);
	if (!exist)
	{
		OmnMark;
	}
	
	AosXmlTagPtr contents = child->getFirstChild("Contents");
	aos_assert_r(contents, false);

	OmnScreen << "Contents:" << contents->toString() << endl;
	rslt = contents->toString();	
	return true;
}


bool
AosSqlApi::changeCreator(
		const u64 &start_docid, 
		const u64 &end_docid,
		const OmnString &value,
		const u32 siteid, 
		const AosRundataPtr &rdata)
{
	u64 userid ;
	OmnString ssid ;
	AosXmlTagPtr userdoc ;
	u64 urldocid;
	login("100018", "12345",	"yunyuyan_account", ssid, urldocid, 
			userid, userdoc, siteid);
	for (u64 docid=start_docid; docid<end_docid; docid++)
	{
		OmnConnBuffPtr buff;
		DocStatus status;
		if (readDoc(docid, buff, false, status, rdata))
		{
			AosXmlParser parser;
			AosXmlTagPtr doc = parser.parse(buff->getData(), "" AosMemoryCheckerArgs);
			if (doc && doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT) 
			{
				OmnString cid = doc->getAttrStr(AOSTAG_CLOUDID);
				if (cid == "")
				{
					OmnAlarm << "Cloud id is empty: " << doc->toString() << enderr;
				}
				else
				{
					if (doc->getAttrStr(AOSTAG_CREATOR) != cid)
					{
						doc->setAttr(AOSTAG_CREATOR, cid);
						if (!sendModifyReq(siteid, ssid,urldocid, doc->toString(), rdata))
						{
							OmnAlarm << "Failed modifying the doc: " 
								<< doc->toString() << enderr;
						}
					}
				}
			}
		}
	}
	return true;
}


#endif
