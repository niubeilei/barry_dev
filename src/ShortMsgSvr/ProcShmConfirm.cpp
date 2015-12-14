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
// 06/23/2011	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgSvr/ProcShmConfirm.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "ShortMsgUtil/GsmModem.h"
#include "ShortMsgSvr/Ptrs.h"
#include "ShortMsgSvr/ShortMsgSvr.h"
#include "ShortMsgSvr/HandlerConfirm.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "Util/OmnNew.h"


AosProcShmConfirm::AosProcShmConfirm(const bool regflag)
:
AosShmReqProc(
		AOSSHORTMSGREQ_SHMCONFIRM, 
		AosShmReqid::eShmConfirm, 
		regflag),
mOperation(AOSSHORTMSGREQ_SHMCONFIRM)
{
}


AosProcShmConfirm::~AosProcShmConfirm()
{

}


bool 
AosProcShmConfirm::proc(const AosRundataPtr &rdata)
{
	// This function is called when there is a 'short-message-confirmation'
	// request. This is done by:
	// 	1. Register a new request with the Short Message Server
	// 	2. Send the message
	// 	3. When the user sends a response, ShortMsgServer will call 
	// 	   this object back. 
	//
	// The request format is:
	// 	<request opr="xxx" 
	// 		AOSTAG_RECEIVERS="xxx,xxx,...">
	// 		<msg><[CDATA[xxx]]></msg>
	// 		<confirmcode><![CDATA[xxx]]></confirmcode>
	// 	</request>
	//
	// 	Then we need put the req into a map. It will be used when
	// 	the short message response comming.

	AosXmlTagPtr root = rdata->getReceivedDoc();
	if (!root)
	{
		rdata->setError() << "Missing request";
		return false;
	}
	
	OmnString sdoc_objid = root->getAttrStr(AOSTAG_SDOC_OBJID);
	if (sdoc_objid == "")
	{
		rdata->setError() << "Missing martdoc's objid!"; 
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString receiver = root->getAttrStr(AOSTAG_SHM_RECEIVERS);
	if (receiver == "")
	{
		rdata->setError() << "Missing receivers!"; 
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString confirm_code = root->xpathQuery("confirmcode");
	if (confirm_code == "")
	{
		rdata->setError() << "Missing confirmation code";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr msgtag = root->getFirstChild();
	if (!msgtag)
	{
		rdata->setError() << "Missing message!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString msg = msgtag->getNodeText();
	if (msg == "")
	{
		rdata->setError() << "Message is empty";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	// Send the short message
	AosGsmModemPtr gsm = AosShortMsgSvr::getSelf()->getGsmModem(mOperation);
	aos_assert_r(gsm, false);
	if(AosShortMsgSvr::getSelf()->getSimulate())
	{
		OmnScreen << " Tortuer is successfull, we need not send message truely!" << endl;
		return true;
	}
	aos_assert_r(gsm->sendMessage((AosUCharPtr_t)receiver.data(),(AosUCharPtr_t)msg.data(), rdata), false);
	createLog(receiver, msg, rdata);
	
	// There we should put the  req into map
	AosShmHandlerPtr pp = OmnNew AosHandlerConfirm(
			receiver, gsm->getLocalNum(), sdoc_objid, confirm_code);
	AosShortMsgSvr::getSelf()->addSenderHandler(receiver, pp);
	return true;
}


