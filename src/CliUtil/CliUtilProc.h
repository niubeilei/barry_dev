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

#ifndef Omn_CliUtil_CliUtilProc_h
#define Omn_CliUtil_CliUtilProc_h

#include "aos/KernelEnum.h"
#include "aosUtil/CharPtree.h"
#include "CliUtil/Ptrs.h"
#include "KernelInterface/CliLevel.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpListener.h"
#include "XmlParser/Ptrs.h"

#include "CliUtil/CliUtil.h"

OmnDefineSingletonClass(OmnCliUtilProcSingleton,
						OmnCliUtilProc,
						OmnCliUtilProcSelf,
						OmnSingletonObjId::eCliUtilProc, 
						"CliUtilProc");


struct aosCharPtree;
class OmnCliUtilCmd; 

class OmnCliUtilProc : public OmnTcpListener
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
	
	/* for related cmds */
	char**				mRelated;
	int					mNum;

	OmnTcpServerPtr		mListen;
	
public:
	OmnCliUtilProc();
	~OmnCliUtilProc();

	bool		start();
	bool		stop();
	OmnRslt		config(const OmnXmlParserPtr &conf);

	static OmnCliUtilProc *	getSelf();
	
	bool getRelatedCmds(char** names, int num);
	bool startListen(short port);
	bool startListen(char* appname);
	// 
	// TcpListener interface
	//
	virtual OmnString	getTcpListenerName() const;
	virtual void		msgRecved(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);
	
	bool OmnUserLand_addCliCmd(char *id, aosCliFunc func);
	
private:
	bool		addCmd(const OmnString &def);
	bool		addCmds(const OmnString &filename);
	bool 		checkRelated(const char* def);
	unsigned short		getPortByAppname(char* name);
public:
	bool		runCli(const OmnString &str, OmnString &rslt,const AosCliLvl::CliLevel level = AosCliLvl::eConfig);
};

#endif

