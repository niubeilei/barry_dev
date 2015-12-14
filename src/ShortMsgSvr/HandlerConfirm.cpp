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
#include "ShortMsgSvr/HandlerConfirm.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SmartDoc/SmartDoc.h"


AosHandlerConfirm::AosHandlerConfirm(
		const OmnString &sender, 
		const OmnString &modem_cellnum,
		const OmnString &sdoc_objid, 
		const OmnString &confirm_code)
:
AosShmHandler(AOSHANDLER_CONFIRM, AosShmHandlerId::eHandlerConfirm, true),
mSender(sender),
mModemCellNum(modem_cellnum),
mSdocObjid(sdoc_objid),
mConfirmCode(confirm_code),
mConfirmed(false)
{
}


AosHandlerConfirm::~AosHandlerConfirm()
{
}


bool
AosHandlerConfirm::procMsg(
		const OmnString &sender,
		const OmnString &modem_cellnum, 
		const OmnString &msg, 
		bool &stop_flag,
		bool &remove_flag,
		const AosRundataPtr &rdata)
{
	// This function is called when the system receives a short message
	// from 'sender' through 'modem_cellnum'. The message body is 'msg'. 
	// It checks whether the message is indeed a confirmation with 
	// a request that this class represents. If not, it ignores the message. 
	// Otherwise, it processes and 'consumes' the message. 
	// 
	// Processing short message is handled by the smart doc: 'mSdocObjid'. 
	rdata->setOk();
	stop_flag = false;
	remove_flag = false;
	if (mModemCellNum == "") 
	{
		rdata->setError() << "Modem number is empty";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (modem_cellnum != mModemCellNum) return true;

	if (mSdocObjid == "")
	{
		rdata->setError() << "Missing smart doc objid. Phone number: " << sender
			<< " through " << modem_cellnum << ". Contents: " << msg;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setArg1(AOSARG_SENDER_CELLNUM, sender);
	
	rdata->setArg1(AOSARG_MODEM_CELLNUM, modem_cellnum);
	rdata->setArg1(AOSARG_RECEIVED_MSG, msg);
	rdata->setArg1(AOSARG_CONFIRM_CODE, mConfirmCode);
	bool rslt = AosSmartDoc::runSmartdoc(mSdocObjid, rdata);
	aos_assert_r(rslt, false);

	// Check whether the message is consumed
	const OmnString &flag = rdata->getArg1(AOSARG_MSG_CONSUMED);
	if (flag == AOSARGVALUE_MSG_CONSUMED)
	{
		stop_flag = true;
		mConfirmed = true;
	}
	else if (flag == AOSARGVALUE_MSG_DENIED)
	{
		stop_flag = true;
		mConfirmed = false;
	}
	rdata->setOk();
	return true;
}	

