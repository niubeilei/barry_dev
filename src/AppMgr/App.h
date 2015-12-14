////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 06/02/2012: Added 'smBaseDir'.
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_AppMgr_OmnApp_h
#define Omn_AppMgr_OmnApp_h

#include "AppMgr/Ptrs.h"
#include "Debug/Rslt.h"
#include "Porting/GetTime.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/Ptrs.h"
#include "SingletonClass/SingletonMgr.h"
#include "SEUtil/Ptrs.h"
#include "Util/Locale.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "XmlUtil/Ptrs.h"
#include "XmlParser/Ptrs.h"
#include <unistd.h>

#define AOSAPP_VERSION_0_10   				"0.10"
#define AOSAPP_VERSION_0_11   				"0.11"

class OmnApp : public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum AppStatus
	{
		eActive,
		eRunning,
		eStopping, 
		eStopped
	};

	enum
	{
		eDftDocClientVersion = 1,
		eNewDocClientVersion = 2,
		eDftBindObjidVersion = 1,
		eNewBindObjidVersion = 2,
		eDFMVersion			 = 1,	// Gavin, 2015/09/01, #31
		eNewDFMVersion		 = 2,	// Gavin, 2015/09/01, #31
	};

private:
	static bool					smNetworkEnabled;
	static bool					smFinished;
	static OmnXmlParserPtr		smParser;
	static OmnSingletonMgrPtr	smSingletonMgr;
	static OmnString			smAppName;
	static AosXmlTagPtr			smAppConfig;
	static AppStatus 			smAppStatus;
	static AosRundataPtr		smRundata;
	static AosRundataPtr		smDeleteObjRdata;
	static bool					smStarted;
	static OmnString			smVersion;
	static OmnString   			smUOI;
	static int64_t     			smInstanceId;
	static int         			smBuildNo;
	static OmnString   			smBuildTime;
	static OmnString   			smPatch;
	static OmnIpAddr   			smSignalAddr;
	static int         			smSignalPort;
	static OmnIpAddr   			smStreamerAddr;
	static int         			smStreamerPort;
	static OmnString			smLogId;
	static OmnString			smSeqno;
	static OmnString			smUser;
	static OmnString			smInstanceName;
	static bool					mFinished;
	static OmnSingletonMgrPtr	mSingletonMgr;
	static bool					smIsFrontEndServer;
	static int					smServerId;
	static int					smSystemId;
	static OmnString			smBaseDir;
	static OmnString			smTransDir;
	static u32					smStartSec;
	static bool					smCheckIgnoredAlarms;
	static bool					smIsSvrProxy;
	static OmnString			smAppDir;
	static OmnString			smConfigDir;
	static int					smDocClientVersion;		// Chen Ding, 2014/11/09
	static int					smBindObjidVersion;		// Ken Lee, 2014/12/01
	static int					smDFMVersion;			// Gavin, 2015/09/01, #31

public:
	OmnApp(const int argc, char **argv);

	// ThreadedObj interface
	virtual bool			threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool			signal(const int threadLogicId);
    virtual void    		heartbeat(const int tid); 
    virtual bool    		checkThread(OmnString &err, const int thrdLogicId) const;

	static void				enableNetwork() {smNetworkEnabled = true;}
	static void				disableNetwork() {smNetworkEnabled = false;}
	static bool				appLoop();
	static OmnRslt			startSingleton(OmnSingletonObj* singleton);
	static void				exitApp();
	static OmnAppPtr		getSelf();
	static OmnAppPtr		getSelf(const int argc, char **argv);
	static bool				appStart(const int argc, char **argv);
	static OmnString   		getLogId() {return smLogId;}
	static OmnString   		getSeqNo() {return smSeqno;}
	static OmnString   		getAppName() {return smAppName;}
	static OmnString   		getUser() {return smUser;}
	static OmnString   		getInstanceName() {return smInstanceName;}
	static bool				procCommandLine(int argc, char **argv, OmnString &errmsg);
	static AosLocale::E		getLocale();
	static bool				isAppStopped();
	static bool				isRunning(){return smAppStatus == eRunning;}
	static void 			running(){smAppStatus = eRunning;}
	static AosXmlTagPtr 	readConfig(const char* fname);
	static bool				setConfig(const AosXmlTagPtr &config);
	static bool				setConfig(const char *fname);
	static AosXmlTagPtr 	getAppConfig();
	static AosXmlTagPtr		getConfigSubtag(const OmnString &name);
	static AosRundataPtr	getRundata();
	static AosRundataPtr	getDeleteObjRundata();
	static OmnString		getVersion() {return smVersion;}
	static void				setVersion(const OmnString &v) {smVersion = v;}
	static OmnString		toString();
	static OmnIpAddr		getSignalAddr() {return smSignalAddr;}
	static int				getSignalPort() {return smSignalPort;}
	static OmnIpAddr		getStreamerAddr() {return smStreamerAddr;}
	static int				getStreamerPort() {return smStreamerPort;}
	static OmnString 		getUOI() {return smUOI;}
	static void				setUOI(const OmnString &uoi) {smUOI = uoi;}
	static int64_t 			getInstanceId() {return smInstanceId;}
	static void				setInstanceId(const int64_t &n) {smInstanceId = n;}
	static int				getBuildNo() {return smBuildNo;}
	static OmnString		getBuildTime() {return smBuildTime;}
	static OmnString		getPatch() {return smPatch;}
	static bool				isFrontEndServer() {return smIsFrontEndServer;}
	static void				setFrontEndServer(const bool b) {smIsFrontEndServer = b;}
	static int				getServerId() {return smServerId;}
	static void				setServerId(const int s) {smServerId = s;}
	static int				getSystemId() {return smSystemId;}
	static void				setSystemId(const int s) {smSystemId = s;}
	static void				setIsSvrProxy(){ smIsSvrProxy = true;};

	static void	setVersionInfo(const OmnString &version, 
							   const int buildNo,
							   const OmnString &buildTime,
							   const OmnString &patch)
	{
		smVersion = version;
		smBuildNo = buildNo;
		smBuildTime = buildTime;
		smPatch   = patch;
	}

	static void	stopApp(const OmnString &username, 
								const OmnString &passwd, 
								const OmnString &appname);
	static OmnString getAppBaseDir() 
	{
		return smBaseDir;
	}

	static OmnString getTransDir()
	{
		return smTransDir;
	}
	static bool createDir(const OmnString &dirname);
	// Ketty 2013/05/16
	static OmnString createSysDir(const OmnString &sub_dir);

	static OmnString getRemoteWorkingDir(const int server_id);
	static OmnString getLocalWorkingDir();

	// Chen Ding, 2013/05/19
	static bool checkIgnoredAlarms();
	static u32 getAppStartTime() {return smStartSec;}

	static OmnString getParentDir(const OmnString &dir);
	static OmnString getDirName(const OmnString &dir);
	static OmnString getAppDir() { return smAppDir;}
	static OmnString getConfigDir() { return smConfigDir; }

	// Chen Ding, 2014/11/09
	static int  getDocClientVersion() {return smDocClientVersion;}
	static void setDocClientVersion(const int v) {smDocClientVersion = v;}

	// Ken Lee, 2014/12/01
	static int	getBindObjidVersion() {return smBindObjidVersion;}
	static void setBindObjidVersion(const int v) {smBindObjidVersion = v;}

	static void getSelfMemoryUsage(const int pid, double &vm_usage, double & resident_set);

	// Gavin, 2015/09/01, STORAGE-31
	static int  getDFMVersion() {return smDFMVersion;}
	static void setDFMVersion(const int v) {smDFMVersion = v;}
	
private:
	bool init(int, char**);
	OmnRslt	startBasicSingletons();

};
#endif

