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
//
// Modification History:
// 12/19/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ScanObjid.h"

#include "SEInterfaces/DocClientObj.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
AosScanObjid::AosScanObjid(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SCAN_OBJID, 
		AosSeReqid::eScanObjid, rflag)
{
}


bool 
AosScanObjid::proc(const AosRundataPtr &rdata)
{
	// It increments the value of an attribute of a specified object
	// by one.
	// 'args' are in the form:
	//      "objid=xxx,attrname=xxx"
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	OmnString startidx, endidx;
	AosParseArgs(args, "startdocid", startidx, "enddocid", endidx);
	if (startidx == "")
	{
		rdata->setError() << "Missing start docid!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (endidx == "")
	{
		rdata->setError() << "Missing end docid!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	u64 userid = rdata->getUserid();
	aos_assert_rr(userid, rdata, false);
	AosUserAcctObjPtr mRequesterAcct = AosDocClientObj::getDocClient()->getUserAcct(userid, rdata); 
	if (!mRequesterAcct->isSuperUser())
	{
		rdata->setError() << "Access Denied!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	u64 startdocid = atoll(startidx.data());
	aos_assert_rr(startdocid, rdata, false);
	u64 enddocid = atoll(endidx.data());
	aos_assert_rr(enddocid, rdata, false);

	u64 docid = 0;
	OmnString content;
	u64 total = 0;
	for (docid = startdocid; docid <= enddocid; docid++)
	{
		AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (doc)
		{
			verifyObjid(doc, rdata, content, total);
		}
	}
	OmnString contents = "<Contents ";
	contents << "total =\"" << total << "\" >"
		<< content << "</Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}


bool 
AosScanObjid::verifyObjid(AosXmlTagPtr &doc, const AosRundataPtr &rdata, OmnString &content, u64 &total)
{
	aos_assert_rr(doc, rdata, false);

	OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	if (objid == "") return true;

	OmnString creator = doc->getAttrStr(AOSTAG_CREATOR,"");
	if (creator == "") return true;

	OmnString prefix, cid;
	AosObjid::decomposeObjid(objid, prefix, cid);
	if (cid == "") return true;
	if (cid == creator) return true;

	AosXmlTagPtr creator_doc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
	if (creator_doc)
	{
		aos_assert_r(creator_doc->getAttrStr(AOSTAG_CLOUDID, "") == cid, false);

		bool rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				doc->getAttrU64(AOSTAG_DOCID, 0), 
				objid, AOSTAG_CREATOR, cid, false, false, true);
		aos_assert_r(rslt, false);
		AosXmlTagPtr checkdoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
		aos_assert_r(checkdoc, false);
		aos_assert_r(checkdoc->getAttrStr(AOSTAG_CREATOR, "") == cid, false);
	}
	else
	{
		content << objid << ",";
		total ++;
	}
	return true;
}
