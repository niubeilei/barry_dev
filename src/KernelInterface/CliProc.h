////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliProc.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliProc_CliProc_h
#define Omn_CliProc_CliProc_h


#include "aos/KernelEnum.h"
#include "aosUtil/CharPtree.h"
#include "aosUtil/Alarm.h"
#include "CliClient/Ptrs.h"
#include "KernelInterface/Ptrs.h"
#include "KernelInterface/CliLevel.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/TcpListener.h"
#include "XmlParser/Ptrs.h"
 
OmnDefineSingletonClass(OmnCliProcSingleton,
						OmnCliProc,
						OmnCliProcSelf,
						OmnSingletonObjId::eCliProc, 
						"CliProc");


struct aosCharPtree;
class OmnCliCmd; 

class OmnCliProc : public OmnTcpListener
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxTcpConns = 5
	};

	int					mTcpIndex;
	OmnIpAddr			mTcpAddr[eMaxTcpConns];
	int					mTcpPort[eMaxTcpConns];
	OmnTcpCommSvrPtr	mTcpComm[eMaxTcpConns];
	aosCharPtree *		mCmdTree;
	OmnString			mFilename;
	AosModuleCliServerPtr	mModuleServer;

	// temp variants
	OmnXmlItemPtr		mModDefs;
	
	OmnString		mConfFileName;
	OmnFilePtr		mConfFile;
	OmnTcpClientPtr mConnClient;
	
public:
	OmnCliProc();
	~OmnCliProc();

	bool		start();
	bool		stop();
	OmnRslt		config(const OmnXmlParserPtr &conf);

	static OmnCliProc *	getSelf();
	
	int 	saveConfig(OmnString &rslt);
	int 	loadConfig(OmnString &rslt);
	int		clearConfig(OmnString &rslt);
	
	// 
	// TcpListener interface
	//
	virtual OmnString	getTcpListenerName() const;
	virtual void		msgRecved(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);

	bool		runCli(const OmnString &str, OmnString &rslt,const AosCliLvl::CliLevel level = AosCliLvl::eConfig);

    bool        runCliAsClient(const OmnString &str, OmnString &rslt,const AosCliLvl::CliLevel level = AosCliLvl::eConfig);
	
	bool        procCli(const OmnString &str, OmnString &rslt,const bool isClient,const AosCliLvl::CliLevel level = AosCliLvl::eConfig);
	
	void		setModuleCliServer(const AosModuleCliServerPtr	moduleServer);

private:
	bool		addCmd(const OmnString &def);
	bool		addCmds(const OmnString &filename);
	bool		getModDefs();

	bool		appendIntToStr(OmnString &rslt, int ret);

	int		clearConfig(const char *moduleName, OmnString &rslt);
	int		saveConfig(const char *moduleName, OmnString &rslt);
	int		loadConfig(const char *moduleName, OmnString &rslt);

	int		sendCmd(const OmnString& cmdStr, OmnCliCmd* cmd, OmnString& rslt, OmnString& errmsg);
};

#endif

