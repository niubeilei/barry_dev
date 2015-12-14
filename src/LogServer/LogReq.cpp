//////////////////////////////////////////////////////////////////////////
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
// Modification History:
// 03/20/2009: Created by Sharon Shen
// 12/31/2012: Turned off by Chen Ding
//////////////////////////////////////////////////////////////////////////
#if 0
#include "LogServer/LogReq.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream.h>
#include <string.h>
#include <fstream.h>

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "LogServer/LogDataTypes.h"
#include "Util/OmnNew.h"


AosLogReq::AosLogReq()
{
}


AosLogReq::AosLogReq(const char * buff)
{
	mXmlDoc.Parse(buff);
}


AosLogReq::~AosLogReq()
{
}


AosNetReqType
AosLogReq::getType() const
{
	return eAosNetReqType_Log;
}


OmnString
AosLogReq::getTargetId() const
{
	const TiXmlElement *entry = mXmlDoc.RootElement();
	return entry->Attribute("logId");
}


OmnString
AosLogReq::getOperator()
{
	//aos_assert_r(mXmlDoc, NULL);
    TiXmlElement *entry = mXmlDoc.RootElement();
	return entry->Attribute("operation");
}


TiXmlElement *
AosLogReq::getContents()
{
    //aos_assert_r(mXmlDoc, NULL);
	TiXmlElement *entry = mXmlDoc.RootElement();
	cout << "Log entry: " << *entry << endl;
	return entry;
}


OmnString
AosLogReq::getAppInstance()
{
    //aos_assert_r(mXmlDoc, NULL);
    TiXmlElement *entry = mXmlDoc.RootElement();
    return entry->Attribute("appInstance");
}


void 
AosLogReq::sendResponse(const OmnString &response)
{

}
#endif
