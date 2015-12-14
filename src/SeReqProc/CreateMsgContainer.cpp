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
#include "SeReqProc/CreateMsgContainer.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


AosCreateMsgContainer::AosCreateMsgContainer(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATE_MSGCONTAINER, 
		AosSeReqid::eCreateMsgContainer, rflag)
{
}


bool 
AosCreateMsgContainer::proc(const AosRundataPtr &rdata)
{
	// This function creates a message container. It will create an
	// XML doc based on the input, and do the following:
	// 	1. Set the parent container to 'inbox objid'. 
	//<request reqid = "" >
	//		<item name='zky_siteid'><![CDATA[100]]></item>
	//		<item name='operation'><![CDATA[serverreq]]></item>
	//		<item name='trans_id'><![CDATA[8]]></item>
	//		<item name='zky_ssid'><![CDATA[xxx]]></item>
	//		<item name='reqid'><![CDATA[create_msgcontainer]]></item>
	//		<objdef>
	//			<msgcontainer minboxid="inbox" subctnr="dd">
	//			</msgcontainer>
	//		</objdef>
	//</request>
	//Doc's format as follow: 
	//	1.inbox container:   msgcontainer_objid = aaa  zky_objid = inbox
	//	2.container:   zky_objid=aaa.100018 zky_pctrs=inbox 
	//	3.msgcontainer(a doc, not a container)       zky_pctrs=aaa.100018  subctnr = dd.100018
	//	4.subcontainer       zky_objid = dd.100018
	
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
	if (cid == "")
	{
		rdata->setError() << "Missing Cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Retrieve the inbox objid
	OmnString inbox_objid = userobj->getAttrStr("minboxid");
	AosXmlTagPtr inbox = AosDocClientObj::getDocClient()->getDocByObjid(inbox_objid, rdata);
	if (!inbox)
	{
		rdata->setError() << "Missing inbox";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString ctnr_objid = inbox->getAttrStr(AOSTAG_MSGCONTAINER_OBJID);
	if (ctnr_objid == "")
	{
		rdata->setError() << "Missing container in the inbox";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	//need check ctnr_objid with the cloudid
	
	bool cldidflag = checkHasCloudId(ctnr_objid, rdata);
	if(cldidflag)
	{
		rdata->setError() << "AOSTAG_MSGCONTAINER_OBJID has the cloud id in the inbox";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool duplicated = true;
	OmnString tempid;
	tempid <<  ctnr_objid << "." << cid;
	AosXmlTagPtr ctnrobj = AosDocClientObj::getDocClient()->getDoc(rdata, "", tempid, duplicated);
	OmnString tempid2;
	if (!ctnrobj) 
	{
		// The container doc in the inbox has not been created yet. 
		// Need to create it. 
		OmnString inboxid = inbox->getAttrStr(AOSTAG_OBJID);
		ctnrobj= createMsgContainer(inboxid, ctnr_objid, cid, rdata);
		if (!ctnrobj) 
		{
			AOSLOG_LEAVE(rdata);
			return false;
		}
		tempid2 = ctnrobj->getAttrStr(AOSTAG_OBJID);
		if (tempid2 == "")
		{
			rdata->setError() << "Missing objid";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		bool cloudidflag = checkHasCloudId(tempid2, rdata);
		if(!cloudidflag)
		{
			rdata->setError() << "the objid of the container in the inbox has not cloudid";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}

// Ice, 09/14/2011
//		AosValueRslt value(ctnr_objid);
//		bool rslt = AosDocClient::getSelf()->modifyAttr(inbox, 
//				AOSTAG_MSGCONTAINER_OBJID, value, false, false, rdata, 
//				__FILE__, __LINE__);
		bool rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
				inbox->getAttrU64(AOSTAG_DOCID, 0), inbox->getAttrStr(AOSTAG_OBJID, ""),
				AOSTAG_MSGCONTAINER_OBJID, ctnr_objid, "", false, false, true); 
					
		if (!rslt)
		{
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	tempid2 = ctnrobj->getAttrStr(AOSTAG_OBJID, "");
	if(tempid2 == "")
	{
		rdata->setError() << "Missing container objid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	userobj->setAttr(AOSTAG_PARENTC, tempid2);
	//create sub container
	OmnString subctnr = userobj->getAttrStr("subctnr");
	if(subctnr == "")
	{
		rdata->setError() << "Missing sub container objid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	bool subidflag= checkHasCloudId(subctnr, rdata);
	if(subidflag)
	{
		rdata->setError() << "the objid of the sub container in the msg container  has cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString sid;
	sid <<  subctnr << "." << cid;
	AosXmlTagPtr accessedObj = AosDocClientObj::getDocClient()->getDocByObjid(sid, rdata);
	if (!accessedObj)
	{
		accessedObj = createSubContainer(subctnr, rdata);
		if(!accessedObj)
		{
			rdata->setError() << "Missing sub container";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		OmnString subobjid = accessedObj->getAttrStr(AOSTAG_OBJID, "");
		if(subobjid == "")
		{
			rdata->setError() << "Missing sub container objid";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		bool sflag= checkHasCloudId(subobjid, rdata);
		if(!sflag)
		{
			rdata->setError() << "the objid of the sub container  has not cloudid";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}
	//create msgcontainer
	//reset the subctnr with the cloudid
	userobj->setAttr("subctnr", sid);
	userobj->setAttr(AOSTAG_CTNR_PUBLIC, "false");
	userobj->setAttr(AOSTAG_PUBLIC_DOC, "false");
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(userobj->toString(), "" AosMemoryCheckerArgs);
	if (!doc)
	{
		rdata->setError() << "Parsing incorrectly the msgcontainer";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr cmd;
	bool resolve_objid = false;
	bool flag = AosDocClientObj::getDocClient()->createDoc1(rdata, cmd, cid, doc, resolve_objid, 0, 0, true);
	if(!flag)
	{
		rdata->setError() << "Fail to create the msgcontainer";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	aos_assert_r(doc, false);
	OmnString rslts = "<Contents>";
	rslts << doc->toString();
	rslts << "</Contents>";
	rdata->setOk();
	rdata->setResults(rslts);
	AOSLOG_LEAVE(rdata);
	return true;
}


AosXmlTagPtr 
AosCreateMsgContainer::createSubContainer(
		const OmnString &sub_objid, 
		const AosRundataPtr &rdata)
{
	OmnString docstr = "<doc ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
		<< AOSTAG_OBJID << "=\"" << sub_objid << "\"/>";
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, docstr, "", "", false, false,
		false, false, true, true);
	OmnString errmsg;
	if (!ctnr)
	{
		AosSetErrorU(rdata, "failed_create_msg_ctnr") << ": " << sub_objid << enderr;
		return 0;
	}
	return ctnr;

}

bool
AosCreateMsgContainer::checkHasCloudId(
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
AosCreateMsgContainer::createMsgContainer(
		const OmnString &pid, 
		const OmnString &ctnr_objid, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	OmnString docstr = "<doc ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
		<< AOSTAG_PARENTC << "=\"" << pid<< "\" "
		<< AOSTAG_OBJID << "=\"" << ctnr_objid<< "\" "
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
	tempctnrid << ctnr_objid << "." << cid;
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDoc(rdata, "", tempctnrid, duplicated);
	OmnString errmsg;
	if (!ctnr)
	{
		AosSetErrorU(rdata, "failed_create_msg_ctnr") << ": " << ctnr_objid << enderr;
		return 0;
	}
	return ctnr;

}

