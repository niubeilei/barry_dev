////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HTTPTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SslTester_SslClientTester_h
#define Omn_SslTester_SslClientTester_h

#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"

/*
class AosSslTest : public OmnRCObject
{
	OmnDefineRCObject;

public:
	int				mRepeat;
	OmnString		mName;
	OmnIpAddr		mAddr;
	int				mPort;
	tcp_vs_conn 	mConn;
	OmnTcpClientPtr	mTcp;

	void set(int r, const OmnString name, const OmnIpAddr &addr, int port)
	{
		mRepeat = r;
		mName = name;
		mAddr = addr;	
		mPort = port;
	}
};
	
class AosSslTest;
class OmnTestcase;
class OmnIpAddr;
*/
class HTTPTester: public OmnTestPkg
{
private:

public:
	HTTPTester()
	{
		mName = "AosSslClientTester";
	}
	~HTTPTester() {}

	virtual bool		start();
	
private:
	bool	basicTest();
	void	AutoTest(); 
	bool    MultiThreadTest();
	void    MultiThreadInit();
	void	MultiThreadDestroy();
	
	//int 	tcp_vs_callback_debug(struct aos_app_proc_req *req);
};
#endif

