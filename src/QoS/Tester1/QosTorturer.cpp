////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.cpp
// Description:
//   
//
// Modification History:
//  11/16/2006  Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "QoS/Tester1/QosTorturer.h"
#include "alarm/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Porting/Sleep.h"
#include "Random/RandomInteger.h"
#include "Random/RandomUtil.h"
#include "StreamSimu/StreamSimuMgr.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
//#include "UtilComm/Ptrs.h"
#include "Random/RandomInteger.h"

#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"


#include "Tester/Test.h"
#include "Tester/TestMgr.h"

// 
// CLI Command selector definition
//
static OmnString sgCliSelectorDef = 
        "<Elements>"
            "<Element>1,  2</Element>"     // status on
            "<Element>2,  2</Element>"     // status off
            "<Element>3,  80</Element>"     // add general rule
            "<Element>4,  20</Element>"      // remove general rule
/*          "<Element>5,  2</Element>"      // modify general rule
            "<Element>6,  2</Element>"      // clear general rule
            "<Element>7,  2</Element>"      // show general rule 
            "<Element>8,  2</Element>"      // add mac rule
            "<Element>9,  2</Element>"      // remove mac rule
            "<Element>10,  2</Element>"     // modify mac rule
            "<Element>11,  2</Element>"     // clear mac rule 
            "<Element>12,  2</ElemeNt>"     // show mac rule
            "<Element>13,  2</Element>"     // add vlan rule
            "<Element>14,  2</Element>"     // remove vlan rule
            "<Element>15,  2</Element>"     // modify vlan rule
            "<Element>16,  2</Element>"     // clear vlan rule
            "<Element>17,  2</Element>"     // show vlan rule
            "<Element>18,  2</Element>"     // add interface rule
            "<Element>19,  2</Element>"     // remove interface rule
            "<Element>20,  2</Element>"     // modify interface rule
            "<Element>21,  2</Element>"     // clear interface rule
            "<Element>22,  2</Element>"     // show interface rule
            "<Element>23,  2</Element>"     // add port rule
            "<Element>24,  2</Element>"     // remove port rule
            "<Element>25,  2</Element>"     // modify port rule
            "<Element>26,  2</Element>"     // clear port rule
            "<Element>27,  2</Element>"     // show port rule
            "<Element>28,  2</Element>"     // add protocol rule
            "<Element>29,  2</Element>"     // remove protocol rule
            "<Element>30,  2</Element>"     // modify protocol rule
            "<Element>31,  2</Element>"     // clear protocol rule
            "<Element>32,  2</Element>"     // show protocol rule
            "<Element>33,  2</Element>"     // save config
            "<Element>34,  2</Element>"     // clear config
            "<Element>35,  2</Element>"     // show config
            "<Element>36,  2</Element>"     // show channel
            "<Element>37,  2</Element>"     // show channel all
            "<Element>38,  2</Element>"     // show channel history
            "<Element>39,  2</Element>"     // show dev
            "<Element>40,  2</Element>"     // show dev all
            "<Element>41,  2</Element>"     // show dev history
            "<Element>42,  2</Element>"     // show priority
            "<Element>43,  2</Element>"     // show priority all
            "<Element>44,  2</Element>"     // show priority history
*/        "</Elements>";


QosTorturer::stream::stream()
:
sport(0),
dport(0),
bandwidth(0),
packetsize(0),
realbw(0),
packetNumPerSec(0),
l2Bandwidth(0)
{
}

QosTorturer::stream::~stream()
{
}

QosTorturer::stream& QosTorturer::stream::operator = (const QosTorturer::stream &stm)
{
	sip             = stm.sip             ;
	sport           = stm.sport           ;
	dip             = stm.dip             ;
	dport           = stm.dport           ;
	protocol        = stm.protocol        ;
	bandwidth       = stm.bandwidth       ;
	packetsize      = stm.packetsize      ;
	priority        = stm.priority        ;
	realbw          = stm.realbw          ;
	packetNumPerSec = stm.packetNumPerSec ;
	l2Bandwidth     = stm.l2Bandwidth     ;
	return *this;
}

void
QosTorturer::stream::clear()
{
	sip             = "";
	sport           = 0;
	dip             = "";
	dport           = 0;
	protocol        = "";
	bandwidth       = 0;
	packetsize      = 0;
	priority        = "";
	realbw          = 0;
	packetNumPerSec = 0;
	l2Bandwidth     = 0;
}


QosTorturer::rule::rule()
:
sport(0),
dport(0),
port(0)
{
}

QosTorturer::rule::~rule()
{
}


QosTorturer::rule& QosTorturer::rule::operator = (const QosTorturer::rule &ru)
{
    type        = ru.type     ;
    sip         = ru.sip      ;
    sport       = ru.sport    ;
    dip         = ru.dip      ;
    dport       = ru.dport    ;
    protocol    = ru.protocol ;
    interface   = ru.interface;
    mac         = ru.mac      ;
    vlantag    = ru.vlantag ;
    direction   = ru.direction;
    port        = ru.port     ;
    app         = ru.app      ;
    priority    = ru.priority ;
	return *this;
}   

void
QosTorturer::rule::clear()
{
    type = "";
    sip = "";
    sport = 0;
    dip = "";
    dport = 0;
    protocol = "";
    interface = "";
    mac = "";
    vlantag = 0;
    direction = "";
    port = 0;
    app = "";
    priority = "";
}



QosTorturer::QosTorturer()
:
mNumStream(0),
mNumRule(0),
mIsStatusOn(false),
mTcBw(0),
mTc_Port(0),
mBouncer_Port(0),
mRhc_Port(0),
mCLITurnNum(0),
mStreamProcTurnNum(0),
mCheckTurnNum(0),
mTimeFromReset(0),
mTimeFromProcStream(0)
{
}

bool 
QosTorturer::start()
{
	
	mTc_Ip			= OmnIpAddr("172.22.155.121");
	mTc_Port		= 28000;
	mBouncer_Ip		= OmnIpAddr("172.22.250.114");
	mBouncer_Port	= 28000;
	mRhc_Ip			= OmnIpAddr("172.22.155.122");
	mRhc_Port		= 28000;
	
   	mTcConn 		= new OmnTcpClient("TC",mTc_Ip,mTc_Port,1,OmnTcp::eNoLengthIndicator);
	mBouncerConn	= new OmnTcpClient("Bouncer",mBouncer_Ip,mBouncer_Port,1,OmnTcp::eNoLengthIndicator);
	mRhcConn		= new OmnTcpClient("Rhc",mRhc_Ip,mRhc_Port,1,OmnTcp::eNoLengthIndicator);
//	sendCmd(OmnString("firewall rule | tc qdisc add dev eth1 root handle 1: htb default 1"), mTcConn);	

    clitest();
    return true;
}

bool 
QosTorturer::clitest()
{
    OmnBeginTest << "Test qos";

    AosRandomIntegerPtr cliSelector = OmnNew AosRandomInteger("CliSelector", 
    														  sgCliSelectorDef,
    														  100);
	initQos();
    mIsStatusOn = true;
    bool rslt;
 
	setTcBw(RAND_INT(eTcMinBw, eTcMaxBw));

    int loopWithRunCLIs = RAND_INT(1,10);
    for(int i = 0;i < loopWithRunCLIs;i++)
    {
        mCLITurnNum ++;
        mStreamProcTurnNum = 0;
        mCheckTurnNum = 0;
        rslt = runCLIs(RAND_INT(20,40),cliSelector);
//		setTcBw(RAND_INT(1,500));

        int loopWithProcStreams = RAND_INT(5,10);
        for(int j = 0;j < loopWithProcStreams;j++)
        {
        	mStreamProcTurnNum ++;
        	mCheckTurnNum = 0;
            rslt = procStreams(RAND_INT(5,10));
			updateStreamPriority();
			clearLowPrioUdp();
			
			int s1 = RAND_INT(30,30);
			int s2 = RAND_INT(30,30);
			
			mTimeFromProcStream = 0;
            OmnSleep(s1);
			mTimeFromProcStream += s1;
			mTimeFromReset += s1;
            resetAllStat();
			mTimeFromReset = 0;
            OmnSleep(s2);
			mTimeFromProcStream += s2;
			mTimeFromReset += s2;
	
            int loopWithBandwidthCheck = RAND_INT(1,5);
            for(int k = 0;k < loopWithBandwidthCheck;k++)
            {
				int s3 = RAND_INT(5,10);
                OmnSleep(s3);
				mTimeFromProcStream += s3;
				mTimeFromReset += s3;
        		mCheckTurnNum ++;
                rslt = checkBandwidth();
        		cout << endl;
        		cout << endl;
            }           
        }   
    }
    return true;   
}   
    

bool
QosTorturer::runCLIs(const int runNum,const AosRandomIntegerPtr &randInt)
{
    bool rslt;
    int count = 0;
    while (count < runNum)
    {
        int index = RAND_INT(3,4);
        
        switch (index)
        {
            case eStatusOn:
                rslt = runStatusOn();
                break;
            case eStatusOff:
                rslt = runStatusOff();
                break;
            case eGenRuleAdd:
                rslt = runAddGeneralRule();
                break;
            case eGenRuleRemove:
                rslt = runRemoveGeneralRule();
                break;
//          case eGenRuleModify:
//              rslt = 
//              break;
//            case eGenRuleClear:
//                rslt = 
//                break;  
//            case eGenRuleShow:
//                rslt = 
//                break;  
//            case eMacRuleAdd:
//                rslt = runAddMacRule();
//                break;  
//            case eMacRuleRemove:
//                rslt = runRemoveMacRule();
//                break;  
/*            case eMacRuleModify:
                rslt = 
                break;  
            case eMacRuleClear:
                rslt = 
                break;  
            case eMacRuleShow:
                rslt = 
                break;  
            case eVlanRuleAdd:
                rslt = 
                break;  
            case eVlanRuleRemove:
                rslt = 
                break;  
            case eVlanRuleModify:
                rslt = 
                break;  
            case eVlanRuleClear:
                rslt = 
                break;  
            case eVlanRuleShow:
                rslt = 
                break;  
            case eIfRuleAdd:
                rslt = 
                break;  
            case eIfRuleRemove:
                rslt = 
                break;  
            case eIfRuleModify:
                rslt = 
                break;  
            case eIfRuleClear:
                rslt = 
                break;  
            case eIfRuleShow:
                rslt = 
                break;  
            case ePortRuleAdd:
                rslt = 
                break;  
            case ePortRuleRemove:
                rslt = 
                break;  
            case ePortRuleModify:
                rslt = 
                break;  
            case ePortRuleClear:
                rslt = 
                break;  
            case ePortRuleShow:
                rslt = 
                break;  
            case eProtoRuleAdd:
                rslt = 
                break;  
            case eProtoRuleRemove:
                rslt = 
                break;  
            case eProtoRuleModify:
                rslt = 
                break;  
            case eProtoRuleClear:
                rslt = 
                break;  
            case eProtoRuleShow:
                rslt = 
                break;  
            case eSaveConfig:
                rslt = 
                break;  
            case eClearConfig:
                rslt = 
                break;  
            case eShowConfig:
                rslt = 
                break;  
            case eShowChannel:
                rslt = 
                break;  
            case eShowChannelAll:
                rslt = 
                break;  
            case eShowChannelHis:
                rslt = 
                break;  
            case eShowDev:
                rslt = 
                break;  
            case eShowDevAll:
                rslt = 
                break;  
            case eShowDevHis:
                rslt = 
                break;  
            case eShowPri:
                rslt = 
                break;  
            case eShowPriAll:
                rslt = 
                break;  
            case eShowPriHis:
                rslt = 
                break;  
                    
    */                          
        }
        count++;
    }   
    return true;
}

bool QosTorturer::runStatusOn()
{
    OmnString cmd = "qos traffic status on";
//    aosRunCli(cmd, true);   
	sendCmd(cmd,mRhcConn);

//	if(!sendCmd(cmd,mRhcConn))
//	{
//		return false;
//	}
 	
 	   
    mIsStatusOn = true;
    
    return true;
}

bool QosTorturer::runStatusOff()
{
    OmnString cmd = "qos traffic status off";
//    aosRunCli(cmd, true);   

	sendCmd(cmd,mRhcConn);
//	if(!sendCmd(cmd,mRhcConn))
//	{
//		return false;
//	}
    
    mIsStatusOn = false;
    
    return true;
}

bool QosTorturer::runAddGeneralRule()
{
	bool cmdCorrect = true;
    OmnString cmd = "qos generalrule add ";
    
    if(PERCENTAGE(eSelectCorrectSIPPct))
    {
        OmnString sip;
		sip = getSipFromList();
	}
	else
	{
		// 
		// Generate an incorrect IP address
		//
		sip = aos_next_incorrect_ip();
		cmdCorrect = false;
	}
	
    // unsigned int sport = RAND_INT(1025,65535);
    unsigned int sport = RAND_INT(eMinPort, eMaxPort);

    OmnString dip;
	if(PERCENTAGE(eSelectCorrectDIPPct))
	{
		dip = getDipFromList();
    }
	else
	{
		dip = aos_next_incorrect_ip();
		cmdCorrect = false;
	}

    unsigned int dport = RAND_INT(eMinPort, eMaxPort);

    // Randomly select protocol
    OmnString protocol;
    genProtocol(protocol);

    //random interface
    OmnString interface;
    genInterfaceRhc(interface);

    //random priority
    OmnString priority;
    genPriority(priority);      
        
    cmd << sip << " " << sport << " " << dip << " " << dport
            << " " << protocol << " " << interface << " " 
            << priority;
            
    // add general rule to the rule list with duplication checking      
    if(!addGenRule(sip,
                   sport,
                   dip,
                   dport,
                   protocol,
                   interface,
                   priority))
    {
        return false;
    }

	bool ret = sendCmd(cmd,mRhcConn);
	OmnTCTrue(ret == cmdCorrect) << cmd << endtc;
    return true;  
}


bool QosTorturer::runRemoveGeneralRule()
{
    OmnString cmd = "qos generalrule remove ";

    // first we decide to send a right CLI or wrong one
    if(RAND_INT(1,100) <= 80)
    {   
        // create a right CLI
        int index[eMaxRule] ;
        int j = 0;
        int i;
        
        for(i = 0; i < mNumRule; i++)
        {
            if(mRuleList[i].type == "general")
            {
                index[j] = i;
                j++;
            }           
        }   

        if(j == 0)
        {
            // no rule to remove
            return false;
        }


        int randIndex1 = RAND_INT(0,j-1);
        int randIndex2 = index[randIndex1];

        // create remove CLI base on rule randIndex2
 
        cmd << mRuleList[randIndex2].sip << " "
            << mRuleList[randIndex2].sport << " "
            << mRuleList[randIndex2].dip << " "
            << mRuleList[randIndex2].dport << " "
            << mRuleList[randIndex2].protocol << " "
            << mRuleList[randIndex2].interface;
        
        if(!sendCmd(cmd,mRhcConn))
		{
		}
		
		// remove from CLI list
        removeRule(randIndex2);   
    }
    else
    {
        // create a wrong CLI.
        // check whether it is wrong CLI.
    }
    return true;
}


 
bool QosTorturer::genProtocol(OmnString &protocol)
{
    OmnString proto[2];
    proto[0] = "tcp";
    proto[1] = "udp";
    
    int i = RAND_INT(0,0);
    protocol = proto[i];
    
    return true;            
}

bool QosTorturer::genMac(OmnString &mac)
{
    OmnString maclist[2];
    maclist[0] = "00:17:31:CF:22:D0";
    maclist[1] = "22:33:44:55:66:77";
    
    int i = RAND_INT(0,0);
    mac = maclist[i];
    
    return true;            
}

bool QosTorturer::genDirection(OmnString &direction)
{
    OmnString direct[2];
    direct[0] = "sending";
    direct[1] = "receiving";
    
    int i = RAND_INT(0,0);
    direction = direct[i];
    
    return true;            
}


bool QosTorturer::genApplication(OmnString &app)
{
    OmnString apps[2];
    apps[0] = "telnet";
    apps[1] = "ftp";
    
    int i = RAND_INT(0,0);
    app = apps[i];
    
    return true;            
}


bool QosTorturer::genInterface(OmnString &interface)
{
    OmnString intf[2];
    intf[0] = "eth0";
    intf[1] = "eth1";
    
    int i = RAND_INT(0,1);
    interface = intf[i];
    
    return true;    
}

bool QosTorturer::genInterfaceRhc(OmnString &interface)
{
    OmnString intf[2];
    intf[0] = "eth2";
    intf[1] = "eth1";
    
    int i = RAND_INT(0,0);
    interface = intf[i];
    
    return true;    
}




bool QosTorturer::genPriority(OmnString &priority)
{
    OmnString prior[3];
    prior[0] = "low";
    prior[1] = "medium";
    prior[2] = "high";
    
    int i = RAND_INT(0,2);
    priority = prior[i];
    
    return true;    
}

bool QosTorturer::genIpAddr(OmnString &ipaddr)
{
	OmnString ipaddrs[32];
	ipaddrs[0] = "192.168.22.2";
	ipaddrs[1] = "192.168.22.1";
	ipaddrs[2] = "192.168.11.1";
	ipaddrs[3] = "192.168.11.2";
	ipaddrs[4] = "192.168.33.1";
	
	int i = RAND_INT(0,4);
	ipaddr = ipaddrs[i];
	
	return true;
	
}



bool 
QosTorturer::removeRule(const int &index)
{
    if(index < 0 || index > mNumRule - 1)
    {
        return false;
    }
    // move back the rule
    for(int i = index;i < mNumRule - 1;i++)
    {

        mRuleList[i] = mRuleList[i+1];
    }
    mRuleList[mNumRule-1].clear();
    mNumRule -= 1;
    return true;
}

bool QosTorturer::addGenRule(
					OmnString &sip,
					unsigned int &sport,
					OmnString &dip,
                    unsigned int &dport,
					OmnString &protocol,
                    OmnString &interface,
					OmnString &priority)
{
    if (mNumRule >= eMaxRule)
    {
        return false;
    }       

    // check whether there is one same rule or 
    // it is tcp and send ip port is occupied.
    for(int i =0;i < mNumRule;i++)
    {
        if(mRuleList[i].sip      == sip      &&
           mRuleList[i].sport    == sport    &&
           mRuleList[i].protocol == protocol &&
           mRuleList[i].dip      == dip      &&
           mRuleList[i].dport    == dport    &&
           mRuleList[i].type     == "general")
    	{
                // same stream (we do not consider about priority
                return false;
        }    
    }
	
    // it is a good rule, append to the array
    mRuleList[mNumRule].type = "general";
    mRuleList[mNumRule].sip = sip;
    mRuleList[mNumRule].sport = sport;
    mRuleList[mNumRule].dip = dip;
    mRuleList[mNumRule].dport = dport;
    mRuleList[mNumRule].protocol = protocol;
    mRuleList[mNumRule].interface = interface;
    mRuleList[mNumRule].priority = priority;
    
    mNumRule ++;
    
    return true;
}                    

bool 
QosTorturer::procStreams(const int genNum)
{
    for(int i = 0;i < genNum;i++)
    {
    	int index = RAND_INT(1,100);
        if(index <= 70)
        {
            genStream();
        }
        else if(index > 70 && index <= 100)
        {    
            removeStream();
        }
//        else
//        {
//            modifyStream();
//        }
        
    }
   	
   	resetAllStat();
    
    return true;
}

bool 
QosTorturer::genStream()
{
	if(mNumStream >= eMaxStream)
	{
		return false;
	}
    int index = RAND_INT(0,mNumRule);
    
    //generate streams base on the rules
    if(mRuleList[index].type == "general")
    {
    	//check whether the stream is already existed
		cleanUsedList();

    	genStreamByGenRule(index);
    }

    else if(mRuleList[index].type == "mac")
    {
    	genStreamByMacRule(index);
    }
    else if(mRuleList[index].type == "vlan")
    {
    	genStreamByVLanRule(index);

    }
//    else if(mRuleList[index].type == "interface")
//    {
//        
//    }
    else if(mRuleList[index].type == "port")
    {
        
    }
    else if(mRuleList[index].type == "protocol")
    {    	
		genStreamByProtocolRule(index);    	
    }
    

    // common this because we can only recognize the priority by the priority checking
    //mStreamList[mNumStream].priority = mRuleList[index].priority;

    // send to BouncerMgr
    OmnString cmd = "bouncer mgr add bouncer ";
    cmd << mStreamList[mNumStream].protocol << " "
        << mStreamList[mNumStream].sip << " "
        << mStreamList[mNumStream].sport  << " "
        << mStreamList[mNumStream].dip << " "
        << mStreamList[mNumStream].dport;
        
    sendCmd(cmd,mBouncerConn);         

    AosStreamSimuMgr::getSelf()->addStream(mStreamList[mNumStream].protocol,
    									   OmnIpAddr(mStreamList[mNumStream].sip),
    									   mStreamList[mNumStream].sport,
    									   OmnIpAddr(mStreamList[mNumStream].dip),
    									   mStreamList[mNumStream].dport,
    									   mStreamList[mNumStream].bandwidth,
    									   mStreamList[mNumStream].packetsize,
    									   (AosStreamType::E)mStreamList[mNumStream].streamType,
    									   mStreamList[mNumStream].maxBw,
    									   mStreamList[mNumStream].minBw
    									   );
    
    int Bw = 0;
    int packetNu = 0;
    AosStreamSimuMgr::getSelf()->getBandwidth(mStreamList[mNumStream].protocol,
    	                                      OmnIpAddr(mStreamList[mNumStream].sip),
    									      mStreamList[mNumStream].sport,
    									      OmnIpAddr(mStreamList[mNumStream].dip),
    									      mStreamList[mNumStream].dport,
    										  Bw,packetNu);
	mStreamList[mNumStream].bandwidth = Bw;
	mStreamList[mNumStream].packetNumPerSec = packetNu;
	    										  
    calcL2Bw(mStreamList[mNumStream].protocol,
    		 mStreamList[mNumStream].bandwidth,
    		 mStreamList[mNumStream].packetsize,
    		 mStreamList[mNumStream].packetNumPerSec,
    		 mStreamList[mNumStream].l2Bandwidth);

    mNumStream ++;
    return true;        
}

bool 
QosTorturer::modifyStream()
{
	if(mNumStream <= 0)
	{
		return false;
	}
    int index = RAND_INT(0,mNumStream - 1);  
    
    int Bw;
        
    long i1 = RAND_INT(eMinBandwidth,eMaxBandwidth);
    long i2 = RAND_INT(eMinBandwidth,eMaxBandwidth);
    
    if(i1 > i2)
    {
    	mStreamList[index].maxBw = i1;
    	mStreamList[index].minBw = i2;	
    }
    else
    {
    	mStreamList[index].minBw = i1;
    	mStreamList[index].maxBw = i2;	        
    }
    
    Bw = RAND_INT(eMinBandwidth,eMaxBandwidth);   
    mStreamList[index].streamType = RAND_INT(1,4);
    
    mStreamList[index].bandwidth  = Bw;
    mStreamList[index].packetsize = RAND_INT(eMinPacketSize,eMaxPacketSize);  
    
    // call streamMgr
    AosStreamSimuMgr::getSelf()->modifyStream(mStreamList[index].protocol,
    									      OmnIpAddr(mStreamList[index].sip),
    									      mStreamList[index].sport,
    									      OmnIpAddr(mStreamList[index].dip),
    									      mStreamList[index].dport,
    									      mStreamList[index].bandwidth,
    									      mStreamList[index].packetsize,
    									      (AosStreamType::E)mStreamList[index].streamType,
    									      mStreamList[index].maxBw,
    									      mStreamList[index].minBw);
    Bw = 0;
    int packetNu = 0;
    AosStreamSimuMgr::getSelf()->getBandwidth(mStreamList[mNumStream].protocol,
    	                                      OmnIpAddr(mStreamList[mNumStream].sip),
    									      mStreamList[mNumStream].sport,
    									      OmnIpAddr(mStreamList[mNumStream].dip),
    									      mStreamList[mNumStream].dport,
    										  Bw,packetNu);
	mStreamList[mNumStream].bandwidth = Bw;
	mStreamList[mNumStream].packetNumPerSec = packetNu;
	    										  
    calcL2Bw(mStreamList[mNumStream].protocol,
    		 mStreamList[mNumStream].bandwidth,
    		 mStreamList[mNumStream].packetsize,
    		 mStreamList[mNumStream].packetNumPerSec,
    		 mStreamList[mNumStream].l2Bandwidth);

    return true;
}


bool 
QosTorturer::removeStream()
{
	if(mNumStream <= 0)
	{
		return false;
	}
    int index = RAND_INT(0,mNumStream - 1);  

    AosStreamSimuMgr::getSelf()->removeStream(mStreamList[index].protocol,
    									      mStreamList[index].sip,
    									      mStreamList[index].sport,
    									      mStreamList[index].dip,
    									      mStreamList[index].dport);
	// move stream list
    for(int i = index;i < mNumStream - 1;i++)
    {
        mStreamList[i] = mStreamList[i+1];
    }
    mStreamList[mNumStream-1].clear();
    mNumStream --;
    return true;
}


bool 
QosTorturer::checkBandwidth()
{
	long epHighBw = 0;
	long epMidBw = 0;
	long epLowBw = 0;
	long realHighBw = 0;
	long realMidBw = 0;
	long realLowBw = 0;
	long realBw = 0;
	long epBw = 0;
	
	if(mIsStatusOn == true)
	{
		aos_assert_r(getExpectBandwidth(epHighBw, epMidBw, epLowBw), false);
		aos_assert_r(getRealBandwidth(realHighBw, realMidBw, realLowBw), false);
		
		bool ret = (realHighBw > epHighBw * (100+eTolerance)/100) || 
		   		   (realHighBw < epHighBw * (100-eTolerance)/100) ||
		   		   (realMidBw  > epMidBw  * (100+eTolerance)/100) || 
		   		   (realMidBw  < epMidBw  * (100-eTolerance)/100) ||
		   		   (realLowBw  > epLowBw  * (100+eTolerance)/100) || 
		   		   (realLowBw  < epLowBw  * (100-eTolerance)/100);
		if (ret)
		{
        	cout << "\n---------------------------" << endl;
			cout<<"TesterRslt: Out of range"<<endl;
			cout << "expect high is: " << epHighBw;	
			cout << "\texpect mid is:  " << epMidBw;
			cout << "\texpect low is:  " << epLowBw << endl;
			cout << "real high is:   " << realHighBw;	
			cout << "\treal mid is:    " << realMidBw;
			cout << "\treal low is:    " << realLowBw << endl;
			cout << "diff highis:    " 
				 << abs(realHighBw - epHighBw)*100/(epHighBw+1) << " %" ;	
			cout << "\tdiff mid is:    " 
				 << abs(realMidBw - epMidBw)*100/(epMidBw+1) << " %" ;
			cout << "\tdiff low is:    " 
				 << abs(realLowBw - epLowBw)*100/(epLowBw+1) << " %" << endl;
			showAllStat();
		}
		else
		{
        	cout << "\n---------------------------" << endl;
			cout << "TesterRslt: Ok" << endl;
			cout << "expect high is : " << epHighBw;	
			cout << "\texpect mid is  : " << epMidBw;
			cout << "\texpect low is  : " << epLowBw << endl;
			cout << "real high is   : " << realHighBw;	
			cout << "\treal mid is    : " << realMidBw;
			cout << "\treal low is    : " << realLowBw << endl;
			cout << "diff highis    : " 
				 << abs(realHighBw - epHighBw)*100/(epHighBw+1) << " %" ;	
			cout << "\tdiff mid is    : " 
				 << abs(realMidBw - epMidBw)*100/(epMidBw+1) << " %" ;
			cout << "\tdiff low is    : " 
				 << abs(realLowBw - epLowBw)*100/(epLowBw+1) << " %" << endl;
			showAllStat();
		}	

		OmnTCTrue(ret) << endtc;
	}				 
	else
	{
		getExpectBandwidth(epBw);
		getRealBandwidth(realBw);
		
		if((realBw > epBw * (100+eTolerance)/100) || 
		   (realBw < epBw * (100-eTolerance)/100))
		{
        	cout << "\n---------------------------" << endl;
			cout << "Out of Range" << endl;
			cout << "expect bw is: " << epBw << endl;
			cout << "real bw is  : " << realBw << endl;
			cout << "diff        : " << abs(realBw -  epBw)*100/(epBw+1) << " %";
			showAllStat();
		}
		else
		{
        	cout << "\n---------------------------" << endl;
			cout << "Good!" << endl;
			cout << "expect bw is: " << epBw << endl;
			cout << "real bw is:" << realBw << endl;
			cout << "diff        : " << abs(realBw -  epBw)*100/(epBw+1) << " %";
			showAllStat();
		}
//		OmnTC(OmnExpected<bool>(true),OmnActual<bool>(realBw < epBw * (100+eTolerance)/100));
//		OmnTC(OmnExpected<bool>(true),OmnActual<bool>(realBw > epBw * (100-eTolerance)/100));
	}
	
	return false;
}


// 
// This function retrieves the total bandwidth that 
// all streams consumed.
//
bool 
QosTorturer::getExpectBandwidth(long &epTotalBw)
{
	long totalNeedBw = 0;
	for(int i = 0;i < mNumStream;i++)
	{
		totalNeedBw += mStreamList[i].bandwidth;
		totalNeedBw += mStreamList[i].l2Bandwidth;
	}
	epTotalBw = totalNeedBw;
	
	if(epTotalBw > mTcBw)
	{
		epTotalBw = mTcBw;
	}
	
	return true;	
}

bool 
QosTorturer::getExpectBandwidth(long &epHighBw,
								long &epMidBw,
								long &epLowBw)
{
	epHighBw = 0;
	epMidBw = 0;
	epLowBw = 0;
	long totalHighBw = 0;
	long totalMidBw  = 0;
	long totalLowBw  = 0;

	// 1. calculate total need bw
	for(int i = 0;i < mNumStream;i++)
	{
		if(mStreamList[i].priority == "high")
		{
			totalHighBw += mStreamList[i].bandwidth;
			totalHighBw += mStreamList[i].l2Bandwidth;
		}
		else if(mStreamList[i].priority == "medium")
		{
			totalMidBw += mStreamList[i].bandwidth;
			totalMidBw += mStreamList[i].l2Bandwidth;
		}
		else if(mStreamList[i].priority == "low")
		{
			totalLowBw += mStreamList[i].bandwidth;
			totalLowBw += mStreamList[i].l2Bandwidth;
		}
	}		

	// 2. calculate minimum need for Middle and Low 
	long minNeedMidBw;
	long minNeedLowBw;
	long restBw = mTcBw;
			
	minNeedMidBw = totalMidBw < eMinMidBw ? totalMidBw:eMinMidBw;
	minNeedLowBw = totalLowBw < eMinLowBw ? totalLowBw:eMinLowBw;

	// 3. calculate high priority expected bw
	epHighBw = totalHighBw < (restBw - minNeedMidBw - minNeedLowBw) ? 
					totalHighBw:(restBw - minNeedMidBw - minNeedLowBw); 
	restBw -= epHighBw;
	
	// 4. calculate middle priority expected bw
	epMidBw = totalMidBw < (restBw - minNeedLowBw) ? 
					totalMidBw:(restBw - minNeedLowBw); 
	restBw -= epMidBw;
	
	// 5. calculate low priority expected bw
	epLowBw = totalLowBw < restBw ? totalLowBw:restBw; 

	return true;
}
								

bool 
QosTorturer::getRealBandwidth(long &realBw)
{

	realBw = 0;
	long curBw ; 
	long aveBw ;
	
	for(int i = 0;i < mNumStream;i++)
	{
		if(!getRealBwPerConn(i, curBw , aveBw))
		{
			// pop error
			return false;
		}						
		realBw += aveBw;
	}
	return true;
}

bool 
QosTorturer::getRealBandwidth(long &realHighBw,
							  long &realMidBw,
							  long &realLowBw)
{
	realHighBw = 0;
	realMidBw = 0;
	realLowBw = 0;
	long curBw ; 
	long aveBw ;
			
	for(int i = 0;i < mNumStream;i++)
	{
		if(!getRealBwPerConn(i, curBw , aveBw))
		{
			// pop error
			break;
		}						
				
		if(mStreamList[i].priority == "high")
		{
			realHighBw += aveBw;
		}
		else if(mStreamList[i].priority == "medium")
		{
			realMidBw += aveBw;
		}
		else if(mStreamList[i].priority == "low")
		{
			realLowBw += aveBw;
		}
	}	
	return true;
}


bool 
QosTorturer::updateStreamPriority()
{
	for(int i = 0;i < mNumStream;i++)
	{
		mStreamList[i].priority = "low";
		for(int j = 0;j < mNumRule;j++)
		{
			if(streamHitRule(i, j))
			{
				// this stream fix this rule
				mStreamList[i].priority = mRuleList[j].priority;
				break;
			}
		}
	}
	return true;
}
	

// 
// Check whether the 'streamNum'-th stream hits the 'ruleNum'-th
// rule. If yes, it returns true. Otherwise, it returns false.
//
bool 
QosTorturer::streamHitRule(const int streamNum,const int ruleNum)
{
	if(mRuleList[ruleNum].type == "general")
	{
		if(mRuleList[ruleNum].sip      == mStreamList[streamNum].sip      &&
		   mRuleList[ruleNum].sport    == mStreamList[streamNum].sport    &&
		   mRuleList[ruleNum].dip      == mStreamList[streamNum].dip      &&
		   mRuleList[ruleNum].dport    == mStreamList[streamNum].dport    &&
		   mRuleList[ruleNum].protocol == mStreamList[streamNum].protocol)
		{
			return true;
		}
	}
	
	if(mRuleList[ruleNum].type == "mac")
	{
		if(mRuleList[ruleNum].mac == "00:17:31:CF:22:D0" &&
			mStreamList[streamNum].sip == "192.168.11.1")
		{
			return true;
		}
	}

	if(mRuleList[ruleNum].type == "vlan")
	{
		if(mStreamList[streamNum].sip == "192.168.11.1")
		{
			return true;
		}
	}
	if(mRuleList[ruleNum].type == "port")
	{
		return false;
	}
	if(mRuleList[ruleNum].type == "protocol")
	{
		if(mRuleList[ruleNum].protocol == mStreamList[streamNum].protocol)
		{
			return true;
		}		
	}

	// not fix any type
	return false;
}

bool 
QosTorturer::getRealBwPerConn(const int streamNum,long &curBw,long &aveBw)
{
	/*gen the command */
	OmnString cmd = "bouncer mgr get bandwidth ";
		
	cmd << mStreamList[streamNum].protocol << " "
	    << mStreamList[streamNum].sip      << " "
	    << mStreamList[streamNum].sport    << " "
	    << mStreamList[streamNum].dip      << " "
	    << mStreamList[streamNum].dport;
	
	/*send command to bouncer*/    
	OmnString rslt;
	OmnString stringBw;
	
	
	if (!sendCmd(cmd, mBouncerConn, rslt))
	{
		OmnAlarm << "Failed to send the command: " << cmd << enderr;
		return false;
	}

	int curPos = 0;
	for(int i = 0;i < 3;i++)
	{	
		curPos = rslt.getWord(curPos,stringBw);
	}
	curBw = stringBw.toInt();	
	for(int i = 0;i < 3;i++)
	{	
		curPos = rslt.getWord(curPos,stringBw);
	}
	aveBw = stringBw.toInt();	
	//curBw = 1	;
	//aveBw = 1;	
	
	mStreamList[streamNum].realbw = aveBw;	
	return true;
}

bool
QosTorturer::sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer)
{
	OmnString strRslt;
	return sendCmd(cmd,dstServer,strRslt);
}


bool
QosTorturer::sendCmd(const OmnString &cmd,
					 const OmnTcpClientPtr &dstServer,
					 OmnString &strRslt)
{
	OmnString errmsg;
	OmnRslt rslt;
	
	int sec = 5;
	bool timeout = false;
	bool connBroken;
	
	if (!dstServer->isConnGood())
	{
		dstServer->closeConn();
		dstServer->connect(errmsg);
		if (!dstServer->isConnGood())
		{
			//alarm and return;
			OmnAlarm << "Failed to process connection" << rslt.getErrmsg() << enderr;
			return false;
		}
	}

	OmnString sendBuff = cmd;
	sendBuff << "\n\n";
	int sendLen = sendBuff.length();
	dstServer->writeTo(sendBuff, sendLen);

	OmnConnBuffPtr revBuff = new OmnConnBuff();
	if (!dstServer->readFrom(revBuff, sec,
						timeout, connBroken, false)||timeout)
	{
		//alarm and return
//        showCmd(cmd,revBuff);
        OmnAlarm << "cannot successfully get message from cli server" << rslt.getErrmsg() << enderr;
        return false;
	}	
	if(revBuff && (! (*revBuff->getBuffer() == '\r' ||*revBuff->getBuffer() == '\n')))
    {
//       showCmd(cmd,revBuff);
    }

	strRslt.assign(revBuff->getBuffer(),revBuff->getDataLength());
    
	return true;
}


bool
QosTorturer::setTcBw(const long &TcBw)
{

	OmnString cmd = "system excute tc class add dev eth1 parent 1: classid 1:1 htb rate ";
	cmd << TcBw << "kbps ceil " << TcBw << "kbps";
	
//	mTcConn = new OmnTcpClient("TC",mTc_Ip,eTc_Port,1,OmnTcp::eNoLengthIndicator);

	sendCmd(OmnString("system excute tc class del dev eth1 parent 1: classid 1:1 htb rate 1kbps ceil 1kbps"),mTcConn);
	sendCmd(OmnString("system excute tc qdisc del dev eth1 root handle 1: htb default 1"), mTcConn);	
	sendCmd(OmnString("system excute tc qdisc add dev eth1 root handle 1: htb default 1"), mTcConn);	
	sendCmd(cmd, mTcConn);	
	mTcBw = TcBw*1024;

	return true;
}

void
QosTorturer::showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff)
{
        cout << "\n----------SEND CMD----------" << endl;
        cout << cmd << endl;
        cout << "----------RESPONSE----------" << endl;
        if(revBuff)
        {
            cout << OmnString(revBuff->getBuffer(),revBuff->getDataLength());
        }
        else
        {
            cout << "empty";
        }
        cout << "\n---------------------------" << endl;
}

bool
QosTorturer::isPortValid(const unsigned int &port)
{
	if(port > 3000)
	{
		return false;	
	
	}	
	return true;

}

OmnString
QosTorturer::getSipFromList()
{
	return "192.168.33.1";
}

OmnString
QosTorturer::getDipFromList()
{
	return "192.168.11.1";
}


bool
QosTorturer::getDipFromVLan(OmnString &mac, OmnString &dip)
{
	dip = "192.168.11.1";
	
	return true;
}

bool
QosTorturer::getDipFromMac(OmnString &mac, OmnString &dip)
{
	if(mac == "00:17:31:CF:22:D0")
	{
		dip = "192.168.11.1";
	}
	
	return true;
}


bool QosTorturer::initQos()
{
//	sendCmd(OmnString("qos generalrule clear"),mRhcConn);
//	sendCmd(OmnString("qos macrule clear"),mRhcConn);
//	sendCmd(OmnString("qos vlanrule clear"),mRhcConn);
//	sendCmd(OmnString("qos ifrule clear"),mRhcConn);
//	sendCmd(OmnString("qos portrule clear"),mRhcConn);
//	sendCmd(OmnString("qos protorule clear"),mRhcConn);
	sendCmd(OmnString("qos clear config"),mRhcConn);
	
	
	sendCmd(OmnString("qos traffic status on"),mRhcConn);
	sendCmd(OmnString("qos monitor direction  in"),mRhcConn);
	sendCmd(OmnString("qos monitor direction add out"),mRhcConn);
	sendCmd(OmnString("qos bandwidth manager on"),mRhcConn);
	sendCmd(OmnString("qos bandwidth manager ctrl eth2 in on"),mRhcConn);
//	sendCmd(OmnString("qos bandwidth manager set detx 16384"),mRhcConn);
//	sendCmd(OmnString("qos bandwidth manager set p1 3000"),mRhcConn);
//	sendCmd(OmnString("qos bandwidth manager set p2 1000 3 2"),mRhcConn);
//	sendCmd(OmnString("qos bandwidth manager set p3 3000"),mRhcConn);
//	sendCmd(OmnString("qos bandwidth manager set param 5 256 4096 600000 4"),mRhcConn);
	
	return true;
}


bool			
QosTorturer::calcL2Bw(const OmnString 	&protocol,
					  const long		bandwidth,
					  const long		packetsize,
					  long				&packetNumPerSec,
					  long				&l2Bandwidth)
{
/*	int normalSendTime = 99;
	int normalSize = bandwidth/100;
	
	int specialSize = normalSize + bandwidth % 100;

	if(normalSize > 0)
	{
		packetNumPerSec = 0;
		packetNumPerSec += (normalSendTime * ( (normalSize - 1) /packetsize +1));
	}
	else // normalSize == 0
	{
	}	
	
	if(specialSize != 0) // normally the specialSize is over 0
	{
		packetNumPerSec += ( (specialSize - 1) /packetsize +1);
	}
*/
	if(protocol == "tcp")
	{
		l2Bandwidth = packetNumPerSec * 54;
	}
	else // udp
	{
		l2Bandwidth = packetNumPerSec * 42;
	}
	return true;
}


		 
bool QosTorturer::resetAllStat()
{
    for(int i = 0;i < mNumStream; i++)
	{
		OmnString cmd = "bouncer mgr reset stat ";
        cmd << mStreamList[i].protocol << " "
            << mStreamList[i].sip << " "
            << mStreamList[i].sport  << " "
            << mStreamList[i].dip << " "
            << mStreamList[i].dport;
            
        sendCmd(cmd,mBouncerConn);         
	}
	return true;	
}


bool QosTorturer::showStreamStat()
{
	cout << "------------------------------\n"
		 << "Stream Status is:\n" 
		 << "------------------------------\n";
	     	
	for(int i = 0;i < mNumStream;i++)
	{
		cout << mStreamList[i].sip   << " "
			 << mStreamList[i].sport << " "
			 << mStreamList[i].dip   << " "
			 << mStreamList[i].dport << " "
			 << mStreamList[i].protocol << " "
			 << mStreamList[i].priority << "\n"
			 << "Expect bandwidth is: " 
			 << mStreamList[i].bandwidth + mStreamList[i].l2Bandwidth << "\t"
			 << "Real bandwidth is: "
			 << mStreamList[i].realbw << endl;
	}
	
	return true;	
}

bool QosTorturer::showRuleStat()
{
	cout << "------------------------------\n"
		 << "Rule Status is:\n" 
		 << "------------------------------\n";
	     	
	for(int i = 0;i < mNumRule;i++)
	{
		cout << mRuleList[i].sip   << " "
			 << mRuleList[i].sport << " "
			 << mRuleList[i].dip   << " "
			 << mRuleList[i].dport << " "
			 << mRuleList[i].protocol << " "
			 << mRuleList[i].interface << " "
			 << mRuleList[i].priority << endl;
	}
	
	return true;	
}

bool QosTorturer::showAllStat()
{
	cout << "--------------------------------\n"
	     << "Tc Bandwidth is: " << mTcBw << endl;
	cout << "CLI Turn: " << mCLITurnNum 
	     << "\tStream Turn: " << mStreamProcTurnNum
             << "\t Check Turn: " << mCheckTurnNum << endl;

	cout << "Calculate Time: "  << mTimeFromReset << "\tTime From Change Stream: " << mTimeFromProcStream << endl;	
	showRuleStat();
	showStreamStat();
	cout << "--------------------------------" << endl;
	
	return true;

}

/*
bool
QosTorturer::runAddIfRule()
{
	OmnString cmd = "qos ifrule add ";
	
	//gen correct cli
	if(RAND_INT(1,100) <= 80)
	{
		//gen random interface
		OmnString interface;
		genInterfaceRhc(interface);
		//gen random priority
		OmnString priority;
		genPriority(priority); 	
		//add to rule list
	 	if(!addIfRule( interface,
                       priority))
        {
            return false;
        }
       
       	cmd << interface << " " << priority;       
       
        if(!sendCmd(cmd,mRhcConn))
		{
		}
        
		
	}
	else
	{
			
	}		
	return true;
	
}

bool 
QosTorturer::addIfRule(OmnString &interface,OmnString &priority)
{
    if (mNumRule >= eMaxRule)
    {
        return false;
    }       
    // check whether there is one same rule or 
    // it is tcp and send ip port is occupied.
    for(int i =0;i < mNumRule;i++)
    {
        if(mRuleList[i].interface      == interface       &&
           mRuleList[i].type     == "interface")
    	{
                // same stream (we do not consider about priority
                return false;
        }    
    }
	
    // it is a good rule, append to the array
    mRuleList[mNumRule].type = "interface";
    mRuleList[mNumRule].interface = interface;
    mRuleList[mNumRule].priority = priority;
    
    mNumRule ++;
    
    return true;
}                    

bool 
QosTorturer::runRemoveIfRule() 
{
    OmnString cmd = "qos ifrule remove ";

    // first we decide to send a right CLI or wrong one
    if(RAND_INT(1,100) <= 80)
    {   
        // create a right CLI
        int index[eMaxRule] ;
        int j = 0;
        int i;
        
        for(i = 0; i < mNumRule; i++)
        {
            if(mRuleList[i].type == "interface")
            {
                index[j] = i;
                j++;
            }           
        }   
        if(j == 0)
        {
            // no rule to remove
            return false;
        }


        int randIndex1 = RAND_INT(0,j-1);
        int randIndex2 = index[randIndex1];

        // create remove CLI base on rule randIndex2
 
        cmd << mRuleList[randIndex2].interface;
        
		// remove from CLI list
        removeRule(randIndex2);   
        
    }
    else
    {
        // create a wrong CLI.
        // check whether it is wrong CLI.
    }
    return true;
}
*/
bool
QosTorturer::runAddVlanRule()
{
	OmnString cmd = "qos vlanrule add ";
	
	//gen correct cli
	if(RAND_INT(1,100) <= 80)
	{
		//gen random interface
		int vlantag;
		vlantag = RAND_INT(1,4095);\
		//gen random priority
		OmnString priority;
		genPriority(priority); 	
		//add to rule list
	 	if(!addVlanRule( vlantag,priority))
        {
            return false;
        }
       
       	cmd << vlantag << " " << priority;       
       
        if(!sendCmd(cmd,mRhcConn))
		{
		}
        
		
	}
	else
	{
			
	}		
	return true;
	
}

bool 
QosTorturer::addVlanRule(int &vlantag,OmnString &priority)
{
    if (mNumRule >= eMaxRule)
    {
        return false;
    }       
    // check whether there is one same rule or 
    // it is tcp and send ip port is occupied.
    for(int i =0;i < mNumRule;i++)
    {
        if(mRuleList[i].vlantag      == vlantag &&
           mRuleList[i].type     == "interface")
    	{
                // same stream (we do not consider about priority
                return false;
        }    
    }
	
    // it is a good rule, append to the array
    mRuleList[mNumRule].type = "vlan";
    mRuleList[mNumRule].vlantag = vlantag;
    mRuleList[mNumRule].priority = priority;
    
    mNumRule ++;
    
    return true;
}                    

bool 
QosTorturer::runRemoveVlanRule() 
{
    OmnString cmd = "qos vlanrule remove ";

    // first we decide to send a right CLI or wrong one
    if(RAND_INT(1,100) <= 80)
    {   
        // create a right CLI
        int index[eMaxRule] ;
        int j = 0;
        int i;
        
        for(i = 0; i < mNumRule; i++)
        {
            if(mRuleList[i].type == "vlan")
            {
                index[j] = i;
                j++;
            }           
        }   
        if(j == 0)
        {
            // no rule to remove
            return false;
        }


        int randIndex1 = RAND_INT(0,j-1);
        int randIndex2 = index[randIndex1];

        // create remove CLI base on rule randIndex2
 
        cmd << mRuleList[randIndex2].vlantag;
        
		// remove from CLI list
        removeRule(randIndex2);   
        
    }
    else
    {
        // create a wrong CLI.
        // check whether it is wrong CLI.
    }
    return true;
}

bool
QosTorturer::runAddMacRule()
{
	OmnString cmd = "qos macrule add ";
	
	//gen correct cli
	if(RAND_INT(1,100) <= 80)
	{
		//gen random interface
		OmnString mac;
		genMac(mac);
		//gen random priority
		OmnString priority;
		genPriority(priority); 	
		//add to rule list
	 	if(!addMacRule( mac,priority))
        {
            return false;
        }
       
       	cmd << mac << " " << priority;       
       
        if(!sendCmd(cmd,mRhcConn))
		{
		}
        
		
	}
	else
	{
			
	}		
	return true;
	
}

bool 
QosTorturer::addMacRule(OmnString &mac,OmnString &priority)
{
    if (mNumRule >= eMaxRule)
    {
        return false;
    }       
    // check whether there is one same rule or 
    // it is tcp and send ip port is occupied.
    for(int i =0;i < mNumRule;i++)
    {
        if(mRuleList[i].mac      == mac &&
           mRuleList[i].type     == "mac")
    	{
                // same stream (we do not consider about priority
                return false;
        }    
    }
	
    // it is a good rule, append to the array
    mRuleList[mNumRule].type = "mac";
    mRuleList[mNumRule].mac = mac;
    mRuleList[mNumRule].priority = priority;
    
    mNumRule ++;
    
    return true;
}            

bool 
QosTorturer::runRemoveMacRule() 
{
    OmnString cmd = "qos macrule remove ";

    // first we decide to send a right CLI or wrong one
    if(RAND_INT(1,100) <= 80)
    {   
        // create a right CLI
        int index[eMaxRule] ;
        int j = 0;
        int i;
        
        for(i = 0; i < mNumRule; i++)
        {
            if(mRuleList[i].type == "mac")
            {
                index[j] = i;
                j++;
            }           
        }   
        if(j == 0)
        {
            // no rule to remove
            return false;
        }


        int randIndex1 = RAND_INT(0,j-1);
        int randIndex2 = index[randIndex1];

        // create remove CLI base on rule randIndex2
 
        cmd << mRuleList[randIndex2].mac;
        
		// remove from CLI list
        removeRule(randIndex2);   
        
    }
    else
    {
        // create a wrong CLI.
        // check whether it is wrong CLI.
    }
    return true;
}


bool
QosTorturer::runAddProtoRule()
{
	OmnString cmd = "qos protorule add ";
	
	//gen correct cli
	if(RAND_INT(1,100) <= 80)
	{
		//gen random interface
		OmnString protocol;
		genProtocol(protocol);
		//gen random priority
		OmnString priority;
		genPriority(priority); 	
		//add to rule list
	 	if(!addProtoRule(protocol,priority))
        {
            return false;
        }
       
       	cmd << protocol << " " << priority;       
       
        if(!sendCmd(cmd,mRhcConn))
		{
		}
        
		
	}
	else
	{
			
	}		
	return true;
	
}

bool 
QosTorturer::addProtoRule(OmnString &protocol,OmnString &priority)
{
    if (mNumRule >= eMaxRule)
    {
        return false;
    }       
    // check whether there is one same rule or 
    // it is tcp and send ip port is occupied.
    for(int i =0;i < mNumRule;i++)
    {
        if(mRuleList[i].protocol      == protocol &&
           mRuleList[i].type     == "protocol")
    	{
                // same stream (we do not consider about priority
                return false;
        }    
    }
	
    // it is a good rule, append to the array
    mRuleList[mNumRule].type = "protocol";
    mRuleList[mNumRule].protocol = protocol;
    mRuleList[mNumRule].priority = priority;
    
    mNumRule ++;
    
    return true;
}                    

bool 
QosTorturer::runRemoveProtoRule() 
{
    OmnString cmd = "qos protorule remove ";

    // first we decide to send a right CLI or wrong one
    if(RAND_INT(1,100) <= 80)
    {   
        // create a right CLI
        int index[eMaxRule] ;
        int j = 0;
        int i;
        
        for(i = 0; i < mNumRule; i++)
        {
            if(mRuleList[i].type == "protocol")
            {
                index[j] = i;
                j++;
            }           
        }   
        if(j == 0)
        {
            // no rule to remove
            return false;
        }


        int randIndex1 = RAND_INT(0,j-1);
        int randIndex2 = index[randIndex1];

        // create remove CLI base on rule randIndex2
 
        cmd << mRuleList[randIndex2].protocol;
        
		// remove from CLI list
        removeRule(randIndex2);   
        
    }
    else
    {
        // create a wrong CLI.
        // check whether it is wrong CLI.
    }
    return true;
}

bool 
QosTorturer::runAddPortRule()
{
    OmnString cmd = "qos portrule add ";
    
    /*80% generate correct parms*/
    if(RAND_INT(1,100) <= 80)
    {
        
        OmnString direction;
        genDirection(direction);
        
  		//random port
        unsigned int port = RAND_INT(1,65535);

		OmnString app;
		genApplication(app);

        OmnString priority;
        genPriority(priority);      
        
        cmd << direction << " " << port << " " << app << " " << priority;
            
        // add general rule to the rule list with duplication checking      
        if(!addPortRule(direction,
                       port,
                       app,
                       priority))
        {
            return false;
        }
//        aosRunCli(cmd, true);
        return true;  
    }
    else
    {
        /*generate incorrect parms*/
        return true;
    }
    
    
}

bool 
QosTorturer::addPortRule(OmnString &direction,
                              unsigned int &port,
                              OmnString &app,
                              OmnString &priority)
{
    if (mNumRule >= eMaxRule)
    {
        return false;
    }       
    // check whether there is one same rule or 
    // it is tcp and send ip port is occupied.
    for(int i =0;i < mNumRule;i++)
    {
        if(mRuleList[i].direction   == direction &&
           mRuleList[i].port    	== port    &&
           mRuleList[i].app    		== app    &&
           mRuleList[i].type     	== "port")
    	{
                // same stream (we do not consider about priority
                return false;
        }    
    }
	
    // it is a good rule, append to the array
    mRuleList[mNumRule].type = "port";
    mRuleList[mNumRule].direction = direction;
    mRuleList[mNumRule].app = app;
    mRuleList[mNumRule].port = port;
    mRuleList[mNumRule].priority = priority;
    
    mNumRule ++;
    
    return true;
}                    


bool 
QosTorturer::runRemovePortRule()
{
    OmnString cmd = "qos portrule remove ";

    // first we decide to send a right CLI or wrong one
    if(RAND_INT(1,100) <= 80)
    {   
        // create a right CLI
        int index[eMaxRule] ;
        int j = 0;
        int i;
        
        for(i = 0; i < mNumRule; i++)
        {
            if(mRuleList[i].type == "port")
            {
                index[j] = i;
                j++;
            }           
        }   
        if(j == 0)
        {
            // no rule to remove
            return false;
        }


        int randIndex1 = RAND_INT(0,j-1);
        int randIndex2 = index[randIndex1];

        // create remove CLI base on rule randIndex2
 
        cmd << mRuleList[randIndex2].direction << " "
            << mRuleList[randIndex2].port << " ";
        
		// remove from CLI list
        removeRule(randIndex2);   
        
    }
    else
    {
        // create a wrong CLI.
        // check whether it is wrong CLI.
    }
    return true;
}


// 
// It will remove all UDP streams that are not high priority
// because UDP does not have flow control. If there is UDP
// low priority traffic, QoS will not work. 
//
bool 
QosTorturer::clearLowPrioUdp()
{
	for(int i = 0;i < mNumStream; i++)
	{	
		if(mStreamList[i].protocol == "udp" && 
		   (mStreamList[i].priority == "low" || 
		   	mStreamList[i].priority == "medium"))
		{
	    	AosStreamSimuMgr::getSelf()->removeStream(mStreamList[i].protocol,
	    									      mStreamList[i].sip,
	    									      mStreamList[i].sport,
	    									      mStreamList[i].dip,
	    									      mStreamList[i].dport);
		}

		// move stream list
	    for(int j = i;j < mNumStream - 1;j++)
	    {
	        mStreamList[j] = mStreamList[j+1];
	    }
	    mStreamList[mNumStream-1].clear();
	    mNumStream --;
	    i--;
	}
	return true;
}


bool 
QosTorturer::genStreamByGenRule(const int index)
{
    //check whether the stream is already existed
    for(int i = 0; i < mNumStream;i ++)
    {
    	if(mRuleList[index].sip      ==  mStreamList[i].sip &&
    	   mRuleList[index].sport    ==  mStreamList[i].sport &&
    	   mRuleList[index].protocol ==  mStreamList[i].protocol)
    	{
    		if(mRuleList[index].protocol == "tcp")
    		{
    			return false;	
    		}
    	
    		if(mRuleList[index].dip      ==   mStreamList[i].dip &&
    		   mRuleList[index].dport    ==   mStreamList[i].dport)
    		{
				return false;
			}
    	}       	
    }
    
    long Bw;
    
    long i1 = RAND_INT(eMinBandwidth, eMaxBandwidth);
    long i2 = RAND_INT(i1+eMinBwDiff, eMaxBandwidth);
    
   	mStreamList[mNumStream].maxBw = i1;
   	mStreamList[mNumStream].minBw = i2;	
    
    Bw = RAND_INT(eMinBandwidth,eMaxBandwidth);
    
    mStreamList[mNumStream].streamType = RAND_INT(1,4);     
           
    //mStreamList[mNumStream].sip 		= mRuleList[index].sip;
    mStreamList[mNumStream].sip 		= getSipFromList();
    mStreamList[mNumStream].sport 		= mRuleList[index].sport;
    //mStreamList[mNumStream].dip 		= getDipFromList();
    mStreamList[mNumStream].dip 		= mRuleList[index].dip;
    mStreamList[mNumStream].dport 		= mRuleList[index].dport;
    mStreamList[mNumStream].protocol 	= mRuleList[index].protocol;
    mStreamList[mNumStream].bandwidth 	= Bw;
    mStreamList[mNumStream].packetsize 	= RAND_INT(eMinPacketSize,eMaxPacketSize);
	
	return true;
}

bool 
QosTorturer::genStreamByProtocolRule(const int index)
{
	long Bw;
    
    long i1 = RAND_INT(eMinBandwidth,eMaxBandwidth);
    long i2 = RAND_INT(eMinBandwidth,eMaxBandwidth);
    
    Bw = RAND_INT(eMinBandwidth,eMaxBandwidth);

    if(i1 > i2)
    {
    	mStreamList[mNumStream].maxBw = i1;
    	mStreamList[mNumStream].minBw = i2;	
    }
    else
    {
    	mStreamList[mNumStream].maxBw = i1;
    	mStreamList[mNumStream].minBw = i2;	        
    }
    
    
    mStreamList[mNumStream].streamType 	= RAND_INT(1,4); 
	    	
	mStreamList[mNumStream].sip 		= getSipFromList();
    mStreamList[mNumStream].sport 		= RAND_INT(1,65535);
    mStreamList[mNumStream].dip 		= getDipFromList();
    mStreamList[mNumStream].dport 		= RAND_INT(1,65535);
    mStreamList[mNumStream].protocol 	= mRuleList[index].protocol;
    mStreamList[mNumStream].bandwidth 	= Bw;
    mStreamList[mNumStream].packetsize 	= RAND_INT(eMinPacketSize,eMaxPacketSize);
	return true;
}



bool 			
QosTorturer::genStreamByMacRule(const int index)
{
	OmnString protocol;
	genProtocol(protocol);
	OmnString sip;
	getDipFromMac(mRuleList[index].mac, sip);
	
	long Bw;
    
    long i1 = RAND_INT(eMinBandwidth,eMaxBandwidth);
    long i2 = RAND_INT(eMinBandwidth,eMaxBandwidth);
    
    Bw = RAND_INT(eMinBandwidth,eMaxBandwidth);

    if(i1 > i2)
    {
    	mStreamList[mNumStream].maxBw = i1;
    	mStreamList[mNumStream].minBw = i2;	
    }
    else
    {
    	mStreamList[mNumStream].maxBw = i1;
    	mStreamList[mNumStream].minBw = i2;	        
    }
    
    
    mStreamList[mNumStream].streamType 	= RAND_INT(1,4); 
	    	
	mStreamList[mNumStream].sip 		= sip;
    mStreamList[mNumStream].sport 		= RAND_INT(1,65535);
    mStreamList[mNumStream].dip 		= getDipFromList();
    mStreamList[mNumStream].dport 		= RAND_INT(1,65535);
    mStreamList[mNumStream].protocol 	= protocol;
    mStreamList[mNumStream].bandwidth 	= Bw;
    mStreamList[mNumStream].packetsize 	= RAND_INT(eMinPacketSize,eMaxPacketSize);
	return true;
}

bool 			
QosTorturer::genStreamByVLanRule(const int index)
{
	OmnString protocol;
	genProtocol(protocol);
	OmnString sip;
	getDipFromVLan(mRuleList[index].mac, sip);
	
	long Bw;
    
    long i1 = RAND_INT(eMinBandwidth,eMaxBandwidth);
    long i2 = RAND_INT(eMinBandwidth,eMaxBandwidth);
    
    Bw = RAND_INT(eMinBandwidth,eMaxBandwidth);

    if(i1 > i2)
    {
    	mStreamList[mNumStream].maxBw = i1;
    	mStreamList[mNumStream].minBw = i2;	
    }
    else
    {
    	mStreamList[mNumStream].maxBw = i1;
    	mStreamList[mNumStream].minBw = i2;	        
    }
    
    
    mStreamList[mNumStream].streamType 	= RAND_INT(1,4); 
	    	
	mStreamList[mNumStream].sip 		= sip;
    mStreamList[mNumStream].sport 		= RAND_INT(1,65535);
    mStreamList[mNumStream].dip 		= getDipFromList();
    mStreamList[mNumStream].dport 		= RAND_INT(1,65535);
    mStreamList[mNumStream].protocol 	= protocol;
    mStreamList[mNumStream].bandwidth 	= Bw;
    mStreamList[mNumStream].packetsize 	= RAND_INT(eMinPacketSize,eMaxPacketSize);
    return true;
}


void
QosTorturer::cleanUsedList()
{
	int expiredNum = 0;
	for(int i = 0;i < mUsedPortNum;i++)
	{
		if(	mUsedPortTime[i] - OmnTime::getCrtSec() < 300)
		{
			break;
		}
		expiredNum = i+1;
	}
	if(expiredNum <= 0)
	{
		return;
	}
	for(int i = expiredNum;i < mUsedPortNum;i++)
	{
		mUsedPorts[i - expiredNum] =  mUsedPorts[i];
		mUsedPortTime[i - expiredNum] =  mUsedPortTime[i];
	}
	mUsedPortNum -= expiredNum;
}


