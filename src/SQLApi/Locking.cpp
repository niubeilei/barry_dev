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
AosSqlApi::lockDoc(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid, 
		const OmnString &docstr)
{
	u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverCmd</item>"
		<< "<command><cmd opr='doclock' res_objid='true'/></command>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
		<< "<item name=\"trans_id\">" << trans_id << "</item>"
		<< "<objdef>" << docstr << "</objdef>";

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
		OmnAlarm << "No connection to server!" << enderr;;
		return 0;
	}

	bool rslt = mConn->procRequest(siteid, "", "", req, resp, errmsg);
	if (!rslt)
	{
		OmnAlarm <<"Failed processing the request. Errmsg: " << enderr;
		return 0;
	}

	if (resp == "")
	{
		OmnAlarm  << "Failed receiving response!" << enderr;
		return 0;
	}

	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr contents = resproot->getFirstChild("Contents");
	aos_assert_r(contents, 0);
	AosXmlTagPtr record = contents->getFirstChild();
	aos_assert_r(record, 0);
	OmnString data = record->toString();
	contents = parser.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(contents, 0);
	return contents;
}
#endif
