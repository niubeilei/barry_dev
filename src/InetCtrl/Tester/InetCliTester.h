////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: InetCliTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_InetCtrl_Tester_InetCliTester_h
#define Omn_InetCtrl_Tester_InetCliTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"


class AosInetCliTester : public OmnTestPkg
{
private:
	enum CliId
	{
		eAddApp = 1, 
		eAddIpFlow = 2, 
		eAddUrlFlow = 3,
		eRemoveIpFlow = 4,
		eRemoveUrlFlow = 5,
		eRemoveApp = 6,
		eShowApps = 7,
		eClearApps = 8,

		eAddTimeRule = 9,
		eAddTimeLimit = 10,
		eAddOnlineTime = 11,
		eRemoveTimeLimit = 12,
		eRemoveOnlineTime = 13,
		eRemoveTimeRule = 14,
		eShowTimeRules = 15,
		eClearTimeRules = 16,

		eAddGroup = 17,
		eAddUserByName = 18,
		eAddUserByIP = 19,
		eAddUserByMac = 20,
		eRemoveUserByName = 21,
		eRemoveUserByIP = 22,
		eRemoveUserByMac = 23,
		eAddAssociation = 24,
		eRemoveAssociation = 25,
		eRemoveGroup = 26,
		eShowGroup = 27, 
		eClearGroup = 28,
	

		eTurnOnOff = 29,
		eShowConfig = 30,
		eLogLevel = 31,
		eDefaultPolicy = 32,
		eSaveConfig = 33
	};

	enum
	{
		eMaxNameLen = 16,
		eMaxTooLongNameLen = 16,

		eMaxApps = 128,
		eMaxIPFlows = 128,
		eMaxTimeRule = 128,
		eMaxRangeLimits = 128,
		eMaxOnLineLimits = 128,

		eMaxGroups = 128,
		eMaxUsersPerGroup = 128,
		eMaxRuleList = 128,
	};

	struct IpFlow
	{
		OmnString		mProto;
		OmnString		mDestIP;
		OmnString		mMask;
		int				mStartPort;
		int				mEndPort;
	};

	struct TimeLimit
	{
		OmnString		mStartTime;
		OmnString		mEndTime;
		OmnString		mDayOfWeek;
	};

	struct OnLineLimit
	{
		OmnString		mOnLineType;
		OmnString		mOnLineDay;
		int				mOnLineMin;
	};
		
		
	struct user
	{
		OmnString 		ip;
		OmnString 		mask;
		OmnString 		mac;
		OmnString		userName;
	};
	
	
	struct groupRule
	{
		OmnString	appName;
		OmnString	timeName;
	};
	
	
	struct IacApp
	{
		OmnString		mName;
		int				mNumIpFlows;
		IpFlow			mIpFlows[eMaxIPFlows];
	};


	struct IacTime
	{
		OmnString		mName;
		int				mNumTimeLimits;
		int				mNumOnLineLimits;
		TimeLimit		mTimeLimits[eMaxRangeLimits];
		OnLineLimit		mOnLineLimits[eMaxOnLineLimits];
	};


	struct IacGroup
	{
		OmnString		mName;
		int				mNumUser;
		user			groupUsers[eMaxUsersPerGroup];
		int 			mNumRule;
		groupRule		ruleList[eMaxRuleList];
	};
	
	OmnTcpClientPtr			mRhcConn;
	OmnIpAddr 				mRhc_Ip;
	int						mRhc_Port;
	
	IacApp			mApps[eMaxApps];
	int				mNumApps;

	IacTime			timeRule[eMaxTimeRule];
	int				mNumTimeRule;

	IacGroup		group[eMaxGroups];
	int 			mNumGroups;
	bool			mDefaultPolicy;
	
	OmnString				mLocalIpRange;
	OmnString               mLocalNetmask;
	OmnString				mRemoteIpRange;
	OmnString               mRemoteNetmask;

public:
	AosInetCliTester();
	~AosInetCliTester();

	virtual bool		start();

	bool 	printfAll();
	// 
	// Functions used by TrafficTester
	//
	bool    isIPInGroup(const int ip);
	bool    isApp(const int port, const OmnString &proto);
	bool    groupAppExist(const int localIP, const int remotePort, 
			const OmnString &proto);
	bool    getIPandPort(int &localIP, 
			int &remoteIP, 
			int &remotePort, 
			OmnString &proto, 
			bool &pass);
	bool    config(const int maxCommands);
    bool	getDefaultPolicy();
	bool	updateAppOnlineTime(int localIP, int remoteIP, int remotePort, int onlineTime);
	bool    basicTest(const u32 tries);
	bool	onlyHitGroup(u32 &sip);
	bool	onlyHitApp(u32 &dip, int &dport,OmnString &protocol);
	bool	notHitGroupApp(u32 &sip, u32 &dip, int &dport,OmnString &protocol);
	bool	hitGroupApp(u32 &sip, u32 &dip, int &dport,OmnString &protocol);
	bool	isConnPass(u32 &sip, int &sport,
							 u32 &dip, int &dport,
							 OmnString &protocol, OmnString &crtTime);
	bool	init();
	
private:
	bool	runAddIPFlow();
	bool	runAddApp();
	bool	runAddUrlFlow();
	bool	runRemoveIpFlow();
	bool	runRemoveUrlFlow();
	bool	runRemoveApp();
	bool	runShowApp();
	bool	runClearApp();
	
	bool	runAddTimeRule();
	bool	runAddTimeRangeCtr();
	bool	runAddTimeOnlineCtr();
	bool	runRemoveTimeRangeCtr();
	bool	runRemoveTimeOnlineCtr();
	bool	runRemoveRuleRule();
	bool	runShowTimeRules();
	bool 	runClearTimeRules();
	
	bool	runAddGroup();
	bool	runRemoveGroup();
	bool	runAddUserByName();
	bool	runAddUserByIP();
	bool	runAddUserByMac();
	bool	runRemoveUserByName();
	bool	runRemoveUserByIP();
	bool	runRemoveUserByMac();
	bool	runAddAssociation();
	bool	runRemoveAssociation();
	bool	runShowGroup();
	bool 	runClearGroup();
	
	bool	runTurnOnOff();
	bool	runDefaultPolicy();
	bool 	runShowConfig();
	bool	runLogLevel();
	bool	runSaveConfig();

	bool	appExist(const OmnString &name);
	bool	selectName(OmnString &name);
	bool	selectNonExistName(OmnString &name, const OmnString &selectType);

	bool	timeRuleExist(const OmnString &name);

	bool	groupExist(const OmnString &name);
	bool	selectNonExistUser(OmnString &username, int groupIndex);
	bool	selectNonExistGroup(OmnString &name);


	void	genName(OmnString &name, int minLen, int maxLen);
	void	genProto(OmnString &proto, bool &isCorrect);
	void 	genDstIp(OmnString &dstIP, bool &isCorrect);
	void 	genSrcIp(OmnString &dstIP, bool &isCorrect);
	void 	genMask(OmnString &mask, bool &isCorrect);
	void 	genStartEndPort(int &startPort, int &endPort, bool &isCorrect);
	void	genStartEndTime(OmnString &startTime, OmnString &endTime, bool &isCorrect);

	void 	genDayOfWeek(OmnString &startTime, bool &isCorrect);
	void	genOnLineType(OmnString &onLineType, bool &isCorrect);
	void	genOnLineMin(int &onLineMin, bool &isCorrect);
	bool 	isInUsed(const OmnString &name, const int type);
	bool 	userAlreadyExist(const OmnString &ipAddr, const OmnString &mask);
	bool	sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer);
	bool	sendCmd(const OmnString &cmd,
					 const OmnTcpClientPtr &dstServer,
					 OmnString &strRslt);
	bool 	findUser(u32 &sip, int &groupIndex);
	bool 	findApp(u32& dip, int& dport, OmnString& protocol, int& groupIndex, int& ruleIndex);
	bool	timeMatch(int& groupIndex, int& ruleIndex, OmnString &crtTime);

	OmnString	genInRangeIp(const OmnIpAddr &netaddr, const OmnIpAddr &netmask);
	OmnString	getRandomDip();
	OmnString	getRandomSip();
	int 		getAppIndex(const OmnString &appName);
	bool		clearAll();
};
#endif

