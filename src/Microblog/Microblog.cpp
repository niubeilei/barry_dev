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
// 06/16/2010 by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "Microblog/Microblog.h"

#include "SingletonClass/SingletonImpl.cpp"
#include "InstantMsg/MsgSvrOnlineMgr.h"
#include "SEUtil/IILName.h"
#include "MsgClient/MsgClient.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SeLogClient/SeLogClient.h"
#include "SEModules/ObjMgr.h"
#include "XmlUtil/SeXmlParser.h"


OmnSingletonImpl(AosMicroblogSingleton, 
				 AosMicroblog, 
				 AosMicroblogSelf, 
				 "AosMicroblog");

AosMicroblog::AosMicroblog()
:
mLock(OmnNew OmnMutex())
{
}


AosMicroblog::~AosMicroblog()
{
}


bool 
AosMicroblog::start()
{
	return true;
}


bool 
AosMicroblog::stop()
{
	return true;
}


bool
AosMicroblog::config(const AosXmlTagPtr &def)
{
	return true;
}


bool 
AosMicroblog::start(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosMicroblog::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	return true;
}


bool
AosMicroblog::signal(const int threadLogicId)
{
	return true;
}


bool
AosMicroblog::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosMicroblog::registFollower(
		const OmnString &byfollower_cid,
		const OmnString &gicid,
		const OmnString &userdata,
		const AosRundataPtr &rdata)
{
	OmnString follower_cid = rdata->getCid();
	aos_assert_rr(follower_cid != "", rdata, false);
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	OmnString followerctnr_objid;
	followerctnr_objid << AOSTAG_CTNR_CFOLLOWER << "." << byfollower_cid;

	OmnString follower_docstr;
	follower_docstr << "<follower "
					<< "zky_follower_cid=\"" << follower_cid << "\" "
					<< "zky_byfollower_cid=\"" << byfollower_cid << "\" "
					<< "zky_pctrs=\"" << followerctnr_objid << "\" "
					<< "gicid=\"" << gicid << "\" "
					<< ">"
					<< userdata
					<< "</follower>";
	//access
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	AosXmlTagPtr follower_doc = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, follower_docstr, byfollower_cid, "", true, true, false, false, true, true);
	rdata->setUserid(userid);
	aos_assert_rr(follower_doc, rdata, false);
OmnScreen << follower_doc->toString() << endl;
	OmnString contents;
	contents << "<Contents>"
			 << "Add follower success"
			 << "</Contents>";
	rdata->setResults(contents);
	return true;
}

bool
AosMicroblog::addFollower(
		const OmnString &byfollower_cid,
		const AosRundataPtr &rdata)
{
	//1. check this user exist byfollower's container
	//2. if not add doc to the byfollower's container
	aos_assert_rr(byfollower_cid != "", rdata, false);
	OmnString follower_cid = rdata->getCid();
	aos_assert_rr(follower_cid != "", rdata, false);
	//aos_assert_rr(byfollower_cid != follower_cid, rdata, false);
	if (byfollower_cid == follower_cid)
	{
		OmnString ctns;
		ctns << "<Contents>"
			 << "You could not follow yourself"
			 << "</Contents>";
		rdata->setResults(ctns);
		return true;
	}
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);

	OmnString byfollowerctnr_objid;
	byfollowerctnr_objid << AOSTAG_CTNR_BYFOLLOWER << "." << follower_cid;

	bool duplicated = false;
	//u64 byfollower_docid = AosIILClient::getSelf()->getCtnrMemDocid(
	u64 byfollower_docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(
		siteid, byfollowerctnr_objid, "zky_follower_cid", follower_cid, 
		"zky_byfollower_cid", byfollower_cid, duplicated, rdata);
	if (byfollower_docid != 0)
	{
		OmnString ctns;
		ctns << "<Contents>"
			 << "You had follow this user"
			 << "</Contents>";
		rdata->setResults(ctns);
		return true;
	}

	OmnString byfollower_docstr;
	byfollower_docstr << "<byfollower "
					  << "zky_follower_cid=\"" << follower_cid << "\" "
					  << "zky_byfollower_cid=\"" << byfollower_cid << "\" "
					  << "zky_pctrs=\"" << byfollowerctnr_objid << "\" "
					  << "/>";

	AosXmlTagPtr byfollower_doc = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, byfollower_docstr, follower_cid, "", true, true, false, false, true, true);
	aos_assert_rr(byfollower_doc, rdata, false);
OmnScreen << byfollower_doc->toString() << endl;


	OmnString followerctnr_objid;
	followerctnr_objid << AOSTAG_CTNR_FOLLOWER << "." << byfollower_cid;

	OmnString follower_docstr;
	follower_docstr << "<follower "
					<< "zky_follower_cid=\"" << follower_cid << "\" "
					<< "zky_byfollower_cid=\"" << byfollower_cid << "\" "
					<< "zky_pctrs=\"" << followerctnr_objid << "\" "
					<< "/>";
	//access
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	AosXmlTagPtr follower_doc = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, follower_docstr, byfollower_cid, "", true, true, false, false, true, true);
	rdata->setUserid(userid);
	aos_assert_rr(follower_doc, rdata, false);
OmnScreen << follower_doc->toString() << endl;
	OmnString contents;
	contents << "<Contents>"
			 << "Add follower success"
			 << "</Contents>";
	rdata->setResults(contents);
	return true;
}


bool
AosMicroblog::removeFollower(
		const OmnString &byfollower_cid,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(byfollower_cid !="", rdata, false);
	OmnString follower_cid = rdata->getCid();
	aos_assert_rr(follower_cid !="", rdata, false);
	aos_assert_rr(follower_cid != byfollower_cid, rdata, false);
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, false);

	OmnString byfollowerctnr_objid;
	byfollowerctnr_objid << AOSTAG_CTNR_BYFOLLOWER << "." << follower_cid;

	OmnString followerctnr_objid;
	followerctnr_objid << AOSTAG_CTNR_FOLLOWER << "." << byfollower_cid;

	bool duplicated1 = false;
	//u64 byfollower_docid = AosIILClient::getSelf()->getCtnrMemDocid(
	u64 byfollower_docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(
		siteid, byfollowerctnr_objid, "zky_follower_cid", follower_cid,
		"zky_byfollower_cid", byfollower_cid, duplicated1, rdata);

	bool duplicated2 = false;
	//u64 follower_docid = AosIILClient::getSelf()->getCtnrMemDocid(
	u64 follower_docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(
		siteid, followerctnr_objid, "zky_follower_cid", follower_cid,
		"zky_byfollower_cid", byfollower_cid, duplicated2, rdata);

OmnScreen << "removefollower: byfollower" << byfollower_docid << " :follower " << follower_docid << endl;
	if (byfollower_docid == 0 || follower_docid == 0)
	{
		OmnString contents;
		contents << "<Contents>"
				 << "You did not follow this user"
				 << "</Contents>";
		rdata->setResults(contents);
		return true;
	}

	OmnString byfollower_docidstr;
	byfollower_docidstr << byfollower_docid;
	bool del1 = AosDocClientObj::getDocClient()->deleteObj(rdata, byfollower_docidstr,
			"", byfollowerctnr_objid, true);
	aos_assert_rr(del1, rdata, false);

	OmnString follower_docidstr;
	follower_docidstr << follower_docid;
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	bool del2 = AosDocClientObj::getDocClient()->deleteObj(rdata, follower_docidstr,
			"", followerctnr_objid, true);
	aos_assert_rr(del2, rdata, false);
	rdata->setUserid(userid);

	OmnString contents;
	contents << "<Contents>"
			 << "Remove follower success"
			 << "</Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	return true;
}

bool 
AosMicroblog::saveMBlogMsg(
		u64 &log_docid,
		const OmnString &msg,
		const AosRundataPtr &rdata)
{
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	OmnString send_time = OmnGetTime(AosLocale::eChina);
	OmnString logstr;
	logstr << "<msg from=\"" << sender_cid << "\" "
		   << "zky_pctrs=\"" << AOSTAG_CTNR_MICROBLOG << "\" "
		   << AOSTAG_CTNR_PUBLIC << "=\"true\" " 
		   << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		   << "type=\"microblogmsg\" "
		   << "zky_send_time=\"" << send_time << "\" "
		   << "><![CDATA["
		   << msg
		   << "]]></msg>";
OmnScreen << "**************microblog msg: " << logstr << endl;
	AosXmlTagPtr log_doc = AosDocClientObj::getDocClient()->createDocSafe1(rdata, logstr, sender_cid, "",
				true, true, false, false, true, true);
	aos_assert_rr(log_doc, rdata, false);
	log_docid = log_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(log_docid, rdata, false);

	OmnString blog_ctnrid;
	blog_ctnrid << AOSTAG_CTNR_MICROBLOG << "." << sender_cid;
	OmnString mblog_docstr;
	mblog_docstr << "<microblog "
				 << "zky_send_cid=\"" << sender_cid << "\" "
				 << "zky_pctrs=\"" << blog_ctnrid << "\" "
				 << "zky_log_docid=\"" << log_docid << "\" "
				 << "><![CDATA["
				 << msg
				 << "]]></microblog>";
	AosXmlTagPtr mblog_doc = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, mblog_docstr, sender_cid, "", true, true, false, false, true, true);
	aos_assert_rr(mblog_doc, rdata, false);
	return true;
}

bool
AosMicroblog::sendCirculationMsg(
		const AosXmlTagPtr &msg,
		const OmnString &follower_cid,
		const AosRundataPtr &rdata)
{
	/*
	//send msg to this user, don't to save
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	OmnString msgstr = msg->toString();
	OmnString request;
	request << "<request operation=\"sendmsg\">"
			<< "<objdef>"
			<< "<Contents "
	 		<< "zky_appname=\"" << AOSTAG_APP_MICROBLOG << "\" "
	 		<< "type=\"circulationmsg\" "
	 		<< ">"
	 		<< "<from>" << sender_cid << "</from>"
	 		<< "<to>" << follower_cid << "</to>"
			<< msgstr
	 		<< "</Contents>"
			<< "</objdef>"
			<< "</request>";

	bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, follower_cid, rdata);
	aos_assert_rr(notify, rdata, false);

	OmnString follower_ctnrid;
	follower_ctnrid << AOSTAG_CTNR_CFOLLOWER << "." << follower_cid;
	OmnString f_iilname = AosIILName::composeCtnrMemberObjidIILName(follower_ctnrid);
	//OmnString f_iilname = AosIILName::composeCtnrMemberListing(follower_ctnrid, "");
	AosQueryRsltObjPtr f_queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr f_query_context = AosQueryContextObj::createQueryContextStatic();

	// Chen Ding, 03/04/2012
	// ICE_WARN, how to set page size. Need review.
	f_query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(f_iilname, f_queryRslt, 0, f_query_context, rdata);

	aos_assert_rr(rslt, rdata, false);

	u64 f_docid = 0;
	bool f_finished = false;
	while(f_queryRslt->nextDocid(f_docid, f_finished, rdata))
	{
		if (f_finished)
		{
			break;
		}
		aos_assert_rr(f_docid !=0, rdata, false);
		AosXmlTagPtr c_follower_doc = AosDocClientObj::getDocClient()->getDocByDocid(f_docid, rdata);
		if (!c_follower_doc)
		{
			rdata->setError() << "follower doc is not exist: " << f_docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			continue;
		}

		OmnString c_follower_cid = c_follower_doc->getAttrStr("zky_follower_cid", "");
		aos_assert_rr(c_follower_cid != "", rdata, false);
		OmnString request;
		request << "<request operation=\"sendmsg\">"
				<< "<objdef>"
				<< "<Contents "
	 			<< "zky_appname=\"" << AOSTAG_APP_MICROBLOG << "\" "
	 			<< "type=\"circulationmsg\" "
	 			<< ">"
	 			<< "<from>" << sender_cid << "</from>"
	 			<< "<to>" << c_follower_cid << "</to>"
				<< msgstr
	 			<< "</Contents>"
				<< "</objdef>"
				<< "</request>";

		bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, c_follower_cid, rdata);
		aos_assert_rr(notify, rdata, false);
	}
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}


bool
AosMicroblog::sendPrivateMsg(
		const OmnString &msg,
		const OmnString &follower_cid,
		const AosRundataPtr &rdata)
{
	/*
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	u64 log_docid = 0;
	aos_assert_rr(saveMBlogMsg(log_docid, msg, rdata), rdata, false);

	OmnString blog_ctnrid;
	blog_ctnrid << AOSTAG_CTNR_MICROBLOG << "." << follower_cid;
	OmnString mblog_docstr;
	mblog_docstr << "<microblog "
			 	 << "zky_send_cid=\"" << sender_cid << "\" "
				 << "zky_pctrs=\"" << blog_ctnrid << "\" "
				 << "zky_log_docid=\"" << log_docid << "\" "
				 << "><![CDATA["
				 << msg
				 << "]]></microblog>";

	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	AosXmlTagPtr mblog_doc = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, mblog_docstr, follower_cid, "", true, true, false, false, true, true);
	rdata->setUserid(userid);
	aos_assert_rr(mblog_doc, rdata, false);

	//check online status ,and notify
	int status = 0;
	AosMsgSvrOnlineMgr::getSelf()->getUserOnlineStatus(status, follower_cid, rdata);
	if (status == 0)
	{
		return true;
	}
	OmnString send_time = OmnGetTime(AosLocale::eChina);
	OmnString request;
	request << "<request operation=\"sendmsg\">"
			<< "<objdef>"
			<< "<Contents "
	 		<< "zky_appname=\"" << AOSTAG_APP_MICROBLOG << "\" "
	 		<< "type=\"microblogmsg\" "
	 		<< ">"
	 		<< "<from>" << sender_cid << "</from>"
	 		<< "<to>" << follower_cid << "</to>"
	 		<< "<msg from=\"" << sender_cid << "\" "
			<< "to=\"" << follower_cid << "\" "
	 		<< "type=\"microblogmsg\" "
	 		<< "zky_send_time=\"" << send_time << "\" "
	 		<< "><![CDATA["
	 		<< msg
	 		<< "]]></msg>"
	 		<< "</Contents>"
			<< "</objdef>"
			<< "</request>";
	bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, follower_cid, rdata);
	aos_assert_rr(notify, rdata, false);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}
	

bool
AosMicroblog::sendMBlogMsg(
		const OmnString &msg,
		const AosRundataPtr &rdata)
{
	/*
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	u64 log_docid = 0;
	aos_assert_rr(saveMBlogMsg(log_docid, msg, rdata), rdata, false);
	OmnString send_time = OmnGetTime(AosLocale::eChina);

	OmnString follower_ctnrid;
	follower_ctnrid << AOSTAG_CTNR_FOLLOWER << "." << sender_cid;
	OmnString f_iilname = AosIILName::composeCtnrMemberObjidIILName(follower_ctnrid);
	//OmnString f_iilname = AosIILName::composeCtnrMemberListing(follower_ctnrid, "");
	AosQueryRsltObjPtr f_queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr f_query_context = AosQueryContextObj::createQueryContextStatic();

	// Chen Ding, 03/04/2012
	// ICE_WARN, how to set page size. Need review.
	f_query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(f_iilname, f_queryRslt, 0, f_query_context, rdata);

	aos_assert_rr(rslt, rdata, false);
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	u64 f_docid = 0;
	bool f_finished = false;
	while(f_queryRslt->nextDocid(f_docid, f_finished, rdata))
	{
		if (f_finished)
		{
			break;
		}
		aos_assert_rr(f_docid !=0, rdata, false);
		AosXmlTagPtr follower_doc = AosDocClientObj::getDocClient()->getDocByDocid(f_docid, rdata);
		if (!follower_doc)
		{
			rdata->setError() << "follower doc is not exist: " << f_docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			continue;
		}

		OmnString follower_cid = follower_doc->getAttrStr("zky_follower_cid", "");
		aos_assert_rr(follower_cid != "", rdata, false);

		OmnString blog_ctnrid;
		blog_ctnrid << AOSTAG_CTNR_MICROBLOG << "." << follower_cid;

		OmnString mblog_docstr;
		mblog_docstr << "<microblog "
				 	 << "zky_send_cid=\"" << sender_cid << "\" "
					 << "zky_pctrs=\"" << blog_ctnrid << "\" "
					 << "zky_log_docid=\"" << log_docid << "\" "
					 << "><![CDATA["
					 << msg
					 << "]]></microblog>";
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		AosXmlTagPtr mblog_doc = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, mblog_docstr, follower_cid, "", true, true, false, false, true, true);
		rdata->setUserid(userid);
		aos_assert_rr(mblog_doc, rdata, false);

		//check online status ,and notify
		int status = 0;
		AosMsgSvrOnlineMgr::getSelf()->getUserOnlineStatus(status, follower_cid, rdata);
		if (status == 0)
		{
			continue;
		}

		OmnString request;
		request << "<request operation=\"sendmsg\">"
				<< "<objdef>"
				<< "<Contents "
		 		<< "zky_appname=\"" << AOSTAG_APP_MICROBLOG << "\" "
		 		<< "type=\"microblogmsg\" "
		 		<< ">"
		 		<< "<from>" << sender_cid << "</from>"
		 		<< "<to>" << follower_cid << "</to>"
		 		<< "<msg from=\"" << sender_cid << "\" "
				<< "to=\"" << follower_cid << "\" "
		 		<< "type=\"microblogmsg\" "
		 		<< "zky_send_time=\"" << send_time << "\" "
		 		<< "><![CDATA["
		 		<< msg
		 		<< "]]></msg>"
		 		<< "</Contents>"
				<< "</objdef>"
				<< "</request>";
		bool notify = AosMsgSvrOnlineMgr::getSelf()->notifyMsgSvr(request, follower_cid, rdata);
		aos_assert_rr(notify, rdata, false);
	}
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}


bool
AosMicroblog::removeMBlogMsg(
		const OmnString &log_docid,
		const AosRundataPtr &rdata)
{
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	OmnString microblogctnr_objid;
	microblogctnr_objid << AOSTAG_CTNR_MICROBLOG << "." << sender_cid;
	bool duplicated = false;
	//u64 microblog_docid = AosIILClient::getSelf()->getCtnrMemDocid(
	u64 microblog_docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(
		siteid, microblogctnr_objid, "zky_send_cid", sender_cid, 
		"zky_log_docid", log_docid, duplicated, rdata);

	if (microblog_docid != 0)
	{
		OmnString microblog_docidstr;
		microblog_docidstr << microblog_docid;
		bool del = AosDocClientObj::getDocClient()->deleteObj(rdata, microblog_docidstr,
			"", microblogctnr_objid, true);
		aos_assert_rr(del, rdata, false);
	}
	return true;
}
