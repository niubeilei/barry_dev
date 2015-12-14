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
// 08/15/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CreateComment.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "SeReqProc/ReqidNames.h"
#include "SmartDoc/SMDMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeLogUtil/LogNames.h"
#include "XmlUtil/XmlTag.h"


AosCreateComment::AosCreateComment(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATECOMMENT, AosSeReqid::eCreateComment, rflag)
{
}


bool 
AosCreateComment::proc(const AosRundataPtr &rdata)
{
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr xmldoc = root->getFirstChild("objdef");
	if (!xmldoc)
	{
		rdata->setError() << "Missing object";
		OmnAlarm <<  rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr doc = xmldoc->getFirstChild();
	aos_assert_r(doc, false);

	bool rslt = createComment(doc, rdata);
	AOSLOG_LEAVE(rdata);
	return rslt;
}


bool
AosCreateComment::createComment(
			const AosXmlTagPtr &doc, 
			const AosRundataPtr &rdata)
{
	// Doc should be this form:
	// <doc zky_pctrs="xxx"
	// 		zky_subcmt_objid="xxx">
	// 		<comment><[CDATA[XXX]]></comment>
	// </doc>
	// ????????????????????????????????????????????????????????
	// 1. In javascript, how can we get zky_subcmt_objid?
	// 2. When we retrieve coment, in general, we want to 
	// 	  retrieve the doc commented the subject and the doc 
	//    commented commented item, but we just index log time,
	//    can we get the doc what we want?
	// ????????????????????????????????????????????????????????
	aos_assert_rr(doc, rdata, false);

	// 1. Check log's parent container, if is "", set user's 
	//    AOSTAG_CTNR_CMT, if user's AOSTAG_CTNR_CMT is "" too,
	//    set user's AOSTAG_CTNR_HOME, if is "" too ,it is a error.
	OmnString pctr = doc->getAttrStr(AOSTAG_PARENTC);
	if (pctr == "")
	{
		u64 userid = rdata->getUserid();
		if (!userid)
		{
			rdata->setError() << "User not logged in";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		AosXmlTagPtr userdoc = AosDocClientObj::getDocClient()->getDoc(userid, "", rdata);
		if (!userdoc)
		{
			rdata->setError() << "Failed to get user information";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		OmnString hctnr;
		OmnString cmtctnr  = userdoc->getAttrStr(AOSTAG_CTNR_CMT);
		if (cmtctnr == "")
		{
			cmtctnr = userdoc->getAttrStr(AOSTAG_CTNR_HOME);
			if (cmtctnr == "")
			{
				rdata->setError() << "Can't Get Comment Parent Container";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		}
		doc->setAttr(AOSTAG_PARENTC, cmtctnr);
	}

	// 2. Get root comment's objid, if is null, it is a error.
	OmnString rootcmtobjid = doc->getAttrStr(AOSTAG_ROOTCOMMENT_OBJID);
	if (rootcmtobjid == "")
	{
		rdata->setError() << "Valid comment";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 3. Get root comment, can't get, it is a error.
	AosXmlTagPtr rootcomment = AosDocClientObj::getDocClient()->getDocByObjid(
							rootcmtobjid, rdata);
	if (!rootcomment)
	{
		rdata->setError() << "Valid comment";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 4. Get root comment's zky_pctrs, if is null, it is a error.
	OmnString rcmtpctr = rootcomment->getAttrStr(AOSTAG_PARENTC);
	if (rcmtpctr == "")
	{
		rdata->setError() << "Failed to retrieve root comment container's objid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 5. set log's otype AOSOTYPE_COMMENT
	doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_COMMENT);

	// 6. create log 
	//bool rslt = AosSeLogClientObj::getLogClient()->addLog(doc, rcmtpctr, AOSLOGNAME_COMMENT, rdata);
	bool rslt = AosAddLog(rdata, rcmtpctr, AOSLOGNAME_COMMENT, doc->toString());
	if (!rslt)
	{
		rdata->setError() << "Create log failed : " << doc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}

