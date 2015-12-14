////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ModuleCliServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CliDeamon_ModuleCliServer_h
#define Aos_CliDeamon_ModuleCliServer_h

#include "TcpAppServer/Ptrs.h"
#include "Thread/Ptrs.h"
#include "UtilComm/TcpMsgReader.h"
#include "XmlParser/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util1/TimerObj.h"


class AosModuleCliServer: public OmnTcpMsgReader,public OmnTimerObj
{
	// 1 This is a singleton.
	// 2 impliment the function void	msgRecved(...)
	// 3 impliment the function int  readStyleCallBack1(...) as callback func
	// 4 impliment the function bool	loadReadStyleCallBack() appoint callback func
	OmnDefineRCObject;

	struct sAOSMCS_Module
	{
		OmnIpAddr	addr;
		int			port;
		OmnString	moduleName;		
	};

	enum
	{
		eAosMCS_MaxModuleNum = 100,
		eAosMCS_WaitTime = 4,
	};
private:
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;

	AosTcpAppServerPtr			mServer;
	sAOSMCS_Module				mModules[eAosMCS_MaxModuleNum];
	int							mModulesNum;	
	OmnString					mFilename;
	bool							mTimeOut;
	OmnString 					mRslt;
	
public:
	AosModuleCliServer();
	~AosModuleCliServer();

	//
	//	Functions of OmnTcpMsgReader
	//
	virtual OmnString	getTcpMsgReaderName() const ;
	virtual int			nextMsg(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);
	//
	//	Functions of OmnTimerObj
	//

	virtual void		timeout(const int timerId, 
								const OmnString &timerName,
								void *parm);



	bool				config(const OmnXmlParserPtr &conf);

	bool				start();
	
	bool				runCli(const OmnString &moduleName, const OmnString &cmd,OmnString &resp);

	bool				addModule(const OmnString &config);
	bool				addModules(const OmnString &filename);
	bool				removeModule(const OmnString &moduleName);
	bool				getModule(const OmnString &modId,OmnIpAddr &addr , int &port);



private:
	void			procMsg(const OmnString & rslt,
		  				   const OmnTcpClientPtr &conn);
	bool			getCmd(const OmnConnBuffPtr &buff , OmnString &cmd);
	bool			sendResp(OmnString &rslt);



};
#endif
