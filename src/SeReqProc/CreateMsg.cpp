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
// 05/27/2011	Created by Jackie Zhao
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CreateMsg.h"

#include "SEInterfaces/DocClientObj.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosCreateMsg::AosCreateMsg(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATE_MSG, 
		AosSeReqid::eCreateMsg, rflag)
{
}


bool 
AosCreateMsg::proc(const AosRundataPtr &rdata)
{
	// This function creates a message container. It will create an
	// XML doc based on the input, and do the following:
	// 	1. Set the parent container to 'inbox objid'. 
	//<request reqid = "" >
	//		<item name='zky_siteid'><![CDATA[100]]></item>
	//		<item name='operation'><![CDATA[serverreq]]></item>
	//		<item name='trans_id'><![CDATA[8]]></item>
	//		<item name='zky_ssid'><![CDATA[xxx]]></item>
	//		<item name='reqid'><![CDATA[create_msg]]></item>
	//		<item name='args'><![CDATA[rlsdctnr="releaseddoccontainer"]]></item>
	//		<objdef>
	//			<msg zky_pctrs="inbox" zky_otype=AOSOTYPE_MESSAGE  zky_sender="tom" zky_receivers="jakcie, ice" 
	//				zky_subject="a" zky_priority="b" zky_tags="c" zky_category="d" zky_msgtype="e">
	//			</msg>
	//		</objdef>
	//</request>
	//notice : 1.modify the zky_pctrs(inbox) to inbox.100018
	//		   2.create doc for each receiver
	//		   3.create one doc for the rlsdctnr;
	
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		rdata->setError() << "Missing siteid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();
	if (!rootchild)
	{
		rdata->setError() << "Missing root child";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	AosXmlTagPtr userobj;
	OmnString errmsg;
	if (!objdef || !(userobj = objdef->getFirstChild()))
	{
		AosSetError(rdata, "missing_userinfo");
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString cid = rdata->getCid();
	OmnString rslts = "<Contents>";
	//notice : modify the parent container, create the doc for each receiver.
	OmnString ctnr = userobj->getAttrStr(AOSTAG_PARENTC, "");
	if (ctnr == "")
	{
		rdata->setError() << "Missing the inbox container";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString receivers = userobj->getAttrStr("zky_receivers", "");
	if (receivers == "")
	{
		rdata->setError() << "Missing the receivers";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnStrParser1 parser(receivers, ",");

	while(parser.hasMore())
	{
		OmnString inboxname;
		inboxname << ctnr;
		AosXmlTagPtr doc = userobj->clone(AosMemoryCheckerArgsBegin);
		doc->setAttr(AOSTAG_PARENTC, inboxname);
		//remove zky_objid, zky_docid, zky_public_doc, zky_public_ctnr
		doc->removeAttr(AOSTAG_OBJID);
		doc->removeAttr(AOSTAG_DOCID);
		doc->removeAttr(AOSTAG_PUBLIC_DOC);
		doc->removeAttr(AOSTAG_CTNR_PUBLIC);
		AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, doc->toString(), parser.nextWord(), "", false, false,
			false, false, true, true);
		aos_assert_r(dd, false);
		rslts << dd->toString();
	}
	//1.get parameters
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		rdata->setError() << "Missing args!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString rlsdctnr;
	AosParseArgs(args, "rlsdctnr", rlsdctnr);
	OmnString rlsdcontainer;
	rlsdcontainer << rlsdctnr;
	AosXmlTagPtr doc1 = userobj->clone(AosMemoryCheckerArgsBegin);
	doc1->setAttr(AOSTAG_PARENTC, rlsdcontainer);
	//remove zky_objid, zky_docid, zky_public_doc, zky_public_ctnr
	doc1->removeAttr(AOSTAG_OBJID);
	doc1->removeAttr(AOSTAG_DOCID);
	doc1->removeAttr(AOSTAG_PUBLIC_DOC);
	doc1->removeAttr(AOSTAG_CTNR_PUBLIC);
	AosXmlTagPtr dd1 = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, doc1->toString(), cid, "", false, false,
		false, false, true, true);
	aos_assert_r(dd1, false);
	rslts << dd1->toString();

	rslts << "</Contents>";
	rdata->setOk();
	rdata->setResults(rslts);
	AOSLOG_LEAVE(rdata);
	return true;
}


