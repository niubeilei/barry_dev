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
// 2013/01/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/ExeReport.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Thread/Thread.h"
#include "XmlUtil/XmlTag.h"



AosExeReport::AosExeReport(const int sleepTime, const u64 reportSize)
:
mSleepTime(sleepTime),
mReportSize(reportSize)
{
	if (mSleepTime < eMinSleepTime) mSleepTime = eMinSleepTime;
	if (mReportSize < eMinReportSize) mReportSize = eMinReportSize;

	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "report", 0, false, false, __FILE__, __LINE__);
	mThread->start();
}


AosExeReport::~AosExeReport()
{
}


bool
AosExeReport::config(const AosXmlTagPtr &config)
{
	if (!config) return true;
	AosXmlTagPtr tag = config->getFirstChild("exe_report");
	if (!tag) return true;
	
	mSleepTime = tag->getAttrInt("sleep_time", eDftSleepTime);
	if (mSleepTime < eMinSleepTime) mSleepTime = eDftSleepTime;
	
	mReportSize = tag->getAttrU64("report_size", eDftReportSize);
	if (mReportSize < eMinReportSize) mReportSize = eMinReportSize;
	
	return true;
}


bool
AosExeReport::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		OmnSleep(mSleepTime);
		AosMemoryMonitor::report(mReportSize);
	}

	return true;
}


bool    
AosExeReport::signal(const int threadLogicId)
{
	return true;
}


bool    
AosExeReport::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

