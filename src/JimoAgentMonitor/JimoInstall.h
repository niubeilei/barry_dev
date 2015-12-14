////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAgentMonitor_JimoInstall_h
#define Aos_JimoAgentMonitor_JimoInstall_h

#include "Util/String.h"
#include "Rundata/Rundata.h"
#include <vector>

class AosJimoInstall : virtual public OmnRCObject
{
	OmnDefineRCObject;
public: 
	enum
	{
		eInitPort = 1024
	};
private:
	int								mVirtuals;
	OmnString						mClusterName;
	OmnString						mLocalIp;
	OmnString						mUser;
	OmnString						mPwd;
	OmnString						mUserHome;
	OmnString						mInstallHome;
	OmnString						mDataHome;
	OmnString						mBkpType;
	OmnString						mClusterHome;
	OmnString						mPortRslt;
	OmnString						mScripts;
	OmnString						mExe;
	OmnString						mSharedLibs;
	OmnString						mSystemDocs;
	OmnString						mWebSite;
	vector<OmnString>				mIpList;
	map<OmnString, AosXmlTagPtr>	mTpls;
	//map<OmnString, OmnString>		mScripts;
	//map<OmnString, OmnString>		mExes;
	//map<OmnString, OmnString>		mDirs;
public:
	AosJimoInstall();
	~AosJimoInstall(){}
	bool init(const OmnString &cluster_name, const AosRundataPtr &rdata);
	bool update();
	bool install();
	bool unInstall();
	bool jimodb(const OmnString &args);
private:

	OmnString 	createServerStr(const u32 svr_id);
	OmnString 	createNetServerStr(const u32 svr_id, const int port);
	OmnString 	createCubeStr(const u32 svr_id);
	OmnString	getLocalIp();
	OmnString	createExe(const u32 svr_id);
	OmnString	createAdminConfig();
	OmnString	createNormConfig(
					const int thrift_port,
					const int front_port);
	OmnString	createNetworkConfig(
					const int port, 
					const OmnString &net_str);
	OmnString	createClusterConfig(
					const OmnString &servers_str, 
					const OmnString &cubes_str);
	OmnString 	createSvrProxyConfig(
					const u32 svr_id, 
					const int port, 
					const OmnString &workhome);
	OmnString	createAccessConfig(
					const int front_port, 
					const int access_port, 
					const int thrift_port);
	OmnString	createHtmlConfig(
					const int html_port, 
					const int front_port,
					const OmnString &master_workhome);
	bool		command(
					const OmnString &user, 
					const OmnString &cmd);
	bool		createFile(
					const OmnString &str, 
					const OmnString &filename);
	int			getPort(const u32 svr_id);
	int			getBroadcastPort();
	bool		initIps(const AosXmlTagPtr &ips);
	bool		initTpls(
					const AosXmlTagPtr &tpls, 
					const AosRundataPtr &rdata);
	bool		initScripts(const AosXmlTagPtr &scripts);
	bool		initExes(const AosXmlTagPtr &exes);
	bool		initDirs(const AosXmlTagPtr &dirs);
	bool 		setNoPassword();
	bool		clean();
};
#endif
