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
// Modification History:
// 2013/12/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/Jimos/ActRunPing.h"


#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/AppendFile.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/TaskDataObj.h"
#include "Tools/CPing.h"


AosActRunPing::AosActRunPing(
		const AosRundataPtr &rdata, 
		const OmnString &version)
:
AosSdocAction(AOSACTTYPE_PINGSERVICE, AosActionType::ePingService, true)
{
	mIpList = new vector<OmnString>;
	//default warning ping time is 200ms
	mWarningTime = 200;
}


AosActRunPing::~AosActRunPing()
{
}


bool
AosActRunPing::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &worker_doc,
		const AosRundataPtr &rdata)
{
	if (!worker_doc)
	{
		AosSetErrorUser3(rdata, "actRunPing_missing_worker_doc", "Syntax Error") << enderr;
		return false;
	}

	if (!task)
	{
		AosSetError(rdata, "internal_erro") << enderr;
		return false;
	}

	mTask = task;

	//get pinging ip addresses and save int a 
	//member variable. To be added later
	//IP list and ping warning time need to be
	//configured in the doc
	if (mIpList->size() < 1) 
	{
		//get ip address from doc(s) and save
		//to be added later

	}

	//ping the ip addresses one by one
	for (int i = 0; i < mIpList->size(); i++)
	{
		OmnString ip = (*mIpList)[i];
		double pingTime;
		PingStatus s; 
		
		pingDevice(ip, s, pingTime);
	
		//save the pinging status to database
		//to be added later
		OmnScreen << "Pinging Status: " << getStatusString(s) 
			<< " Ping time: " << pingTime << endl;
	}
	
	return true;
}

OmnString
AosActRunPing::getStatusString(PingStatus s)
{
	if (s == eFail)
		return "Failed";
	else if (s == eWarning)
		return "Warning";
	else if (s == eSuccess)
		return "Success";

	return "Unknown";
}

bool
AosActRunPing::pingDevice(OmnString ip, PingStatus &s, double &pingTime)
{
	CPing *ping = new CPing("192.168.99.181", 3);  

	if (ping->start())
		ping->send_packet();

	if (ping->GetSendTimes() > 0) 
	{
		ping->recv_packet();
		ping->finish();
		pingTime = ping->GetAvgResponseTime();
		if (pingTime >= mWarningTime)
			s = eWarning;
		else
			s = eSuccess;

	} else {
		ping->finish();
		s = eFail;
		return false;
	}

	return true;
}

bool
AosActRunPing::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	return true;
}


AosJimoPtr
AosActRunPing::cloneJimo() const
{
	try
	{
		return OmnNew AosActRunPing(*this);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Internal error" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


AosActionObjPtr
AosActRunPing::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActRunPing(*this);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

