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
AosSqlApi::sendInMsg(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &friend_cid, 
			const OmnString &msg)
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>sendinmsg</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<msg>"<< msg <<"</msg>"
		<< "</Contents></objdef></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSqlApi::addFriend(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &ctn_name, 
			const OmnString &friend_cid) 
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>addfriend</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<ctn_name>"<< ctn_name <<"</ctn_name>"
		<< "</Contents></objdef></request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}

bool
AosSqlApi::inviteFriend(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &ctn_name, 
			const OmnString &friend_cid) 
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>invitefriend</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<ctn_name>"<< ctn_name <<"</ctn_name>"
		<< "</Contents></objdef></request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSqlApi::denyFriend(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &ctn_name, 
			const OmnString &friend_cid) 
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>denyfriend</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<ctn_name>"<< ctn_name <<"</ctn_name>"
		<< "</Contents></objdef></request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSqlApi::removeFriend(
			const OmnString &ssid,
			const u64 &urldocid, 
			const u32 siteid, 
			const OmnString &ctn_name, 
			const OmnString &friend_cid) 
{
	//u32 trans_id = mTransId++;
	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='zky_ssid'>" << ssid << "</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>removefriend</item>"
		<< "<objdef><Contents><friend_cid>"<< friend_cid <<"</friend_cid>"
		<< "<ctn_name>"<< ctn_name <<"</ctn_name>"
		<< "</Contents></objdef></request>";
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSqlApi::addFriendResp(
	const u32 siteid, 
	const OmnString &requester, 
	const OmnString &friendid)
{
	u32 trans_id = mTransId++;

	OmnString req = "<request>";
	req << "<item name='zky_siteid'>"<< siteid <<"</item>"
		<< "<item name='operation'>serverreq</item>"
		<< "<item name='reqid'>addFriendResp</item>"
		<< "<item name='requester'>"<< requester <<"</item>"
		<< "<item name='friend'>"<< friendid <<"</item>"
		<< "<item name='trans_id'>"<< trans_id << "</item></request>";
			
	OmnString errmsg;
	OmnString resp;
	aos_assert_r(mConn, false);
	aos_assert_r(mConn->procRequest(siteid, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	return true;
}


bool
AosSqlApi::sendShortMsg(
		const OmnString &msg,
		const OmnString &receiver,
	    const u32 siteid)
{
	OmnString req = "<request>";
	req << "<item name=\"operation\">serverreq</item>"
		<< "<item name=\"" << AOSTAG_SITEID << "\">" << siteid << "</item>"
	    << "<item name=\"reqid\">sdshm</item>"
		<< "<item name=\"needresp\">noresp</item>"
		<< "<objdef>"
		<< "<shm " << "shm_receivers=\"" << receiver << "\" >"
		<< "<contents>" << msg << "</contents>"
		<< "</shm>"
		<< "</objdef>"
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
	return true;
}
#endif
