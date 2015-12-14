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
#include "MsgService/MsgService.h"

#include "SEUtil/IILName.h"
#include "MsgClient/MsgClient.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/MsgClientObj.h"
#include "SEModules/ObjMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/SeXmlParser.h"



OmnSingletonImpl(AosMsgServiceSingleton, 
				 AosMsgService, 
				 AosMsgServiceSelf, 
				 "AosMsgService");

AosDocClientObjPtr AosMsgService::smDocClient;
AosIILClientObjPtr AosMsgService::smIILClient;

AosMsgService::AosMsgService()
{
}


AosMsgService::~AosMsgService()
{
}


bool 
AosMsgService::start()
{
	return true;
}


bool 
AosMsgService::stop()
{
	return true;
}


bool
AosMsgService::config(const AosXmlTagPtr &def)
{
	return true;
}


bool 
AosMsgService::start(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosMsgService::getUserOnlineStatus(
		int &status,
		const OmnString &friend_cid,
		const AosRundataPtr &rdata)
{
	OmnString home_ctnr;
	home_ctnr << AOSCTNR_USERHOME << "." << friend_cid;
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);
	AosXmlTagPtr home_doc = smDocClient->getDocByObjid(home_ctnr, rdata);
	aos_assert_rr(home_doc, rdata, false);
	status = home_doc->getAttrInt("zky_status", 0);
	return true;
}

bool
AosMsgService::setUserOnlineStatus(
		const OmnString &status,
		const AosRundataPtr &rdata)
{
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	OmnString home_ctnr;
	home_ctnr << AOSCTNR_USERHOME << "." << sender_cid;
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);
	AosXmlTagPtr home_doc = smDocClient->getDocByObjid(home_ctnr, rdata);
	aos_assert_rr(home_doc, rdata, false);
	u64 home_docid = home_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(home_docid != 0, rdata, false);
	
	bool modify = smDocClient->modifyAttrStr1(rdata, home_docid, home_ctnr, 
			"zky_status", status, "", false, false, true);
	aos_assert_rr(modify, rdata, false);

	if (status == "0")
	{
		OmnString o_request;
		o_request << "<request operation=\"disconnect\" cid=\"" << sender_cid << "\"/>";
		bool rslt = AosMsgClientObj::procMsgSvrRequestStatic(o_request, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosMsgService::sendMsgToUser(
		const AosXmlTagPtr &contents,
		const AosRundataPtr &rdata)
{
	OmnString request;
	request << "<request operation=\"sendmsg\">"
			<< "<objdef>"
			<< contents->toString()
			<< "</objdef>"
			<< "</request>"; 
	bool send = AosMsgClientObj::procMsgSvrRequestStatic(request, rdata);
	aos_assert_r(send, false);
	return true;
} 

bool 
AosMsgService::sendMsgByAccount(
		const OmnString &msg_type,
		const OmnString &account_objid,
		const AosXmlTagPtr &contents,
		const AosRundataPtr &rdata)
{
	/*
	u64 sender_userid = rdata->getUserid();
	aos_assert_rr(sender_userid != 0, rdata, false);
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);
	AosXmlTagPtr account_doc = smDocClient->getDocByObjid(account_objid, rdata);
	aos_assert_rr(account_doc, rdata, false);

	bool sender_check = account_doc->getAttrBool("zky_sender_check", false);
	if (sender_check)
	{
		OmnString sender_objid = "";
		bool exist = existInSenderList(sender_objid, account_objid, rdata);
		if (!exist)
		{
			return true;
		}
	}

	if (!smIILClient) smIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(smIILClient, false);
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);

	u64 docid = 0;
	u64 msgiilid = 0;
	if (msg_type ==  "normal")
	{
		//1. save doc
		AosXmlTagPtr doc = smDocClient->createDocSafe1(rdata, contents->toString(),
			"", "", false, true, true, false, false, true);
		aos_assert_rr(doc, rdata, false);

		docid = doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_rr(docid != 0, rdata, false);

		//2. add docid to account msg iil
		msgiilid = account_doc->getAttrU64("zky_msg_iilid", 0);
		if (msgiilid == 0)
		{
			//1. create it
			//2. set to account doc
			bool create = smIILClient->createTablePublic(msgiilid, eAosIILType_U64, rdata);
			aos_assert_rr(create, rdata, false);
			OmnString msgiilidstr;
			msgiilidstr << msgiilid;
			u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
			bool modify = smDocClient->modifyAttrStr1(rdata, 0, account_objid, 
					"zky_msg_iilid", msgiilidstr, "", false, false, true);
			rdata->setUserid(userid);
			aos_assert_rr(modify, rdata, false);
		}
		aos_assert_rr(msgiilid != 0, rdata, false);
		bool add = smIILClient->addU64ValueDocToTable(msgiilid, docid, 0, false, true, rdata);
		aos_assert_rr(add, rdata, false);
	}
	
	u64 follower_iilid = account_doc->getAttrU64("zky_follower_iilid", 0);
	if (follower_iilid == 0)
	{
		return true;
	}

	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);
	query_context->setBlockSize(300);
	AosQueryColumn(follower_iilid, queryRslt, 0, query_context, rdata);
	if (queryRslt->isEmpty())
	{
		return true;
	}
	u64 follower_userid = 0;
	bool finished = false;
	while(queryRslt->nextDocid(follower_userid, finished, rdata))
	{
		if (finished)
		{
			break;
		}
		aos_assert_rr(follower_userid != 0, rdata, false);

		if (follower_userid ==  sender_userid)
		{
			continue;
		}
		//msg type: normal, readonce, online
		int status = 0;
		OmnString follower_cid = smDocClient->getCloudid(follower_userid, rdata);
		getUserOnlineStatus(status, follower_cid, rdata);
		//if (follower_type == "normal")
		if (msg_type == "normal")
		{
			//save msg to the follower's queue (msg iil)
			//add docid to follower's msg iil
			AosUserAcctObjPtr user_acct = smDocClient->getUserAcct(follower_userid, rdata);
			aos_assert_rr(user_acct, rdata, false);
			AosXmlTagPtr user_doc = user_acct->getDoc();
			aos_assert_rr(user_doc, rdata, false);
			msgiilid = user_doc->getAttrU64("zky_msg_iil", 0);
			if (msgiilid == 0)
			{
				bool create = smIILClient->createTablePublic(msgiilid, eAosIILType_U64, rdata);
				aos_assert_rr(create, rdata, false);
				OmnString msgiilidstr;
				msgiilidstr << msgiilid;
				u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
				bool modify = smDocClient->modifyAttrStr1(rdata, sender_userid, "", 
						"zky_msg_iilid", msgiilidstr, "", false, false, true);
				rdata->setUserid(userid);
				aos_assert_rr(modify, rdata, false);
			}
			aos_assert_rr(msgiilid != 0, rdata, false);
			bool add = smIILClient->addU64ValueDocToTable(msgiilid, docid, 0, false, true, rdata);
			aos_assert_rr(add, rdata, false);
		}
		if (status)
		{
			contents->setAttr("to", follower_cid);
			OmnString sender_cid = rdata->getCid();
			aos_assert_rr(sender_cid != "", rdata, false);
			contents->setAttr("from", sender_cid);
			OmnString request;
			request << "<request operation=\"sendmsg\">"
					<< "<objdef>"
					<< contents->toString()
					<< "</objdef>"
					<< "</request>";
			bool send = AosMsgClient::getSelf()->procMsgSvrReq(request, follower_userid, rdata);
			aos_assert_rr(send, rdata, false);
		}
	}
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}


bool 
AosMsgService::createAccount(
		const OmnString &account_objid,
		const AosRundataPtr &rdata)
{
	/*
	OmnString creator = rdata->getCid();
	aos_assert_rr(creator!= "", rdata, false);
	OmnString docstr;
	docstr << "<ctnr "
		   << "zky_objid=\"" << account_objid << "\" " 
		   << "zky_sender_iilid=\"\""
		   << "zky_follower_iilid=\"\""
		   << "zky_check=\"false\""
		   << "zky_sender_check=\"" << false << "\" "
		   << "zky_owner=\"" << creator << "\" "
		   << "zky_otype=\"zky_ctnr\""
		   << "zky_stype=\"zky_auto\""
		   << "/>";
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);
	AosXmlTagPtr account_doc = smDocClient->createDocSafe1(
			rdata, docstr, "", "", false, true, true, false, false, true);
	aos_assert_rr(account_doc, rdata, false);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}

bool
AosMsgService::removeAccount(
		const OmnString &account_objid,
		const AosRundataPtr &rdata)
{
	/*
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);
	bool del = smDocClient->deleteObj(rdata, "",
			        account_objid, "", true);
	aos_assert_rr(del, rdata, false);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}

bool
AosMsgService::addFollowerToAcct(
		const OmnString &follower_cid,
		const OmnString &account_objid,
		const AosRundataPtr &rdata)
{
	/*
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);

	AosXmlTagPtr follower_doc = smDocClient->getDocByCloudid(follower_cid, rdata);
	aos_assert_rr(follower_doc, rdata, false);
	u64 follower_userid = follower_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(follower_userid != 0, rdata, false);

	AosXmlTagPtr account_doc = smDocClient->getDocByObjid(account_objid, rdata);
	aos_assert_rr(account_doc, rdata, false);
	u64 follower_iilid = account_doc->getAttrU64("zky_follower_iilid", 0);

	if (!smIILClient) smIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(smIILClient, false);
	if (!follower_iilid)
	{
		bool create = smIILClient->createTablePublic(follower_iilid, eAosIILType_U64, rdata);
		aos_assert_rr(create, rdata, false);
		OmnString follower_iilidstr;
		follower_iilidstr << follower_iilid;
		u64 account_docid = account_doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_rr(account_docid, rdata, false);
		u32 siteid = rdata->getSiteid();
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		bool modify = smDocClient->modifyAttrStr1(rdata, account_docid, account_objid, 
			"zky_follower_iilid", follower_iilidstr, "", false, false, true);
		rdata->setUserid(userid);
		aos_assert_rr(modify, rdata, false);
	}
	aos_assert_rr(follower_iilid != 0, rdata, false);
	bool add = smIILClient->addU64ValueDocToTable(
			follower_iilid, follower_userid, follower_userid, true, true, rdata);
	aos_assert_rr(add, rdata, false);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}

bool
AosMsgService::removeFollowerFromAcct(
		const OmnString &follower_cid,
		const OmnString &account_objid,
		const AosRundataPtr &rdata)
{
	/*
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);

	AosXmlTagPtr follower_doc = smDocClient->getDocByCloudid(follower_cid, rdata);
	aos_assert_rr(follower_doc, rdata, false);
	u64 follower_userid = follower_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(follower_userid != 0, rdata, false);

	AosXmlTagPtr account_doc = smDocClient->getDocByObjid(account_objid, rdata);
	aos_assert_rr(account_doc, rdata, false);
	u64 follower_iilid = account_doc->getAttrU64("zky_follower_iilid", 0);
	if (follower_iilid == 0)
	{
		return true;
	}
	if (!smIILClient) smIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(smIILClient, false);
	bool remove = smIILClient->removeU64ValueDoc(follower_iilid, follower_userid, follower_userid, rdata);
	aos_assert_rr(remove, rdata, false);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}

bool
AosMsgService::existInSenderList(
		OmnString &sender_objid,
		const OmnString &account_objid,
		const AosRundataPtr &rdata)
{
	/*
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);

	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	if (!smIILClient) smIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(smIILClient, false);

	OmnString account_iilname = AosIILName::composeCtnrMemberObjidIILName(account_objid);

	// Chen Ding, 03/04/2012
	query_context->setOpr(eAosOpr_an);
	query_context->setBlockSize(300);
	bool rslt = AosQueryColumn(account_iilname, queryRslt, 0, query_context, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (queryRslt->isEmpty())
	{
		return false;
	}
	
	u64 sender_userid = rdata->getUserid();
	aos_assert_rr(sender_userid != 0, rdata, false);
	AosUserAcctObjPtr sender_acct = smDocClient->getUserAcct(sender_userid, rdata);
	aos_assert_rr(sender_acct, rdata, false);

	bool found = false;
	bool finished = false;
	u64 docid = 0;
	while(queryRslt->nextDocid(docid, finished, rdata))
	{
		if (finished)
		{
			break;
		}
		aos_assert_rr(docid != 0, rdata, false);
		AosXmlTagPtr sender_doc = smDocClient->getDocByDocid(docid, rdata);
		aos_assert_rr(sender_doc, rdata, false);
		OmnString sender = sender_doc->getAttrStr("zky_sender", "");
		OmnString sender_type = sender_doc->getAttrStr("zky_sender_type", "");
		//sender type: username, cloudid, user_group, user_roles
		OmnString value = "";
		if (sender_type ==  "username")
		{
			value = sender_acct->getUsername();
		}
		else if (sender_type == "cloudid")
		{
			value = sender_acct->getCloudid();
		}
		else if (sender_type == "user_group")
		{
			value = sender_acct->getUserGroups();
		}
		else if (sender_type == "user_roles")
		{
			value = sender_acct->getUserRoles();
		}
		aos_assert_rr(value != "", rdata, false);
		aos_assert_rr(sender != "", rdata, false);
		if (sender == value)
		{
			sender_objid = sender_doc->getAttrStr("zky_objid", "");
			found = true;
			break;
		}
	}
	return found;
	*/
	OmnNotImplementedYet;
	return true;
}

bool
AosMsgService::addSender(
		const OmnString &sender,
		const OmnString &sender_type,
		const OmnString &account_objid,
		const AosRundataPtr &rdata)
{
	// 1. Retrieve the account
	// 2. Check whether the sender is in the list
	// 3. Add it.
	/*
	aos_assert_rr(sender != "", rdata, false);
	aos_assert_rr(sender_type != "", rdata, false);
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);
	AosXmlTagPtr account_doc = smDocClient->getDocByObjid(account_objid, rdata);
	aos_assert_rr(account_doc, rdata, false);
	bool check = account_doc->getAttrBool("zky_check", false);
	if (check)
	{
		//save this request
		//the owner account owner to add this sender
		return true;
	}
	OmnString sender_objid = "";
	bool exist = existInSenderList(sender_objid, account_objid, rdata);
	if (exist || sender_objid != "")
	{
		return true;
	}

	OmnString docstr;
	docstr << "<sender "
		   << "zky_public_ctnr=\"true\""
		   << "zky_sender=\"" << sender << "\" "
		   << "zky_sender_type=\"" << sender_type << "\" "
		   << "zky_pctrs=\"" << account_objid << "\" "
		   << "/>";

	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);
	//u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	AosXmlTagPtr sender_doc = smDocClient->createDocSafe1(
			rdata, docstr, "", "", false, true, true, false, false, true);
	//rdata->setUserid(userid);
	aos_assert_rr(sender_doc, rdata, false);
OmnScreen << "sender doc: " << sender_doc->toString() << endl;
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}


bool
AosMsgService::removeSender(
		const OmnString &account_objid,
		const AosRundataPtr &rdata)
{
	/*
	OmnString sender_objid = "";
	bool exist = existInSenderList(sender_objid, account_objid, rdata);
	if (!exist || sender_objid == "")
	{
		return true;
	}
	if (!smDocClient) smDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(smDocClient, false);
	aos_assert_rr(sender_objid != "", rdata, false)
	bool del = smDocClient->deleteObj(rdata, "",
			        sender_objid, account_objid, true);
	aos_assert_rr(del, rdata, false);
	return true;
	*/
	OmnNotImplementedYet;
	return true;
}

