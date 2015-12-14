////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AlarmSender.cpp
// Description:
//	This is an interface used by AlarmMgr to send alarms to outside.
//  If we want to send alarms to outside, it should implement this 
//  interface and register with OmnAlarmMgr.  
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Alarm/AlarmSender.h"

#include "Debug/Debug.h"




OmnAlarmSender::OmnAlarmSender()
:
OmnObject(OmnClassId::eOmnAlarmSender)
{
}


OmnAlarmSender::~OmnAlarmSender()
{
}


bool
OmnAlarmSender::sendAlarm(const OmnAlarmEntry &alarm)
{
	OmnScreen << "********** Not implemented yet" << endl;
//			theEntry.setRecvDn(OmnDnMgrSelf->getDn(OmnDnId::eDnsServer));
//			OmnInfobusRequesterPtr thisPtr(this, false);
//			OmnInfobusCltSelf->sendMsg(theEntry.getPtr(), thisPtr);
	return false;
}

