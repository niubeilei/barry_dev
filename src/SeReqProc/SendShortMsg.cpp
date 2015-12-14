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
// 08/11/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/SendShortMsg.h"

#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "ShortMsgClt/ShortMsgClt.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SmartDoc/SMDMgr.h"
#include "XmlUtil/XmlTag.h"


AosSendShortMsg::AosSendShortMsg(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SENDSHORTMSG, AosSeReqid::eSendShortMsg, rflag)
{
}


bool 
AosSendShortMsg::proc(const AosRundataPtr &rdata)
{
	// This function is created by Brian Zhang 11/08/2011
	// root should be this format:
	//<request reqid = "sdshm" >
	//<item name="needresp">need/noneed</item>
	//  <objdef>
	//  	<shm recevers="xxx">
	//  		<content>xxxx</content>
	//		</shm>
	//  </objdef>
	//</request>
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

	// This mean this request is handed by smartdoc.
	// smartdoc should have reveivers and message.
	bool rslt;
	OmnString sobjid = root->getChildTextByAttr("name", "sobjid");
	if (sobjid != "")
	{
		rslt = AosSmartDocObj::procSmartdocsStatic(sobjid, rdata);
		if (!rslt)
		{
			rdata->setResults("执行发送短信的smartdoc失败");//"run smartdoc failed");
			OmnAlarm << rdata->getResults() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		AOSLOG_LEAVE(rdata);
		return true;
	}

	OmnString receivers = doc->getAttrStr(AOSTAG_SHM_RECEIVERS);
	if (receivers == "")
	{
		rdata->setError() << "No receivers";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString msg = doc->xpathQuery("contents/_#text");
	if (msg == "")
	{
		rdata->setError() << "No contents";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr resp;
	rslt = AosShortMsgClt::getSelf()->sendShortMsg(receivers, msg, resp, 120, rdata);
	if (!rslt)
	{
		rdata->setError() << "Send short message failed";
		OmnAlarm << rdata->getErrmsg() << enderr;
		rdata->setResults("发送短信失败");//Failed to send short message");
		AOSLOG_LEAVE(rdata);
		return false;
	}
	rdata->setResults("发送短信成功");//resp->toString());
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return false;
}

