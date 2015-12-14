////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.h
// Description:
//   
//
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerExamples_SimpleTorturer_h
#define Omn_TorturerExamples_SimpleTorturer_h

#include "Debug/Debug.h"
#include "Random/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpClient.h"
#include "Tester/TestPkg.h"


class QosTorturer : public OmnTestPkg
{
private:
	enum
	{
		eMinBwDiff = 1000,
		eTcMinBw = 30,
		eTcMaxBw = 500
	};

	enum CliId
	{
		eStatusOn = 1, 
		eStatusOff = 2, 
		
		eGenRuleAdd = 3,
		eGenRuleRemove = 4,
		eGenRuleModify = 5,
		eGenRuleClear = 6,
		eGenRuleShow = 7,
		
		eMacRuleAdd = 8,
		eMacRuleRemove = 9,
		eMacRuleModify = 10,
		eMacRuleClear = 11,
		eMacRuleShow = 12,
		
		eVlanRuleAdd = 13,
		eVlanRuleRemove = 14,
		eVlanRuleModify = 15,
		eVlanRuleClear = 16,
		eVlanRuleShow = 17,
		
		eIfRuleAdd = 18,
		eIfRuleRemove = 19,
		eIfRuleModify = 20,
		eIfRuleClear = 21,
		eIfRuleShow = 22,
		
		ePortRuleAdd = 23,
		ePortRuleRemove = 24,
		ePortRuleModify = 25,
		ePortRuleClear = 26,
		ePortRuleShow = 27,
		
		eProtoRuleAdd = 28,
		eProtoRuleRemove = 29,
		eProtoRuleModify = 30,
		eProtoRuleClear = 31,
		eProtoRuleShow = 32,
		
		eSaveConfig = 33,
		eClearConfig = 34,
		eShowConfig = 35,
		
		eShowChannel = 36,
		eShowChannelAll = 37,
		eShowChannelHis = 38,
		eShowDev = 39,
		eShowDevAll = 40,
		eShowDevHis = 41,
		eShowPri = 42,
		eShowPriAll = 43,
		eShowPriHis = 44
	};
	
	enum
	{
		eMaxRule = 64,
		eMaxStream = 64,
		eMinBandwidth = 1024,
		eMaxBandwidth = 131072,
		eMinPacketSize = 1024,
		eMaxPacketSize = 1024,
		eTolerance = 15,
		eMinMidBw = 2048,
		eMinLowBw = 2048
		
	};

	
	struct rule
	{
		OmnString type;
		OmnString sip;
		unsigned int sport;
		OmnString dip;
		unsigned int dport;
		OmnString protocol;
		OmnString interface;
		OmnString mac;
		int vlantag;
		OmnString direction;
		unsigned int port;
		OmnString app;
		OmnString priority;
		
		
		rule();
		~rule();
		rule& operator = (const rule &ru);
		void clear();
	};
	
	struct stream
	{
		OmnString 		sip;
		unsigned int 	sport;
		OmnString 		dip;
		unsigned int 	dport;
		OmnString 		protocol;
		long 			bandwidth;
		int 			packetsize;
		OmnString 		priority;
		long 			realbw;
		long 			packetNumPerSec;
		long 			l2Bandwidth;
		long 			maxBw;
		long 			minBw;
		int  			streamType;

		stream();
		~stream();
		stream& operator = (const stream &stm);
		void clear();
	};
	
	
	struct rule 		mRuleList[eMaxRule];
	struct stream 		mStreamList[eMaxStream];
	int 				mNumStream;
	int 				mNumRule;
	bool                mIsStatusOn;
	OmnTcpClientPtr		mBouncerConn;
	OmnTcpClientPtr		mRhcConn;
	OmnTcpClientPtr		mTcConn;

	long 				mTcBw;
	
	OmnIpAddr 			mTc_Ip;
	int					mTc_Port;
	OmnIpAddr 			mBouncer_Ip;
	int					mBouncer_Port;
	OmnIpAddr 			mRhc_Ip;
	int					mRhc_Port;
	
	int			mCLITurnNum;
	int			mStreamProcTurnNum;
	int			mCheckTurnNum;	
	int			mTimeFromReset;	
	int			mTimeFromProcStream;	
	
	int			mUsedPorts[1000];
	long		mUsedPortTime[1000];
	int			mUsedPortNum;
	
public:
	QosTorturer();
	~QosTorturer() {}
	virtual bool		start();

private:
	bool 			clitest();
	bool 			runCLIs(const int runNum,
							 const AosRandomIntegerPtr &randInt); 
	bool 			procStreams(const int genNum);
         			
	bool 			genStream();
	bool 			modifyStream();
	bool 			removeStream();
         			
	bool 			checkBandwidth();
         			
	bool 			addGenRule(OmnString &sip,unsigned int &sport,OmnString &dip,
	     			                    unsigned int &dport,OmnString &protocol,
	     			                    OmnString &interface,OmnString &priority);
         			
	bool 			removeRule(const int &index);
	bool			getDipFromMac(OmnString &mac, OmnString &dip);
	bool 			genPriority(OmnString &priority);
	bool 			genInterface(OmnString &interface);
	bool 			genInterfaceRhc(OmnString &interface);
	bool 			genProtocol(OmnString &protocol);
	bool			genMac(OmnString &mac);
	bool			genDirection(OmnString &direction);
	bool			genApplication(OmnString &app);
	
	bool 			runRemoveGeneralRule();
	bool 			runAddGeneralRule();
	bool 			runStatusOff();
	bool 			runStatusOn();
         			
         			
	bool 			getExpectBandwidth(long &epHighBw,
										long &epMidBw,
										long &epLowBw);
         			
	bool 			getRealBandwidth(long &epHighBw,
									  long &epMidBw,
									  long &epLowBw);

	bool 			getExpectBandwidth(long &epTotalBw);
         			
	bool 			getRealBandwidth(long &epTotalBw);
         			
	bool 			updateStreamPriority();
         			
	bool 			streamHitRule(const int streamNum,const int ruleNum);

	bool 			getRealBwPerConn(const int streamNum,long &curBw,long &aveBw);
	bool 			genIpAddr(OmnString &ipaddr);
	bool 			sendCmd(const OmnString &cmd,const OmnTcpClientPtr &dstServer);
	bool			sendCmd(const OmnString &cmd,
						 	const OmnTcpClientPtr &dstServer,
						 	OmnString &strRslt);
	void			showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff);


	bool 			setTcBw(const long &TcBw);
	bool			isPortValid(const unsigned int &port);	
	bool			initQos();
	bool			showRuleStat();
	bool			showStreamStat();
	bool			showAllStat();

	
	OmnString		getSipFromList();
	OmnString		getDipFromList();
	

    bool			calcL2Bw(const OmnString 	&protocol,
				    		 const long			bandwidth,
				    		 const long			packetsize,
				    		 long				&packetNumPerSec,
				    		 long				&l2Bandwidth);
	bool 			resetAllStat();

	bool			runAddVlanRule();
	bool			addVlanRule(int &vlantag,OmnString &priority);
	bool			runRemoveVlanRule();
	bool			runAddMacRule();
	bool			addMacRule(OmnString &mac,OmnString &priority);
	bool			runRemoveMacRule();
	bool			runAddProtoRule();
	bool			addProtoRule(OmnString &protocol,OmnString &priority);
	bool			runRemoveProtoRule();
	bool			runAddPortRule();
	bool			addPortRule(OmnString &direction,
                              unsigned int &port,
                              OmnString &app,
                              OmnString &priority);
    bool			runRemovePortRule();

	void			cleanUsedList();

	bool 			genStreamByGenRule(const int index);
	bool 			genStreamByProtocolRule(const int index);
	bool 			genStreamByMacRule(const int index);
	bool 			genStreamByVLanRule(const int index);

	bool			getDipFromVLan(OmnString &mac, OmnString &dip);
	bool 			clearLowPrioUdp();


};

#endif

