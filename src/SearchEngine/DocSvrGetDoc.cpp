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
// 07/20/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/DocServer1.h"

#include "DocClient/DocidMgr.h"
#include "Security/SecurityMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/XmlDoc.h"

/*
u64 
AosDocServer::getDocidByCloudid(
		const OmnString &siteid, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	OmnString iilname = AOSZTG_CLOUDID;
	iilname << siteid;
	OmnString errmsg;
	bool isunique = false;
	u64 docid = 0;
	//Zky2248,Linda, 01/13/2011
	aos_assert_r(cid!="", 0);
	AosIILClient::getSelf()->getDocid(iilname, cid, docid, isunique, rdata);
	if (docid != AOS_INVDID)
	{
		aos_assert_r(isunique, AOS_INVDID);
		return docid;
	}
	return AOS_INVDID;
}



AosXmlTagPtr
AosDocServer::getTempDoc(
		const OmnString &siteid,
		const OmnString &objid)
{
	// The temporary doc objid format is:
	//	AOSZTG_TEMPOBJ + "_" + seqno + "_" + offset
	// It checks whether the objid starts with AOSZTG_TEMPOBJ. If no, return null.
	// Otherwise, it retrieves the seqno and offset from the object, and then
	// retrieve the doc from the log file. If found, create it and return it.
	// Otherwise, return null.
	aos_assert_r(objid!="", 0);
	OmnString pattern;
	pattern << AOSZTG_TEMPOBJ << "_";
	int len = pattern.length();
	if (strncmp(objid.data(), pattern.data(), len) != 0) return 0;
	u64 seqno, offset;
	bool rs = false;
	u32 wlen;
	OmnStrParser1 parser(objid, "_");
	parser.nextWord();
	OmnString word = parser.nextWord();
	if (word != "")
	{
		wlen = word.length();
		rs = word.parseU64((u32)0, wlen, seqno);
	}
	if (!rs) return 0;
	word = parser.nextWord();
	if (word != "")
	{	
		wlen = word.length();
		rs = word.parseU64((u32)0, wlen, offset);
	}
	if (!rs) return 0;	
	return getTempDoc(seqno, offset);
}


OmnString 
AosDocServer::getCloudid(const u64 &user_docid, const AosRundataPtr &rdata)
{
	AosUserAcctObjPtr acct = AosDocMgr1::getSelf()->getUserAcct(user_docid, rdata);
	if (!acct) return "";
	return acct->getCloudid();
}

*/
