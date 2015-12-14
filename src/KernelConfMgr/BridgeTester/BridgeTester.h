////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BridgeTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SystemCli_BridgeTester_BridgeTester_h
#define Omn_SystemCli_BridgeTester_BridgeTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"


class AosBridgeTester : public OmnTestPkg
{
private:
	enum CliId
	{
		eBridgeAdd 			= 1,    // 1)	bridge group add <bridge-name> 20%                  
		eBridgeDel 			= 2,    // 2)	bridge group del <bridge-name> 10%                  
		eBridgeSetIp 		= 3,    // 3)	bridge group set ip <bridge-name> <ip> 10%          
		eBridgeMemberAdd 	= 4,    // 4)	bridge group member add <bridge-name> <dev-name> 20%
		eBridgeMemberDel 	= 5,    // 5)	bridge group member del <bridge-name> <dev-name> 10%
		eBridgeMemberSetIp 	= 6,    // 6)	bridge group member set ip <dev-name> <ip>  10%     
		eBridgeShowConfig 	= 7,    // 7)	bridge show config 5%                               
		eBridgeClearConfig 	= 8,    // 8)	bridge clear config 5%                              
		eBridgeGroupUp 		= 9,    // 9)	bridge group up <bridge-name> 5%                    
		eBridgeGroupDown 	= 10,    // 10)	bridge group down <bridge-name>5%                   
	};
    
    enum
    {
    	eMaxDev				     = 16,
    	eMaxBridgDevLen			 = 32,
    	eMaxBridgeNameLen		 = 64,
    	eMaxTooLongBridgeNameLen = 128,
    	eMaxBridgeNum			 = 16,
	    eMaxDevNum 			     = 16
    };
    
    struct Device
    {
    	OmnString mDevName;
    	OmnString mIp;
    	
    	Device();
    	~Device();
    };
    
	struct Bridge
	{
		OmnString 	mBridgeName;
		int			mNumDevice;
	    Device		mDev[eMaxDevNum];
		OmnString 	mBridgeIp;
		bool		mStatus;
		
		Bridge();
		~Bridge();   
	};


private:
	OmnString 		devArray[eMaxDev];
	int				devNum;
    Bridge 			bridge[eMaxBridgeNum];  
	int				mNumBridge;
	
	Bridge			showBridge[eMaxBridgeNum];
	int				mShowNumBridge;
public:
	AosBridgeTester();
	~AosBridgeTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	initialize();

	bool 	runBridgeAdd();
	bool 	runBridgeDel();
	bool 	runBridgeSetIp();
	bool 	runBridgeMemberAdd();
	bool 	runBridgeMemberDel();
	bool 	runBridgeMemberSetIp();  
	bool 	runBridgeShowConfig(); 
	bool 	runBridgeClearConfig();  
	bool 	runBridgeGroupUp();
	bool 	runBridgeGroupDown();

	bool	genName(OmnString &name, int minLen, int maxLen);
	bool	nameExist(OmnString &name);
	bool	addBridgeName(OmnString &name);
	bool	addBridgeIp(int bridgeIndex, OmnString &ip);
	bool	genIpAddr(bool isCorrect, OmnString &ip);
	bool	genDevName(OmnString &devName, bool isCorrect);
	
	bool	addBridgeMem(int bridgeIndex, OmnString &devName);
	bool	delBridgeMem(int bridgeIndex, int devIndex);
	bool	addMemberIp(int bridgeIndex, int devIndex, OmnString &ip);
	bool	devExist(OmnString &devName);
	bool	delBridgeName(int bridgeIndex);
	bool	readBlock(const char *buffer, const char *start,
					  const char *end, char *result, int result_len);
};
#endif

