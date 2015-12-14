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
#include "ShortMsgSvr/HandlerMicroblog.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Microblog/Microblog.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SmartDoc/SmartDoc.h"
#include "SEInterfaces/IILClientObj.h"


AosHandlerMicroblog::AosHandlerMicroblog(
		const bool exclusive, 
		const OmnString &modem_cellnum)
:
AosShmHandler(AOSHANDLER_MICROBLOG, AosShmHandlerId::eMicroblog, true),
mExclusive(exclusive),
mModemCellnum(modem_cellnum)
{
}


AosHandlerMicroblog::~AosHandlerMicroblog()
{

}


bool
AosHandlerMicroblog::procMsg(
		const OmnString &sender,
		const OmnString &modem_cellnum, 
		const OmnString &msg, 
		bool &stop_flag,
		const AosRundataPtr &rdata)
{
	// This hanlder converts a short message into an email. 
	// It retrieves the user account information based on 'sender'. 
	// If not found, it may or may not stop the message processing
	// depending on the flag 'mExclusive'. 
	//
	// If the user account is found, it retrieves the user's email
	// address. If not found, it is an error. Otherwise, it sends
	// the message as an email to the email address.
	aos_assert_r(sender != "", false);
	aos_assert_r(msg != "", false);
	stop_flag = false;

	OmnString iilname = "";//AOSUSERIIL;
	OmnString iilkey = "";//AOSPHONE;
	iilkey << sender;
	aos_assert_r (iilname != "" && iilkey != "", false);

	u64 docid;
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, iilkey, docid, rdata);
	if (!rslt || docid == 0)
	{
		rdata->setError() << "Retrieve Docid failed";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr getdoc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	if (!getdoc)
	{
		rdata->setError() << "Failed to retrieve doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	/*
	 bool sendmsg = AosMicroblog::getSelf()->sendMBlogMsg(getdoc, msg, rdata);
	if (!sendmsg)
	{
		rdata->setError() << "Failed to send microblog message";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	*/
	return true;
}	

