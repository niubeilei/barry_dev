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
// 08/03/2011 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "InstantMsg/IMManager.h"

#include "InstantMsg/IMSessionMgr.h"
#include "InstantMsg/MsgSvrOnlineMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/DocClientObj.h"
#include "MsgClient/MsgClient.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEModules/ObjMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEUtil/IILName.h"


OmnSingletonImpl(AosIMManagerSingleton, 
				 AosIMManager, 
				 AosIMManagerSelf, 
				 "AosIMManager");

AosIMManager::AosIMManager()
:
mLock(OmnNew OmnMutex())
{
}


AosIMManager::~AosIMManager()
{
}


bool 
AosIMManager::start()
{
	return true;
}


bool 
AosIMManager::stop()
{
	return true;
}


bool
AosIMManager::config(const AosXmlTagPtr &def)
{
	return true;
}


bool 
AosIMManager::start(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosIMManager::isYourFriend(
	const OmnString &friend_cid,
	const AosRundataPtr &rdata)
{
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	OmnString friend_ctnr;
	friend_ctnr << AOSTAG_CTNR_FRIEND << "." << sender_cid;
	//OmnString g_iilname = AosIILName::composeCtnrMemberListing(friend_ctnr, "");
	OmnString g_iilname = AosIILName::composeCtnrMemberObjidIILName(friend_ctnr);
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

	// Chen Ding, 03/04/2012
	// ICE_WARN, how to set page size, need review!!!!!!!!!
	query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(g_iilname, queryRslt, 0, query_context, rdata);

	aos_assert_rr(rslt, rdata, false);
	u64 docid = 0;
	bool finished = false;
	while(queryRslt->nextDocid(docid, finished, rdata))
	{
		if (finished)
		{
			break;
		}
		aos_assert_rr(docid != 0, rdata, false);
		AosXmlTagPtr group_doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (!group_doc)
		{
			rdata->setError() << "group doc is not exist: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			continue;
		}

		OmnString group_id = group_doc->getAttrStr("zky_objid", "");
		aos_assert_rr(group_id != "", rdata, false);
		OmnString f_iilname = AosIILName::composeCtnrMemberObjidIILName(group_id);
		//OmnString f_iilname = AosIILName::composeCtnrMemberListing(group_id, "");
		AosQueryRsltObjPtr f_queryRslt = AosQueryRsltObj::getQueryRsltStatic();
		AosQueryContextObjPtr f_query_context = AosQueryContextObj::createQueryContextStatic();
	
		// Chen Ding, 03/04/2012
		// ICE_WARN, how to set page size, need review!!!!!!!!!
		query_context->setOpr(eAosOpr_an);
		bool f_rslt = AosQueryColumn(f_iilname, f_queryRslt, 0, f_query_context, rdata);

		aos_assert_rr(f_rslt, rdata, false);
		u64 f_docid = 0;
		bool f_finished = false;
		while(f_queryRslt->nextDocid(f_docid, f_finished, rdata))
		{
			if (f_finished)
			{
				break;
			}
			aos_assert_rr(f_docid != 0, rdata, false);
			AosXmlTagPtr self_friend_doc = AosDocClientObj::getDocClient()->getDocByDocid(f_docid, rdata);
			if (!self_friend_doc)
			{
				rdata->setError() << "friend doc is not exist: " << f_docid;
				OmnAlarm << rdata->getErrmsg() << enderr;
				continue;
			}

			OmnString f_cid = self_friend_doc->getAttrStr("zky_friend_cid", "");
			if (f_cid == friend_cid)
			{
				return true;
			}
		}
	}
	return false;
}


bool
AosIMManager::applyFriend(
	const OmnString &friend_cid,
	OmnString &self_gid,
	const OmnString &msg,
	const AosRundataPtr &rdata)
{
	/*
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	bool isfriend = isYourFriend(friend_cid, rdata);
	if (isfriend)
	{
		OmnString ctns;
		ctns << "<Contents>"
			 //<< "This user has be your friend"
			 << "002"
			 << "</Contents>";
		rdata->setResults(ctns);
		return true;
	}

	OmnString sysconfigid;
	sysconfigid << "sysconfig." << friend_cid;
	AosXmlTagPtr sysconfig_doc = AosDocClientObj::getDocClient()->getDocByObjid(sysconfigid, rdata);
	bool exist = false;
	bool idcheck = true;
	if (sysconfig_doc)
	{
		idcheck = (sysconfig_doc->xpathQuery("idcheck/@zky_idcheck", exist, "true")=="true")?true:false;
	}
	if (!idcheck)
	{
		OmnString friend_gid = sysconfig_doc->xpathQuery("idcheck/@zky_default_groupid");
		bool rslt = addFriend(friend_cid, self_gid, friend_gid, rdata);
		aos_assert_rr(rslt, rdata, false);
		return true;
	}
	else
	{
		AosXmlTagPtr sender_doc = AosDocClientObj::getDocClient()->getDocByCloudid(sender_cid, rdata);
		aos_assert_rr(sender_doc, rdata, false);
		OmnString pic = sender_doc->getAttrStr(AOSTAG_OBJIMAGE, "");
		OmnString sender_realnm = sender_doc->getAttrStr(AOSTAG_REALNAME, "");
		OmnString request;
		request << "<request operation=\"sendmsg\">"
			<< "<objdef>"
			<< "<Contents "
			<< "zky_port=\"aos_instant_appport\" "
			<< "from=\"" << sender_cid << "\" "
			<< "to=\"" << friend_cid << "\" "
			<< "zky_appname=\"" << AOSTAG_APP_IM << "\" "
			<< "type=\"applyfriend\" "
			<< ">"
			//<< "<from>" << sender_cid << "</from>"
			//<< "<to>" << friend_cid << "</to>"
			<< "<msg from=\"" << sender_cid << "\" "
			<< "to=\"" << friend_cid << "\" "
			<< "zky_sender_cid=\"" << sender_cid << "\" "
			<< "zky_sender_realnm=\"" << sender_realnm<< "\" "
			<< "zky_sender_pic=\"" << pic << "\" "
			<< "zky_sender_gid=\"" << self_gid << "\" "
			<< "type=\"applyfriend\" "
			<< "zky_send_time=\"" << OmnGetTime(AosLocale::eChina) << "\" "
			<< "><![CDATA["
			<< msg
			<< "]]></msg>"
			<< "</Contents>"
			<< "</objdef>"
			<< "</request>";

		bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, friend_cid, rdata);
		aos_assert_rr(notify, rdata, false);
		OmnString cnts;
		cnts << "<Contents>"
			 //<< "apply friend request has sent"
			 << "001"
			 << "</Contents>";
		rdata->setResults(cnts);
		return true;
	}
	*/
	OmnNotImplementedYet;
	return true;
}

bool 
AosIMManager::checkGroupid(
	const OmnString &cid,
	OmnString &gid,
	const AosRundataPtr &rdata)
{
	OmnString friend_ctnr;
	friend_ctnr << AOSTAG_CTNR_FRIEND << "." << cid;
	OmnString g_iilname = AosIILName::composeCtnrMemberObjidIILName(friend_ctnr);
	//OmnString g_iilname = AosIILName::composeCtnrMemberListing(friend_ctnr, "");
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

	// Chen Ding, 03/04/2012
	// ICE_WARN, how to set page size, need review!!!!!!!!!
	query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(g_iilname, queryRslt, 0, query_context, rdata);

	aos_assert_rr(rslt, rdata, false);
	u64 docid = 0;
	bool finished = false;
	while(queryRslt->nextDocid(docid, finished, rdata))
	{
		if (finished)
		{
			break;
		}
		aos_assert_rr(docid != 0, rdata, false);
		AosXmlTagPtr group_doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (!group_doc)
		{
			rdata->setError() << "group doc is not exist: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			continue;
		}
		OmnString group_id = group_doc->getAttrStr("zky_objid", "");
		if (gid == "")
		{
			gid = group_id;
		}
		if (gid == group_id)
		{
			return true;
		}
	}
	return false;
}

bool
AosIMManager::addFriend(
	const OmnString &friend_cid,
	OmnString &self_gid,
	OmnString &friend_gid,
	const AosRundataPtr &rdata)
{
	/*
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	checkGroupid(sender_cid, self_gid, rdata);
	checkGroupid(friend_cid, friend_gid, rdata);
	OmnString friend_docstr1;
	friend_docstr1 << "<friend "
			       << "zky_pctrs=\"" << self_gid << "\" "
				   << "zky_user_cid=\"" << sender_cid << "\" "
				   << "zky_friend_cid=\"" << friend_cid << "\" "
				   << "zky_friend_name=\"\" "
				   << "zky_friend_gid=\"" << friend_gid << "\" "
				   << "/>";
	AosXmlTagPtr doc1 = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, friend_docstr1, sender_cid, "", true, true, false, false, true, true);
	aos_assert_rr(doc1, rdata, false);

	AosXmlTagPtr pdoc1 = AosDocClientObj::getDocClient()->getDocByObjid(self_gid, rdata);
	aos_assert_rr(pdoc1, rdata, false);
	int num1 = pdoc1->getAttrInt("zky_count", 0);
	num1++;
	OmnString zky_count1;
	zky_count1 << num1;
	u64 docid1 = pdoc1->getAttrU64(AOSTAG_DOCID, 0);
	bool rslt1 = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
			docid1, self_gid, "zky_count", zky_count1, "", 
			false, false, true);
	aos_assert_rr(rslt1, rdata, false);

	OmnString friend_docstr2;
	friend_docstr2 << "<friend "
			       << "zky_pctrs=\"" << friend_gid << "\" "
				   << "zky_user_cid=\"" << friend_cid << "\" "
				   << "zky_friend_cid=\"" << sender_cid << "\" "
				   << "zky_friend_name=\"\" "
				   << "zky_friend_gid=\"" << self_gid << "\" "
				   << "/>";
	AosXmlTagPtr doc2 = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, friend_docstr2, friend_cid, "", true, true, false, false, true, true);
	aos_assert_rr(doc2, rdata, false);

	AosXmlTagPtr pdoc2 = AosDocClientObj::getDocClient()->getDocByObjid(friend_gid, rdata);
	aos_assert_rr(pdoc2, rdata, false);
	int num2 = pdoc2->getAttrInt("zky_count", 0);
	num2++;
	OmnString zky_count2;
	zky_count2 << num2;
	u64 docid2 = pdoc2->getAttrU64(AOSTAG_DOCID, 0);
	//access denie
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid, rdata, false);
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	bool rslt2 = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
			docid2, friend_gid, "zky_count", zky_count2, "", 
			false, false, true);
	rdata->setUserid(userid);
	aos_assert_rr(rslt2, rdata, false);

	int status = 0;
	AosMsgSvrOnlineMgr::getSelf()->getUserOnlineStatus(status, friend_cid, rdata);
	if (status)
	{
		OmnString request;
		request << "<request operation=\"sendmsg\">"
				<< "<objdef>"
				<< "<Contents "
				<< "zky_port=\"aos_instant_appport\" "
				<< "from=\"" << sender_cid << "\" "
				<< "to=\"" << friend_cid << "\" "
				<< "zky_appname=\"" << AOSTAG_APP_IM << "\" "
				<< "status=\"" << status << "\" "
				<< "type=\"addfriend\" "
				<< ">"
				//<< "<from>" << sender_cid << "</from>"
				//<< "<to>" << friend_cid << "</to>"
				<< doc2->toString()
				<< "</Contents>"
				<< "</objdef>"
				<< "</request>";

		bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, friend_cid, rdata);
		aos_assert_rr(notify, rdata, false);
	}

	status = 0;
	AosMsgSvrOnlineMgr::getSelf()->getUserOnlineStatus(status, friend_cid, rdata);
	OmnString ctns;
	ctns << "<Contents " 
		 << "status=\"" << status << "\" "
		 << ">"
		 << doc1->toString()
		 << "</Contents>";
	rdata->setResults(ctns);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}


bool 
AosIMManager::denyFriend(
	const OmnString &friend_cid,
	const OmnString &msg,
	const AosRundataPtr &rdata)
{
	/*
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);

	AosXmlTagPtr sender_doc = AosDocClientObj::getDocClient()->getDocByCloudid(sender_cid, rdata);
	aos_assert_rr(sender_doc, rdata, false);
	OmnString pic = sender_doc->getAttrStr(AOSTAG_OBJIMAGE, "");
	OmnString sender_realnm = sender_doc->getAttrStr(AOSTAG_REALNAME, "");

	OmnString request;
	request << "<request operation=\"sendmsg\">"
			<< "<objdef>"
			<< "<Contents "
			<< "zky_port=\"aos_instant_appport\" "
			<< "from=\"" << sender_cid << "\" "
			<< "to=\"" << friend_cid << "\" "
			<< "zky_appname=\"" << AOSTAG_APP_IM << "\" "
			<< "type=\"denyfriend\" "
			<< ">"
			//<< "<from>" << sender_cid << "</from>"
			//<< "<to>" << friend_cid << "</to>"
			<< "<msg from=\"" << sender_cid << "\" "
		 	<< "to=\"" << friend_cid << "\" "
			<< "zky_sender_cid=\"" << sender_cid << "\" "
			<< "zky_sender_realnm=\"" << sender_realnm<< "\" "
			<< "zky_sender_pic=\"" << pic << "\" "
			<< "type=\"denyfriend\" "
			<< "><![CDATA["
			<< msg
			<< "]]></msg>"
			<< "</Contents>"
			<< "</objdef>"
			<< "</request>";

	bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, friend_cid, rdata);
	aos_assert_rr(notify, rdata, false);

	OmnString ctns;
	ctns << "<Contents>"
		 << "deny friend success"
		 << "</Contents>";
	rdata->setResults(ctns);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}


bool
AosIMManager::moveTo(
			const OmnString &friend_cid,
			const OmnString &srcgroupid,
			const OmnString &desgroupid,
			const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	bool duplicated = false;
	//u64 docid = AosIILClient::getSelf()->getCtnrMemDocid(
	u64 docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(
			siteid, srcgroupid, "zky_friend_cid", friend_cid, duplicated, rdata);
	aos_assert_rr(docid != 0, rdata, false);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	aos_assert_rr(doc, rdata, false);

	OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_rr(objid != "", rdata, false);

	bool rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
			docid, objid, "zky_pctrs", desgroupid, "", 
			false, false, true);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosIMManager::removeFriendGroup(
			const OmnString &groupid,
			const AosRundataPtr &rdata)
{
	/*
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(groupid, rdata);
	aos_assert_rr(doc, rdata, false);
	
	OmnString iilname = AosIILName::composeCtnrMemberObjidIILName(groupid);
	//OmnString iilname = AosIILName::composeCtnrMemberListing(groupid, "");
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

	// Chen Ding, 03/04/2012
	// ICE_WARN, how to set page size, need review!!!!!!!!!
	query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(iilname, queryRslt, 0, query_context, rdata);

	aos_assert_rr(rslt, rdata, false);

	if (!queryRslt->isEmpty())
	{
		rdata->setError() << "please remove friend to another group";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString friend_ctnr;
	friend_ctnr << AOSTAG_CTNR_FRIEND << "." << sender_cid;
	OmnString docid = doc->getAttrStr(AOSTAG_DOCID, "");
	aos_assert_rr(docid != "", rdata, false);
	bool del = AosDocClientObj::getDocClient()->deleteObj(rdata, docid,
		"", friend_ctnr, true);
	aos_assert_rr(del, rdata, false);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}


bool
AosIMManager::removeFriend(
	const OmnString &friend_cid,
	const OmnString &self_gid,
	const OmnString &friend_gid,
	const AosRundataPtr &rdata)
{
	/*
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);

	bool duplicated1 = false;
	u64 docid1 = AosIILClient::getSelf()->getCtnrMemDocid(
			siteid, self_gid, "zky_friend_cid", friend_cid,
			"zky_user_cid", sender_cid, duplicated1, rdata);
	aos_assert_rr(docid1 != 0, rdata, false);
	OmnString docidstr1;
	docidstr1 << docid1;
	bool del1 = AosDocClientObj::getDocClient()->deleteObj(rdata, docidstr1,
			"", self_gid, true);
	aos_assert_rr(del1, rdata, false);

	AosXmlTagPtr pdoc1 = AosDocClientObj::getDocClient()->getDocByObjid(self_gid, rdata);
	aos_assert_rr(pdoc1, rdata, false);
	int num1 = pdoc1->getAttrInt("zky_count", 0);
	if (num1 > 0)
	{
		num1--;
	}
	OmnString zky_count1;
	zky_count1 << num1;
	u64 pdocid1 = pdoc1->getAttrU64(AOSTAG_DOCID, 0);
	bool rslt1 = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
			pdocid1, self_gid, "zky_count", zky_count1, "", 
			false, false, true);
	aos_assert_rr(rslt1, rdata, false);

	bool duplicated2 = false;
	u64 docid2 = AosIILClient::getSelf()->getCtnrMemDocid(
			siteid, friend_gid, "zky_friend_cid", sender_cid,
			"zky_user_cid", friend_cid, duplicated2, rdata);
	aos_assert_rr(docid2 != 0, rdata, false);

	OmnString docidstr2;
	docidstr2 << docid2;
	//access denie
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	bool del2 = AosDocClientObj::getDocClient()->deleteObj(rdata, docidstr2,
			"", friend_gid, true);
	aos_assert_rr(del2, rdata, false);

	AosXmlTagPtr pdoc2 = AosDocClientObj::getDocClient()->getDocByObjid(friend_gid, rdata);
	aos_assert_rr(pdoc2, rdata, false);
	int num2 = pdoc2->getAttrInt("zky_count", 0);
	if (num2 > 0)
	{
		num2--;
	}
	OmnString zky_count2;
	zky_count2 << num2;
	u64 pdocid2 = pdoc2->getAttrU64(AOSTAG_DOCID, 0);
	bool rslt2 = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
			pdocid2, friend_gid, "zky_count", zky_count2, "", 
			false, false, true);
	aos_assert_rr(rslt2, rdata, false);
	rdata->setUserid(userid);

	int status = 0;
	AosMsgSvrOnlineMgr::getSelf()->getUserOnlineStatus(status, friend_cid, rdata);
	if (status != 0)
	{
		OmnString request;
		request << "<request operation=\"sendmsg\">"
				<< "<objdef>"
				<< "<Contents "
				<< "zky_port=\"aos_instant_appport\" "
				<< "from=\"" << sender_cid << "\" "
				<< "to=\"" << friend_cid << "\" "
				<< "zky_appname=\"" << AOSTAG_APP_IM << "\" "
				<< "type=\"removefriend\" "
				<< ">"
				//<< "<from>" << sender_cid << "</from>"
				//<< "<to>" << friend_cid << "</to>"
				<< "<friend zky_friend_cid=\"" << sender_cid << "\" "
				<< "zky_user_cid=\"" << friend_cid << "\" "
				<< "zky_pctrs=\"" << friend_gid << "\" "
				<< "/>"
				<< "</Contents>"
				<< "</objdef>"
				<< "</request>";

		bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, friend_cid, rdata);
		aos_assert_rr(notify, rdata, false);
	}

	OmnString ctns;
	ctns << "<Contents>"
		 << "<friend zky_friend_cid=\"" << friend_cid << "\" "
		 << "zky_user_cid=\"" << sender_cid << "\" "
		 << "zky_pctrs=\"" << self_gid << "\" "
		 << "/>"
		 << "</Contents>";
	rdata->setResults(ctns);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}

bool 
AosIMManager::sendTalkGroupMsg(
	const OmnString &type,
	const OmnString &toid, 
	const OmnString &msg,
	const AosRundataPtr &rdata)
{
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	AosXmlTagPtr talkgroup_doc = AosDocClientObj::getDocClient()->getDocByDocid(
			atoll(toid.data()), rdata);
	aos_assert_rr(talkgroup_doc, rdata, false);
	AosXmlTagPtr member = talkgroup_doc->getFirstChild("friend");
	OmnString friend_cid = "";
	//save log
	while(member)
	{
		friend_cid = member->getAttrStr("zky_friend_cid", "");
		aos_assert_rr(friend_cid != "", rdata, false);
		if (friend_cid == sender_cid)
		{
			member = talkgroup_doc->getNextChild();
			continue;
		}
		bool rslt = sendInstantMsg(type, toid, friend_cid, msg, rdata);
		aos_assert_rr(rslt, rdata, false);
		member = talkgroup_doc->getNextChild();
	}

	AosXmlTagPtr sender_doc = AosDocClientObj::getDocClient()->getDocByCloudid(sender_cid, rdata);
	aos_assert_rr(sender_doc, rdata, false);
	OmnString sender_realnm = sender_doc->getAttrStr(AOSTAG_REALNAME, "");
	aos_assert_rr(sender_realnm != "", rdata, false);
	OmnString ctns;
	ctns << "<Contents "
		 << "zky_port=\"aos_instant_appport\" "
		 << "from=\"" << sender_cid << "\" "
		 << "to=\"" << friend_cid << "\" "
		 << "zky_appname=\"" << AOSTAG_APP_IM << "\" "
		 << "toid=\"" << toid << "\" "
		 << "type=\"" << type << "\" "
		 << ">"
		 //<< "<from>" << sender_cid << "</from>"
		 //<< "<to>" << friend_cid << "</to>"
		 << "<msg from=\"" << sender_cid << "\" "
		 << "to=\"" << friend_cid << "\" "
		 << "toid=\"" << toid << "\" "
		 << "type=\"" << type << "\" "
		 << "zky_sender_realnm=\"" << sender_realnm << "\" "
		 << "zky_send_time=\"" << OmnGetTime(AosLocale::eChina) << "\" "
		 << "><![CDATA["
		 << msg
		 << "]]></msg>"
		 << "</Contents>";
	rdata->setResults(ctns);
	return true;
}


bool 
AosIMManager::sendInstantMsg(
	const OmnString &type, 
	const OmnString &toid, 
	const OmnString &friend_cid, 
	const OmnString &msg,
	const AosRundataPtr &rdata)
{
	/*
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	AosXmlTagPtr sender_doc = AosDocClientObj::getDocClient()->getDocByCloudid(sender_cid, rdata);
	aos_assert_rr(sender_doc, rdata, false);
	OmnString sender_realnm = sender_doc->getAttrStr(AOSTAG_REALNAME, "");
	aos_assert_rr(sender_realnm != "", rdata, false);
	OmnString ctns;
	ctns << "<Contents "
		 << "zky_port=\"aos_instant_appport\" "
		 << "from=\"" << sender_cid << "\" "
		 << "to=\"" << friend_cid << "\" "
		 << "zky_appname=\"" << AOSTAG_APP_IM << "\" "
		 << "toid=\"" << toid << "\" "
		 << "type=\"" << type << "\" "
		 << ">"
		 //<< "<from>" << sender_cid << "</from>"
		 //<< "<to>" << friend_cid << "</to>"
		 << "<msg from=\"" << sender_cid << "\" "
		 << "to=\"" << friend_cid << "\" "
		 << "toid=\"" << toid << "\" "
		 << "type=\"" << type << "\" "
		 << "zky_sender_realnm=\"" << sender_realnm << "\" "
		 << "zky_send_time=\"" << OmnGetTime(AosLocale::eChina) << "\" "
		 << "><![CDATA["
		 << msg
		 << "]]></msg>"
		 << "</Contents>";

	//save log
	if (type == "chatmessage")
	{
		OmnString sysconfigid;
		sysconfigid << "sysconfig." << friend_cid;
		AosXmlTagPtr sysconfig_doc = AosDocClientObj::getDocClient()->getDocByObjid(sysconfigid, rdata);
		bool exist = false;
		bool savelog = false;
		if (sysconfig_doc)
		{
			savelog = sysconfig_doc->xpathQuery("recordsecurity/@zky_savechatrecord", exist, "false")
				=="true"? true:false;
		}
		if (savelog)
		{
			AosIMSessionMgr::getSelf()->addMsg(friend_cid, ctns, rdata);
		}
	}

	//send msg to the msgserver
	OmnString request;
	request << "<request operation=\"sendmsg\">"
			<< "<objdef>"
			<< ctns
			<< "</objdef>"
			<< "</request>";

	bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, friend_cid, rdata);
	aos_assert_rr(notify, rdata, false);
	if (type != "chatmessage_group")
	{
		rdata->setResults(ctns);
	}
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}

bool 
AosIMManager::notifyOnlineStatus(
	const OmnString &status,
	const AosRundataPtr &rdata)
{
	/*
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata ,false);
	
	//bool setstatus = AosMsgSvrOnlineMgr::getSelf()->setUserOnlineStatus(status, rdata);
	//aos_assert_rr(setstatus, rdata, false);

	OmnString type;
	if (status == "1")
	{
		type = "online";
	}
	else if (status == "2")
	{
		type = "leave";
	}
	else if (status == "0")
	{
		type = "offline";
	}
	else
	{
		rdata->setError() << "Unrecognized status type";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString friend_ctnr;
	friend_ctnr << AOSTAG_CTNR_FRIEND << "." << sender_cid;
	OmnString g_iilname = AosIILName::composeCtnrMemberObjidIILName(friend_ctnr);
	//OmnString g_iilname = AosIILName::composeCtnrMemberListing(friend_ctnr, "");
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

	// Chen Ding, 03/04/2012
	// ICE_WARN, how to set page size, need review!!!!!!!!!
	query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(g_iilname, queryRslt, 0, query_context, rdata);

	aos_assert_rr(rslt, rdata, false);
	u64 docid = 0;
	bool finished = false;
	while(queryRslt->nextDocid(docid, finished, rdata))
	{
		if (finished)
		{
			break;
		}
		aos_assert_rr(docid != 0, rdata, false);
		AosXmlTagPtr group_doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (!group_doc)
		{
			rdata->setError() << "group doc is not exist: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			continue;
		}
		OmnString group_id = group_doc->getAttrStr("zky_objid", "");
		aos_assert_rr(group_id != "", rdata, false);

		OmnString f_iilname = AosIILName::composeCtnrMemberObjidIILName(group_id);
		//OmnString f_iilname = AosIILName::composeCtnrMemberListing(group_id, "");
		AosQueryRsltObjPtr f_queryRslt = AosQueryRsltObj::getQueryRsltStatic();
		AosQueryContextObjPtr f_query_context = AosQueryContextObj::createQueryContextStatic();

		// Chen Ding, 03/04/2012
		// ICE_WARN, how to set page size, need review!!!!!!!!!
		query_context->setOpr(eAosOpr_an);
		bool f_rslt = AosQueryColumn(f_iilname, f_queryRslt, 0, f_query_context, rdata);

		aos_assert_rr(f_rslt, rdata, false);
		u64 f_docid = 0;
		bool f_finished = false;
		while(f_queryRslt->nextDocid(f_docid, f_finished, rdata))
		{
			if (f_finished)
			{
				break;
			}
			aos_assert_rr(f_docid != 0, rdata, false);
			AosXmlTagPtr self_friend_doc = AosDocClientObj::getDocClient()->getDocByDocid(f_docid, rdata);
			if (!self_friend_doc)
			{
				rdata->setError() << "friend doc is not exist: " << f_docid;
				OmnAlarm << rdata->getErrmsg() << enderr;
				continue;
			}
			OmnString friend_cid = self_friend_doc->getAttrStr("zky_friend_cid", "");
			aos_assert_rr(friend_cid !="", rdata, false);

			//OmnString tmp_ctnr;
			//tmp_ctnr << AOSTAG_CTNR_FRIEND << "." << friend_cid;
			//AosXmlTagPtr tmp_ctnr_doc = AosDocClientObj::getDocClient()->getDocByObjid(tmp_ctnr, rdata);
			//aos_assert_rr(tmp_ctnr_doc, rdata, false);
			//OmnString online_status = tmp_ctnr_doc->getAttrStr("zky_status", "0");

			int online_status = 0;
			AosMsgSvrOnlineMgr::getSelf()->getUserOnlineStatus(online_status, friend_cid, rdata);
			if (online_status == 0)
			{
				continue;
			}
			OmnString friend_gid = self_friend_doc->getAttrStr("zky_friend_gid", "");
			aos_assert_rr(friend_gid !="", rdata, false);

			u32 siteid = rdata->getSiteid();
			aos_assert_rr(siteid != 0, rdata, false);
			bool duplicated = false;
			u64 ff_docid = AosIILClient::getSelf()->getCtnrMemDocid(
					siteid, friend_gid, "zky_friend_cid", sender_cid,
					"zky_user_cid", friend_cid, duplicated, rdata);
			aos_assert_rr(ff_docid != 0, rdata, false);
			AosXmlTagPtr friend_doc = AosDocClientObj::getDocClient()->getDocByDocid(ff_docid, rdata);
			aos_assert_rr(friend_doc, rdata, false);

			OmnString request;
			request << "<request operation=\"sendmsg\">"
					<< "<objdef>"
					<< "<Contents "
					<< "zky_port=\"aos_instant_appport\" "
					<< "from=\"" << sender_cid << "\" "
					<< "to=\"" << friend_cid << "\" "
					<< "zky_appname=\"" << AOSTAG_APP_IM << "\" "
					<< "type=\"" << type << "\" "
					<< "status=\"" << status << "\" "
					<< ">"
					//<< "<from>" << sender_cid << "</from>"
					//<< "<to>" << friend_cid << "</to>"
					<< friend_doc->toString()
					<< "</Contents>"
					<< "</objdef>"
					<< "</request>";

			bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, friend_cid, rdata);
			aos_assert_rr(notify, rdata, false);
		}
	}
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}

bool 
AosIMManager::getOnlineFriends(
		const AosRundataPtr &rdata)
{
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	OmnString friend_ctnr;
	friend_ctnr << AOSTAG_CTNR_FRIEND << "." << sender_cid;

	OmnString g_iilname = AosIILName::composeCtnrMemberObjidIILName(friend_ctnr);
	//OmnString g_iilname = AosIILName::composeCtnrMemberListing(friend_ctnr, "");
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

	// Chen Ding, 03/04/2012
	// ICE_WARN, how to set page size, need review!!!!!!!!!
	query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(g_iilname, queryRslt, 0, query_context, rdata);

	aos_assert_rr(rslt, rdata, false);
	u64 docid = 0;
	bool finished = false;
	OmnString contents;
	contents << "<Contents>";
	while(queryRslt->nextDocid(docid, finished, rdata))
	{
		if (finished)
		{
			break;
		}
		aos_assert_rr(docid != 0, rdata, false);
		AosXmlTagPtr group_doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (!group_doc)
		{
			rdata->setError() << "group doc is not exist: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			continue;
		}

		OmnString group_id = group_doc->getAttrStr("zky_objid", "");
		aos_assert_rr(group_id != "", rdata, false);
		OmnString f_iilname = AosIILName::composeCtnrMemberObjidIILName(group_id);
		//OmnString f_iilname = AosIILName::composeCtnrMemberListing(group_id, "");
		AosQueryRsltObjPtr f_queryRslt = AosQueryRsltObj::getQueryRsltStatic();
		AosQueryContextObjPtr f_query_context = AosQueryContextObj::createQueryContextStatic();

		// Chen Ding, 03/04/2012
		// ICE_WARN, how to set page size, need review!!!!!!!!!
		query_context->setOpr(eAosOpr_an);
		bool f_rslt = AosQueryColumn(f_iilname, f_queryRslt, 0, f_query_context, rdata);

		aos_assert_rr(f_rslt, rdata, false);
		AosXmlTagPtr group_doc_clone = group_doc->clone(AosMemoryCheckerArgsBegin);
		u64 f_docid = 0;
		bool f_finished = false;
		while(f_queryRslt->nextDocid(f_docid, f_finished, rdata))
		{
			if (f_finished)
			{
				break;
			}
			aos_assert_rr(f_docid != 0, rdata, false);
			AosXmlTagPtr self_friend_doc = AosDocClientObj::getDocClient()->getDocByDocid(f_docid, rdata);
			if (!self_friend_doc)
			{
				rdata->setError() << "friend doc is not exist: " << f_docid;
				OmnAlarm << rdata->getErrmsg() << enderr;
				continue;
			}

			OmnString friend_cid = self_friend_doc->getAttrStr("zky_friend_cid", "");
			aos_assert_rr(friend_cid !="", rdata, false);
			int status = 0;
			AosMsgSvrOnlineMgr::getSelf()->getUserOnlineStatus(status, friend_cid, rdata);
			if (status == 0)
			{
				continue;
			}
			group_doc_clone->addNode(self_friend_doc);
		}
		contents << group_doc_clone->toString();
	}
	contents << "</Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	return true;
}

bool 
AosIMManager::removeTalkGroup(
		const OmnString &talkgroupid,
		const AosRundataPtr &rdata)
{
	//1. if this talkgroup don't create by yourself
	//remove self talkgroup container item and
	//remove slef member from talkgroup_doc
	//2. if this talkgroup create by yourself 
	//remove talkgroup_doc and remove all member talkgroup
	//container item
	
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	AosXmlTagPtr talkgroup_doc = AosDocClientObj::getDocClient()->getDocByDocid(
			atoll(talkgroupid.data()), rdata);
	aos_assert_rr(talkgroup_doc, rdata, false);
	OmnString creator_cid = talkgroup_doc->getAttrStr(AOSTAG_CREATOR, "");
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	if (sender_cid ==  creator_cid)
	{
		AosXmlTagPtr member = talkgroup_doc->getFirstChild("friend");
		while(member)
		{
			OmnString member_cid = member->getAttrStr("zky_friend_cid", "");
			aos_assert_rr(member_cid != "", rdata, false);
			OmnString talkgroup_ctnrid;
			talkgroup_ctnrid << AOSTAG_CTNR_TALKGROUP << "." << member_cid;
			bool duplicated = false;
			//u64 docid = AosIILClient::getSelf()->getCtnrMemDocid(
			u64 docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(
					siteid,talkgroup_ctnrid, "zky_talk_groupid", talkgroupid, 
					"zky_crtor", member_cid, duplicated, rdata);
			aos_assert_rr(docid != 0, rdata, false);

			OmnString docidstr;
			docidstr << docid;
			//access
			u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
			bool del = AosDocClientObj::getDocClient()->deleteObj(rdata, docidstr,
					"", talkgroup_ctnrid, true);
			rdata->setUserid(userid);
			aos_assert_rr(del, rdata, false);
			member = talkgroup_doc->getNextChild();
		}
		OmnString talkgroup_docid = talkgroup_doc->getAttrStr(AOSTAG_DOCID, "");
		bool del1 = AosDocClientObj::getDocClient()->deleteObj(rdata, talkgroup_docid,
				"", AOSTAG_CTNR_TALKGROUP, true);
		aos_assert_rr(del1, rdata, false);
	}
	else
	{
		OmnString self_talkgroup_ctnrid;
		self_talkgroup_ctnrid << AOSTAG_CTNR_TALKGROUP << "." << sender_cid;
		OmnString talkgroup_iilname = AosIILName::composeCtnrMemberObjidIILName(self_talkgroup_ctnrid);
		//OmnString talkgroup_iilname = AosIILName::composeCtnrMemberListing(self_talkgroup_ctnrid, "");
		AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
		AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

		// Chen Ding, 03/04/2012
		// ICE_WARN, how to set page size, need review!!!!!!!!!
		query_context->setOpr(eAosOpr_an);
		bool rslt = AosQueryColumn(talkgroup_iilname, queryRslt, 0, query_context, rdata);

		aos_assert_rr(rslt, rdata, false);
		u64 item_docid = 0;
		bool finished = false;
		while(queryRslt->nextDocid(item_docid, finished, rdata))
		{
			if (finished)
			{
				break;
			}
			aos_assert_rr(item_docid != 0, rdata, false);
			AosXmlTagPtr item_doc = AosDocClientObj::getDocClient()->getDocByDocid(item_docid, rdata);
			//aos_assert_rr(item_doc, rdata, false);
			if (!item_doc)
			{
				rdata->setError() << "talkgroup itme doc is not exist: " << item_docid;
				OmnAlarm << rdata->getErrmsg() << enderr;
				continue;
			}
			OmnString item_docidstr;
			item_docidstr << item_docid;
			bool delitem = AosDocClientObj::getDocClient()->deleteObj(rdata, item_docidstr,
					"", self_talkgroup_ctnrid, true);
			aos_assert_rr(delitem, rdata, false);
		}
		talkgroup_doc->removeNode1("friend", "zky_friend_cid", sender_cid);
		//access
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		bool save_doc = AosDocClientObj::getDocClient()->modifyObj(talkgroup_doc, rdata);
		rdata->setUserid(userid);
		aos_assert_rr(save_doc, rdata, false);
	}
	return true;
}

bool 
AosIMManager::createTalkGroup(
		const AosXmlTagPtr &talkgroup,
		const AosRundataPtr &rdata)
{
	//1. create group doc set group container
	//2. create group item into everyone's group container
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	OmnString talk_groupstr;
	talk_groupstr << talkgroup->toString();
	AosXmlTagPtr talkgroup_doc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, talk_groupstr, sender_cid, "", 
			true, true, false, false, true, true);
	aos_assert_rr(talkgroup_doc, rdata, false);
	u64 talk_groupid = talkgroup_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(talk_groupid != 0 , rdata, false);

	OmnString talk_groupname = talkgroup->getAttrStr("zky_talk_groupname", "talkgroup");
	aos_assert_rr(talk_groupname != "", rdata, false);
	
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	OmnString ctns;
	AosXmlTagPtr member = talkgroup->getFirstChild();
	while(member)
	{
		OmnString member_cid = member->getAttrStr("zky_friend_cid", "");
		aos_assert_rr(member_cid != "", rdata, false);
		int level = 0;
		if (member_cid == sender_cid)
		{
			level = 1;
		}
		OmnString item_talkgroup_str;
		item_talkgroup_str << "<talkgroup "
						   << "zky_pctrs=\"" << AOSTAG_CTNR_TALKGROUP << "." << member_cid << "\" "
						   << "zky_level=\"" << level <<"\" "
						   << "zky_talk_groupid=\"" << talk_groupid << "\" "
						   << "zky_talk_groupname=\"" << talk_groupname << "\" "
						   << "/>";

		//access
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));	
		AosXmlTagPtr item_talkgroup_doc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, item_talkgroup_str, member_cid, "", 
			false, true, false, false, true, true);
		rdata->setUserid(userid);
		aos_assert_rr(item_talkgroup_doc, rdata, false);
		if (member_cid == sender_cid)
		{
			ctns << "<Contents>"
				 << item_talkgroup_doc->toString()
				 << "</Contents>";
		}
		member = talkgroup->getNextChild();
	}
	rdata->setResults(ctns);
	rdata->setOk();
	return true;
}


