////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: VLanTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SystemCli_VLanTester_VLanTester_h
#define Omn_SystemCli_VLanTester_VLanTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"



//4.1	Add a VLAN to an Interface
//4.2	Remove a VLAN from an Interface
//4.3	Remove All VLAN Tags from an Interface
//4.4	Add a VLAN IP Address to an Interface
//4.5	Remove a VLAN IP Address from an Interface
//4.6	Remove All VLAN IP Address from an Interface
//4.7	Turn the Attached Ethernet Switch VLAN On/Off
//4.8	Add a VLAN Tag on the VLAN Switch
//4.9	Remove a VLAN Tag from a Port of the Attached Switch
//4.10	Remove All VLAN Tags from the Attached Switch
//4.11	Remove All VLAN Tags from a Port of the Attached Switch
//4.12	Get Number of Ports on the Attached Switch
//4.13	Reset Attached Switch VLANs 
//4.14	Clear VLAN Configuration 
//4.15	Retrieve VLAN Configuration 
//4.16	Save VLAN Configuration
//4.17	Load VLAN Configuration

//1.1	vlan device add
//1.2	vlan device del
//1.3	vlan set ip
//1.4	vlan clear config
//1.5	vlan save config
//1.6	vlan load config
//1.7	vlan show config
//1.8	vlan switch status
//1.9	vlan switch reset
//1.10	vlan switch set



class AosVLanTester : public OmnTestPkg
{
private:
	enum CliId
	{
		eVLanAddDev 		= 1,
		eVLanDelDev 		= 2,
		eVLanSetIp  		= 3,
		eVLanSwitchStatus 	= 4,
		eVLanSwitchSet 		= 5,
        eVLanSwitchReset	= 6,
		eVLanClearConfig 	= 7,
		eVLanSaveConfig 	= 8,
		eVLanLoadConfig 	= 9,
		eVLanShowConfig 	= 10
	};
    
    enum
    {
    	eMaxDev = 256, 
		eMaxVlanTag = 4095, 
		eMinSwitchPort = 1,
		eMaxSwitchPort = 5
    };
    
	struct vlanList
	{
		OmnString interface;
		int vlanid;
		OmnString ip;
		
		vlanList();
		~vlanList();
		vlanList& operator = (const vlanList &devl);
		void clear();
	};
	
	
/*	struct vlanSwitch
	{
		int vlanid;
		unsigned int port;
		
		rule();
		~rule();
		rule& operator = (const rule &ru);
		void clear();
	};
*/	
	
//	OmnString 		configList[64];
	bool				isVlanOn;
	bool				isSwitchOn;
	struct vlanList 	mDevList[eMaxDev];
	struct vlanList		mSaveList[eMaxDev];
	int					mDevNum;
	int					mSaveNum;
	int					mMinSwitchPort;
	int					mMaxSwitchPort;
	bool				isSwitchStatus;

public:
	AosVLanTester();
	~AosVLanTester() {}

	virtual bool		start();
	void			setSwitchPorts(const int min, const int max);

private:
	bool			basicTest();
	bool			runAddDev();
	bool			runDelDev();
	bool			runSetIp();
	bool			runSwitchStat();
	bool			runSwitchSet();
	bool			runSwitchReset();
	bool			runClearConfig();
	bool			runSaveConfig();
	bool			runLoadConfig();
	bool			runShowConfig();
	bool			genInterface(OmnString &interface);
	bool			saveToDevList(OmnString &intf, int &id);
	bool			saveToDevList(const int &index, OmnString &ipaddr);
	bool			delFromDevList(const int &index);
	bool			saveDevList();
	bool			loadDevList();
	bool			clearDevList();
	OmnString		genIncorrectVlantag();
};
#endif

