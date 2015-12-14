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
#include "SeReqProc/DeleteMsg.h"

#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "SEInterfaces/SmartDocObj.h"
#include "Thread/Mutex.h"
#include "SEServer/SeReqProc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosDeleteMsg::AosDeleteMsg(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_DELETE_MSG, 
		AosSeReqid::eDeleteMsg, rflag)
{
}


bool 
AosDeleteMsg::proc(const AosRundataPtr &rdata)
{
	// This function deletes a message from an inbox. The
	// message is deleted from the system (indexing) but the message
	// itself is not deleted. Instead, it is put into a container
	// (called Trash Can). 
	//<request reqid = "" >
	//		<item name='zky_siteid'><![CDATA[100]]></item>
	//		<item name='operation'><![CDATA[serverreq]]></item>
	//		<item name='trans_id'><![CDATA[8]]></item>
	//		<item name='zky_ssid'><![CDATA[xxx]]></item>
	//		<item name='reqid'><![CDATA[delete_msg]]></item>
	//		<item name='args'><![CDATA[sdocobjid="msgobjid", container="movetocontainer"]]></item>
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
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
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

	if (sdocobjid == "")
	{
		rdata->setError() << "Missing sdocobjid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (containerid == "")
	{
		rdata->setError() << "Missing container id!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString tagstr;
	tagstr << "<tag " << "containerid=\"" << containerid << "\"></tag>";
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
	OmnString rslts = "<Contents>";
	rslts << srcdoc->toString();
	rslts << "</Contents>";
	rdata->setResults(rslts);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;

//	return AosDocServer::getSelf()->deleteObj(rdata, "", objid, container, false);
}

