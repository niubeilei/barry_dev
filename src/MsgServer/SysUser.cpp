////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This class is used to manage an online user. There shall be one
// instance of this class for each of the online users. When an online
// user wants to pull messages, it sends a request, which will eventually
// call the function: readMsg(...). If there are already messages, it
// reads the messages and returns. Otherwise, it will wait there. 
//
// When someone has something to send to an online user, it will call
// the member function msgReceived(...). If the user was trying to read
// messags, this function will wake up the user, which will read the
// messages and return.
//
// Modification History:
// 08/01/2011: Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "MsgServer/SysUser.h"
#include "SEUtil/IILName.h"
#include "SEInterfaces/DocClientObj.h"
#include "Util/SPtr.h"


AosSysUser::AosSysUser(const OmnString &cid)
:
mLock(OmnNew OmnMutex()),
mWaitTimeSec(eDefaultWaitSec),
mConn(0),
mStatus(eOffline),
mLastRecvTime(0),
mCallBack(""),
mWait(false),
mCid(cid)
{
}


AosSysUser::~AosSysUser()
{
}

bool
AosSysUser::readMsg(OmnString &msg)
{
	msg = "";
	while (1)
	{
		mLock->lock();
		if (mMsgs.empty()) 
		{
			mLock->unlock();
			return true;
		}
		msg = mMsgs.front();
		mMsgs.pop();
		mLock->unlock();
		return true;
	}
	OmnShouldNeverComeHere;
	return false;
}

bool
AosSysUser::sendMsg(const OmnString &msg)
{
	mLock->lock();
	mMsgs.push(msg);
	mLock->unlock();
	return true;
}

bool
AosSysUser::readMsgFromDb(
		u64 &docid,
		OmnString &msg,
		const OmnString &appname,
		const AosRundataPtr &rdata)
{
	//1. construct pending msg container
	//2. read msg from container
	//3. if read success, delete it by the container
	/*
	OmnString pendingmsg_ctnr;
	pendingmsg_ctnr << AOSTAG_CTNR_PENDINGMSG << "." << mCid;
	OmnString iilname = AosIILName::composeCtnrMemberListing(pendingmsg_ctnr, "");
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	bool rslt = AosIILClient::getSelf()->querySafe(
	iilname, queryRslt, 0, eAosOpr_an, "", rdata);
	aos_assert_rr(rslt, rdata, false);
	if (queryRslt->isEmpty())
	{
		return true;
	}
	bool finished = false;
	if (queryRslt->nextDocid(docid, finished, rdata))
	{
		aos_assert_rr(docid !=0, rdata, false);
		AosXmlTagPtr msg_doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		aos_assert_rr(msg_doc, rdata, false);
		AosXmlTagPtr contents = msg_doc->getFirstChild("Contents");
		aos_assert_rr(contents, rdata, false);
		msg = contents->toString();
		//delete doc
		//OmnString del_docid;
		//del_docid << docid;
		//bool del = AosDocClientObj::getDocClient()->deleteObj(rdata, del_docid,
		//		"", pendingmsg_ctnr, true);
		//aos_assert_rr(del, rdata, false);
	}
	*/
	return true;
}

bool
AosSysUser::saveMsgToDb(
		const OmnString &contents,
		const OmnString &recver_cid,
		const OmnString &appname,
		const AosRundataPtr &rdata)
{
	//1. save msg to the container
	//2. msg format :
	//<pendingmsg zky_pctrs="xxxx">
	//	<Contents zky_appname="xxxx">
	//		<msg from="xxxx" to="xxxx">xxxx</msg>
	//	</Contents>
	//</pendingmsg>
	/*
	OmnString msg;
	msg << "<pendingmsg "
		<< "zky_pctrs=\"" << AOSTAG_CTNR_PENDINGMSG << "." << recver_cid << "\">"
		<< contents
		<< "</pendingmsg>";
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(
	rdata, msg, recver_cid, "", true, true, false, false, true, true);
	aos_assert_rr(doc, rdata, false);
OmnScreen << doc->toString() << endl;
	*/
	return true;
}

