////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BandwidthManagerTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_QoSTester_QosChannelMonitorManagerTester_h
#define Omn_QoSTester_QosChannelMonitorManagerTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"


class AosQoSChannelMonitorManagerTester : public OmnTestPkg
{
private:
	struct Conn 
	{
		u32 sip;
		u32 dip;
		u16 sport;
		u16 dport;
		u8 proto;
		int if_id;
		OmnString ifname;
		u32 length;
		u32 bandwidth;
		u32 start;
		void *ch;
	};
	enum
	{
		eMaxConns = 200
	};
	struct Conn mConns[eMaxConns];
	u32    mNumConns;
	bool   mStartMonitoring;
	bool   mInterfaceStatus[10];

	u32    mDifference;
	int    mTestCount;
	u32    mSec;
	u32    mUsec;
	u32    mSec1;
	u32    mUsec1;
	u32    mStart;
	u32    mCrt1;


public:
	AosQoSChannelMonitorManagerTester()
	{
		mName = "AosQoSChannelMonitorManagerTester";
		mNumConns = 0;
	}
	~AosQoSChannelMonitorManagerTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	volumeTest();
	bool	addRemoveFlows();
	bool	addFlows();
	bool	removeFlows();
	bool	checkBandwidthUsage();

	bool	changeConfig();
	bool	startMonitoring();
	bool	stopMonitoring();
	bool	addInterface();
	bool	removeInterface();
	bool	show();
};
#endif

