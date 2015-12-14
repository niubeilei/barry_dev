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
// 10/25/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "InstantMsg/IMSession.h"

#include "SEInterfaces/DocClientObj.h"
#include "SeLogClient/SeLogClient.h"
#include "XmlUtil/SeXmlParser.h"


AosIMSession::AosIMSession(
		const OmnString &sender_cid,
		const OmnString &recver_cid)
:
mLock(OmnNew OmnMutex()),
mMsgs(""),
mSenderCid(sender_cid),
mRecverCid(recver_cid)
{
}


AosIMSession::~AosIMSession()
{
}


bool 
AosIMSession::addMsg(const OmnString &msg, const AosRundataPtr &rdata)
{
	// This function adds message 'msg' to the session. Messages
	// are kept in memory. When it is too big, contents are saved
	// in files. 
	if (msg == "") return true;
	mLock->lock();
	mMsgs << msg;
	mLock->unlock();
	return true;
}


bool 
AosIMSession::finishSession(
		const OmnString &recver_cid,
		const AosXmlTagPtr &notes,
		const AosRundataPtr &rdata)
{
	// This function finishes the session. It checks whether 
	// the contents need to be saved. If not, it does nothing. 
	// Otherwise, it will create an XML doc and send it to the Log Manager.
	// Create an XML Doc
	mLock->lock();
	OmnString logstr;
	if (notes)
	{
		notes->setAttr("zky_sender_cid", mSenderCid);
		notes->setAttr("zky_recver_cid", mRecverCid);
		logstr = notes->toString();
	}
	else
	{
		if (mMsgs == "")
		{
			mLock->unlock();
			return true;
		}
		logstr << "<logstr zky_sender_cid=\"" << mSenderCid << "\" "
			   << "zky_receiver_cid=\"" << mRecverCid << "\">"
			   << mMsgs
			   << "</logstr>";
	}

	AosXmlParser parser;
	AosXmlTagPtr logdoc = parser.parse(logstr, "" AosMemoryCheckerArgs);
	//AosXmlTagPtr logdoc = parser.parse(logstr, "");
	aos_assert_rl(logdoc, mLock, false);
	//u64 logid = AosSeLogClient::getSelf()->addLogWithResp(
	//		AOSTAG_CTNR_INSTANT_MSG, "", logdoc, rdata);
	//aos_assert_rr(logid>0, rdata, false);
	u64 logid = 1;

	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);

	OmnString entrystr1;
	entrystr1 << "<log "
			  << "zky_pctrs=\"" << AOSTAG_INSTANTMSG_CTNR << "." << sender_cid << "\" "
			  << "zky_friend_cid=\"" << recver_cid << "\" "
			  << "zky_user_cid=\"" << sender_cid << "\" "
			  << "zky_logid=\"" << logid << "\" "
			  << "/>";
	AosXmlTagPtr doc1 = AosDocClientObj::getDocClient()->createDocSafe1(rdata, entrystr1, 
			sender_cid, "", true, true, false, false, true, true);
	aos_assert_rr(doc1, rdata, false);

	OmnString entrystr2;
	entrystr2 << "<log "
			  << "zky_pctrs=\"" << AOSTAG_INSTANTMSG_CTNR << "." << recver_cid << "\" "
			  << "zky_friend_cid=\"" << sender_cid << "\" "
			  << "zky_user_cid=\"" << recver_cid << "\" "
			  << "zky_logid=\"" << logid << "\" "
			  << "/>";
	AosXmlTagPtr doc2 = AosDocClientObj::getDocClient()->createDocSafe1(rdata, entrystr2, 
			sender_cid, "", true, true, false, false, true, true);
	aos_assert_rr(doc2, rdata, false);
	mMsgs = "";
	mLock->unlock();
	return true;
}

