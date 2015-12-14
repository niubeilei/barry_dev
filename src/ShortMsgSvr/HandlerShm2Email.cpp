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
#include "ShortMsgSvr/HandlerShm2Email.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SmartDoc/SmartDoc.h"


AosHandlerShm2Email::AosHandlerShm2Email(
		const bool exclusive, 
		const OmnString &modem_cellnum)
:
AosShmHandler(AOSHANDLER_SHM2EMAIL, AosShmHandlerId::eShm2Email, true),
mExclusive(exclusive),
mModemCellnum(modem_cellnum)
{
}


AosHandlerShm2Email::~AosHandlerShm2Email()
{

}


bool
AosHandlerShm2Email::procMsg(
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
	return true;
}	

