////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemoryTester.h
// Description:
//   
//
// Modification History:
// 02/15/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UtilComm_Tester_TcpServerTester_h
#define Aos_UtilComm_Tester_TcpServerTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/Ptrs.h"


class AosTcpServerTester : public OmnTestPkg,
						   virtual public OmnTcpListener
{
private:
	OmnTcpServerEpollPtr	mServer;

public:
	AosTcpServerTester();
	~AosTcpServerTester();

	virtual bool		start();

private:
	bool	basicTest();
	virtual OmnString   getTcpListenerName() const {return "TransSvrConn";};
	virtual void        msgRecved(const OmnConnBuffPtr &buff,                
			                      const OmnTcpClientPtr &conn);
	virtual void 		connCreated(const OmnTcpClientPtr &conn){return;}
	virtual void 		connClosed(const OmnTcpClientPtr &conn){return;}
};
#endif

