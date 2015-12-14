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
// 2013/01/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ExeReport_ExeReport_h
#define AOS_ExeReport_ExeReport_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "XmlUtil/Ptrs.h"


class AosExeReport : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eMinSleepTime = 10,
		eDftSleepTime = 600,
		eMinReportSize = 1000,
		eDftReportSize = 1000000
	};

	int				mSleepTime;
	u64				mReportSize;
	OmnThreadPtr	mThread;

public:
	AosExeReport(
			const int sleepTime = eDftSleepTime,
			const u64 reportSize = eDftReportSize);
	~AosExeReport();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

    virtual bool    config(const AosXmlTagPtr &conf);
};
#endif
