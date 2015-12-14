////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgSvr/ProcSendShm.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "ShortMsgUtil/GsmModem.h"
#include "ShortMsgSvr/ShortMsgSvr.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

static OmnMutex	sgLock;

AosProcSendShm::AosProcSendShm(const bool regflag)
:
AosShmReqProc(
		AOSSHORTMSGREQ_SENDMSG, 
		AosShmReqid::eSendShortmsg, 
		regflag),
mOperation(AOSSHORTMSGREQ_SENDMSG)
{
}


AosProcSendShm::~AosProcSendShm()
{
}


bool 
AosProcSendShm::proc(const AosRundataPtr &rdata)
{
	// This function is called when there are messages to be sent. 
	// The request format is:
	// 	<request opr="xxx" 
	// 		AOSTAG_RECEIVERS="xxx,xxx,...">
	// 		<msg><[CDATA[xxx]]></msg>
	// 	</request>
	AosXmlTagPtr root = rdata->getReceivedDoc();
	if (!root)
	{
		rdata->setError() << "Missing request";
		return false;
	}

	OmnString receivers = root->getAttrStr(AOSTAG_SHM_RECEIVERS);
	if (receivers == "")
	{
		rdata->setError() << "Missing receivers!"; 
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr msgtag = root->getFirstChild("msg");
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

	receivers.replace("，", ",", true);

	OmnStrParser1 parser(receivers, ",");
	rdata->setOk();
	while (parser.hasMore())
	{
		OmnSleep(2);
		OmnString receiver = parser.nextWord();
		if (receiver == "")
		{
			rdata->setError() << "Internal error: receiver is null!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			continue;
		}

		// Send the short message
		AosGsmModemPtr gsm = AosShortMsgSvr::getSelf()->getGsmModem(mOperation);
		aos_assert_r(gsm, false);
		if(AosShortMsgSvr::getSelf()->getSimulate())
		{
			OmnScreen << " Tortuer is success,we need not send message truely!" << endl;
			return true;
		}
	
		sgLock.lock();
		gsm->sendMessage((AosUCharPtr_t)receiver.data(),(AosUCharPtr_t)msg.data(), rdata);
		sgLock.unlock();
	}
	return true;
}


