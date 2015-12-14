////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCmd.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelInterface_CliCmd_h
#define Omn_KernelInterface_CliCmd_h

#include "aos/KernelEnum.h"
#include "CliClient/Ptrs.h"
#include "KernelInterface/Ptrs.h"
#include "KernelInterface/CliLevel.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "Util/OmnNew.h"

class OmnStrParser;

class OmnCliParm : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxLineLength = 2000,
		eMaxFileLength = 100000
	};

	enum ParmType
	{
		eInvalid,
		eString, 
		eInt,
		eAddr,
		ePort,
		eInteractive,
		eFile,
		eMacAddr,
		ePortRange,
	};

	struct ValueDef
	{
		ValueDef *	mNext;
		OmnString	mStrValue;
		int			mIntValue;
		bool		mStop;


		ValueDef(const OmnString &v, const bool stop)
			:
		mNext(0),
		mStrValue(v), 
		mIntValue(-1),
		mStop(stop)
		{
		}

		ValueDef(const int &v, const bool stop)
			:
		mNext(0),
		mIntValue(v),
		mStop(stop)
		{
		}
	};

private:
	OmnString		mHelp;
	ParmType		mType;
	bool			mOptional;		// Whether it is an optional parm
	bool			mFinished;		// If true, no more parms after this one
	OmnString		mFileName;
	OmnString		mFileExt;
	ValueDef	   *mValues;
	bool			mSaveToFile;	// Used by eInteractive. If set to true, 
									// it will save the contents into a file
									// whose name is 'mFileName'.
	bool			mFilenameFromPrev;	// If true, the file name is the value of
									// of the previous parameter.

	// 
	// For optional parameters, the following two defines
	// the minimum and maximum occurances. mMin == n means
	// the same parameter should appear at least n times. 
	// mMaX == n means the same parameter should appear no
	// more than n times. '-1' means unlimited. By default, 
	// mMin = 0 and mMax = -1.
	//
	int				mMin;
	int				mMax;
	int				mOccurances;

	static bool			mReadFromBuff;
	static OmnString	mBuff;
	static int			mBuffCrt;
	static int			mBuffLen;

public:
	OmnCliParm()
	:
	mType(eInvalid), 
	mOptional(false),
	mFinished(false),
	mValues(0),
	mSaveToFile(false),
	mFilenameFromPrev(false),
	mMin(0),
	mMax(-1),
	mOccurances(0)
	{
	}

	~OmnCliParm(){}

	bool checkDef(OmnString &rslt) const
	{
		return true;
	}
		

	void	setHelp(const OmnString &h) {mHelp = h;}	
	void	setType(const ParmType t) {mType = t;}
	void	setOptional(const bool b) {mOptional = b;}
	void	setMin(const int b) {mMin = b;}
	void	setMax(const int b) {mMax = b;}
	void	setSaveFlag(const bool b) {mSaveToFile = b;}
	void	filenameFromPrevParm() {mFilenameFromPrev = true;}
	void	setFilename(const OmnString &n) {mFileName = n;}
	bool	needToSave() const {return mSaveToFile;}
	bool	nameFromPrevParm() const {return mFilenameFromPrev;}
	bool	finished() const {return !mOptional || mMax > 0 && mOccurances >= mMax;}
	bool	isOptional() const {return mOptional;}
	ParmType	getType() const {return mType;}
	OmnString	getFileName() const {return mFileName;}	
	OmnString	getFileExt() const {return mFileExt;}
	void		setFileExt(const OmnString &e) {mFileExt = e;}
	void	addValue(const OmnString &v, const bool stop)
	{
		ValueDef *def = OmnNew ValueDef(v, stop);
		if (!mValues)
		{
			mValues = def;
		}
		else
		{
			ValueDef *v = mValues;
			while (v && v->mNext) v = v->mNext;
			v->mNext = def;
		}
	}
	bool	isInteger() const {return mType == eInt || mType == eAddr || mType == ePort;}

	bool	parseArg(OmnStrParser& arg, 
					 int64_t &intv, 
					 OmnString &strv, 
					 bool &stop, 
					 OmnString &rslt);
/*
	bool	parseArgEx(OmnStrParser& arg, 
					 int &intv, 
					 OmnString &strv, 
					 bool &stop, 
					 OmnString &rslt,
					 int &errno);
*/
	bool	getInteractive(OmnString &value);
	bool	getFromFile(const OmnString &fn, OmnString &strv);

	static  void setInput(const OmnString buff)
	{
		mBuff = buff;
		mBuffCrt = 0;
	}
	static void	readFromBuff() {mReadFromBuff = true;}
};

class OmnCliCmd : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxIntArgs = 32,
		eMaxStrArgs = 32,
		eMaxDataSize = 128000,
		eMaxCommandLen = 20000,
	};

protected:
	OmnString				mPrefix;
	OmnString				mUsage;
	OmnVList<OmnCliParmPtr>	mParms;
	int						mBufsize;
	OmnString				mOprId;
	OmnString				mModId;
	OmnString				mModAddr;
	unsigned short			mModPort;
	AosCliLvl::CliLevel		mLevel;
	static bool				mLoadConfig;	// Indicate loading config

	//
	// add by lxx for module cli 03/13/2006
	//
	AosModuleCliServerPtr	mModuleCliServer;	
public:
	OmnCliCmd(const OmnString &def);
	OmnCliCmd();
	virtual ~OmnCliCmd() ;

	OmnString		getPrefix() const {return mPrefix;}
	//
	// mod by lxx for module cli 03/13/2006
	//
	virtual bool	run(const OmnString &cmd,const int parmPos, OmnString &rslt,const AosModuleCliServerPtr server);
	virtual bool	checkDef(OmnString &rslt);

	static void 	setLoadConfig(const bool b) {mLoadConfig = b;}
	static bool		isLoadConfig() {return mLoadConfig;}
	
	OmnString getOprId()const {return mOprId;}
	OmnString getModId()const {return mModId;}
	OmnString getModAddr()const {return mModAddr;}
	unsigned short getModPort()const {return mModPort;}
	void setModAddr(OmnString addr) {mModAddr = addr;}
	void setModPort(unsigned short port) {mModPort = port;}

	bool		runAppPreparing(const OmnString &parms, 
				  char **data, 
				  unsigned int *totalLen,
				  OmnString &rslt);
				  
	AosCliLvl::CliLevel		getLevel()const;


private:
	virtual bool	runKernel(OmnString cmd, const OmnString &parms, OmnString &rslt);
	bool 			retrieveInMemLog(const OmnString &parms, OmnString &rslt);
	bool			appendIntToStr(OmnString &rslt, int ret);

#ifdef AOS_OLD_CLI
	virtual bool	saveConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	loadConfig(const OmnString &parms, OmnString &rslt);
	bool	clearConfig(OmnString &rslt);

	//kevin
	virtual bool	systemUname(const OmnString &parms, OmnString &rslt);
	virtual bool	systemArpProxyAdd(const OmnString &parms, OmnString &rslt);
	virtual bool	systemArpProxyDel(const OmnString &parms, OmnString &rslt);
	virtual bool	systemIpAddress(const OmnString &parms, OmnString &rslt);
	virtual bool	systemDns(const OmnString &parms, OmnString &rslt);
	virtual bool	systemRouteCommon(const OmnString &parms, OmnString &rslt);
	virtual bool	systemRouteDefault(const OmnString &parms, OmnString &rslt);
	virtual bool	systemDelRouteCommon(const OmnString &parms, OmnString &rslt);
	virtual bool	systemDelRouteDefault(const OmnString &parms, OmnString &rslt);
	virtual bool	systemRouteShow(const OmnString &parms, OmnString &rslt);
	virtual bool	systemDevRoute(const OmnString &parms, OmnString &rslt);
	virtual bool	systemDelDevRoute(const OmnString &parms, OmnString &rslt);
	
	// for system update
	virtual bool	systemUpdate(const OmnString &parms, OmnString &rslt);
	
	// dxr, for dhcp server
	virtual bool	dhcpStart(const OmnString &parms, OmnString &rslt);
	virtual bool	dhcpStop(const OmnString &parms, OmnString &rslt);
	virtual bool    dhcpServerShowConfig(const OmnString &parms, OmnString &rslt);	
    virtual bool    dhcpServerLeaseTimeSet(const OmnString &parms, OmnString &rslt);	
    virtual bool    dhcpServerRouterSet(const OmnString &parms, OmnString &rslt);	
    virtual bool    dhcpServerDnsSet(const OmnString &parms, OmnString &rslt);	
    virtual bool    dhcpServerIpBlock(const OmnString &parms, OmnString &rslt);	
    virtual bool    dhcpServerAddBind(const OmnString &parms, OmnString &rslt);	
    virtual bool    dhcpServerDelBind(const OmnString &parms, OmnString &rslt);	
	virtual bool	saveDhcpServerConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	loadDhcpServerConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	dhcpClientGetIp(const OmnString &parms, OmnString &rslt);
	virtual bool	dhcpClientShowIp(const OmnString &parms, OmnString &rslt);
	//xyb for cpu mgr
	virtual bool    cpuMgrSetRecordTime(const OmnString &parms, OmnString &rslt);
	virtual bool    cpuMgrSetRecordStop(const OmnString &parms, OmnString &rslt);
	virtual bool    cpuMgrShowCpu(const OmnString &parms, OmnString &rslt);
	virtual bool    cpuMgrShowProcess(const OmnString &parms, OmnString &rslt);

	virtual bool    memMgrSetRecordTime(const OmnString &parms, OmnString &rslt);
	virtual bool    memMgrSetRecordStop(const OmnString &parms, OmnString &rslt);
	virtual bool    memMgrShowMem(const OmnString &parms, OmnString &rslt);

	virtual bool    diskMgrSetRecordTime(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrSetRecordStop(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrShowStatistics(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrFileTypeAdd(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrFileTypeRemove(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrFileTypeClear(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrFileTypeShow(const OmnString &parms, OmnString &rslt);

	/*
	virtual bool    diskMgrQuotaStatus(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrQuotaStatusShow(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrQuotaList(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrQuotaSet(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrQuotaDelete(const OmnString &parms, OmnString &rslt);
	virtual bool    diskMgrQuotaShow(const OmnString &parms, OmnString &rslt);
	*/

	virtual bool    watchdogSetStatus(const OmnString &parms, OmnString &rslt);
	virtual bool    watchdogStop(const OmnString &parms, OmnString &rslt);

	//dxr for SCA
	virtual bool    secureAuthcmdPolicy(const OmnString &parms, OmnString &rslt);
	virtual bool    secureAuthcmdCommandAdd(const OmnString &parms, OmnString &rslt);
	virtual bool    secureAuthcmdCommandDel(const OmnString &parms, OmnString &rslt);
	virtual bool    secureAuthcmdCommandShow(const OmnString &parms, OmnString &rslt);
	virtual bool    secureAuthcmdCommandReset(const OmnString &parms, OmnString &rslt);
	// GB, for dnsproxyserver
	virtual bool	dnsproxyStart(const OmnString &parms, OmnString &rslt);
	virtual bool	dnsproxyStop(const OmnString &parms, OmnString &rslt);
    virtual bool	dnsproxySetName(const OmnString &parms, OmnString &rslt);
    virtual bool    dnsproxySetIp(const OmnString &parms, OmnString &rslt);	
    virtual bool    dnsproxySetLIp(const OmnString &parms, OmnString &rslt);	
    virtual bool    dnsproxyAddGlobal(const OmnString &parms, OmnString &rslt);	
    virtual bool    dnsproxyDelGlobal(const OmnString &parms, OmnString &rslt);	
    virtual bool	dnsproxyShow(const OmnString &parms, OmnString &rslt);
	virtual bool	saveDnsproxyConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	loadDnsproxyConfig(const OmnString &parms, OmnString &rslt);

	virtual bool	pppoeStart(const OmnString &parms, OmnString &rslt);
	virtual bool	pppoeStop(const OmnString &parms, OmnString &rslt);
	virtual bool    pppoeStatus(const OmnString &parms, OmnString &rslt);	
	virtual bool    pppoeShow(const OmnString &parms, OmnString &rslt);	
    virtual bool    pppoeUsernameSet(const OmnString &parms, OmnString &rslt);	
    virtual bool    pppoePasswordSet(const OmnString &parms, OmnString &rslt);	
    virtual bool    pppoeDnsSet(const OmnString &parms, OmnString &rslt);	
	virtual bool	savePppoeConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	loadPppoeConfig(const OmnString &parms, OmnString &rslt);

	virtual bool	pptpStart(const OmnString &parms, OmnString &rslt);
	virtual bool	pptpStop(const OmnString &parms, OmnString &rslt);
	virtual bool	pptpIpLocal(const OmnString &parms, OmnString &rslt);
	virtual bool	pptpIpRange(const OmnString &parms, OmnString &rslt);
	virtual bool	pptpAddUser(const OmnString &parms, OmnString &rslt);
	virtual bool	pptpDelUser(const OmnString &parms, OmnString &rslt);
	virtual bool	pptpShowConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	savePptpConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	loadPptpConfig(const OmnString &parms, OmnString &rslt);

 	virtual bool	macSet(const OmnString &parms, OmnString &rslt);
 	virtual bool	macBackSet(const OmnString &parms, OmnString &rslt);
 	virtual bool	saveMacConfig(const OmnString &parms, OmnString &rslt);
 	virtual bool	macShowConfig(const OmnString &parms, OmnString &rslt);
 	virtual bool	loadMacConfig(const OmnString &parms, OmnString &rslt);
	//GB 11/30/2006 


 	virtual bool	bridgeGroupAdd(const OmnString &parms, OmnString &rslt);
 	virtual bool	bridgeGroupDel(const OmnString &parms, OmnString &rslt);
	virtual bool    bridgeGroupMemberAdd(const OmnString &parms, OmnString &rslt);
	virtual bool    bridgeGroupMemberDel(const OmnString &parms, OmnString &rslt);
	virtual bool    bridgeGroupMemberIp(const OmnString &parms, OmnString &rslt);
	virtual bool    bridgeGroupIp(const OmnString &parms, OmnString &rslt);
	virtual bool    bridgeShowConfig(const OmnString &parms, OmnString &rslt);
	virtual bool    bridgeClearConfig(const OmnString &parms, OmnString &rslt);
	// CHK 2006-12-12
	virtual bool    saveBridgeConfig(const OmnString &parms, OmnString &rslt);
	virtual bool    loadBridgeConfig(const OmnString &parms, OmnString &rslt);
	virtual bool    bridgeGroupUp(const OmnString &parms, OmnString &rslt);
	virtual bool    bridgeGroupDown(const OmnString &parms, OmnString &rslt);
	
	virtual bool	saveFwConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	loadFwConfig(const OmnString &parms, OmnString &rslt);
	virtual bool	fwRules(const OmnString &parms, OmnString &rslt);
	
	virtual bool	fwIpfilAll(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilTcp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilUdp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilIcmp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilStateNew(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilStateAck(const OmnString &parms, OmnString &rslt);
	
	virtual bool	fwTimefil(const OmnString &parms, OmnString &rslt);
	virtual bool	fwDelTimefil(const OmnString &parms, OmnString &rslt);
	
	virtual bool	fwMacfilAll(const OmnString &parms, OmnString &rslt);
	virtual bool	fwMacfilTcp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwMacfilUdp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwMacfilIcmp(const OmnString &parms, OmnString &rslt);
	
	virtual bool	fwNatSnat(const OmnString &parms, OmnString &rslt);
	virtual bool	fwNatDnatPort(const OmnString &parms, OmnString &rslt);
	virtual bool	fwNatDnatIP(const OmnString &parms, OmnString &rslt);
	virtual bool	fwNatMasq(const OmnString &parms, OmnString &rslt);
	virtual bool	fwNatRedi(const OmnString &parms, OmnString &rslt);
		
	virtual bool	fwIpfilDelAll(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilDelTcp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilDelUdp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilDelIcmp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilDelStateNew(const OmnString &parms, OmnString &rslt);
	virtual bool	fwIpfilDelStateAck(const OmnString &parms, OmnString &rslt);
	
	virtual bool	fwMacfilDelAll(const OmnString &parms, OmnString &rslt);
	virtual bool	fwMacfilDelTcp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwMacfilDelUdp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwMacfilDelIcmp(const OmnString &parms, OmnString &rslt);
	
	virtual bool	fwNatDelSnat(const OmnString &parms, OmnString &rslt);
	virtual bool	fwNatDelDnatPort(const OmnString &parms, OmnString &rslt);
	virtual bool	fwNatDelDnatIP(const OmnString &parms, OmnString &rslt);
	virtual bool	fwNatDelMasq(const OmnString &parms, OmnString &rslt);
	virtual bool	fwNatDelRedi(const OmnString &parms, OmnString &rslt);
	
	virtual bool	fwAntiSyn(const OmnString &parms, OmnString &rslt);
	virtual bool	fwAntiIcmp(const OmnString &parms, OmnString &rslt);
	virtual bool	fwAntiDelSyn(const OmnString &parms, OmnString &rslt);
	virtual bool	fwAntiDelIcmp(const OmnString &parms, OmnString &rslt);
	
	virtual bool 	fwBlacklistLoadFile(const OmnString &parms, OmnString &rslt);
	virtual bool 	fwBlacklistLoadUrl(const OmnString &parms, OmnString &rslt);

	virtual bool 	fwBlacklistRemoveFile(const OmnString &parms, OmnString &rslt);
	virtual bool 	fwBlacklistRemoveUrl(const OmnString &parms, OmnString &rslt);
	
	virtual bool	webwallRedi(const OmnString &parms, OmnString &rslt);
	virtual bool	webwallDelRedi(const OmnString &parms, OmnString &rslt);
	
	
		// jzz, for knocking on the SecuredShellMgr
	virtual bool	startSecuredShell(const OmnString &parms, OmnString &rslt);
	virtual bool	createSecuredShell(const OmnString &parms, OmnString &rslt);
	virtual bool	removeShellCommands(const OmnString &parms, OmnString &rslt);
	virtual bool	restoreShellCommands(const OmnString &parms, OmnString &rslt);
	virtual bool	clearActiveShells(const OmnString &parms, OmnString &rslt);
	virtual bool	stopSecuredShell(const OmnString &parms, OmnString &rslt);
	virtual bool	username(const OmnString &parms, OmnString &rslt);
	
	
#endif



#ifdef AOS_ENABLE_SYSTEM_EXCUTE_CLI
	virtual bool	systemExcute(const OmnString &parms, OmnString &rslt);
#endif

};

#ifndef OmnDeclareCliCmd
#define OmnDeclareCliCmd(x, str) \
class x : public OmnCliCmd \
{ \
public: \
	x() {mCmdStr = (str);} \
	virtual ~x() {} \
	virtual bool	run(const OmnString &cmd,const int parmPos, OmnString &rslt); \
};
#endif

#endif

