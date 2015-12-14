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
#include "SeReqProc/MoveMsg.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "SEServer/SeReqProc.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosMoveMsg::AosMoveMsg(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_MOVE_MSG, 
		AosSeReqid::eMoveMsg, rflag)
{
}


bool 
AosMoveMsg::proc(const AosRundataPtr &rdata)
{
	// This function moves a message from one container to another.
	//<request reqid = "" >
	//		<item name='zky_siteid'><![CDATA[100]]></item>
	//		<item name='operation'><![CDATA[serverreq]]></item>
	//		<item name='trans_id'><![CDATA[8]]></item>
	//		<item name='zky_ssid'><![CDATA[xxx]]></item>
	//		<item name='reqid'><![CDATA[move_msg]]></item>
	//		<item name='args'><![CDATA[sdocobjid="objid",movetocontainer ="containerid" ]]></item>
	//		<objdef>
	//			<records>
	//				<record zky_objid="a" zky_docid="b">
	//				<record zky_objid="c" zky_docid="d">
	//				...
	//				...
	//				...
	//			<records>
	//		</objdef>
	//</request>
	//notice : movetocontainer must have the cloudid
	
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

	OmnString cid = rdata->getCid();
	if (cid == "")
	{
		rdata->setError() << "Missing Cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
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

	OmnString sdocobjid, containerid;
	AosParseArgs(args, "sdocobjid", sdocobjid, "containerid", containerid);
	if(sdocobjid == "")
	{
		rdata->setError() << "Missing smart doc objid!";
	    OmnAlarm << rdata->getErrmsg() << enderr;
	    AOSLOG_LEAVE(rdata);
	   	return false;
	}
	if(containerid == "")
	{
		rdata->setError() << "Missing the container objid!";
	    OmnAlarm << rdata->getErrmsg() << enderr;
	    AOSLOG_LEAVE(rdata);
	   	return false;
	}
	OmnString wd = OmnStrParser1::getLastWord(containerid, '.');
	OmnString cloudid = wd.substr(1);
	if(cloudid == "$cloudid")
	{
		containerid.trim(9);
		containerid << "." << cid;
	}
	bool cflag = checkHasCloudId(containerid, rdata);
	if(!cflag)
	{
		rdata->setError() << "container has not the cloudid!";
	    OmnAlarm << rdata->getErrmsg() << enderr;
	    AOSLOG_LEAVE(rdata);
	   	return false;
	}
	bool duplicated = true;
	AosXmlTagPtr ctnrobj = AosDocClientObj::getDocClient()->getDoc(rdata, "", containerid, duplicated);
	if(!ctnrobj)
	{
		ctnrobj = createContainer(containerid, cid, rdata);
		if(!ctnrobj)
		{
			rdata->setError() << "Fail to create the container!";
	    	OmnAlarm << rdata->getErrmsg() << enderr;
	    	AOSLOG_LEAVE(rdata);
	   		return false;
		}
		OmnString ctnrid = ctnrobj->getAttrStr(AOSTAG_OBJID, "");
		bool flag = checkHasCloudId(ctnrid, rdata);
		if(!flag)
		{
			rdata->setError() << "container has not the cloudid!";
	    	OmnAlarm << rdata->getErrmsg() << enderr;
	    	AOSLOG_LEAVE(rdata);
	   		return false;
		}

	}
	containerid = ctnrobj->getAttrStr(AOSTAG_OBJID, "");
	OmnString tagstr;
	tagstr << "<tag " << "containerid=\"" << containerid<< "\"></tag>";
	AosXmlParser parser;
	AosXmlTagPtr srcdoc= parser.parse(tagstr, "" AosMemoryCheckerArgs);
	if (!srcdoc)
	{
		rdata->setError() << "Missing parse tagstr error!";
	    OmnAlarm << rdata->getErrmsg() << enderr;
	    AOSLOG_LEAVE(rdata);
	   	return false;
	}
	rdata->setSourceDoc(srcdoc, true);
	AosSmartDocObj::procSmartdocsStatic(sdocobjid, rdata);
	/*
	AosXmlTagPtr msg = AosDocMgr::getSelf()->getDoc(rdata->getSiteid(), sdocobjid);
	if (!msg)
	{
		rdata->setError() << "Missing doc!";
	    OmnAlarm << rdata->getErrmsg() << enderr;
	    AOSLOG_LEAVE(rdata);
	   	return false;
	}
	
	
	//move a message to a container.

	OmnString dft = "";
	int ttl=0;
	bool rslt = AosDocServer::getSelf()->modifyAttrStr1(rdata, msg->getAttrU64(AOSTAG_DOCID, 0),
			objid, AOSTAG_PARENTC, containerid, dft, true, true, ttl, __FILE__, __LINE__);

	if (!rslt)
	{
		rdata->setError() << "Fail to move the message";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
*/
	OmnString rslts = "<Contents>";
	rslts << srcdoc->toString();
	rslts << "</Contents>";
	rdata->setResults(rslts);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

bool
AosMoveMsg::checkHasCloudId(
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
AosMoveMsg::createContainer(
		const OmnString &ctnr_objid, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	OmnString docstr = "<doc ";
	docstr << AOSTAG_OBJID << "=\"" << ctnr_objid<< "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"false\" "
		<< AOSTAG_PUBLIC_DOC << "=\"true\" " << "/>";
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
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDoc(rdata, "", ctnr_objid, duplicated);
	OmnString errmsg;
	if (!ctnr)
	{
		AosSetErrorU(rdata, "failed_create_msg_ctnr") << ": " << ctnr_objid << enderr;
		return 0;
	}
	return ctnr;

}
