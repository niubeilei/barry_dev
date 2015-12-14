
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FactoryDefault.h
// Description:
//   
//
// Modification History:
// 12/07/2006: Created by JZZ
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Router_FactoryDefault_FactoryDefault_h
#define Aos_Router_FactoryDefault_FactoryDefault_h

#include "Debug/Debug.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/ValList.h"

class AosFactoryDefault: public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString						mName;
	OmnString						mPort;

public:
	AosFactoryDefault();
	~AosFactoryDefault();

	bool setToDefault(OmnString &rslt);



	AosFactoryDefault(const OmnString &name, const OmnString &port)
	{
		set(name,port);
		return;
	}
	
	bool 
	set(const OmnString &name, const OmnString &port)
	{
		mName = name;
		mPort = port;
	}
	
	OmnString	name(void)	{return mName;}
	
	OmnString 	port(void)	{return mPort;}
	
	bool connectToServer(const OmnTcpClientPtr &cliConn);
	
	bool login(OmnTcpClientPtr &cliConn,const OmnString &username, const OmnString &password,OmnString & sessionId);

	bool sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer,const OmnString &sessionId, const OmnString &username);
	
	void showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff,const OmnString &sessionId ,const OmnTcpClientPtr &dstServer);
};

#ifdef CLIDEAMON_NEED_AUTH
enum
{
	eSignature1 = 0xfb,
	eSignature2 = 0xfe,
	eSessionLength = 8
};
#else
enum
{
	eSignature1 = '\n',
	eSignature2 = '\n',
	eSessionLength = 0
};
#endif


#endif

