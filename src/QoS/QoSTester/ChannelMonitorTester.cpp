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

//#include "QoS/QoSTester/BandwidthManagerTester.h"
#include "QoS/QoSTester/ChannelMonitorTester.h"

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
//#include "Util1/Time.h"
#include "UtilComm/Ptrs.h"
#include "QoS/QoSTester/QosTestUtil.h"
#include "QoS/qos_util.h"
#include "QoS/qos_traffic.h"
#include "QoS/qos_global.h"
#include "KernelSimu/timer.h"
#include "XmlParser/XmlItem.h"

#include <stdlib.h>
extern int qos_get_channel_info(u32 sip,u16 sport,u32 dip,u16 dport,u8 proto,u32 *src_ip,u16 *src_port, u32 *dst_ip, u16 *dst_port,u8 *outproto,void **p);


bool AosQoSChannelMonitorManagerTester::start()
{
	unsigned int ret;
	// 
	// Test default constructor
	//

	//basicTest();
	volumeTest();
	return true;
}

bool AosQoSChannelMonitorManagerTester::basicTest()
{
	u32 last_rcv_time = 0;
	u32 cur_time;
	int times = 0;
	int sip_rang, dip_rang, sport_rang, dport_rang, skb_len_rang,mac_rang,proto,ifid,proto_rang;

	OmnBeginTest << "Test SSL Server";
	
	
	while ( times < MAX_TEST_TIMES )
	{
		cur_time = QOS_GET_JIFFIES();
		if ((cur_time - last_rcv_time) >= 10)
		{
			sip_rang = 0;
			dip_rang = 0;
			sport_rang = 0;
			dport_rang = 0;
			skb_len_rang = 0;
			ifid = 1;
			mac_rang = 0;
			proto = AOS_QOS_TCP; 
			last_rcv_time = cur_time;
			times++;
			//aos_trace("test times %d", times);
		//	sip_rang = OmnRandom::nextInt(0,5);
			sip_rang = 0; 
		//	dip_rang = OmnRandom::nextInt(0, 1);
			dip_rang = 0; 
		//	sport_rang = OmnRandom::nextInt(0,5);
		//	sport_rang = 0; 
			dport_rang = OmnRandom::nextInt(0,5);
			dport_rang = 0; 
		//	ifid = OmnRandom::nextInt(0,3);
			ifid = 0;
			mac_rang = OmnRandom::nextInt(0,5);
		//	mac_rang = 0;
		//	proto_rang = OmnRandom::nextInt(0,1);
			proto_rang = 0;
			switch (proto_rang) {
				case 0:
					proto = AOS_QOS_TCP;
					break;
				case 1:
					proto = AOS_QOS_UDP;
					break;
			}
			//dip_rang = 0;
			//skb_len_rang = OmnRandom::nextInt(100, 10000);
		//OmnRandomSelector
			//skb_len_rang = 100 * dip_rang + 100;
			skb_len_rang = 1000;
			qos_rcv_skb(0, dip_rang, sport_rang, 0, skb_len_rang);		
	//		all_qos_rcv_skb(sip_rang, dip_rang, sport_rang, dport_rang, skb_len_rang,ifid,mac_rang,proto);		
		}
		aos_timer_check_and_run();		
		usleep(1*100); // sleep 2 ms
	}

	return true;
}	

	bool
AosQoSChannelMonitorManagerTester::volumeTest()
{
	// 
	// This function will randomly generate a number of concurrent 
	// flows. Each flow bandwidth will also be randomly determined.
	//
	int tries = 0;

	mStartMonitoring = true;
	u32 diff_min = 0;
	u32 diff_max = 0;
	--diff_min;
	while (tries < 100)
	{
//		aos_trace("Trying.................................");
		//changeConfig();

		addRemoveFlows();

		// 
		// We now will generate traffic for a randomly determined duration. 
		//
	//	int duration = OmnRandom::nextInt(100, 3000);  // ms
	//	int duration = OmnRandom::nextInt(1500, 4500);  // Ping Wang
		int duration = 3000;  // Ping Wang
		unsigned sec, usec;
		uint sec1, usec1;
		OmnTime::getRealtime(sec, usec);

		u32 start = sec * 1000 + usec/1000;
		u32 crt = start;
		u32 dur = crt + duration;
		int test_count;
		test_count = 0;
		
		while (crt < dur)
		{
		//	aos_trace("crt:%u\n",crt);
/*			if (dur - crt <= 1000) {
				if (test_count == 0) {
					OmnTime::getRealtime(sec,usec);
					start = sec * 1000 + usec/1000;
				}
				++test_count;
			}*/
			for (int i=0; i<mNumConns; i++)
			{
				//qos_rcv_skb1(mConns[i].sip, mConns[i].sport, mConns[i].dip, mConns[i].dport, 
				//			 mConns[i].proto, mConns[i].if_id, mConns[i].ifname.data(), mConns[i].length);
				all_qos_rcv_skb1(mConns[i].sip,mConns[i].dip, mConns[i].sport, mConns[i].dport, mConns[i].length,mConns[i].if_id,mConns[i].proto);		
			}

			usleep(1000);	// sleep 10 ms
		
			crt += 10;
			++test_count;
		}
		OmnTime::getRealtime(sec1,usec1);
		u32 crt1 = sec1 * 1000 + usec1 / 1000;
		u32 difference = crt1 - start;
		if (difference < diff_min)
			diff_min = difference;

		if (difference > diff_max)
			diff_max = difference;
//		u32 difference = ((sec1-sec)*1000 + (usec1-usec)/1000);
//		if (usec1 < usec)
//		{
//			aos_trace("*******************************************\n");
//		}
		for (int i=0; i<mNumConns; ++i)
		{
			mConns[i].bandwidth = mConns[i].length * test_count * 1000/difference;
		}
/*		aos_trace("sec1:%u,sec:%u\n",sec1,sec);
		aos_trace("usec1:%u,usec:%u\n",usec1,usec);
		aos_trace("crt:%u,crt1:%u\n",crt,crt1);
		aos_trace("difference:%u\n",difference);
		aos_trace("test_count:%d\n",test_count);*/
		mDifference = difference;
		mTestCount = test_count;
		mSec = sec;
		mSec1 = sec1;
		mUsec = usec;
		mUsec1 = usec1;
		mStart = start;
		mCrt1 = crt1;

		checkBandwidthUsage();
		//aos_trace("diff_min:%u,diff_max%u",diff_min,diff_max);

		tries++;
	}
}


bool
AosQoSChannelMonitorManagerTester::addRemoveFlows()
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
AosQoSChannelMonitorManagerTester::addFlows()
{
	u8 tmp_proto;

//	if (mNumConns >= eMaxConns)
	if (mNumConns >= 100)
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
	mConns[mNumConns].sip = (u32) OmnRandom::nextInt(100, 0xfffffffe);
	mConns[mNumConns].dip = (u32) OmnRandom::nextInt(100, 0xfffffffe);
//	mConns[mNumConns].dip = (u32) OmnRandom::nextInt(100, 100);
	mConns[mNumConns].sport = (u16) OmnRandom::nextInt(1, 0xffff);
	mConns[mNumConns].dport = (u16) OmnRandom::nextInt(1, 0xffff);
	//mConns[mNumConns].proto= (u8) OmnRandom::nextInt(0, 1);
	tmp_proto= (u8) OmnRandom::nextInt(0, 1);              //Ping
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

	if (tmp_proto == 0)
	{
		mConns[mNumConns].proto = AOS_QOS_TCP;
	}
	else
	{
		mConns[mNumConns].proto = AOS_QOS_UDP;
	}

	mNumConns++;
	return true;
}


bool
AosQoSChannelMonitorManagerTester::removeFlows()
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
AosQoSChannelMonitorManagerTester::checkBandwidthUsage()
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
			u32 band;
			u32 rslt = qos_get_channel_bw(
						htonl(mConns[i].sip), htons(mConns[i].sport), 
						htonl(mConns[i].dip), htons(mConns[i].dport), 
						mConns[i].proto, 
						&band);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt < 0)) << endtc;
			aos_trace("rslt:%u\n",rslt);
		}

		return true;
	}

	int sec = OmnTime::getCrtSec();

	for (int i=0; i<mNumConns; i++)
	{
		// 
		// We should not check a channel unless it has started for at least two seconds. 
		//
		if (sec - mConns[i].start <= 2)
		{
			continue;
		}

		// 
		// Check whether the interface is being monitored
		//
/*		if (!mInterfaceStatus[mConns[i].if_id])
		{
			// 
			// Not monitoroed
			//
			continue;
		}*/
		u32 band;
		u32 rslt = qos_get_channel_bw(
						mConns[i].sip, mConns[i].sport, 
						mConns[i].dip, mConns[i].dport, 
						mConns[i].proto, 
						&band);
/*		if (!rslt) {
			u32 src_ip;
			u32 dst_ip;
			u16 src_port;
			u16 dst_port;
			u8  proto;
			void *p;
			int NumMatched;
			int j;
			int test;
			NumMatched = 0;
			test = qos_get_channel_info(mConns[i].sip,mConns[i].sport,mConns[i].dip,mConns[i].dport,mConns[i].proto,&src_ip,&src_port,&dst_ip,&dst_port,&proto,&p);
			mConns[i].ch = p;
			aos_assert1(test == 0);
			for (j = 0; j <mNumConns; ++j)
			{
				if (src_ip != mConns[j].sip)
					continue;
				if (dst_ip != mConns[j].dip)
					continue;
				if (src_port != mConns[j].sport)
					continue;
				if (dst_port != mConns[j].dport)
					continue;
				if (proto != mConns[j].proto)
					continue;
				++NumMatched;
			}
			if (NumMatched == 0) {
				aos_trace ("src_ip:%u.%u.%u.%u",NIPQUAD(src_ip));
				aos_trace ("dst_ip:%u.%u.%u.%u",NIPQUAD(dst_ip));
				aos_trace("src_port:%u",src_port);
				aos_trace("dst_port:%u",dst_port);
				aos_trace("proto:%u",proto);
				aos_trace ("Member");
				aos_trace ("src_ip:%u.%u.%u.%u",NIPQUAD(mConns[i].sip));
				aos_trace ("dst_ip:%u.%u.%u.%u",NIPQUAD(mConns[i].dip));
				aos_trace("src_port:%u",mConns[i].sport);
				aos_trace("dst_port:%u",mConns[i].dport);
				aos_trace("proto:%u",mConns[i].proto);
			}
			aos_trace("NumMatched:%d",NumMatched);
		}
*/
		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
		//bool bandcomp = mConns[i].bandwidth >= band * 0 && mConns[i].bandwidth <= band * 10;
		bool bandcomp = band >= mConns[i].bandwidth * 0.95 && band <= mConns[i].bandwidth * 1.05;
		if (!bandcomp)
		{
			aos_trace("band:%u,bw:%u,percent:%f",
						band,mConns[i].bandwidth,(100.0 * abs((int)(mConns[i].bandwidth - band)))/(float)mConns[i].bandwidth);
			aos_trace("len for each pkt:%u",mConns[i].length);
			aos_trace("difference:%u",mDifference);
			aos_trace("TestCount:%d",mTestCount);
/*			aos_trace("mStart,mSec,mUsec:%u,%u,%u",mStart,mSec,mUsec);
			aos_trace("mCrt1,mSec1,mUsec1:%u,%u,%u",mCrt1,mSec1,mUsec1);
			aos_trace("sip:%u.%u.%u.%u",NIPQUAD(mConns[i].sip));
			aos_trace("dip:%u.%u.%u.%u",NIPQUAD(mConns[i].dip));
			aos_trace("sport:%u",mConns[i].sport);
			aos_trace("dport:%u",mConns[i].dport);
			aos_trace("proto:%u",mConns[i].proto);*/
			aos_trace("index:%u",i);
/*			int j;
			int NumMatched;
			NumMatched = 0;
			for (j = 0; j < mNumConns; ++j)
			{
				if (mConns[i].sip != mConns[j].sip)
					continue;
				if (mConns[i].dip != mConns[j].dip)
					continue;
				if (mConns[i].sport != mConns[j].sport)
					continue;
				if (mConns[i].dport != mConns[j].dport)
					continue;
				if (mConns[i].proto != mConns[j].proto)
					continue;
				++NumMatched;
			}
			aos_trace("NumMatched %d",NumMatched);*/
		}
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(bandcomp)) << endtc;
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
AosQoSChannelMonitorManagerTester::changeConfig()
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
/*	case 2:
		stopMonitoring();
		break;*/
/*	case 3:
		addGenRule();
		break;
	case 4:
		delGenRule();
		break;
	case 5:
		modGenRule();
		break;
	case 6:
		addMacRule();
		break;
	case 7:
		delMacRule();
		break;
	case 8:
		modMacRule();
		break;
	case 9:
		addIfRule();
		break;
	case 10:
		modIfRule();
		break;
	case 11:
		addPortRule();
		break;
	case 12:
		delPortRule();
		break;
	case 13:
		modPortRule();
		break;
	case 14:
		addProtoRule();
		break;
	case 15:
		delProtoRule();
		break;
	case 16:
		ModProtoRule();
		break;*/


	default:
		 OmnAlarm << "Unrecognized command: " << index << enderr;
		 return false;
	}

	return true;
}


bool
AosQoSChannelMonitorManagerTester::startMonitoring()
{
	aosRunCli("qos traffic status on",true);
	mStartMonitoring = true;

	return true;
}

bool
AosQoSChannelMonitorManagerTester::stopMonitoring()
{
	aosRunCli("qos traffic status off", true);
	mStartMonitoring = false;
	return true;
}

bool
AosQoSChannelMonitorManagerTester::addInterface()
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
AosQoSChannelMonitorManagerTester::removeInterface()
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
AosQoSChannelMonitorManagerTester::show()
{
	aosRunCli("???", true);
	return true;
}


