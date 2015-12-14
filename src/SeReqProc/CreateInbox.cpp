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
#include "SeReqProc/CreateInbox.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosCreateInbox::AosCreateInbox(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATE_INBOX, 
		AosSeReqid::eCreateInbox, rflag)
{
}


bool 
AosCreateInbox::proc(const AosRundataPtr &rdata)
{
	// This function creates a inbox, and do the following:
	// 1. create inbox container, then set parent container of the inbox
	// 		by getting the attribute of AosTag_home_ctnr from the user's doc.
	// 2. create a container in the inbox,
	//<request reqid = "" >
	//		<item name='zky_siteid'><![CDATA[100]]></item>
	//		<item name='operation'><![CDATA[serverreq]]></item>
	//		<item name='trans_id'><![CDATA[8]]></item>
	//		<item name='zky_ssid'><![CDATA[xxx]]></item>
	//		<item name='reqid'><![CDATA[create_inbox]]></item>
	//		<objdef>
	//			<inbox zky_objid="inbox" AOSTAG_MSGCONTAINER_OBJID="container">
	//			</inbox>
	//		</objdef>
	//</request>
	//Doc's format as follow: 
	//	1.inbox container:   msgcontainer_objid = aaa  zky_objid = inbox
	//	2.container:   zky_objid=aaa.100018 zky_pctrs=inbox 
	//	3.msgcontainer A      zky_pctrs=aaa name = "dd"
	//	4.msgcontainer B      zky_pctrs=aaa name = "cc"
	
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
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

	//get user doc
	OmnString cid = rdata->getCid();
	if(cid == "")
	{
		rdata->setError() << "Missing cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	u32 siteid = rdata->getSiteid();
	u64 did = AosDocClientObj::getDocClient()->getDocidByCloudid(siteid, cid, rdata);
	if (did == AOS_INVDID)
	{
		rdata->setError() << "Missing Container cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr userdoc = AosDocClientObj::getDocClient()->getDocByDocid(did, rdata);
	OmnString home_ctnr = userdoc->getAttrStr(AOSTAG_CTNR_HOME);
	userobj->setAttr(AOSTAG_PARENTC, home_ctnr);
	userobj->setAttr(AOSTAG_OTYPE, AOSOTYPE_CONTAINER);


	//check inbox is exist
	OmnString tempid = userobj->getAttrStr(AOSTAG_OBJID, "");
	if(tempid == "")
	{
		rdata->setError() << "Missing inbox objid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	bool objflag = checkMsgCtnrObjid(userobj, rdata);
	if(!objflag)
	{
		rdata->setError() << "AOSTAG_MSGCONTAINER_OBJID is not correct in the inbox";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr inbox = AosDocClientObj::getDocClient()->getDocByObjid(tempid, rdata);
	if (!inbox)
	{
		//create inbox
		inbox = createInbox(userobj, cid, rdata);
		if(!inbox)
		{
			rdata->setError() << "Can not creat one inbox";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}
	bool objflag2 = checkMsgCtnrObjid(inbox, rdata);
	if(!objflag2)
	{
		rdata->setError() << "AOSTAG_MSGCONTAINER_OBJID is not correct in the inbox";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//create container
	OmnString inboxid = inbox->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(inboxid != "",false); 
	OmnString containerid = inbox->getAttrStr(AOSTAG_MSGCONTAINER_OBJID, ""); 
	AosXmlTagPtr container = createContainer(containerid, cid, inboxid, rdata);
	if(!container)
	{
		rdata->setError() << "Can not create container";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	//check parent container is the same
	OmnString parentid = container->getAttrStr(AOSTAG_PARENTC, "");
	if(parentid == "")
	{
		rdata->setError() << "Missing parent id";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	if(parentid != inboxid)
	{
		rdata->setError() << "It is wrong to create parent id";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}


	OmnString rslts = "<Contents>";
	rslts << inbox->toString();
	rslts << container->toString();
	rslts << "</Contents>";
	rdata->setResults(rslts);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}


bool AosCreateInbox::checkMsgCtnrObjid(
		const AosXmlTagPtr &inboxobj, 
		const AosRundataPtr &rdata)
{
	OmnString containerid = inboxobj->getAttrStr(AOSTAG_MSGCONTAINER_OBJID, ""); 
	if(containerid == "")
	{
		return false;
	}

	//check if AOSTAG_MSGCONTAINER_OBJID has the cloud id
	bool flag = checkHasCloudId(containerid, rdata);
	if(flag)
	{
		return false;
	}
	return true;

}

bool
AosCreateInbox::checkHasCloudId(
		const OmnString &ctnr_objid, 
		const AosRundataPtr &rdata)
{
	int c = ctnr_objid.find('.', false);
	if(c == -1)
	{
		return false;
	}
	OmnString temp = OmnStrParser1::getLastWord(ctnr_objid, '.');
	OmnString cloudid = temp.substr(1);
	if (cloudid == "")
	{
		return false;
	}
	OmnString cid = rdata->getCid();
	if(cloudid != cid)
	{
		return false;
	}
	u32 siteid = rdata->getSiteid();
	u64 did = AosDocClientObj::getDocClient()->getDocidByCloudid(siteid, cloudid, rdata);
	if (did == AOS_INVDID)
	{
		return false;
	}
	return true;
}

AosXmlTagPtr 
AosCreateInbox::createInbox(
		const AosXmlTagPtr &userobj, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{

	OmnString docstr = userobj->toString();
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, docstr, cid, "", true, false,
		false, false, true, true);
	OmnString errmsg;
	if (!ctnr)
	{
		AosSetErrorU(rdata, "failed_create_msg_ctnr") << ": " << docstr << enderr;
		return 0;
	}
	return ctnr;

}

AosXmlTagPtr 
AosCreateInbox::createContainer(
		const OmnString &containerid, 
		const OmnString &cid, 
		const OmnString &inboxid, 
		const AosRundataPtr &rdata)
{
	OmnString docstr = "<doc ";
	docstr 	<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
		    << AOSTAG_PARENTC << "=\"" << inboxid << "\" "
		    << AOSTAG_OBJID << "=\"" << containerid << "\" "
		    << AOSTAG_CTNR_PUBLIC << "=\"true\" "
		    << AOSTAG_PUBLIC_DOC << "=\"false\" "
			<< AOSTAG_STYPE << "=\"" << AOSOTYPE_MSGCONTAINER << "\"/>";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	if (!doc)
	{
		return NULL;
	}
	AosXmlTagPtr cmd;
	bool resolve_objid = false;
	bool flag = AosDocClientObj::getDocClient()->createDoc1(rdata, cmd, cid, doc, resolve_objid, 0, 0, true);
	if(!flag)
	{
		return NULL;
	}
	bool duplicated = true;
	OmnString tempctnrid;
	tempctnrid << containerid << "." << cid;
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDoc(rdata, "", tempctnrid, duplicated);
	OmnString errmsg;
	if (!ctnr)
	{
		AosSetErrorU(rdata, "failed_create_msg_ctnr") << ": " << docstr << enderr;
		return NULL;
	}
	return ctnr;
}

