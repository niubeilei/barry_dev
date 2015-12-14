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
#include "ShortMsgSvr/HandlerShmBroadcast.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "ShortMsgUtil/ShmHandlerId.h"
#include "ShortMsgUtil/ShmResponseMsg.h"
#include "SmartDoc/SmartDoc.h"


AosHandlerShmBroadcast::AosHandlerShmBroadcast(
		const AosShmHandlerId::E id, 
		const bool exclusive, 
		const OmnString &modem_cellnum)
:
AosShmHandler(AOSHANDLER_BROADCAST, AosShmHandlerId::eBroadcast, true),
mExclusive(exclusive),
mModemCellnum(modem_cellnum),
mOperation(id)
{

}


AosHandlerShmBroadcast::~AosHandlerShmBroadcast()
{
	OmnMark;
}


bool
AosHandlerShmBroadcast::procMsg(
		const OmnString &sender,
		const OmnString &modem_cellnum, 
		const OmnString &msg, 
		bool &stop_flag,
		bool &remove_flag,
		const AosRundataPtr &rdata)
{
	// This hanlder listen to the modem, if modem receive
	// a short message, and the message is format as this:
	// <&>operation<&>sender's phone number<&>contents of short message<&>. 
	// it will check the operation, is operation is shmtoshm, it will do this hander.
	if (msg == "")
	{
		AosSetError(rdata, AosErrmsgId::eValueNotFound);
		OmnAlarm << ". Contents is empty!" << enderr;
		sendRespToSender(AOSRESPMSG_NOCONTENTS, sender, rdata);
		return false;
	}
	
	if (sender == "")
	{
		AosSetError(rdata, AosErrmsgId::eValueNotFound);
		OmnAlarm << ". Inter Error!" << enderr;
		sendRespToSender(AOSRESPMSG_NOSENDER, sender, rdata);
		return false;
	}
	
	AosStrSplit split(msg, "<&>");
	OmnString operation = split.nextWord();
	operation = split.nextWord();
	if (operation == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingOperation);
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendRespToSender(AOSRESPMSG_NOOPERATION, sender, rdata);
		return false;
	}
	AosShmHandlerId::E id = AosShmHandlerId::toEnum(operation);
	if (!AosShmHandlerId::isValid(id))
	{
		AosSetError(rdata, AosErrmsgId::eInvalidOperation);
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendRespToSender(AOSRESPMSG_INVALIDOPERATION, sender, rdata);
		return false;
	}
	if (id != mId)
	{
		AosSetError(rdata, AosErrmsgId::eWrongOperation);
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendRespToSender(AOSRESPMSG_MISMATHOPERATION, sender, rdata);
		return false;
	}
	
	OmnString receiver = split.nextWord();
	if (receiver == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingReceiver);
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendRespToSender(AOSRESPMSG_NORECEIVER, sender, rdata);
		return false;
	}

	OmnString msg_content = split.nextWord();
	if (msg_content == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingContents);
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendRespToSender(AOSRESPMSG_NOSENDCONTENT, sender, rdata);
		// send response to sender
		return false;
	}

	// Add request to ShortMsgSvr
	bool rslt = sendRespToSender(msg_content, receiver, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eSendShortMsgFailed);
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendRespToSender(AOSRESPMSG_SENDMESSAGEFAILED, sender, rdata);
		return false;
	}
	return true;
}	

