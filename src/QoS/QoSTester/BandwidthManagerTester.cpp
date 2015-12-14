////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ServerTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "QoS/QoSTester/BandwidthManagerTester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Porting/Sleep.h"
#include "Random/RandomObj.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "UtilComm/Ptrs.h"
#include "Util1/Time.h"
#include "QoS/QoSTester/QosTestUtil.h"
#include "QoS/qos_util.h"
#include "QoS/qos_traffic.h"
#include "QoS/qos_global.h"
#include "KernelSimu/timer.h"
#include "XmlParser/XmlItem.h"


AosQoSBandwidthManagerTester::AosQoSBandwidthManagerTester()
:
mNumConns(0)
{
	mName = "AosQoSBandwidthManagerTester";
}


bool AosQoSBandwidthManagerTester::start()
{
	unsigned int ret;
	// 
	// Test default constructor
	//

	basicTest();
	// volumeTest();
	return true;
}

bool AosQoSBandwidthManagerTester::basicTest()
{
	u32 last_rcv_time = 0;
	u32 cur_time;
	int times = 0;
	int sip_rang, dip_rang, sport_rang, dport_rang, skb_len_rang;

	OmnBeginTest << "Test SSL Server";
	
	
	while ( times < MAX_TEST_TIMES )
	{
		cur_time = QOS_GET_JIFFIES();
		if ((cur_time - last_rcv_time) > 10)
		{
			last_rcv_time = cur_time;
			times++;
			//aos_trace("test times %d", times);
			dip_rang = OmnRandom::nextInt(0, 0);
			skb_len_rang = OmnRandom::nextInt(100, 1000);
			//qos_rcv_skb(0, dip_rang, 0, 0, skb_len_rang);		
			qos_rcv_skb(0, 0, 0, 0, skb_len_rang);		
		}
		aos_timer_check_and_run();		
		usleep(10*1000); // sleep 2 ms
	}

	return true;
}	


bool
AosQoSBandwidthManagerTester::volumeTest()
{
	// 
	// This function will randomly generate a number of concurrent 
	// flows. Each flow bandwidth will also be randomly determined.
	//
	int tries = 0;

	while (tries < 10000)
	{
		// changeConfig();

		addRemoveFlows();

		// 
		// We now will generate traffic for a randomly determined duration. 
		//
		int duration = OmnRandom::nextInt(100, 3000);  // ms
		uint sec, usec;
//		OmnTime::getRealtime(sec, usec);

		u32 crt = sec * 1000 + usec/1000;
		u32 dur = crt + duration;
		
		while (crt < dur)
		{
			for (int i=0; i<mNumConns; i++)
			{
				qos_rcv_skb1(mConns[i].sip, mConns[i].sport, mConns[i].dip, mConns[i].dport, 
							 mConns[i].proto, mConns[i].if_id, mConns[i].ifname.data(), mConns[i].length);
			}
			aos_timer_check_and_run();		
			usleep(10000);	// sleep 10 ms
		
			crt += 10;
		}

		checkBandwidthUsage();

		tries++;
	}
}


bool
AosQoSBandwidthManagerTester::addRemoveFlows()
{
	// 
	// Randomly determine whether to add or remove flows
	//
	int v = OmnRandom::nextInt(0, 100);
	if (v < 80)
	{
		addFlows();
	}
	else
	{
		removeFlows();
	}

	return true;
}


bool
AosQoSBandwidthManagerTester::addFlows()
{
	if (mNumConns >= eMaxConns)
	{
		// 
		// Already have too many connections. Do not add
		//
		return true;
	}

	// 
	// We will randomly determine the attributes for the connection, including the sending IP, 
	// receiving IP, sending port, receiving port, protocol, interface, bandwidth.
	// 
	mConns[mNumConns].sip = (u32) OmnRandom::nextInt(100, 0xffffffff);
	mConns[mNumConns].dip = (u32) OmnRandom::nextInt(100, 0xffffffff);
	mConns[mNumConns].sport = (u16) OmnRandom::nextInt(1, 0xffff);
	mConns[mNumConns].dport = (u16) OmnRandom::nextInt(1, 0xffff);
	mConns[mNumConns].dport = (u8) OmnRandom::nextInt(1, 255);
	mConns[mNumConns].if_id = (u8) OmnRandom::nextInt(0, 1);
	mConns[mNumConns].length = (u16) OmnRandom::nextInt(1, 200);
	mConns[mNumConns].bandwidth = mConns[mNumConns].length * 100; // We send one per 10 ms
	mConns[mNumConns].start = OmnTime::getCrtSec();
	
	if (mConns[mNumConns].if_id == 0)
	{
		mConns[mNumConns].ifname = "eth0";
	}
	else
	{
		mConns[mNumConns].ifname = "eth1";
	}

	mNumConns++;
	return true;
}


bool
AosQoSBandwidthManagerTester::removeFlows()
{
	int index = OmnRandom::nextInt(0, mNumConns-1);

	if (index == mNumConns-1)
	{
		mNumConns--;
		return true;
	}

	mConns[index] = mConns[mNumConns-1];
	mNumConns--;
	return true;
}


bool
AosQoSBandwidthManagerTester::checkBandwidthUsage()
{
	// 
	// For every channel in mConns, there should be a channel in QoS. If not, it is an error.
	// For each channel in mConns, the bandwidth should be similar to the one found in QoS. 
	//
	
	if (!mStartMonitoring)
	{
		// 
		// It is not monitoring. It should not have the data
		//
		for (int i=0; i<mNumConns; i++)
		{
//			u32 rslt = qos_get_bandwidth(
//						mConns[i].sip, mConns[i].sport, 
//						mConns[i].dip, mConns[i].dport, 
//						mConns[i].proto, 
//						&band);
//			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt < 0)) << endtc;
		}

		return true;
	}

	int sec = OmnTime::getCrtSec();

	for (int i=0; i<mNumConns; i++)
	{
		// 
		// We should not check a channel unless it has started for at least two seconds. 
		//
		if (sec - mConns[i].start < 2)
		{
			continue;
		}

		// 
		// Check whether the interface is being monitored
		//
		if (!mInterfaceStatus[mConns[i].if_id])
		{
			// 
			// Not monitoroed
			//
			continue;
		}
		u32 band;
//		u32 rslt = qos_get_channel_bw(
//						mConns[i].sip, mConns[i].sport, 
//						mConns[i].dip, mConns[i].dport, 
//						mConns[i].proto, 
//						&band);

//		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
//		bool bandcomp = mConns[i].bandwidth >= band * 0.9 && mConns[i].bandwidth <= band * 1.1;
//		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(bandcomp)) << endtc;
	}

	return true;
}

static OmnString sgCLiSelectorDef =
		"<Random> "
            "<Name>CliSelector</Name>"
            "<Type>Selector</Type>"
            "<Selector>"
	            "<Type>IntegerRandom</Type>"
	            "<Elements>"
	                "<Element>1, 60</Element>"
	                "<Element>2, 15</Element>"
	                "<Element>3, 10</Element>"
	                "<Element>4, 5</Element>"
	                "<Element>4, 10</Element>"
		        "</Elements>"
		    "</Selector>"
		"</Random>";


bool
AosQoSBandwidthManagerTester::changeConfig()
{
	//
	// There are the following CLI commands:
	// 1. Start monitoring
	// 2. Stop monitoring
	// 3. Start monitoring an interface
	// 4. Stop monitoring an interface
	// 5. Show 
	//
	OmnXmlItemPtr configXmlPtr = OmnNew OmnXmlItem(sgCLiSelectorDef);
	AosRandomObjPtr cliSelector = AosRandomObj::createRandomObj(configXmlPtr);	

	int index = cliSelector->nextInt();
	switch (index)
	{
	case 1:
		 startMonitoring();
		 break;

	case 2:
		 stopMonitoring();
		 break;

	case 3: 
		 addInterface();
		 break;

	case 4: 
		 removeInterface();
		 break;

	case 5: 
		 show();
		 break;

	default:
		 OmnAlarm << "Unrecognized command: " << index << enderr;
		 return false;
	}

	return true;
}


bool
AosQoSBandwidthManagerTester::startMonitoring()
{
	aosRunCli("????", true);
	mStartMonitoring = true;

	return true;
}

bool
AosQoSBandwidthManagerTester::stopMonitoring()
{
	aosRunCli("????", true);
	mStartMonitoring = false;
	return true;
}

bool
AosQoSBandwidthManagerTester::addInterface()
{
	// 
	// Randomly determine whether to start monitoring interface 0 or 1
	//
	int inf_id = OmnRandom::nextInt(0, 1);

	char cmd[100];
	sprintf(cmd, "??????", inf_id);
	aosRunCli(cmd, true);

	mInterfaceStatus[inf_id] = true;
	return true;
}

bool
AosQoSBandwidthManagerTester::removeInterface()
{
	// 
	// Randomly determine whether to stop monitoring interface 0 or 1
	//
	int inf_id = OmnRandom::nextInt(0, 1);

	char cmd[100];
	sprintf(cmd, "??????", inf_id);
	aosRunCli(cmd, true);

	mInterfaceStatus[inf_id] = false;
	return true;
}

bool
AosQoSBandwidthManagerTester::show()
{
	aosRunCli("???", true);
	return true;
}


