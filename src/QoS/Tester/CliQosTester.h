////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimerTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Qos_Tester_CliQosTester_h
#define Aos_Qos_Tester_CliQosTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosCliQosTester : public OmnTestPkg
{
	enum
	{
		eMaxMac = 256,
		eMaxProto = 256,
		eMaxVLan = 256,
		eMaxPort = 256,
		eMaxInterface = 256,
		eMaxGeneral = 256
	};

	struct qosProtoInfo
	{
		OmnString proto;
		OmnString priority;
	};

	struct qosGeneralInfo
	{
		OmnString 	srcIp;
		OmnString 	dstIp;
		OmnString 	protocol;
		OmnString 	interface;
		OmnString 	priority;
		int		  	srcPort;
		int			dstPort;
	};

	struct qosMacInfo
	{
		OmnString mac;
		OmnString priority;
	};



	struct qosVLanInfo
	{
		int tag;
		OmnString priority;
	};

	struct qosInterfaceInfo
	{
		OmnString name;
		OmnString priority;
	};


	struct qosPortInfo
	{
		OmnString direction;
		int port;
		OmnString app;
		OmnString priority;
	};

private:

	int					mNumGenerals,
						mNumMacs,
						mNumProtos,
						mNumVLans,
						mNumInterfaces,
						mNumPorts;

	qosGeneralInfo		mGenerals[eMaxGeneral];
	qosMacInfo			mMacAddrs[eMaxMac];
	qosProtoInfo		mProtos[eMaxProto];
	qosVLanInfo			mVLans[eMaxVLan];
	qosInterfaceInfo	mInterfaces[eMaxInterface];
	qosPortInfo			mPorts[eMaxPort];

public:
	AosCliQosTester();
	~AosCliQosTester() {}

	virtual bool		start();

private:

	bool			test(int tries);

	bool			testGeneralCli(int);
	bool			runAddGeneral();
	bool			runRemoveGeneral();
	bool			runShowGeneral();
	bool			runClearGeneral();
	bool			runModifyGeneral();
	bool			isNewGeneral(qosGeneralInfo general);
	bool			checkGeneral(qosGeneralInfo general);
	qosGeneralInfo  getGeneral();
	
	
	bool			testProtoCli(int);
	bool			runAddProto();
	bool			runRemoveProto();
	bool			runShowProto();
	bool			runClearProto();
	bool			runModifyProto();
	bool			isNewProto(const OmnString &proto);
	bool			checkProto(qosProtoInfo proto);
	OmnString  		getProto();
	

	bool			testVLanCli(int);
	bool			runAddVLan();
	bool			runRemoveVLan();
	bool			runShowVLan();
	bool			runClearVLan();
	bool			runModifyVLan();
	bool			isNewVLan(int vLan);
	bool			checkVLan(qosVLanInfo vLan);
	OmnString  		getVLan();
	
	
	bool			testPortCli(int);
	bool			runAddPort();
	bool			runRemovePort();
	bool			runShowPort();
	bool			runClearPort();
	bool			runModifyPort();
	bool			isNewPort(int port,const OmnString &direction);
	bool			checkPort(qosPortInfo port);
	OmnString  		getPort();
	
	
	bool			testInterfaceCli(int);
	bool			runAddInterface();
	bool			runRemoveInterface();
	bool			runShowInterface();
	bool			runClearInterface();
	bool			runModifyInterface();
	bool			isNewInterface(const OmnString &interface);
	bool			checkInterface(qosInterfaceInfo interface);
	OmnString  		getInterface();



	bool			testMacCli(int);
	bool			runAddMac();
	bool			runRemoveMac();
	bool			runShowMac();
	bool			runClearMac();
	bool			runModifyMac();
	bool 			checkMac(qosMacInfo mac);
	
	
	bool			runLoadConfig();
	bool			runSaveConfig();
	bool			runResetConfig();
	bool			isNewMac(const char* mac);
	OmnString  		getMacAddr();
};
#endif

