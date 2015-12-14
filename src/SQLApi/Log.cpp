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



AosXmlTagPtr
AosSqlApi::createLog(
		const u32 siteid,
		const OmnString &logname,
		const OmnString &ctnr_objid,
		const OmnString &contents)
{
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='logname'>" << logname << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>crtlog</item>"
		<< "<objdef>"
		<< "<doc zky_log_ctnr_objid=\"" << ctnr_objid << "\" "
		<< " zky_needresp=\"true\" >"
		<< "<contents>" << contents << "</contents>"
		<< "</doc>"
		<< "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(resproot, 0);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr respcontents = resproot->getFirstChild("Contents");
	aos_assert_r(respcontents, 0);
	return respcontents;
}


AosXmlTagPtr
AosSqlApi::retrieveLog(
		const u32 siteid,
		const u64 &logid)
{
	OmnString req;
	req << "<request>"
		<< "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>reclog</item>"
		<< "<objdef>"
		<< "<loginfo>" << logid << "</loginfo>"
		<< "</objdef>"
		<< "</request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, 0);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), 0);
	aos_assert_r(resp != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(resproot, 0);
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, 0);
	bool exist;
	if (child->xpathQuery("status/code", exist, "") != "200")
	{
		return 0;
	}

	AosXmlTagPtr respcontents = resproot->getFirstChild("Contents");
	aos_assert_r(respcontents, 0);
	AosXmlTagPtr rcvLog = respcontents->getFirstChild();
	aos_assert_r(rcvLog, 0);
	return rcvLog;
}
#endif
