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
// 05/27/2011	Deleted by Jackie Zhao
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/DeleteMsgContainer.h"

#include "SEInterfaces/DocClientObj.h"
#include "SearchEngine/DocServerCb.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "SEServer/SeReqProc.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"


AosDeleteMsgContainer::AosDeleteMsgContainer(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_DELETE_MSGCONTAINER, 
		AosSeReqid::eDeleteMsgContainer, rflag)
{
}


bool 
AosDeleteMsgContainer::proc(const AosRundataPtr &rdata)
{
	// This function deletes a message container. 
	//<request reqid = "" >
	//		<item name='zky_siteid'><![CDATA[100]]></item>
	//		<item name='operation'><![CDATA[serverreq]]></item>
	//		<item name='trans_id'><![CDATA[8]]></item>
	//		<item name='zky_ssid'><![CDATA[xxx]]></item>
	//		<item name='reqid'><![CDATA[delete_msgcontainer]]></item>
	//		<item name='args'><![CDATA[cname="deletedmsgcontainer"]]></item>
	//</request>
	AOSLOG_ENTER_R(rdata, false);

	u32 siteid = rdata->getSiteid();
	if (siteid == 0) 
	{
		rdata->setError() << "Missing siteid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		rdata->setError() << "Missing args!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString cname;
	AosParseArgs(args, "cname", cname);

	if (cname == "")
	{
		rdata->setError() << "Missing cname!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Retrieve the container doc 
	AosXmlTagPtr ctnrdoc= AosDocClientObj::getDocClient()->getDocByObjid(cname, rdata);
	if (!ctnrdoc)
	{
		rdata->setError() << "Missing the container doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString subctnrid = ctnrdoc->getAttrStr("subctnr");
	if (subctnrid == "")
	{
		rdata->setError() << "Missing sub container objid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Check whether it is allowed. 
	//if (!AosSecurityMgrObj::getSecurityMgr()->checkDelMember(subctnrid, rdata))
	//{
	//	// Access denied
	//	AOSLOG_LEAVE(rdata);
	//	return false;
	//}

/* Cannot compile, Chen Ding, 07/23/2011
	//check are there some member in the sub container
	bool ok = AosIILClientObj::getIILClient()->checkMember(siteid, subctnrid);
	if (ok)
	{
		rdata->setError() << "there are some members in the sub container!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDoc(siteid, cname);
	if (!doc)
	{
		rdata->setError() << "Missing doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString pctnr = doc->getAttrStr(AOSTAG_PARENTC, "");
	if(pctnr == "")
	{
		rdata->setError() << "Missing parent container!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool rslt = AosDocServer::getSelf()->removeContainerMember(siteid, pctnr, 
			doc, doc->getAttrU64(AOSTAG_DOCID, 0));
	if (!rslt)
	{
		// Failed removing.
		rdata->setError() << "Missing rslt!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	rdata->setOk();
	AOSLOG_LEAVE(rdata);
*/
	return true;
}

